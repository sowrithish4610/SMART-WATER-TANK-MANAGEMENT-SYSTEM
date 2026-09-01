# Smart Water Tank Management System

An embedded firmware project for an **NXP LPC21xx (ARM7TDMI)** microcontroller that monitors and manages an overhead water tank in real time. It samples multiple sensors (water level, temperature, ultrasonic distance), reads the wall-clock time from a DS1307 RTC, logs readings to an I²C EEPROM, drives a water pump via a servo, and streams all telemetry over UART.

---

## ✨ Features

- **Multi-sensor acquisition** every cycle:
  - Analog water-level sensor on **AD0.0** (P0.27) — mapped to a 0–100 % reading
  - LM35-style temperature sensor on **AD0.2** (P0.29) — reported in °C
  - HC-SR04 ultrasonic distance sensor on **P0.12 (TRIG) / P0.13 (ECHO)**
- **Real-time clock** read from DS1307 over I²C0 — date and time printed every cycle
- **Persistent telemetry** in I²C EEPROM (24Cxx-style, address 0xA0) — water %, distance cm, temperature °C
- **Automatic pump control** via PWM2 (P0.7) driving a servo:
  - Pump ON when water < 10 %
  - Pump OFF when water > 90 %
  - Hysteresis dead-band in between
- **Manual override** through three push-buttons (active-low on P0.14 / P0.15 / P0.16) with edge-detected debouncing:
  - **SW1** — enter Manual mode
  - **SW2** — return to Auto mode
  - **SW3** — toggle the pump in Manual mode
- **UART0 telemetry** at 9600 baud for live monitoring from a PC terminal

---

## 🧰 Hardware

| Peripheral | Pin / Address | Purpose |
|---|---|---|
| UART0 | P0.0 (TX) / P0.1 (RX) | Debug terminal @ 9600 bps |
| I²C0 | P0.2 (SCL) / P0.3 (SDA) | RTC + EEPROM bus |
| ADC0 | AD0.0, AD0.2 | Level + temperature |
| PWM2 | P0.7 | Servo / pump control |
| GPIO | P0.12, P0.13 | Ultrasonic TRIG / ECHO |
| GPIO | P0.14, P0.15, P0.16 | Mode / manual switches |
| RTC | DS1307 @ 0xD0 | Wall-clock |
| EEPROM | 24Cxx @ 0xA0 | Telemetry storage |

PCLK is assumed to be **15 MHz** (`VPBDIV = 0x00`), which all bit-rate and delay timing is derived from.

---

## 🗂 Project layout

```
.
├── main.c            # Application loop
├── p_header.h        # Public prototypes
├── lpc21xx.h         # MCU register definitions (vendor header)
├── adc_dri.c         # ADC driver
├── delay.c           # delay_ms (Timer0) + delay_us (calibrated loop)
├── i2c_dri.c         # Bit-banged I²C0 master (polled, with STOP helper)
├── sensor_cali.c     # Sensor calibration: level %, temp, ultrasonic
├── servo_dri_pro.c   # PWM2 init + system_pump_control (auto / manual FSM)
└── uart_dri.c        # UART0 + integer / BCD-time formatters
```

---

## 🏗 Build

This project targets bare-metal ARM7. Any of the following work:

- **Keil µVision** — create a new LPC21xx project, add the `.c` files, supply the NXP `lpc21xx.h` vendor header, set the IROM/IRAM sizes for your device (LPC2148: 512 kB / 32 kB is typical).
- **arm-none-eabi-gcc** — compile each `.c` with `-mcpu=arm7tdmi -mthumb`, link with a startup file and a linker script for your LPC21xx part.
- **Local syntax / link check (host)** — supply a host-side `lpc21xx.h` (a minimal stub mapping the symbols used in this firmware to volatile pointers works) and run:

```
$ gcc -ffreestanding -fno-builtin -Wall -Wextra \
      -I. -include lpc21xx.h -c *.c
```

---

## ⚙ Configuration

- **RTC** is initialised on first boot with a hard-coded date/time (`main.c`). Modify the `i2c_write(0xD0, ...)` block at the top of `main()` to suit your time zone / build date.
- **Water thresholds** (`< 10 %` ON, `> 90 %` OFF) live in `servo_dri_pro.c::system_pump_control`.
- **Level calibration curve** (raw ADC → %) lives in `sensor_cali.c::smart_tank_water_level_percent`. Re-calibrate by replacing the `120` (empty) and `900` (full) thresholds to match your sensor at your reference voltages.

---

## 🧪 UART output example

```
--- SMART WATER TANK SYSTEM ---
Water Level: 42
Distance    : 117 cm
Temperature: 27 C
Date: 18/08/2026
Current Time: 16:05:00
Mode: AUTO   | Pump: ON  (Filling)
[ EEPROM READBACK ]
Saved Water : 42 %
Saved Dist  : 117 cm
Saved Temp  : 27 C
----------------------
```

---

## 📜 License

MIT — see `LICENSE`.

---

## 🙋 Author

**sowrithish4610** — [github.com/sowrithish4610](https://github.com/sowrithish4610)