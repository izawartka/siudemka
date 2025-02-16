import argparse
import os
import signal
from concurrent.futures import ThreadPoolExecutor
from PIL import Image, ImageDraw
import numpy as np
import scipy.ndimage
from enum import Enum

class ScriptMode(Enum):
    draw = 'draw'
    crop = 'crop'
    clear = 'clear'

    def __str__(self):
        return self.value

# Flag to indicate whether the script should stop
stop_flag = False
args = None
crop_info_file = None

def handler(signum, frame):
    global stop_flag
    print("\nSoft stop requested. Finishing ongoing operations...")
    stop_flag = True


def get_image_filenames(base_path):
    filenames = []

    for f in os.listdir(base_path):
        path = os.path.join(base_path, f)
        if os.path.isfile(path):
            filenames.append(f)

    return filenames


def save_crop_info(filename, bbox):
    if not crop_info_file:
        return

    if not bbox:
        return

    width = bbox[2] - bbox[0]
    height = bbox[3] - bbox[1]
    text = f"{filename} {bbox[0]} {bbox[1]} {width} {height}\n"
    crop_info_file.write(text)


def process_image(filename, tolerance):
    if stop_flag:
        return None

    input_image_path = os.path.join(args.input_dir, filename)
    output_image_path = os.path.join(args.output_dir, filename)

    # Check if the input image exists
    if not os.path.exists(input_image_path):
        raise Exception(f"Input image does not exist")

    # Load the image
    input_image = Image.open(input_image_path)
    image = input_image.copy()

    if tolerance > 0:
        # Convert image to numpy array for processing
        image_data = np.array(image)

        # Find non-transparent regions
        if image_data.shape[2] == 4:  # RGBA
            alpha_channel = image_data[:, :, 3]
        else:
            raise Exception(f"Image does not have an alpha channel")

        # Label connected regions
        labeled, num_features = scipy.ndimage.label(alpha_channel > 0)
        sizes = scipy.ndimage.sum(alpha_channel > 0, labeled, range(num_features + 1))

        # Create a mask of small regions
        small_regions_mask = sizes < tolerance
        small_regions_mask = small_regions_mask[labeled]
        
        # Remove small regions from the alpha channel
        alpha_channel[small_regions_mask] = 0

        # Update image data
        image_data[:, :, 3] = alpha_channel

        # Convert back to PIL Image
        image = Image.fromarray(image_data)

    # Get the bounding box of the non-transparent region
    bbox = image.getbbox()

    # Check if there is a non-transparent region
    if not bbox:
        print(f"No non-transparent region found in image {filename}")

        if args.decrease_tolerance and tolerance > 0:
            new_tolerance = tolerance - 1
            print(f"Trying with tolerance = {new_tolerance}")
            return process_image(filename, new_tolerance)
        else:
            return None

    # Add margin to the bounding box
    if args.margin > 0:
        bbox = (bbox[0] - args.margin, bbox[1] - args.margin, bbox[2] + args.margin, bbox[3] + args.margin)

    # Create the output image
    out_image = create_output_image(input_image, bbox)

    if not out_image:
        print("Could not create output image")
        return None

    # Save the image
    out_image.save(output_image_path)

    save_crop_info(filename, bbox)

    print(f"Processed image {filename}")
    return filename 


def create_output_image(image, bbox):
    out_image = Image.new("RGBA", image.size, (0, 0, 0, 0))

    # Paste only the non-transparent region
    if args.mode == ScriptMode.clear:
        if not bbox:
            return None

        region = image.crop(bbox)
        out_image.paste(region, bbox)

    # Draw the bounding box on the image
    elif args.mode == ScriptMode.draw:
        if not bbox:
            return image

        out_image.paste(image, (0, 0))
        draw = ImageDraw.Draw(out_image)
        draw.rectangle(bbox, outline="white")

    # Crop the image to the bounding box
    elif args.mode == ScriptMode.crop:
        if not bbox:
            return None

        out_image = image.crop(bbox)

    return out_image


def process_images():
    filenames = get_image_filenames(args.input_dir)
    files_count = len(filenames)
    real_num_threads = min(args.threads, files_count)

    global stop_flag
    signal.signal(signal.SIGINT, handler)

    global crop_info_file
    if args.info_file:
        crop_info_file_path = os.path.join(args.output_dir, args.info_file)
        crop_info_file = open(crop_info_file_path, "w")

    with ThreadPoolExecutor(max_workers=real_num_threads) as executor:
        future_to_filename = {executor.submit(process_image, filename, args.tolerance): filename for filename in filenames}

        for future in future_to_filename:
            if stop_flag:
                break

            filename = future_to_filename[future]
            try:
                result = future.result()
            except Exception as e:
                e.print_exc()
                print(f"!!! Failed to process image {filename}: {e}")
                if not args.ignore_exceptions:
                    print("Stopping the script...")
                    stop_flag = True

    if crop_info_file:
        crop_info_file.close()


def main():
    global args

    parser = argparse.ArgumentParser(description="Crop a set of images to the bounding box of the non-transparent region")
    parser.add_argument("input_dir", type=str, help="Directory containing the input images")
    parser.add_argument("output_dir", type=str, help="Directory to save the output images")
    parser.add_argument("--mode", type=ScriptMode, choices=list(ScriptMode), default=ScriptMode.draw, help="Mode of operation: draw, crop, or clear (default: draw)")
    parser.add_argument("--tolerance", type=int, default=4, help="Ignore islands of less than this number of pixels (default: 4)")
    parser.add_argument("--margin", type=int, default=0, help="Add a margin around the bounding box (default: 0)")
    parser.add_argument("--threads", type=int, default=16, help="Number of threads to use for processing images (default: 16)")
    parser.add_argument("--ignore-exceptions", action="store_true", help="Stop the script if an exception occurs")
    parser.add_argument("--decrease-tolerance", action="store_true", help="Decrease tolerance if no non-transparent region is found")
    parser.add_argument("--info-file", type=str, help="File to save crop information")
    args = parser.parse_args()

    if args.threads < 1:
        print("Number of threads must be at least 1")
        return

    if args.tolerance < 0:
        print("Tolerance must be non-negative")
        return

    if not os.path.exists(args.input_dir) or not os.path.isdir(args.input_dir):
        print("Input directory must exist and be a directory")
        return

    if os.path.exists(args.output_dir):
        if not os.path.isdir(args.output_dir):
            print("Save directory must be a directory")
            return

        if os.listdir(args.output_dir):
            print("Warning: Save directory is not empty. Type 'y' to continue or any other key to cancel.")
            response = input()

            if response.lower() != 'y':
                return
    else:
        os.makedirs(args.output_dir)

    process_images()

if __name__ == "__main__":
    main()
