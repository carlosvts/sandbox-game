import shutil
import subprocess
from pathlib import Path

from PIL import Image

VIDEO_FILE = "badapple.mp4"

FRAME_DIR = Path("frames_png")
BMP_DIR = Path("frames_bmp")

WIDTH = 200
HEIGHT = 200


def ensure_ffmpeg():
    if shutil.which("ffmpeg") is None:
        raise RuntimeError(
            "ffmpeg not found on PATH."
        )


def extract_frames():
    FRAME_DIR.mkdir(exist_ok=True)

    cmd = [
        "ffmpeg",
        "-i",
        VIDEO_FILE,
        "-vsync",
        "0",
        str(FRAME_DIR / "frame_%06d.png"),
        "-y",
    ]

    subprocess.run(cmd, check=True)


def convert_frames():
    BMP_DIR.mkdir(exist_ok=True)

    png_files = sorted(FRAME_DIR.glob("*.png"))

    total = len(png_files)

    for idx, png in enumerate(png_files, start=1):
        img = Image.open(png)

        img = img.resize(
            (WIDTH, HEIGHT),
            Image.Resampling.LANCZOS
        )

        bmp_path = BMP_DIR / (png.stem + ".bmp")

        img.save(bmp_path, format="BMP")

        if idx % 100 == 0 or idx == total:
            print(f"[{idx}/{total}] converted")


def cleanup():
    shutil.rmtree(FRAME_DIR)


def main():
    ensure_ffmpeg()

    print("Extracting frames...")
    extract_frames()

    print("Converting to BMP...")
    convert_frames()

    print("Removing PNGs...")
    cleanup()

    print("Done.")
    print(f"BMPs at: {BMP_DIR}")


if __name__ == "__main__":
    main()
