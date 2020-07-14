import sys
from PIL import Image, ImageOps
import numpy as np

np.set_printoptions(threshold=np.inf)

def resize_image(image, _width=32, _height=32):
    new_image = Image.open(image)
    new_image = ImageOps.fit(new_image , (_width, _height), Image.ANTIALIAS)
    new_image_rgb = new_image.convert('RGB')
    return np.asarray(new_image_rgb).flatten()

def print_array_for_c(_array):
    print("{",end="")
    for pixel in _array:
        print(pixel,end=",")
    print("}")

def main():
    if len(sys.argv) == 2:
        print("resize and convert image: "+sys.argv[1])
        print_array_for_c(resize_image(sys.argv[1]))
    else:
        print("Usage: python resize_image_and_to_array.py path_to_image")

if __name__ == "__main__":
    main()