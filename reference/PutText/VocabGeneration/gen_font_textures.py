# coding: utf-8
import argparse
import os
from pathlib import Path

import PIL
import numpy as np
from PIL import ImageDraw, ImageFont


class cd:
    def __init__(self, target):
        self.target = target
        self.old_path = None

    def __enter__(self):
        self.old_path = os.getcwd()
        os.chdir(self.target)

    def __exit__(self, etype, value, traceback):
        os.chdir(self.old_path)


class FontRender:
    def __init__(self, font_file: str, char_table_file: str, font_size_pixel: int = 50, output_dir=None, chan=1):
        self.font_file = font_file
        self.font_size_pixel: int = font_size_pixel
        self.char_table_file = char_table_file
        self.output_dir = Path(output_dir or Path(__file__).parent / "output")
        self.chan = chan

        self.font = ImageFont.truetype(
            self.font_file,
            self.font_size_pixel,
            layout_engine=ImageFont.Layout.BASIC,
        )
        ascent, descent = self.font.getmetrics()
        self.max_font_height = ascent + descent

    def put_text(self, draw, font, c, pos, debug=False):
        (w, h) = pos
        draw.text((w, h), c, (255, 255, 255) if self.chan > 1 else 255, font=font)
        bbox = list(font.getbbox(c))
        advance = bbox[2] - bbox[0]
        ascent, descent = font.getmetrics()
        if debug:
            bbox[0] += w
            bbox[1] += h
            bbox[2] += w
            bbox[3] += h
            # draw glyph
            draw.rectangle(bbox, outline="red")
            # draw baseline
            draw.line((w, h + ascent, w + self.font_size_pixel, h + ascent), fill="red", width=3)
            draw.line((w, h, w + self.font_size_pixel, h), fill="blue", width=1)
        return advance, ascent + descent

    @property
    def font_name(self):
        return self.font_file.rsplit(".", 1)[0]

    @property
    def char_table_name(self):
        return self.char_table_file.rsplit(".", 1)[0]

    @property
    def name(self):
        return f"{self.font_name}_{self.font_size_pixel}px"

    @property
    def font_img_advance_filename(self):
        return f"{self.name}.txt"

    @property
    def font_img_bin_filename(self):
        return f"{self.name}.bin"

    @property
    def font_img_filename(self):
        return f"{self.name}.png"

    def gen(self):
        with open(self.char_table_file, encoding="utf-8") as f:
            char_table = [line for line in f.read().split("\n") if line]

        height, width = (len(char_table) * self.max_font_height, self.font_size_pixel)
        shape = (height, width, self.chan) if self.chan > 1 else (height, width)
        pil_img = PIL.Image.fromarray(np.zeros(shape, dtype=np.uint8))
        draw = ImageDraw.Draw(pil_img)

        c_with_advance = []
        for idx, c in enumerate(char_table):
            advance, height = self.put_text(draw, self.font, c, (0, idx * self.max_font_height))
            c_with_advance.append((c, advance, height))
        img_array = np.array(pil_img)
        img_array_binary = np.where(img_array >= 20, 255, 0).astype(np.uint8)

        with cd(self.output_dir):
            with open(self.font_img_advance_filename, "w", encoding="utf-8") as f:
                f.writelines(f"{c}+{advance}+{height}\n" for c, advance, height in c_with_advance)
            img_array_binary.tofile(self.font_img_bin_filename)
            pil_img.save(self.font_img_filename)


def cli_parse():
    parser = argparse.ArgumentParser(description="Process some parameters.")

    parser.add_argument("--font", type=str, required=True, help="Font type.")
    parser.add_argument("--font_size_pixel", type=int, required=True, help="Font size in pixel.")
    parser.add_argument("--char_table", type=str, required=True, help="Character table.")
    parser.add_argument("--output", type=str, default=None, help="Output dir")

    args = parser.parse_args()
    return args


def main():
    args = cli_parse()
    render = FontRender(args.font, args.char_table, args.font_size_pixel, args.output, chan=1)
    render.gen()


if __name__ == "__main__":
    main()
