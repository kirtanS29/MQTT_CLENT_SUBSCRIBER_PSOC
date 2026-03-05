# PSoC Edge E84 – WiFi MQTT LED Control (FreeRTOS)

## Overview

This project demonstrates an **IoT system using the Infineon PSoC Edge E84 board** where LEDs on the board are controlled remotely using the **MQTT protocol over Wi-Fi**.

The system connects to a public MQTT broker and allows **three independent LEDs to be controlled through different MQTT topics**.

This project demonstrates:

* Embedded networking
* MQTT publish/subscribe architecture
* FreeRTOS task communication using queues
* Hardware control using GPIO
* IoT cloud messaging concepts

---

## System Architecture

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

## Hardware Used

* Infineon **PSoC Edge E84 Development Kit**
* Onboard **Wi-Fi module**
* 3 Onboard LEDs
* User Button (optional)

---

## Software Components

* **FreeRTOS**
* **Infineon WiFi Connection Manager**
* **Infineon MQTT Client Library**
* **Device Configurator**
* **ModusToolbox**

---

## MQTT Configuration

Public MQTT broker used:

```
Broker: test.mosquitto.org
Port: 1883
Protocol: MQTT
```

---

## MQTT Topics

| Topic             | Function              |
| ----------------- | --------------------- |
| `kirtan/led1`     | Control LED 1         |
| `kirtan/led2`     | Control LED 2         |
| `kirtan/led3`     | Control LED 3         |
| `kirtan/psoc/pub` | Publish device status |

---

## MQTT Payload Commands

| Payload    | Action        |
| ---------- | ------------- |
| `TURN ON`  | Turns LED ON  |
| `TURN OFF` | Turns LED OFF |

Example message:

```
Topic: kirtan/led1
Payload: TURN ON
```

---

## FreeRTOS Task Architecture

The application uses multiple tasks:

### MQTT Task

Responsible for:

* MQTT initialization
* Connecting to broker
* Maintaining connection
* Handling reconnects

---

### Publisher Task

Publishes device status to the MQTT broker.

Example publish message:

```
Publisher: Publishing 'TURN OFF' on topic 'kirtan/psoc/pub'
```

---

### Subscriber Task

Handles incoming MQTT messages and controls LEDs.

Steps:

1. Receive MQTT message
2. Identify topic
3. Convert payload to device state
4. Send command to queue
5. Control corresponding LED

---

## Queue Communication

FreeRTOS queues are used to communicate between tasks.

Example structure used:

```c
typedef struct{
    subscriber_cmd_t cmd;
    uint32_t data;
    uint8_t led_id;
} subscriber_data_t;
```

This allows safe communication between:

```
MQTT Callback → Subscriber Task → LED Control
```

---

## MQTT Callback Flow

When a message is received:

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

## Example Serial Output

```
Wi-Fi Connection Manager initialized.

Wi-Fi Connecting to 'kirtan'

Successfully connected to Wi-Fi network 'kirtan'.
IPv4 Address Assigned: 10.246.170.144

MQTT library initialization successful.

'psocedge-mqtt-client862' connecting to MQTT broker 'test.mosquitto.org'...

MQTT connection successful.

Subscribed to topics successfully

Incoming MQTT message:
Topic: kirtan/led1
Payload: TURN ON
```

---

## Testing the Project

You can test this project using any MQTT client.

Recommended mobile apps:

* MQTT Dash
* IoT MQTT Panel
* MQTT Explorer

Send messages like:

```
Topic: kirtan/led1
Payload: TURN ON
```

LED1 on the board will turn ON.

---

## Key Concepts Demonstrated

* Embedded Wi-Fi connectivity
* MQTT publish/subscribe model
* FreeRTOS task management
* Inter-task communication using queues
* Hardware control using GPIO
* IoT device architecture

---

## Future Improvements

Possible enhancements:

* Add TLS secure MQTT connection
* Use AWS IoT or Azure IoT Hub
* Add sensor data publishing
* Add device shadow/state synchronization
* Implement OTA firmware updates

---

## Author

Kirtan

Embedded Systems Engineer (Learning & Building)

---

## License

This project is for educational and demonstration purposes.
