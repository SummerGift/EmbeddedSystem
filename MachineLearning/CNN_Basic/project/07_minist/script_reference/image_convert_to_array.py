import sys
from PIL import Image
import numpy as np

def print_array_for_c(_array):
    print("{",end="")
    for pixel in _array:
        print(pixel,end=",")
    print("}")

def main():
    if len(sys.argv) == 2:
        print("convert image: "+sys.argv[1])
        img=np.asarray(Image.open(sys.argv[1]).convert("RGB")).flatten()
        print(img.shape)
        print_array_for_c(img)
    else:
        print("Usage: python image_convert_to_array.py path_to_image")

if __name__ == "__main__":
    main()

