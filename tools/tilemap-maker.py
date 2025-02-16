import argparse
import os
from PIL import Image, ImageDraw
from rectpack import newPacker

args = None

class TilemapImage:
    def __init__(self, file_name, crop_x, crop_y, width, height, x=0, y=0):
        self.file_name = file_name
        self.crop_x = crop_x
        self.crop_y = crop_y
        self.width = width
        self.height = height
        self.x = x
        self.y = y

    def __str__(self):
        return f"{self.file_name} {self.crop_x} {self.crop_y} {self.width} {self.height} {self.x} {self.y}"


def get_images(input_info_file_path):
    with open(input_info_file_path, "r") as f:
        lines = f.readlines()

    images = []

    for line in lines:
        parts = line.split()
        images.append(TilemapImage(
            parts[0],
            int(parts[1]),
            int(parts[2]),
            int(parts[3]),
            int(parts[4])
        ))

    return images


def get_packed_tilemap(images):    
    global args
    # Create a new packer
    packer = newPacker(rotation=False)

    # Add the rectangles to the packer
    for (index, image) in enumerate(images):
        packer.add_rect(image.width, image.height, index)

    tot_width = sum(image.width for image in images)
    tot_height = max(image.height for image in images)
    base_size = max(tot_width, tot_height)
    if args.base_size:
        base_size = args.base_size
    base_size_sqrt = base_size ** 0.5

    packer.add_bin(base_size_sqrt, base_size_sqrt, count=1)

    # Pack the rectangles
    packer.pack()

    if len(packer.rect_list()) != len(images):
        print("Some images could not be packed")
        return None

    packed_images = []
    for rect in packer.rect_list():
        image = images[rect[5]]

        packed_images.append(TilemapImage(
            image.file_name,
            image.crop_x,
            image.crop_y,
            image.width,
            image.height,
            rect[1],
            rect[2]
        ))

    return packed_images


def process_images():
    print("Reading input info file...")
    images = get_images(args.input_info_file)
    images_count = len(images)

    print("Creating tilemap from " + str(images_count) + " images...")
    tilemap_info = get_packed_tilemap(images)

    if not tilemap_info:
        print("Failed to create tilemap")
        return

    # Calculate the size of the tilemap
    max_x = max(image.x + image.width for image in tilemap_info)
    max_y = max(image.y + image.height for image in tilemap_info)

    tilemap_image = Image.new("RGBA", (max_x, max_y), (0, 0, 0, 0))

    for image in tilemap_info:
        input_image_path = os.path.join(args.input_dir, image.file_name)
        input_image = Image.open(input_image_path)
        tilemap_image.paste(input_image, (image.x, image.y))

    tilemap_image.save(args.output_file)

    print("Saving info file...")
    with open(args.output_info_file, "w") as f:
        for image in tilemap_info:
            f.write(f"{image}\n")

    print("Tilemap created successfully")


def main():
    global args

    parser = argparse.ArgumentParser(description="Create a tilemap from a set of images")
    parser.add_argument("input_dir", type=str, help="Directory containing the input images")
    parser.add_argument("output_file", type=str, help="File to save the tilemap image")
    parser.add_argument("input_info_file", type=str, help="File containing the input image crop information")
    parser.add_argument("output_info_file", type=str, help="File to save tilemap information")
    parser.add_argument("--base-size", type=int, help="Base size of the tilemap")
    parser.add_argument("--ignore-exceptions", action="store_true", help="Stop the script if an exception occurs")
    args = parser.parse_args()

    if args.base_size and args.base_size < 1:
        print("Base size must be greater than 0")
        return

    if not os.path.exists(args.input_dir) or not os.path.isdir(args.input_dir):
        print("Input directory must exist and be a directory")
        return

    args.input_info_file = os.path.join(args.input_dir, args.input_info_file)
    args.output_info_file = os.path.join(args.input_dir, args.output_info_file)
    args.output_file = os.path.join(args.input_dir, args.output_file)

    if not os.path.exists(args.input_info_file) or not os.path.isfile(args.input_info_file):
        print("Input info file must exist and be a file")
        return

    already_exists = False

    if os.path.exists(args.output_file):
        if os.path.isdir(args.output_file):
            print("Output path must be a file")
            return

        already_exists = True

    if os.path.exists(args.output_info_file):
        if os.path.isdir(args.output_info_file):
            print("Output info file must be a file")
            return

        already_exists = True

    if already_exists:
        print("Warning: Output file(s) already exist(s). Type 'y' to continue or any other key to cancel.")
        response = input()

        if response.lower() != 'y':
            return

    process_images()


if __name__ == "__main__":
    main()
