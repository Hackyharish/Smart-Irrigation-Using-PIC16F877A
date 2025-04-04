#include <xc.h>
#include <stdint.h>
#include <stdio.h>
#define _XTAL_FREQ 20000000

// Configuration bits
#pragma config FOSC = HS
#pragma config WDTE = OFF
#pragma config PWRTE = ON
#pragma config CP = OFF
#pragma config CPD = OFF
#pragma config BOREN = ON
#pragma config LVP = OFF

// LCD and DHT11 connections
#define DHT11_PIN RB0
#define DHT11_PIN_DIR TRISB0
#define RS RD2
#define EN RD3
#define D4 RD4
#define D5 RD5
#define D6 RD6
#define D7 RD7

// Relay control (connected to RB1 to avoid conflict with DHT11 on RB0)
#define RELAY RB1

#include "lcd.h"

// DHT11 Variables
char temperature[] = "Temp = 00.0 C";
char humidity[] = "RH   = 00.0 %";
uint8_t T_byte1, T_byte2, RH_byte1, RH_byte2, CheckSum;
uint8_t dht_valid_data = 0;
uint8_t dht_error_state = 0;  // 0: No error, 1: No response, 2: Time out, 3: Checksum error

// Soil Moisture Variables
volatile uint16_t adc_value = 1023; // Default ADC value
volatile uint8_t adc_ready = 0;     // Flag for ADC update
int moisture_value = 0;
char moisture_text[10];

// Display control variables
volatile uint16_t display_timer = 0;
volatile uint8_t display_mode = 0; // 0 for DHT11, 1 for Soil Moisture

// Function prototypes
void Start_Signal();
uint8_t Check_Response();
uint8_t Read_Data(uint8_t *dht_data);
void ADC_Init();
void Read_DHT11();
void Update_Soil_Moisture();
void IntToStr(int value, char *buffer);
void Display_DHT11_Data();
void Display_Soil_Moisture();

void __interrupt() ISR() {
    if (PIR1bits.ADIF) {
        adc_value = ((uint16_t)ADRESH << 8) | (uint16_t)ADRESL;
        adc_ready = 1;     // Set flag indicating new data available
        PIR1bits.ADIF = 0; // Clear ADC interrupt flag
        GO_nDONE = 1;      // Start next ADC conversion
    }
    
    if (PIR1bits.TMR1IF) {
        display_timer++;
        
        // Toggle display mode every 5 seconds
        if (display_timer >= 5) {
            display_timer = 0;
            display_mode = !display_mode; // Toggle between 0 and 1
        }
        
        PIR1bits.TMR1IF = 0; // Clear Timer1 interrupt flag
    }
}

void TMR1_Init() {
    T1CON = 0x31;        // Timer1 ON, 1:8 prescaler, internal clock
    TMR1H = 0x0B;        // For 1 second interrupt at 20MHz
    TMR1L = 0xDC;        // (0xFFFF - 0x0BDC) = 61220 cycles with 1:8 = ~1 second
    PIE1bits.TMR1IE = 1; // Enable Timer1 interrupt
    PIR1bits.TMR1IF = 0; // Clear Timer1 interrupt flag
}

void ADC_Init() {
    ADCON0 = 0b01000001; // ADC ON, Fosc/16, Channel 0
    ADCON1 = 0b11000000; // Internal reference voltage
    PIE1bits.ADIE = 1;   // Enable ADC interrupt
    PIR1bits.ADIF = 0;   // Clear ADC interrupt flag
    GO_nDONE = 1;        // Start first ADC conversion
}

void Start_Signal() {
    DHT11_PIN_DIR = 0; // Output mode
    DHT11_PIN = 0;
    __delay_ms(25);
    DHT11_PIN = 1;
    __delay_us(25);
    DHT11_PIN_DIR = 1; // Input mode
}

uint8_t Check_Response() {
    __delay_us(40);
    if (!DHT11_PIN) {
        __delay_us(80);
        if (DHT11_PIN) {
            __delay_us(80);
            return 1;
        }
    }
    return 0;
}

uint8_t Read_Data(uint8_t *dht_data) {
    uint8_t timeout = 0;
    uint8_t bitPosition;
    *dht_data = 0;
    
    for (bitPosition = 0; bitPosition < 8; bitPosition++) {
        // Wait for pin to go high (signifies start of bit)
        timeout = 0;
        while (!DHT11_PIN) {
            timeout++;
            if (timeout > 100) return 1; // Timeout error
            __delay_us(1);
        }
        
        // Measure duration of high pulse
        // A longer high pulse (>40us) means '1', shorter means '0'
        __delay_us(30);
        
        if (DHT11_PIN) {
            // Bit is '1'
            *dht_data |= (1 << (7 - bitPosition));
            
            // Wait for pin to go low before next bit
            timeout = 0;
            while (DHT11_PIN) {
                timeout++;
                if (timeout > 100) return 1; // Timeout error
                __delay_us(1);
            }
        }
        // Otherwise bit is '0', no need to change data
    }
    
    return 0; // Success
}

int map(int x, int in_min, int in_max, int out_min, int out_max) {
    if (x < in_max) x = in_max;
    if (x > in_min) x = in_min;
    return (int)((long)(x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min);
}

void IntToStr(int value, char *buffer) {
    sprintf(buffer, "%d%%", value);
}

void Read_DHT11() {
    uint8_t read_error = 0;
    
    // Completely disable all interrupts during DHT11 communication
    uint8_t saved_gie = INTCONbits.GIE;
    uint8_t saved_peie = INTCONbits.PEIE;
    INTCONbits.GIE = 0;
    INTCONbits.PEIE = 0;
    
    Start_Signal();
    if (Check_Response()) {
        // Read all bytes with improved error handling
        read_error = Read_Data(&RH_byte1);
        if (!read_error) read_error = Read_Data(&RH_byte2);
        if (!read_error) read_error = Read_Data(&T_byte1);
        if (!read_error) read_error = Read_Data(&T_byte2);
        if (!read_error) read_error = Read_Data(&CheckSum);
        
        if (read_error) {
            dht_error_state = 2; // Time out
            dht_valid_data = 0;
        } else {
            // Validate checksum
            if (CheckSum == ((RH_byte1 + RH_byte2 + T_byte1 + T_byte2) & 0xFF)) {
                temperature[7] = (T_byte1 / 10) + '0';
                temperature[8] = (T_byte1 % 10) + '0';
                temperature[10] = (T_byte2 / 10) + '0';
                humidity[7] = (RH_byte1 / 10) + '0';
                humidity[8] = (RH_byte1 % 10) + '0';
                humidity[10] = (RH_byte2 / 10) + '0';
                dht_valid_data = 1;
                dht_error_state = 0;
            } else {
                dht_valid_data = 0;
                dht_error_state = 3; // Checksum error
            }
        }
    } else {
        dht_valid_data = 0;
        dht_error_state = 1; // No response
    }
    
    // Restore interrupt state
    INTCONbits.PEIE = saved_peie;
    INTCONbits.GIE = saved_gie;
}

void Update_Soil_Moisture() {
    if (adc_ready) {
        uint8_t saved_gie = INTCONbits.GIE;
        INTCONbits.GIE = 0; // Disable interrupts during read
        
        moisture_value = map((int)adc_value, 1023, 278, 0, 100);
        adc_ready = 0; // Reset flag
        
        // Ensure moisture_value is within bounds
        if (moisture_value > 100) moisture_value = 100;
        if (moisture_value < 0) moisture_value = 0;
        
        // Control relay based on moisture level
        if (moisture_value < 40) {
            RELAY = 0; // Turn ON relay (activate water pump)
        } else {
            RELAY = 1; // Turn OFF relay
        }
        
        IntToStr(moisture_value, moisture_text);
        INTCONbits.GIE = saved_gie; // Restore interrupt state
    }
}

void Display_DHT11_Data() {
    uint8_t saved_gie = INTCONbits.GIE;
    INTCONbits.GIE = 0; // Disable interrupts during LCD update
    
    Lcd_Clear();
    
    if (dht_valid_data) {
        Lcd_Set_Cursor(1, 1);
        Lcd_Print_String(temperature);
        Lcd_Set_Cursor(2, 1);
        Lcd_Print_String(humidity);
    } else {
        Lcd_Set_Cursor(1, 1);
        switch (dht_error_state) {
            case 1:
                Lcd_Print_String("No response");
                break;
            case 2:
                Lcd_Print_String("Time out!");
                break;
            case 3:
                Lcd_Print_String("Checksum error");
                break;
            default:
                Lcd_Print_String("DHT11 Error");
                break;
        }
    }
    
    INTCONbits.GIE = saved_gie; // Restore interrupt state
}

void Display_Soil_Moisture() {
    uint8_t saved_gie = INTCONbits.GIE;
    INTCONbits.GIE = 0; // Disable interrupts during LCD update
    
    Lcd_Clear();
    
    Lcd_Set_Cursor(1, 1);
    Lcd_Print_String("Moisture: ");
    Lcd_Print_String(moisture_text);
    
    Lcd_Set_Cursor(2, 1);
    if (RELAY) {
        Lcd_Print_String("Pump: OFF");
    } else {
        Lcd_Print_String("Pump: ON");
    }
    
    INTCONbits.GIE = saved_gie; // Restore interrupt state
}

void main() {
    // Set up I/O ports
    TRISA0 = 1;    // Configure RA0 as input for ADC
    TRISB = 0x01;  // Configure RB0 as input for DHT11, rest as outputs
    TRISD = 0x00;  // Configure PORTD as output for LCD
    
    RELAY = 1;     // Initially turn OFF the relay
    
    // Initialize peripherals
    ADC_Init();
    TMR1_Init();
    Lcd_Start();
    Lcd_Clear();
    
    // Show startup message
    Lcd_Set_Cursor(1, 1);
    Lcd_Print_String("Initializing...");
    __delay_ms(1000);
    
    // Enable global and peripheral interrupts
    INTCONbits.PEIE = 1; // Enable peripheral interrupts
    INTCONbits.GIE = 1;  // Enable global interrupts
    
    uint8_t dht_read_counter = 0;
    
    while (1) {
        // Read soil moisture on every loop
        Update_Soil_Moisture();
        
        // Read DHT11 less frequently (every ~2 seconds)
        dht_read_counter++;
        if (dht_read_counter >= 10) {  // Increased from 5 to 10 for more stable readings
            Read_DHT11();
            dht_read_counter = 0;
            __delay_ms(50);  // Small delay after DHT11 read
        }
        
        // Display data based on current mode
        if (display_mode == 0) {
            Display_DHT11_Data();
        } else {
            Display_Soil_Moisture();
        }
        
        __delay_ms(500); // Small delay for system stability
    }
}