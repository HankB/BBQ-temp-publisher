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

* Cloned and updated README
* Added ADC reading, one channel.

### TODO

* ~Explore ADC usage on the ESP32 using ESP-IDF~
* Calibrate resistance (measured as voltage across the probes) against temperature.
* Produce a function to convert ADC readings to temperature.
* Publish the readings periodically to an MQTT broker.

## Analog interfacing

Time to deal with the real world. With the analog input floating, it read zero.

Had some 9K Ohm pots (some kind of volume knob.) Useful pin diagram at <https://www.mischianti.org/wp-content/uploads/2020/11/ESP32-DOIT-DEV-KIT-v1-pinout-mischianti.png> Connected the "ends" to 3V3 and GND pins and the middle to GPIO36. Before making the last connection I rotated the know tro have minimal voltage on the ADC input. Some devices are 0-1V and others have an internal voltage divider to allow 0-3.3V. It turns out that this one takes 0-3.3V. I monitored voltage using a DVM and vound it read tro within anbout 0.05V which seemed acceptable. There was some noise in the readings ao I modified the code that reads the ADC to read 6 consecutive times and return the average of the result This seems to pr4oduce consistent results. With the DVM reading  1.499V, the application reported the 5 following (6 sample eadh) readings.

```text
I (103675) esp32 main: first   reading: 1548 mV
I (103675) esp32 main: voltage reading: 1548 mV
I (103675) esp32 main: voltage reading: 1549 mV
I (103675) esp32 main: voltage reading: 1548 mV
I (103685) esp32 main: voltage reading: 1548 mV
```

These seem reasonable enough. Next test will be to configure a second channel.



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

