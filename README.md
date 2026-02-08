# CS 467 - Online Capstone Project (Winter 2026)

Project: Microcontroller Home Humidity Sensor
Team: Brianna Kromrey, John David Lopez, Eugenia Uvarov


## structure
.
├── README.md
├── embedded
│   ├── README.md
│   ├── Wiring_Schematic.png
│   └── src
│       ├── CMakeLists.txt
│       ├── main.c
│       ├── hardware/        # buttons, led array drivers
│       ├── core1/           # core1 entry + logic
│       └── data_flow/       # shared data types
├───── web
│       ├── frontend             # React/Vite/TypeScript (TBD)
│       └── backend              # Node.js + MongoDB (TBD)
└───── pico-sdk                 # pico sdk location   

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
