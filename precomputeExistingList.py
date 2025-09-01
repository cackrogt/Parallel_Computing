#!/usr/bin/env python3
import sys
import os

def extract_cosmetic_rules(easylist_path, output_header):
    cosmetic_rules = []

    with open(easylist_path, "r", encoding="utf-8", errors="ignore") as f:
        for line in f:
            line = line.strip()
            # Skip empty or comment lines
            if not line or line.startswith("!"):
                continue
            # Cosmetic filter markers
            if "##" in line or "#@#" in line:
                # Escape quotes for C++ string literals
                escaped = line.replace('"', '\\"')
                cosmetic_rules.append(escaped)

    # Write C++ header
    with open(output_header, "w", encoding="utf-8") as out:
        out.write("// Auto-generated from EasyList by easylist_to_cosmetic_cpp.py\n")
        out.write("#pragma once\n\n")
        out.write("#include \"wtf/text/WTFString.h\"\n")
        out.write("#include \"wtf/Vector.h\"\n\n")
        out.write("namespace blink {\n\n")
        out.write("static const Vector<WTF::String> kCosmeticFilters = {\n")
        for rule in cosmetic_rules:
            out.write(f'    WTF::String::FromUTF8("{rule}"),\n')
        out.write("};\n\n")
        out.write("} // namespace blink\n")

    print(f"Extracted {len(cosmetic_rules)} cosmetic rules into {output_header}")


if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Usage: python easylist_to_cosmetic_cpp.py <easylist.txt> <output_header.h>")
        sys.exit(1)

    easylist_path = sys.argv[1]
    output_header = sys.argv[2]

    if not os.path.exists(easylist_path):
        print(f"Error: {easylist_path} not found")
        sys.exit(1)

    extract_cosmetic_rules(easylist_path, output_header)
