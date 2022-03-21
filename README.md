# BBQ-temp-publisher

Publish BBQ temperature readings via MQTT using an ESP32.

* Use the temperature probes from a Maverick ET732 Wireless Thermometer.
* Connect the probes to an ESP32 (DOIT ESP32 DEVKIT V1 Board in hand) Which can read more than one Analog input.
* Publish the readings using MQTT in JSONN format and usintg conventions suitable for my HomeAssistant setup.
* Use ESP-IDF framework and the PlatformIO plugin in VS Code (on Linux)

This project is derived from <https://github.com/HankB/ESP32-ESP-IDF-start> which provides working WiFi, MQTT and SNTP.

## Motivation

Provide a running log of temperature readings from a smoker/cooker automatically. The Maverick thermometer works reasonably well but I have to manually transcribe readings. An option was to use another radio to receive the transmissions, but it looked like this would be easier. 

## Status

Cloned and updating README

### TODO

* Explore ADC usage on the ESP32 using ESP-IDF
* Calibrate resistance (measured as voltage across the probes) against temperature.
* Produce a function to convert ADC readings to temperature.
* Publish the readings periodically to an MQTT broker.

## Testing

...

## Process

* Clone <https://github.com/HankB/ESP32-ESP-IDF-start>
* Add ADC processing.
* Publish raw ADC readings for the purpose of calibaration
* Develop the reading -> temperature function.
* Publish temperatures.

## Requirements

* Full PlatformIO and tool chain installation, and using VS Code.
* User provided `include/secrets.h` that has definitions for

```text
#define SSID "your_SSID"
#define  PWD "your_password"
#define sntp_server "your_ntp_server" 
```

## Errata

The boot_count in `main.c` is not counting up. 

