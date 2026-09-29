#!/usr/bin/env python3
import gzip
import os

ROOT_DIR = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
HTML_FILE = os.path.join(ROOT_DIR, "5_Programmation", "XIAO-esp32-v3", "main", "dashboard.html")
HEADER_FILE = os.path.join(ROOT_DIR, "5_Programmation", "XIAO-esp32-v3", "main", "DashboardHtml.h")

def build_header():
    with open(HTML_FILE, "r", encoding="utf-8") as f:
        html_content = f.read()

    raw_bytes = html_content.encode("utf-8")
    gz_data = gzip.compress(raw_bytes, compresslevel=9, mtime=0)

    header_lines = [
        "#ifndef DASHBOARDHTML_H",
        "#define DASHBOARDHTML_H",
        "",
        "#include <Arduino.h>",
        "",
        "// Dashboard Web Light Trainer - HTML5/CSS3/JS pré-compressé GZIP",
        f"// Taille originale: {len(raw_bytes)} octets | Taille GZIP: {len(gz_data)} octets ({(len(gz_data)/len(raw_bytes))*100:.1f}%)",
        f"const size_t DASHBOARD_HTML_GZ_LEN = {len(gz_data)};",
        "const uint8_t DASHBOARD_HTML_GZ[] PROGMEM = {"
    ]

    for i in range(0, len(gz_data), 16):
        chunk = gz_data[i:i+16]
        hex_str = ", ".join(f"0x{b:02X}" for b in chunk)
        if i + 16 < len(gz_data):
            hex_str += ","
        header_lines.append(f"  {hex_str}")

    header_lines.append("};")
    header_lines.append("")
    header_lines.append("#endif // DASHBOARDHTML_H")
    header_lines.append("")

    with open(HEADER_FILE, "w", encoding="utf-8") as f:
        f.write("\n".join(header_lines))

    print(f"Generated {HEADER_FILE}: {len(gz_data)} bytes GZIP from {len(raw_bytes)} bytes HTML.")

if __name__ == "__main__":
    build_header()
