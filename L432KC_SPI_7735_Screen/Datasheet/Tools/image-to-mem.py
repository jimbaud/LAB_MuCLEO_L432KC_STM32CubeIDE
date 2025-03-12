#!/usr/bin/env python

# vim: set ai et ts=4 sw=4:

from PIL import Image
import sys
import os

if len(sys.argv) < 2:
    print("Usage: {} <image-file>".format(sys.argv[0]))
    sys.exit(1)

fname = sys.argv[1]

img = Image.open(fname)
if img.width > 160 or img.height > 80:
    print("Error: max 160x80 image expected")
    sys.exit(2)

# Créez une nouvelle image blanche de 160x80 pixels
new_img = Image.new("RGB", (160, 80), (255, 255, 255))

# Collez l'image d'origine au centre de la nouvelle image
x_offset = (160 - img.width) // 2
y_offset = (80 - img.height) // 2
new_img.paste(img, (x_offset, y_offset))

print("const uint16_t test_img_160x80[][128] = {")
new_img = new_img.convert("RGB")

for y in range(0, new_img.height):
    s = "{"
    for x in range(0, new_img.width):
        (r, g, b) = new_img.getpixel((x, y))
        color565 = ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | ((b & 0xF8) >> 3)
        # for right endiness, so ST7735_DrawImage would work
        color565 = ((color565 & 0xFF00) >> 8) | ((color565 & 0xFF) << 8)
        s += "0x{:04X},".format(color565)
    s += "},"
    print(s)

print("};")
