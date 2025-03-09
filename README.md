# 🌱 IoT-Based Stevia Plant Monitoring & Irrigation System

This project implements an **IoT-based system** for monitoring and automating the irrigation process of **Stevia plants** using **ESP32** and **MQTT**. The system collects **soil moisture and pH data**, then controls a **water pump** based on predefined thresholds or remote commands via MQTT.

---

## 📌 Project Overview
- ✅ **Technology Used:** ESP32, MQTT, Soil Moisture Sensors, pH Sensor, Water Pump.
- 🌡 **Data Collected:**
  - **Soil Moisture** (3 sensors for accuracy)
  - **pH Level** of the soil
- 📡 **Communication:**
  - Uses **WiFi** for MQTT communication.
  - Publishes data to an **MQTT broker** (`broker.emqx.io`).
  - Receives irrigation control commands from the MQTT topic.
- 🔋 **Automation & Efficiency:**
  - Activates the **water pump** when soil moisture is low.
  - Allows **manual activation** of irrigation via MQTT commands.
  - Implements **publishing intervals** to reduce network load.

---

## 🛠️ Hardware Components
| Component               | Description                       |
|-------------------------|----------------------------------|
| **ESP32**              | Microcontroller for IoT system  |
| **Soil Moisture Sensors** | Measures soil moisture levels  |
| **pH Sensor**          | Monitors soil acidity/alkalinity |
| **Water Pump**         | Controls irrigation based on moisture levels |

---

## 🔄 System Workflow
1️⃣ **ESP32 connects to WiFi & MQTT broker**.

2️⃣ **Reads data** from **3 soil moisture sensors & pH sensor**.

3️⃣ **Publishes data** to MQTT topic (`topic/panjipanjipanji`).

4️⃣ **Checks soil moisture levels**:
   - If below the threshold **or** if a manual command (`1`) is received → **Activates pump**.
   - Keeps the pump **ON for 10 seconds**, then turns it **OFF**.

5️⃣ **Repeats data transmission every 10 cycles** to maintain efficiency.
