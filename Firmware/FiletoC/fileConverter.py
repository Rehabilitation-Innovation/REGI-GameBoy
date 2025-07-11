# # # import tkinter as tk
# # # from tkinter import filedialog
# # # from PIL import Image
# # # import numpy as np
# # # from colorthief import ColorThief
# # # import webcolors
# # # image_colors = []
# # # #C:/Users/abina/Desktop/FiletoC/aurora-1x.png
# # # #C:/Users/abina/Desktop/FiletoC/ezgif-split/cactus000.png
# # # if __name__ == '__main__':
# # #     # root = tk.Tk()
# # #     # root.withdraw()

# # #     # pal_file_path = 'C:/Users/abina/Desktop/FiletoC/aurora-1x.png'#filedialog.askopenfilename()
# # #     # if pal_file_path == '':
# # #     #     exit()
# # #     # print(pal_file_path)
# # #     # palI = Image.open(pal_file_path)
# # #     # # width, height = palI.size()
# # #     # # print(width)
# # #     # # print(height)
# # #     # pal = np.asarray(palI)
# # #     # print(pal.ravel())
# # #     # print(len(pal.ravel()))
# # #     # colors = palI.convert('RGB').getcolors(maxcolors=255)
# # #     # print(colors)
# # #     # for c in colors:
# # #     #     print(webcolors.rgb_to_hex(c))
# # #     # color_thief = ColorThief(pal_file_path)
# # #     # color_palette = color_thief.get_palette(color_count=255, quality=5)
# # #     # for color in color_palette:
# # #     #     print(webcolors.rgb_to_hex(color))
    
# # #     # # palI.show()
# # #     # # image_file_path = 'C:/Users/abina/Desktop/FiletoC/ezgif-split/cactus000.png' #filedialog.askopenfilename()
# # #     # # image_to_convert = Image.open(image_file_path)
# # #     # # print(image_file_path)
# # #     # # p_img = Image.new('P', (16,16)) # 256 color pallette.
# # #     # # p_img.putpalette(pal)

# # #     # # conv = image_to_convert.quantize(palette=p_img, dither=0)
# # #     # # conv.show()
# # #     root = tk.Tk()
# # #     root.withdraw()

# # #     # Allow file selection
# # #     pal_file_path = 'C:/Users/abina/Desktop/FiletoC/aurora-1x.png'  # Or use filedialog.askopenfilename()

# # #     if pal_file_path == '':
# # #         exit()

# # #     print("Selected file:", pal_file_path)

# # #     # Load image
# # #     palI = Image.open(pal_file_path)
# # #     pal = np.asarray(palI)

# # #     print("Flattened pixel data:")
# # #     print(pal.ravel())
# # #     print("Total number of values:", len(pal.ravel()))

# # #     #GPT generated code. Literally the only coding its good for is python.

# # #     # Get unique colors (RGB)
# # #     try:
# # #         colors = palI.convert('RGB').getcolors(maxcolors=256*256)  # Increase to avoid None
# # #         if colors is None:
# # #             print("Too many unique colors to process using getcolors().")
# # #         else:
# # #             print("\nExtracted RGB colors (via PIL):")
# # #             for count, rgb in colors:
# # #                 print(f"Count: {count}, RGB: {rgb}, HEX: {webcolors.rgb_to_hex(rgb)}")
# # #     except Exception as e:
# # #         print("Error extracting colors:", e)

# # #     # Extract dominant palette using ColorThief
# # #     try:
# # #         color_thief = ColorThief(pal_file_path)
# # #         color_palette = color_thief.get_palette(color_count=10, quality=5)
# # #         print("\nColorThief Palette:")
# # #         for color in color_palette:
# # #             print(f"RGB: {color}, HEX: {webcolors.rgb_to_hex(color)}")
# # #     except Exception as e:
# # #         print("ColorThief error:", e)

# # #     # Optionally display image
# # #     palI.show()

# # import tkinter as tk
# # from tkinter import filedialog
# # from PIL import Image
# # import os

# # def flatten_palette_to_bytes(rgb_list):
# #     """Flatten list of (R,G,B) into a flat list of bytes for putpalette (length must be 768)"""
# #     flat = []
# #     for r, g, b in rgb_list:
# #         flat.extend([r, g, b])
# #     while len(flat) < 768:
# #         flat.extend([0, 0, 0])  # Pad with black if fewer than 256 colors
# #     return flat[:768]

# # if __name__ == '__main__':
# #     root = tk.Tk()
# #     root.withdraw()

# #     # --- Step 1: Select image to extract palette from ---
# #     print("Select an image to extract a 256-color palette from...")
# #     palette_path = filedialog.askopenfilename(title="Select Palette Source Image")
# #     if not palette_path:
# #         exit()

# #     palette_image = Image.open(palette_path).convert('RGB')
# #     palette_pixels = list(palette_image.getdata())

# #     seen = set()
# #     ordered_palette = []
# #     for rgb in palette_pixels:
# #         if rgb not in seen:
# #             seen.add(rgb)
# #             ordered_palette.append(rgb)
# #         if len(ordered_palette) == 256:
# #             break

# #     if len(ordered_palette) == 0:
# #         raise ValueError("No colors found.")

# #     print(f"✅ Extracted {len(ordered_palette)} unique RGB colors (no alpha).")

# #     # --- Step 2: Create P-mode image with this palette ---
# #     pal_img = Image.new('P', (16, 16))  # dummy size
# #     pal_img.putpalette(flatten_palette_to_bytes(ordered_palette))

# #     # --- Step 3: Select image to convert ---
# #     print("Select an image to convert to indexed PNG...")
# #     target_path = filedialog.askopenfilename(title="Select Target Image")
# #     if not target_path:
# #         exit()

# #     target_img = Image.open(target_path).convert('RGB')  # discard alpha

# #     # --- Step 4: Quantize using extracted palette (no alpha) ---
# #     indexed_img = target_img.quantize(palette=pal_img, dither=0)

# #     # --- Step 5: Save indexed image ---
# #     output_path = os.path.join(os.path.dirname(target_path), "indexed_output.png")
# #     indexed_img.save(output_path, format='PNG')

# #     print(f"\n✅ Indexed (no alpha) image saved to: {output_path}")
# #     indexed_img.show()

# import tkinter as tk
# from tkinter import filedialog
# from PIL import Image
# import os

# def flatten_palette_to_bytes(rgb_list):
#     flat = []
#     for r, g, b in rgb_list:
#         flat.extend([r, g, b])
#     while len(flat) < 768:
#         flat.extend([0, 0, 0])
#     return flat[:768]

# def rgb_to_uint32(rgb):
#     r, g, b = rgb
#     return (r << 16) | (g << 8) | b

# def save_palette_header(path, palette):
#     lines = []
#     lines.append("#ifndef IMAGE_PALETTE_H")
#     lines.append("#define IMAGE_PALETTE_H\n")
#     lines.append("#include <stdint.h>")
#     lines.append(f"#define IMAGE_PALETTE_SIZE {len(palette)}")
#     lines.append(f"const uint32_t image_palette[{len(palette)}] = {{")
#     for i, c in enumerate(palette):
#         comma = "," if i < len(palette) - 1 else ""
#         lines.append(f"    0x{c:06X}{comma}")
#     lines.append("};\n#endif // IMAGE_PALETTE_H\n")
#     with open(path, 'w') as f:
#         f.write("\n".join(lines))
#     print(f"✅ Palette header saved to: {path}")

# def save_pixels_header(path, pixels, width, height):
#     lines = []
#     lines.append("#ifndef IMAGE_PIXELS_H")
#     lines.append("#define IMAGE_PIXELS_H\n")
#     lines.append("#include <stdint.h>")
#     lines.append(f"#define IMAGE_WIDTH {width}")
#     lines.append(f"#define IMAGE_HEIGHT {height}")
#     lines.append(f"const uint8_t image_pixels[{width * height}] = {{")
    
#     # Format pixels with 16 per line for readability
#     per_line = 16
#     for i in range(0, len(pixels), per_line):
#         chunk = pixels[i:i+per_line]
#         line = ", ".join(str(p) for p in chunk)
#         comma = "," if i + per_line < len(pixels) else ""
#         lines.append(f"    {line}{comma}")
#     lines.append("};\n#endif // IMAGE_PIXELS_H\n")
#     with open(path, 'w') as f:
#         f.write("\n".join(lines))
#     print(f"✅ Pixels header saved to: {path}")

# if __name__ == '__main__':
#     root = tk.Tk()
#     root.withdraw()

#     # Step 1: Select palette source image
#     print("Select an image to extract a 256-color palette from...")
#     palette_path = filedialog.askopenfilename(title="Select Palette Source Image")
#     if not palette_path:
#         exit()

#     palette_image = Image.open(palette_path).convert('RGB')
#     palette_pixels = list(palette_image.getdata())

#     seen = set()
#     ordered_palette = []
#     for rgb in palette_pixels:
#         if rgb not in seen:
#             seen.add(rgb)
#             ordered_palette.append(rgb)
#         if len(ordered_palette) == 256:
#             break

#     if len(ordered_palette) == 0:
#         raise ValueError("No colors found.")

#     print(f"✅ Extracted {len(ordered_palette)} unique RGB colors (no alpha).")

#     # Create P-mode image palette
#     pal_img = Image.new('P', (16, 16))
#     pal_img.putpalette(flatten_palette_to_bytes(ordered_palette))

#     # Step 2: Select target image
#     print("Select an image to convert to indexed PNG...")
#     target_path = filedialog.askopenfilename(title="Select Target Image")
#     if not target_path:
#         exit()

#     target_img = Image.open(target_path).convert('RGB')

#     # Quantize using palette
#     indexed_img = target_img.quantize(palette=pal_img, dither=0)

#     # Save indexed PNG
#     output_dir = os.path.dirname(target_path)
#     indexed_png_path = os.path.join(output_dir, "indexed_output.png")
#     indexed_img.save(indexed_png_path, format='PNG')
#     print(f"\n✅ Indexed PNG saved to: {indexed_png_path}")

#     # --- Save palette as .h ---
#     palette_uint32 = [rgb_to_uint32(c) for c in ordered_palette]
#     palette_h_path = os.path.join(output_dir, "image_palette.h")
#     save_palette_header(palette_h_path, palette_uint32)

#     # --- Save indexed pixels as .h ---
#     pixels = list(indexed_img.getdata())
#     width, height = indexed_img.size
#     pixels_h_path = os.path.join(output_dir, "image_pixels.h")
#     save_pixels_header(pixels_h_path, pixels, width, height)

#     # Show indexed image
#     indexed_img.show()

import tkinter as tk
from tkinter import filedialog
from PIL import Image
import os

def save_pixels_header(path, pixels, width, height):
    lines = []
    lines.append("#ifndef IMAGE_PIXELS_H")
    lines.append("#define IMAGE_PIXELS_H\n")
    lines.append("#include <stdint.h>")
    lines.append(f"#define IMAGE_WIDTH {width}")
    lines.append(f"#define IMAGE_HEIGHT {height}")
    lines.append(f"const uint8_t image_pixels[{width * height}] = {{")
    
    per_line = 16
    for i in range(0, len(pixels), per_line):
        chunk = pixels[i:i+per_line]
        line = ", ".join(f"0x{p:02X}" for p in chunk)
        comma = "," if i + per_line < len(pixels) else ""
        lines.append(f"    {line}{comma}")
    lines.append("};\n#endif // IMAGE_PIXELS_H\n")
    with open(path, 'w') as f:
        f.write("\n".join(lines))
    print(f"✅ Pixels header saved to: {path}")

if __name__ == '__main__':
    root = tk.Tk()
    root.withdraw()

    print("Select a palette image (to extract palette)...")
    palette_path = filedialog.askopenfilename(title="Select Palette Source Image")
    if not palette_path:
        exit()

    palette_image = Image.open(palette_path).convert('RGB')
    palette_pixels = list(palette_image.getdata())

    seen = set()
    ordered_palette = []
    for rgb in palette_pixels:
        if rgb not in seen:
            seen.add(rgb)
            ordered_palette.append(rgb)
        if len(ordered_palette) == 256:
            break

    pal_img = Image.new('P', (16, 16))
    flat_palette = []
    for r, g, b in ordered_palette:
        flat_palette.extend([r, g, b])
    while len(flat_palette) < 768:
        flat_palette.extend([0, 0, 0])
    pal_img.putpalette(flat_palette[:768])

    print("Select an image to convert...")
    target_path = filedialog.askopenfilename(title="Select Target Image")
    if not target_path:
        exit()

    target_img = Image.open(target_path).convert('RGB')
    indexed_img = target_img.quantize(palette=pal_img, dither=0)

    # Save indexed PNG
    output_dir = os.path.dirname(target_path)
    indexed_png_path = os.path.join(output_dir, "indexed_output.png")
    indexed_img.save(indexed_png_path, format='PNG')
    print(f"\n✅ Indexed PNG saved to: {indexed_png_path}")

    # Save indexed pixels only
    pixels = list(indexed_img.getdata())
    width, height = indexed_img.size
    pixels_h_path = os.path.join(output_dir, "image_pixels.h")
    save_pixels_header(pixels_h_path, pixels, width, height)

    indexed_img.show()