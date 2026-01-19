# Stopwatch ESP-IDF Project

This project implements a stopwatch using ESP-IDF on an ESP32 microcontroller with an OLED display.
## Hardware Architecture
The ESP32-S3 GPIO pins have been assigned as follows: 

GPIO12 ------> SCL <br>
GPIO11 ------> SDA <br> 
GPIO10 ------> CS <br>
GPIO9  ------> DC<br>
GPIO8  ------> RES<br>

GPIO5  ------> left button: Start/Stop <br>
GPIOP6 ------> middle button: lapping/page navigation <br>
GPIO7  ------> right button: reset<br>

You can check the details in the schematic.

### How to order prototypes in JLCPCB
- Go to jlcpcb.com
- Click "Order now"
- Click "Add Gerber file" and upload /Hardware/Gerber_xxxxxxx.zip file
- Select the PCB Qty
- Enable the PCB Assembly
- Choose the Standard and Top Side assembly
- Click the NEXT button in the right panel. You will go to the next page
- Add the BOM and CPL(Pick-and-Place) files from hardware directory
- You can check the component stocks in the list
- In the Component Placements, you can check the assembled board.
- You can check the price in the Quote & Order page and save to cart

## Firmware Development

### Prerequisites

- [Visual Studio Code](https://code.visualstudio.com/)
- ESP32 development board (e.g., ESP32-S3)
- USB cable for flashing

### Installing ESP-IDF in VS Code

Follow these steps to set up the ESP-IDF development environment in Visual Studio Code. [Refer to this link.](https://www.youtube.com/watch?v=D0fRc4XHBNk) 

### 1. Install Visual Studio Code
If you haven't already, download and install VS Code from the official website.

### 2. Install the ESP-IDF Extension
1. Open VS Code.
2. Go to the Extensions view by clicking on the Extensions icon in the Activity Bar on the side of the window (or press `Ctrl+Shift+X`).
3. Search for "ESP-IDF".
4. Find the extension published by "Espressif Systems" and click "Install".

### 3. Open the Project
1. Open VS Code.
2. Select "File" > "Open Folder" and navigate to the `stopwatch-espidf/Firmware` directory.

### 4. Build and Flash the Project
1. In VS Code, open the Command Palette (`Ctrl+Shift+P`).
2. Type "ESP-IDF: Open ESP-IDF Terminal" and click it
3. Run "idf.py build".
4. Connect your ESP32 board via USB.
5. Run "idf.py flash monitor" to flash the firmware.
6. Optionally, run "idf.py -p COM[X] flash monitor" if the COM port is not automatically selected

## Project Structure

- `Firmware/`: Contains the ESP-IDF project files.
- `Firmware/main/`: Source code for the stopwatch application.
- `Firmware/components/`: Additional components like u8g2 for OLED display.
- `images/`: Project images like testing board diagram
