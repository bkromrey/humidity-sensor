# CS 467 - Online Capstone Project (Winter 2026)

Project: Microcontroller Home Humidity Sensor

Team: Brianna Kromrey, John David Lopez, Eugenia Uvarov

## Repository Structure

```bash
.
├── README.md
├── embedded
│   ├── README.md
│   ├── Wiring_Schematic.png
│   └── src
│       ├── CMakeLists.txt
│       ├── main.c
│       ├── hardware/    # buttons, LED array drivers
│       ├── core1/       # core1 entry + logic
│       └── data_flow/   # shared data types
├── web
│   ├── frontend         # React + Vite + TypeScript
│   └── backend          # Node.js + Express (MongoDB later)
└── pico-sdk             # Raspberry Pi Pico SDK
```

## Hardware - Build Pico Code

Build the embedded firmware (Pico W)
Prerequisites

* cmake
* ninja
* ARM toolchain: arm-none-eabi-gcc (and related tools)

### Environment Variables

In order to utilize the networking functionality of this application (transmit
the sensor readings to a web application) you must first set the following
environment variables before building.

The `PICO_MQTT_SERVER` should be a standard IPv4 address belonging to the MQTT
server; no port number is necessary.

`export PICO_MQTT_SERVER="..."
export PICO_MQTT_USER="..."
export PICO_MQTT_PASS="..."
export PICO_WIFI_SSID="..."
export PICO_WIFI_PASS="..."
`

### Build steps

From the repository root:

`mkdir -p build_ninja
cmake -S . -B build_ninja -G Ninja
cmake --build build_ninja`

Linux build instructions - within `embedded` directory:

`cd embedded
mkdir build-local && cd build-local
cmake ../src
make
`

## Web Application - Frontend (React + TypeScript + Vite + TailvindCSS)

UI for the Humidity Sensor project.

### Requirements

* Node.js 20+

### Install

cd web/frontend
npm install

run dev verson:
`npm run dev`

production
`npm run build`

and preview:
`npm run preview`

Lint:
`npm run lint`

Styling:
This project uses Tailwind CSS v4.
Tailwind is enabled via the Vite plugin (@tailwindcss/vite)
Global styles are in src/index.css (@import "tailwindcss";)

## Web Application - Backend (Node + Express + TypeScript)

API server for the Humidity Sensor project.

### Requirements
* Node.js 20+

### Install

cd web/backend
npm install
