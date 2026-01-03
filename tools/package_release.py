#!/usr/bin/env python3
from __future__ import annotations

import hashlib
import os
import shutil
import sys
import zipfile
from pathlib import Path

ENVS = [
    ("boot-4mb",  "4mb"),
    ("boot-8mb",  "8mb"),
    ("boot-16mb", "16mb"),
]

OFFSETS = {
    "bootloader": "0x0000",
    "partitions": "0x8000",
    "boot_app0":  "0xE000",
    "app":        "0x10000",
}

def die(msg: str, code: int = 1) -> None:
    print(f"[ERROR] {msg}", file=sys.stderr)
    sys.exit(code)

def sha256_file(p: Path) -> str:
    h = hashlib.sha256()
    with p.open("rb") as f:
        for chunk in iter(lambda: f.read(1024 * 1024), b""):
            h.update(chunk)
    return h.hexdigest()

def find_boot_app0_bin() -> Path:
    # Tipico path su macOS/Linux con PlatformIO:
    candidates = [
        Path.home() / ".platformio/packages/framework-arduinoespressif32/tools/partitions/boot_app0.bin",
        Path.home() / ".platformio/packages/framework-arduinoespressif32/tools/partitions/boot_app0.bin",
        # fallback: cerca dentro alla cartella .platformio/packages
        Path.home() / ".platformio/packages",
    ]

    for c in candidates:
        if c.is_file():
            return c

    # ricerca più ampia se non trovato nei path "diretti"
    base = Path.home() / ".platformio/packages"
    if base.exists():
        hits = list(base.rglob("boot_app0.bin"))
        if hits:
            # prendi il più “probabile”
            return hits[0]

    die("Impossibile trovare boot_app0.bin. Controlla che PlatformIO abbia installato framework-arduinoespressif32.")
    raise RuntimeError("unreachable")

def write_text(p: Path, content: str) -> None:
    p.parent.mkdir(parents=True, exist_ok=True)
    p.write_text(content, encoding="utf-8")

def make_scripts(pkg_dir: Path) -> None:
    # Script mac/linux
    install_sh = f"""#!/usr/bin/env bash
set -euo pipefail

PORT="${{1:-}}"
if [[ -z "$PORT" ]]; then
  echo "Usage: ./install_mac_linux.sh <PORTA_SERIAL>"
  echo "Example: ./install_mac_linux.sh /dev/cu.usbmodemXXXX"
  exit 1
fi

# con soft-latch meglio non alzare troppo subito
BAUD="${{BAUD:-230400}}"

ROOT_DIR="$(cd "$(dirname "${{BASH_SOURCE[0]}}")" && pwd)"
BIN_DIR="${{ROOT_DIR}}/bin"

BOOTLOADER="${{BIN_DIR}}/bootloader.bin"
PARTITIONS="${{BIN_DIR}}/partitions.bin"
BOOTAPP0="${{BIN_DIR}}/boot_app0.bin"
APPBIN="${{BIN_DIR}}/alina-boot-ui.bin"

for f in "$BOOTLOADER" "$PARTITIONS" "$BOOTAPP0" "$APPBIN"; do
  [[ -f "$f" ]] || {{ echo "Missing file: $f"; exit 1; }}
done

echo "== Alina Vista Boot | INSTALL (ERASE + FLASH) =="
echo "Port: $PORT"
echo "Baud:  $BAUD"
echo

esptool --chip esp32s3 --port "$PORT" --baud "$BAUD" erase-flash

esptool --chip esp32s3 --port "$PORT" --baud "$BAUD" write-flash -z \\
  {OFFSETS["bootloader"]}  "$BOOTLOADER" \\
  {OFFSETS["partitions"]}  "$PARTITIONS" \\
  {OFFSETS["boot_app0"]}   "$BOOTAPP0" \\
  {OFFSETS["app"]}         "$APPBIN"

echo
echo "✅ Installation complete."
"""

    update_sh = f"""#!/usr/bin/env bash
set -euo pipefail

PORT="${{1:-}}"
if [[ -z "$PORT" ]]; then
  echo "Usage: ./update_boot_mac_linux.sh <PORTA_SERIAL>"
  echo "Example: ./update_boot_mac_linux.sh /dev/cu.usbmodemXXXX"
  exit 1
fi

BAUD="${{BAUD:-230400}}"
ROOT_DIR="$(cd "$(dirname "${{BASH_SOURCE[0]}}")" && pwd)"
APPBIN="${{ROOT_DIR}}/bin/alina-boot-ui.bin"

[[ -f "$APPBIN" ]] || {{ echo "Missing file: $APPBIN"; exit 1; }}

echo "== Alina Vista Boot | UPDATE BOOT UI (NO ERASE) =="
echo "Port: $PORT"
echo "Baud:  $BAUD"
echo

esptool --chip esp32s3 --port "$PORT" --baud "$BAUD" write-flash -z \\
  {OFFSETS["app"]} "$APPBIN"

echo
echo "✅ Boot update complete."
"""

    # Script Windows
    install_bat = f"""@echo off
setlocal enabledelayedexpansion

set PORT=%1
if "%PORT%"=="" (
  echo Usage: install_windows.bat COM5
  exit /b 1
)

set BAUD=230400
set ROOT=%~dp0
set BIN=%ROOT%bin

set BOOTLOADER=%BIN%\\bootloader.bin
set PARTITIONS=%BIN%\\partitions.bin
set BOOTAPP0=%BIN%\\boot_app0.bin
set APPBIN=%BIN%\\alina-boot-ui.bin

if not exist "%BOOTLOADER%" ( echo Missing file: %BOOTLOADER% & exit /b 1 )
if not exist "%PARTITIONS%" ( echo Missing file: %PARTITIONS% & exit /b 1 )
if not exist "%BOOTAPP0%" ( echo Missing file: %BOOTAPP0% & exit /b 1 )
if not exist "%APPBIN%" ( echo Missing file: %APPBIN% & exit /b 1 )

echo == Alina Vista Boot ^| INSTALL (ERASE + FLASH) ==
echo Port: %PORT%
echo Baud:  %BAUD%
echo.

esptool --chip esp32s3 --port %PORT% --baud %BAUD% erase-flash
if errorlevel 1 exit /b 1

esptool --chip esp32s3 --port %PORT% --baud %BAUD% write-flash -z ^
  {OFFSETS["bootloader"]} "%BOOTLOADER%" ^
  {OFFSETS["partitions"]} "%PARTITIONS%" ^
  {OFFSETS["boot_app0"]} "%BOOTAPP0%" ^
  {OFFSETS["app"]} "%APPBIN%"

if errorlevel 1 exit /b 1

echo.
echo ✅ Installation complete.
"""

    update_bat = f"""@echo off
setlocal

set PORT=%1
if "%PORT%"=="" (
  echo Usage: update_boot_windows.bat COM5
  exit /b 1
)

set BAUD=230400
set ROOT=%~dp0
set APPBIN=%ROOT%bin\\alina-boot-ui.bin

if not exist "%APPBIN%" ( echo Missing file: %APPBIN% & exit /b 1 )

echo == Alina Vista Boot ^| UPDATE BOOT UI (NO ERASE) ==
echo Port: %PORT%
echo Baud:  %BAUD%
echo.

esptool --chip esp32s3 --port %PORT% --baud %BAUD% write-flash -z {OFFSETS["app"]} "%APPBIN%"

if errorlevel 1 exit /b 1

echo.
echo ✅ Boot update complete.
"""

    readme = f"""Alina Vista Boot - Flash Packages

Offsets (match PlatformIO upload):
- bootloader  @ {OFFSETS["bootloader"]}
- partitions  @ {OFFSETS["partitions"]}
- boot_app0   @ {OFFSETS["boot_app0"]}
- app (boot)  @ {OFFSETS["app"]}

macOS/Linux:
  chmod +x install_mac_linux.sh update_boot_mac_linux.sh
  ./install_mac_linux.sh /dev/cu.usbmodemXXXX
  ./update_boot_mac_linux.sh  /dev/cu.usbmodemXXXX

Windows:
  install_windows.bat COM5
  update_boot_windows.bat COM5

Note (soft-latch):
Keep the power-on pressed during flashing/reset transitions if your board cuts power on reset.
"""

    write_text(pkg_dir / "install_mac_linux.sh", install_sh)
    write_text(pkg_dir / "update_boot_mac_linux.sh", update_sh)
    write_text(pkg_dir / "install_windows.bat", install_bat)
    write_text(pkg_dir / "update_boot_windows.bat", update_bat)
    write_text(pkg_dir / "README.txt", readme)

    # rende eseguibili gli .sh nello staging
    for sh in ["install_mac_linux.sh", "update_boot_mac_linux.sh"]:
        p = pkg_dir / sh
        p.chmod(0o755)

def zip_dir(src_dir: Path, zip_path: Path) -> None:
    with zipfile.ZipFile(zip_path, "w", compression=zipfile.ZIP_DEFLATED) as z:
        for p in src_dir.rglob("*"):
            if p.is_file():
                z.write(p, p.relative_to(src_dir))

def main() -> None:
    if len(sys.argv) < 2:
        die("Uso: python3 tools/package_release.py <VERSIONE>  (es: 0.1.0)")

    version = sys.argv[1].lstrip("v")
    root = Path(__file__).resolve().parents[1]
    build_root = root / ".pio" / "build"
    dist_root = root / "dist" / f"v{version}"
    dist_root.mkdir(parents=True, exist_ok=True)

    boot_app0 = find_boot_app0_bin()
    print(f"[INFO] boot_app0.bin: {boot_app0}")

    for env_name, label in ENVS:
        env_dir = build_root / env_name
        if not env_dir.exists():
            print(f"[WARN] Skip {env_name}: build dir not found: {env_dir}")
            continue

        bootloader = env_dir / "bootloader.bin"
        partitions = env_dir / "partitions.bin"
        firmware   = env_dir / "firmware.bin"

        for f in [bootloader, partitions, firmware]:
            if not f.is_file():
                die(f"Manca {f}. Fai prima: pio run -e {env_name}")

        pkg_name = f"alina-vista-boot-{version}-{label}"
        pkg_dir = dist_root / pkg_name
        if pkg_dir.exists():
            shutil.rmtree(pkg_dir)
        (pkg_dir / "bin").mkdir(parents=True, exist_ok=True)

        # Copie in bin/
        shutil.copy2(bootloader, pkg_dir / "bin" / "bootloader.bin")
        shutil.copy2(partitions, pkg_dir / "bin" / "partitions.bin")
        shutil.copy2(boot_app0,  pkg_dir / "bin" / "boot_app0.bin")
        shutil.copy2(firmware,   pkg_dir / "bin" / "alina-boot-ui.bin")

        # SHA256 utile (se vuoi pubblicarla nel manifest o nelle release notes)
        app_sha = sha256_file(pkg_dir / "bin" / "alina-boot-ui.bin")
        write_text(pkg_dir / "bin" / "alina-boot-ui.sha256.txt", f"{app_sha}  alina-boot-ui.bin\n")

        # Script + README
        make_scripts(pkg_dir)

        # Zip finale
        zip_path = dist_root / f"{pkg_name}.zip"
        if zip_path.exists():
            zip_path.unlink()
        zip_dir(pkg_dir, zip_path)

        print(f"[OK] Built: {zip_path.name}")
        print(f"     app sha256: {app_sha}")

if __name__ == "__main__":
    main()
