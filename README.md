# Alina Vista Boot Firmware

This repository contains the **boot firmware (App0)** for the *Alina Vista* controller.

The boot firmware is responsible for:
- initial device startup
- WiFi provisioning (via captive portal)
- secure OTA updates of the main firmware
- selecting which application firmware to boot

⚠️ **This is NOT the main firmware.**  
⚠️ **This firmware is NOT updated via OTA.**

---

## Role in the Alina Vista system

The Alina Vista firmware architecture is split into two parts:

- **Boot firmware (this repo)**  
  - flashed via USB
  - stored in a dedicated application partition
  - never overwritten by OTA
- **Application firmware ("Luna")**  
  - installed and updated via OTA
  - contains the actual device logic

The boot firmware always runs first and decides whether to:
- start the main firmware
- enter boot mode (UI + WiFi provisioning + OTA)

---

## Features

- Boot mode selection via NVS flag
- WiFi provisioning using **AP + captive portal**
- Embedded configuration web page (no filesystem required)
- OTA update of the application firmware
- HTTPS support with root CA pinning
- Version compatibility check (`min_boot`)
- Display-based UI for local interaction

---

## What this firmware does NOT do

- It does **not** contain device logic
- It does **not** control hardware peripherals
- It does **not** update itself via OTA
- It is **not** intended for end users


---

## Build environment

- PlatformIO
- ESP32 / ESP32-S3
- Custom partition tables (shared with the application firmware)

⚠️ **Boot and application firmware must use compatible partition tables.**

---

## Relationship with the application firmware

The boot firmware installs and boots the main firmware based on metadata provided by a remote manifest.

Each application release declares:
- target flash size
- minimum required boot version
- SHA256 checksum

---

## Project status

This project is currently **under active development**.

- APIs and internal structure may change
- Not considered stable
- Intended for development and experimentation

---

## License

This project is part of the Alina Vista ecosystem.  
See the LICENSE file for details.


