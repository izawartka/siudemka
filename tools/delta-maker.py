import argparse
import os
import signal
from concurrent.futures import ThreadPoolExecutor
from PIL import Image

# Flag to indicate whether the script should stop
stop_flag = False
args = None

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


def compare_pixels(base_pixel, overlay_pixel, tolerance):
    return abs(base_pixel[0] - overlay_pixel[0]) <= tolerance and \
           abs(base_pixel[1] - overlay_pixel[1]) <= tolerance and \
           abs(base_pixel[2] - overlay_pixel[2]) <= tolerance and \
           abs(base_pixel[3] - overlay_pixel[3]) <= tolerance


def process_image(filename):
    if stop_flag:
        return None

    base_image_path = os.path.join(args.bases_dir, filename)
    overlay_image_path = os.path.join(args.overlays_dir, filename)
    output_image_path = os.path.join(args.output_dir, filename)

    # Check if the base image exists
    if not os.path.exists(base_image_path):
        raise Exception(f"Base image does not exist")

    # Check if the overlay image exists
    if not os.path.exists(overlay_image_path):
        raise Exception(f"Overlay image does not exist")

    # Load the images
    base_image = Image.open(base_image_path)
    overlay_image = Image.open(overlay_image_path)

    # Check if the images have the same size
    if base_image.size != overlay_image.size:
        raise Exception(f"Images have different sizes")

    # Create a new image with the same size as the base image
    output_image = Image.new("RGBA", base_image.size)

    # get image data
    base_data = base_image.getdata()
    overlay_data = overlay_image.getdata()
    output_data = []

    # Compare the pixels of the two images
    for base_pixel, overlay_pixel in zip(base_data, overlay_data):
        if compare_pixels(base_pixel, overlay_pixel, args.tolerance):
            output_data.append((0, 0, 0, 0))
        else:
            output_data.append(overlay_pixel)

    # Save the output image
    output_image.putdata(output_data)
    output_image.save(output_image_path) 

    print(f"Saved image {filename}")
    return filename


def process_images():
    filenames = get_image_filenames(args.bases_dir)
    files_count = len(filenames)
    real_num_threads = min(args.threads, files_count)

    global stop_flag
    signal.signal(signal.SIGINT, handler)

    with ThreadPoolExecutor(max_workers=real_num_threads) as executor:
        future_to_filename = {executor.submit(process_image, filename): filename for filename in filenames}

        for future in future_to_filename:
            if stop_flag:
                break

            filename = future_to_filename[future]
            try:
                result = future.result()
            except Exception as e:
                print(f"!!! Failed to process image {filename}: {e}")
                if not args.ignore_exceptions:
                    print("Stopping the script...")
                    stop_flag = True


def main():
    global args

    parser = argparse.ArgumentParser(description="Create delta images from a set of base and overlay images")
    parser.add_argument("bases_dir", type=str, help="Directory containing the base images")
    parser.add_argument("overlays_dir", type=str, help="Directory containing the overlay images")
    parser.add_argument("output_dir", type=str, help="Directory to save the output images")
    parser.add_argument("--tolerance", type=int, default=4, help="Tolerance for pixel comparison (default: 4)")
    parser.add_argument("--threads", type=int, default=16, help="Number of threads to use for processing images (default: 16)")
    parser.add_argument("--ignore-exceptions", action="store_true", help="Stop the script if an exception occurs")
    args = parser.parse_args()

    if args.threads < 1:
        print("Number of threads must be at least 1")
        return

    if args.tolerance < 0 or args.tolerance > 255:
        print("Tolerance must be between 0 and 255")
        return

    if not os.path.exists(args.bases_dir) or not os.path.isdir(args.bases_dir):
        print("Bases directory must exist and be a directory")
        return

    if not os.path.exists(args.overlays_dir) or not os.path.isdir(args.overlays_dir):
        print("Overlays directory must exist and be a directory")
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
