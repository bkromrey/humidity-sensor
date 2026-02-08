# CS 467 - Online Capstone Project (Winter 2026)

Project: Microcontroller Home Humidity Sensor
Team: Brianna Kromrey, John David Lopez, Eugenia Uvarov


## structure
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
## build pico code 
Build the embedded firmware (Pico W)
Prerequisites

* cmake
* ninja 
* ARM toolchain: arm-none-eabi-gcc (and related tools)


Build steps

From the repository root:

`mkdir -p build_ninja
cmake -S . -B build_ninja -G Ninja
cmake --build build_ninja`

## Frontend (React + TypeScript + Vite + TailvindCSS)

UI for the Humidity Sensor project.

### Requirements
- Node.js 20+

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

## Backend (Node + Express + TypeScript)

API server for the Humidity Sensor project.

### Requirements
- Node.js 20+

### Install
cd web/backend
npm install
