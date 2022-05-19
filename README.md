# Controller

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

Außerdem besitz die Uhr 2 unabhängig voneinander setzbare Alarms.

Das setzen und auslesen von Uhrzeiten und Alarms passiert über I2C, welches auf ein Register auf der RTC zugreifft (dokumentation befindet sich im Datenblatt auf Seite 11).

Pins der RTC:
- GND
- VCC (bis zu 5V)
- SDA (I2C)
- SCL (I2C)
- SQW (Signal von Alarm, standart mäßig als Rechtecksignal, kann aber im Register (INTCN) auf Interupt umgestellt werden)
- 32K

<a href="https://datasheets.maximintegrated.com/en/ds/DS3231.pdf">DS3132 Datenblatt</a>
