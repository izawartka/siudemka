import argparse
import os
from PIL import Image, ImageDraw

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

    def check_collision(self, other):
        if self == other:
            return False

        if self.x > other.x + other.width:
            return False
        
        if self.x + self.width < other.x:
            return False

        if self.y > other.y + other.height:
            return False

        if self.y + self.height < other.y:
            return False

        return True


def get_size_sorted_images(input_info_file_path):
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

    images.sort(key=lambda x: (x.height, x.width), reverse=True)

    return images  


def get_packed_tilemap(size_sorted_images, force_height=0):
    images = size_sorted_images
    tilemap_images = []
    tilemap_width = 0
    tilemap_height = force_height if force_height > 0 else images[0].height

    for image in images:
        tilemap_images.append(image)

        print(f"Processing image {image.file_name} ({len(tilemap_images)}/{len(images)})...")

        while True:
            collision = False

            for other_image in tilemap_images:
                if image.check_collision(other_image):
                    suggested_y = other_image.y + other_image.height
                    image.y = suggested_y if suggested_y != image.y else image.y + 1
                    collision = True
                    break

            if not collision:
                break

            if image.y + image.height > tilemap_height:
                image.y = 0
                image.x += 1

        if image.x + image.width > tilemap_width:
            tilemap_width = image.x + image.width

    return tilemap_images, tilemap_width, tilemap_height


def process_images():
    print("Reading input info file...")
    sorted_images = get_size_sorted_images(args.input_info_file)

    if args.height > 0 and args.height < sorted_images[0].height:
        print(f"!!! Forced height cannot be less than the height of the largest image ({sorted_images[0].height})")
        return

    print("Creating tilemap...")
    tilemap_info = get_packed_tilemap(sorted_images, args.height)   

    print("Creating tilemap image...")
    tilemap_image = Image.new("RGBA", (tilemap_info[1], tilemap_info[2]))
    draw = ImageDraw.Draw(tilemap_image)

    for image in tilemap_info[0]:
        input_image_path = os.path.join(args.input_dir, image.file_name)
        input_image = Image.open(input_image_path)
        tilemap_image.paste(input_image, (image.x, image.y))

    tilemap_image.save(args.output_file)

    print("Saving info file...")
    with open(args.output_info_file, "w") as f:
        for image in tilemap_info[0]:
            f.write(f"{image}\n")

    print("Tilemap created successfully")  


def main():
    global args

    parser = argparse.ArgumentParser(description="Create a tilemap from a set of images")
    parser.add_argument("input_dir", type=str, help="Directory containing the input images")
    parser.add_argument("output_file", type=str, help="File to save the tilemap image")
    parser.add_argument("input_info_file", type=str, help="File containing the input image crop information")
    parser.add_argument("output_info_file", type=str, help="File to save tilemap information")
    parser.add_argument("--ignore-exceptions", action="store_true", help="Stop the script if an exception occurs")
    parser.add_argument("--height", type=int, default=0, help="Height of the tilemap (default: 0, auto)")
    args = parser.parse_args()

    if args.height < 0:
        print("Height must be at least 0")
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
