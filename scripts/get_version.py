#!/usr/bin/env python3
import os
import re

ROOT_DIR = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
CONFIG_PATH = os.path.join(ROOT_DIR, "5_Programmation", "XIAO-esp32-v3", "main", "Config.h")

def get_firmware_version() -> str:
    with open(CONFIG_PATH, "r", encoding="utf-8") as f:
        content = f.read()
    match = re.search(r'#define\s+FIRMWARE_VERSION\s+"([^"]+)"', content)
    if not match:
        raise ValueError("FIRMWARE_VERSION not found in Config.h")
    return match.group(1).strip()

if __name__ == "__main__":
    print(get_firmware_version())
