# CS 467 - Online Capstone Project (Winter 2026)

Project: Microcontroller Home Humidity Sensor
Team: Brianna Kromrey, John David Lopez, Eugenia Uvarov

### build for WSL 2 using ninja ( from root folder of project)
```
rm -rf ninja_build
mkdir ninja_build
cd ninja_build
cmake ../src -G Ninja -DPICO_BOARD=pico
cmake --build . -j
```
