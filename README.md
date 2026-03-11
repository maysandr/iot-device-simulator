IoT Device Telemetry Simulator (C / TCP)

A modular C-based system that simulates an IoT device sending telemetry data to a server over TCP.
The project demonstrates systems programming, networking, modular architecture, and telemetry processing.

The system includes a simulated device that periodically sends sensor data and a server that receives, validates, and logs the telemetry.


Architecture Overview

DEVICE PROGRAM
────────────────────────────────
config loader
      ↓
sensor simulator
      ↓
protocol encoder
      ↓
TCP socket client
      ↓
network

SERVER PROGRAM
────────────────────────────────
TCP socket server
      ↓
message receiver
      ↓
protocol parser
      ↓
telemetry validation
      ↓
console output
      ↓
log writer


Example Telemetry Message

The device sends telemetry in a structured message format:

device_id=BAT_001;timestamp=1773257332;temp=24.90;humidity=59.60;battery=100;status=OK

The server parses this message and converts it back into structured data.


Project Structure

iot-device-simulator
│
├── config
│   └── device.conf
│
├── logs
│   └── server.log
│
├── include
│   ├── common.h
│   ├── config.h
│   ├── logger.h
│   ├── protocol.h
│   ├── sensor.h
│   └── socket_utils.h
│
├── src
│   ├── config.c
│   ├── device_main.c
│   ├── logger.c
│   ├── protocol.c
│   ├── sensor.c
│   ├── server_main.c
│   └── socket_utils.c
│
└── README.md


Module Overview
Sensor Module

Simulates environmental telemetry from an embedded device.

Generated values:

temperature

humidity

battery level

device status

Example output:

temperature: 24.90
humidity: 59.60
battery: 100
status: OK

The sensor values include small random fluctuations to simulate real sensor drift.


Protocol Module

Handles conversion between structured data and network messages.

Encoding

TelemetryData → message string

Decoding

message string → TelemetryData

The protocol also validates incoming telemetry values.

Socket Module

Implements TCP networking using the Windows Winsock API.

Features:

TCP client creation

TCP server creation

message sending

message receiving

connection management

Configuration Module

Loads runtime configuration from a file.

Example:

device_id=BAT_001
server_ip=127.0.0.1
server_port=8080
interval_sec=3

This allows the device behavior to be modified without recompiling the program.

Logger Module

Writes received telemetry to a log file.

Log format:

timestamp,device_id,temperature,humidity,battery,status

Example log entry:

1773257332,BAT_001,24.90,59.60,100,OK

Build Instructions

Open MSYS2 UCRT64 terminal.

Navigate to the project directory:

cd /c/Users/Maysan/projects/iot-device-simulator

Compile the server:

gcc -Iinclude src/server_main.c src/logger.c src/protocol.c src/socket_utils.c -o server.exe -lws2_32

Compile the device:

gcc -Iinclude src/device_main.c src/config.c src/sensor.c src/protocol.c src/socket_utils.c -o device.exe -lws2_32



Running the System

Open two terminals.

Terminal 1 – Start server

./server.exe

Output:

Server listening on port 8080

Terminal 2 – Start device

./device.exe

Output:

Device connected to server
Sent: device_id=BAT_001;timestamp=...



Server Output

Client connected

Telemetry received
device_id: BAT_001
temperature: 24.90
humidity: 59.60
battery: 100
status: OK

Telemetry will also be written to:

logs/server.log


Features Demonstrated

This project demonstrates:

C systems programming

TCP networking

client-server architecture

modular software design

telemetry protocol implementation

configuration file parsing

logging systems

cross-module architecture

Possible Future Improvements

Potential enhancements include:

multi-client server support

binary telemetry protocol

TLS encrypted communication

device authentication

real-time dashboard

persistent database storage

Linux cross-platform build support

Author

Computer Science student interested in:

embedded systems

networking

systems programming

distributed systems

License

MIT License

