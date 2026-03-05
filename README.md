# PSoC Edge E84 – WiFi MQTT LED Control (FreeRTOS)

## Overview

This project demonstrates an **IoT system using the Infineon PSoC Edge E84 board** where LEDs on the board are controlled remotely using the **MQTT protocol over Wi-Fi**.

The system connects to a public MQTT broker and allows **three independent LEDs to be controlled through different MQTT topics**.

This project demonstrates:

- Embedded networking
- MQTT publish/subscribe architecture
- FreeRTOS task communication using queues
- Hardware control using GPIO
- IoT cloud messaging concepts

---

# System Architecture

```
Mobile MQTT App / MQTT Client
           │
           │  MQTT Publish
           ▼
     MQTT Broker
   (test.mosquitto.org)
           │
           │  MQTT Message
           ▼
    PSoC Edge E84 Board
           │
   Subscriber Task
           │
           ▼
   FreeRTOS Queue
           │
           ▼
     LED Control Logic
           │
           ▼
      Board LEDs
```

---

# Hardware Used

- Infineon **PSoC Edge E84 Development Kit**
- Onboard **Wi-Fi module**
- 3 Onboard LEDs
- User Button (optional)

---

# Software Components

- **FreeRTOS**
- **Infineon WiFi Connection Manager**
- **Infineon MQTT Client Library**
- **Device Configurator**
- **ModusToolbox**

---

# MQTT Configuration

Public MQTT broker used:

```
Broker: test.mosquitto.org
Port: 1883
Protocol: MQTT
```

---

# MQTT Topics

| Topic | Function |
|------|------|
| `kirtan/led1` | Control LED 1 |
| `kirtan/led2` | Control LED 2 |
| `kirtan/led3` | Control LED 3 |
| `kirtan/psoc/pub` | Publish device status |

---

# MQTT Payload Commands

| Payload | Action |
|------|------|
| `TURN ON` | Turns LED ON |
| `TURN OFF` | Turns LED OFF |

Example message:

```
Topic: kirtan/led1
Payload: TURN ON
```

---

# Project Setup Instructions

## 1. Install ModusToolbox

Download and install **ModusToolbox IDE** from the Infineon website.

---

## 2. Clone the Repository

```bash
git clone https://github.com/kirtanS29/MQTT_CLENT_SUBSCRIBER_PSOC.git
```

Open the project in **ModusToolbox IDE**.

---

# 3. Configure WiFi Credentials

Open the file:

```
wifi_config.h
```

Update your WiFi details:

```c
#define WIFI_SSID       "YOUR_WIFI_NAME"
#define WIFI_PASSWORD   "YOUR_WIFI_PASSWORD"
```

Example:

```c
#define WIFI_SSID       "kirtan"
#define WIFI_PASSWORD   "12345678"
```

---

# 4. Verify MQTT Configuration

Open the file:

```
mqtt_client_config.h
```

Broker configuration:

```c
#define MQTT_BROKER_ADDRESS   "test.mosquitto.org"
#define MQTT_PORT             1883
```

Topics used:

```c
#define MQTT_SUB_TOPIC1     "kirtan/led1"
#define MQTT_SUB_TOPIC2     "kirtan/led2"
#define MQTT_SUB_TOPIC3     "kirtan/led3"
```

---

# 5. Build the Project

From the project terminal run:

```bash
make build
```

Or build directly from **ModusToolbox IDE**.

---

# 6. Program the Board

Connect the **PSoC Edge E84 board** via USB.

Flash the firmware:

```bash
make program
```

Or click **Program** inside the IDE.

---

# 7. Open Serial Terminal

Use the following configuration:

```
Baudrate: 115200
```

Example serial output:

```
Wi-Fi Connection Manager initialized.

Wi-Fi Connecting to 'kirtan'

Successfully connected to Wi-Fi network 'kirtan'.
IPv4 Address Assigned: 10.246.170.144

MQTT library initialization successful.

MQTT connection successful.
```

---

# Testing the Project

Use any MQTT client.

Recommended apps:

- MQTT Dash
- IoT MQTT Panel
- MQTT Explorer
- HiveMQ Web Client

---

## Example Test

Send message:

```
Topic: kirtan/led1
Payload: TURN ON
```

Result:

```
LED1 on the board turns ON
```

Another test:

```
Topic: kirtan/led2
Payload: TURN OFF
```

Result:

```
LED2 turns OFF
```

---

# FreeRTOS Task Architecture

The application uses multiple tasks.

---

## MQTT Task

Responsible for:

- MQTT initialization
- Broker connection
- Reconnection handling

---

## Publisher Task

Publishes device status to MQTT broker.

Example:

```
Publisher: Publishing 'TURN OFF' on topic 'kirtan/psoc/pub'
```

---

## Subscriber Task

Handles incoming MQTT messages and controls LEDs.

Steps:

1. Receive MQTT message
2. Identify topic
2. Convert payload to device state
3. Send command to queue
4. Control corresponding LED

---

# Queue Communication

FreeRTOS queues are used for inter-task communication.

Example structure:

```c
typedef struct{
    subscriber_cmd_t cmd;
    uint32_t data;
    uint8_t led_id;
} subscriber_data_t;
```

Flow:

```
MQTT Callback → Queue → Subscriber Task → LED Control
```

---

# MQTT Callback Flow

```
MQTT Broker
      │
      ▼
MQTT Subscription Callback
      │
      ▼
Message Parsed
      │
      ▼
Queue Message Created
      │
      ▼
Subscriber Task
      │
      ▼
LED State Updated
```

---

# Example Serial Output

```
Incoming MQTT message:
Topic: kirtan/led1
Payload: TURN ON
```

```
Incoming MQTT message:
Topic: kirtan/led2
Payload: TURN OFF
```

---

# Key Concepts Demonstrated

- Embedded WiFi connectivity
- MQTT publish/subscribe architecture
- FreeRTOS task management
- Inter-task communication using queues
- Hardware control using GPIO
- IoT device architecture

---

# Future Improvements

Possible enhancements:

- Add TLS secure MQTT connection
- Integrate with AWS IoT or Azure IoT
- Add sensor data publishing
- Implement OTA firmware updates
- Add device state synchronization

---

# Author

Kirtan

Embedded Systems Engineer (Learning & Building)

---

# License

This project is for **educational and demonstration purposes**.
