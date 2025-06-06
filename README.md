# Smart Irrigation System Using PIC16F877A

## 📌 Project Overview
This project automates irrigation using a **PIC16F877A microcontroller**, **DHT11 temperature and humidity sensor**, and a **soil moisture sensor** to optimize water usage. The system activates irrigation only when necessary, preventing overwatering and conserving resources.

## 🧭 Before You Start
Before beginning this project, we used a **ZIF (Zero Insertion Force) development kit** equipped with a **20 MHz crystal oscillator**, into which the **PIC16F877A microcontroller** was inserted. The PIC16F877A we used had a **pre-installed bootloader**, allowing for easier programming via serial communication. 

> 💡 **Note:** While the **PICkit 3** can also be used to program the PIC16F877A, using a microcontroller with a bootloader simplifies the upload process significantly and avoids several common issues.

## 🚀 Features
- **Sensor-Based Control** – Uses DHT11 and soil moisture sensors to determine watering needs.
- **Efficient Water Management** – Prevents water wastage and over-irrigation.
- **Microcontroller-Based Automation** – PIC16F877A processes data and controls irrigation.
- **Real-Time Monitoring** – Ensures timely and precise watering.

## 🛠 Components Required
```plaintext
- PIC16F877A Microcontroller
- DHT11 Temperature & Humidity Sensor
- Soil Moisture Sensor
- Relay Module
- Water Pump
- Power Supply (5V/12V)
- LCD Display (16X2)
```

## 📁 Repository Contents
```plaintext
- Source Code – Embedded C for PIC16F877A.
- Circuit Diagram – Designed in Proteus.
- Simulation Files – For testing before hardware implementation.
```

## 🧩 Circuit Diagram


![Circuit Diagram](https://raw.githubusercontent.com/Hackyharish/Smart-Irrigation-Using-PIC16F877A/refs/heads/main/Circuit%20Diagram.jpg)


## 🔄 Workflow Diagram

![Workflow Diagram](https://raw.githubusercontent.com/Hackyharish/Smart-Irrigation-Using-PIC16F877A/refs/heads/main/Workflow%20%20Diagram.jpg)



## 🔧 Setup & Usage
```plaintext
1. Connect the sensors and components to the PIC16F877A as per the circuit diagram.
2. Upload the provided source code to the microcontroller using Mplab or Boot Loder.
3. Also don't forget to add the header file lcd.h in headers section.
4. Power the system and monitor sensor readings.
5. The system will automatically activate irrigation based on soil moisture and temperature.
```
## 🎥 Working Video

[![Watch the video](https://github.com/Hackyharish/Smart-Irrigation-Using-PIC16F877A/blob/main/Cover.png)](https://youtube.com/shorts/ZagB9urzt0Q?feature=share)
[![Watch the video]()](https://youtube.com/shorts/ZagB9urzt0Q?feature=share)

## 🧱 Challenges Encountered
- **Programming via PICkit 3**: Although widely used, the application for PICkit 3 often suffers from bugs and inconsistent performance, making the upload process unreliable.
- **MPLAB Configuration**: Setting up and using MPLAB for code upload was time-consuming and complex due to toolchain configuration hurdles.
- **Recommended Solution**: Using a **PIC16F877A with a built-in bootloader** proved to be the most efficient approach, greatly simplifying the development workflow and minimizing setup errors.

## 🌱 Future Enhancements
```plaintext
- IoT Integration – Remote monitoring and control.
- Solar Power – Sustainable and energy-efficient operation.
- Mobile App – User-friendly interface for system status updates.
```

## 🤝 Authors
- [Harish R](https://www.linkedin.com/in/harish-r-8b68a333b?utm_source=share&utm_campaign=share_via&utm_content=profile&utm_medium=android_app)
- [Mauli Rajguru](https://www.linkedin.com/in/maulir?utm_source=share&utm_campaign=share_via&utm_content=profile&utm_medium=android_app)
- [Aryan Jaljith](https://www.linkedin.com/in/aryan-jaljith-64283b240?utm_source=share&utm_campaign=share_via&utm_content=profile&utm_medium=android_app)

## 📩 Contact
1. cb.en.u4eee23112@cb.students.amrita.edu 
2. cb.en.u4eee23120@cb.students.amrita.edu
3. cb.en.u4eee23105@cb.students.amrita.edu

