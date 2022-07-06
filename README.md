# Controller

## Allgemeines

Die gesamte Hardware dieses Projekts wird über einen ATMEGA 2560 gesteuert.

Die angeschlossenen Messgeräte sind:
- DS3231 (RTC) (Um den Microcontroller zu wecken)
- Ultraschall Sensor (Zur messung der Schneehöhe)
- Schnee-Flächen-Messung
  - 2x Servo Motor (zur Vertikalen und Horizontalen bewegung)
  - Lasermessgerät (zur Schneehöhenmessung)
- Wettermessgerät (zur Messung von Temperatur, Luftfeuchtigkeit und Luftdruck)
- Bluetoothmodul (zur Kommunikation zwischen Microcontroller und der Handy-App)
- SDCardreader (zur Speicherung der gemossenen Daten)

Zubehör:
- Solarzellen
- Bleiakku
- Micro-SDCard

### Eine genauere Beschreibung des Projekts befindet sich im in diesem <a href="https://github.com/SnowHeight/-Controller/blob/master/Snow_Depth_Measurement.pdf">PDF</a>.


## ATMEAG 2560

<img src="https://www.exp-tech.de/media/image/10/64/e3/xARDUINO-Mega-Pinout-Diagram.png.pagespeed.ic.c8O2buxrE8.webp" alt="Image not found">geklaut von <a href="https://www.exp-tech.de/blog/arduino-mega-2560-pinbelegung">dieser Website</a></img>

<a href="https://ww1.microchip.com/downloads/en/devicedoc/atmel-2549-8-bit-avr-microcontroller-atmega640-1280-1281-2560-2561_datasheet.pdf">ATMEGA 2560 Datasheet</a>

## Uart

<img src="https://ae01.alicdn.com/kf/H6643303ef25f469e86cd438eaadb88f0u/3m-5m-silabs-CP210x-CP2102-USB-UART-TTL-3-3-v-Draht-Ende-f-r-PLC.jpg_Q90.jpg_.webp" alt="Image not found" width="500" height="500"></img>
<p>geklaut von <a href="https://de.aliexpress.com/item/32819446112.html">dieser Website</a></p>

## DS3231(RTC)

Die DS3231 ist eine Real Time Clock, die Zeit Speicher und aktuell halten kann.

Die gespeicherte Zeit beinhaltet:
- Sekunden
- Minuten
- Stunden
- Wochentag (1-7)
- Tag (Datum)
- Monat
- Jahr

Außerdem besitzt die Uhr 2 unabhängig voneinander setzbare Alarms.

Das setzen und auslesen von Uhrzeiten und Alarms passiert über I2C, welches auf ein Register auf der RTC zugreifft (dokumentation befindet sich im Datenblatt auf Seite 11).

Pins der RTC:
- GND
- VCC (bis zu 5V)
- SDA (I2C)
- SCL (I2C)
- SQW (Signal von Alarm, standart mäßig als Rechtecksignal, kann aber im Register (INTCN) auf Interupt umgestellt werden)
- 32K

<a href="https://datasheets.maximintegrated.com/en/ds/DS3231.pdf">DS3132 Datenblatt</a>


## BME280 (Wettersensor)

Dieser Sensor wird zur Messung von Temperatur, Druck und Luftfeuchtigkeit verwendet.

Hier werden 2 dieser Sensoren verwendet, wobei einer der zwei, außerhald der Hülle und einer innerhalb misst.

Die Kommunikation mit dem Microcontroller passiert über I2C.

Die hier verwendeten I2C Adressen sind:
- 0x76
- 0x77

<a href="https://www.mouser.com/datasheet/2/783/BST-BME280-DS002-1509607.pdf">BME280 Datenblatt</a>


## MG996R TowardPro (Servomotor 1)

Dieser Servo dient dem Laser zur Einstellung der Horizontalen Winkel.

Die Kommunikation findet über ein PWM-Signal mit dem Mikrocontroller statt.
-Servo1PWMOut	PB5 (Pin 11)

<a href="https://www.electronicoscaldas.com/datasheet/MG996R_Tower-Pro.pdf">MG996R TowardPro Datenblatt</a>


## Modelcraft RS2 MG/BB (Servomotor 2)

Dieser Servo dient dem Laser zur Einstellung der Vertikalen Winkel.

Die Kommunikation findet über ein PWM-Signal mit dem Mikrocontroller statt.
-Servo2PWMOut	PB6 (Pin 12)

<a href="https://www.conrad.com/p/modelcraft-standard-servo-rs2-mgbb-analogue-servo-gear-box-material-metal-connector-system-jr-205111">Modelcraft RS2 MG/BB Datenblatt</a>,
<a href="https://servodatabase.com/servo/modelcraft/rs-2">Modelcraft RS2 MG/BB Datenblatt 2</a>
