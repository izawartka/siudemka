import argparse
import os
import signal
from concurrent.futures import ThreadPoolExecutor
from PIL import Image, ImageDraw
import numpy as np
import cv2
from enum import Enum

class ScriptMode(Enum):
    draw = 'draw'
    crop = 'crop'

    def __str__(self):
        return self.value

stop_flag = False
args = None
crop_info_file = None

def handler(signum, frame):
    global stop_flag
    print("\nSoft stop requested. Finishing ongoing operations...")
    stop_flag = True

def get_image_filenames(base_path):
    return [f for f in os.listdir(base_path) if os.path.isfile(os.path.join(base_path, f))]

def save_crop_info(filename, polygon, bbox):
    if not crop_info_file:
        return

    points = " ".join(f"{int(p[0]) - int(bbox[0])} {int(p[1]) - int(bbox[1])}" for p in polygon)
    crop_info_file.write(f"{filename} {int(bbox[0])} {int(bbox[1])} {points}\n")

def get_polygon_mask(image_data):
    alpha_channel = image_data[:, :, 3]
    non_transparent_pixels = np.column_stack(np.where(alpha_channel > 0))

    if len(non_transparent_pixels) < 3:
        return None  # Not enough points to form a polygon

    # Swap (row, col) -> (x, y)
    non_transparent_pixels = non_transparent_pixels[:, ::-1]

    # Compute convex hull
    polygon = cv2.convexHull(non_transparent_pixels)

    return polygon.reshape(-1, 2)  # Return as (x, y) coordinates

def create_crop_box(bbox):
    x, y, w, h = bbox
    return (x, y, x + w, y + h)

def process_image(filename):
    if stop_flag:
        return None

    input_image_path = os.path.join(args.input_dir, filename)
    output_image_path = os.path.join(args.output_dir, filename)
    
    if not os.path.exists(input_image_path):
        raise Exception(f"⚠️ Input image does not exist: {filename}")
    
    input_image = Image.open(input_image_path).convert("RGBA")
    image_data = np.array(input_image)
    polygon = get_polygon_mask(image_data)
    
    # Ensure polygon is valid
    if polygon is None or polygon.size == 0 or len(polygon) < 3:
        print(f"⚠️ Skipping image {filename}: No valid polygon found.")
        return None
    
    # Convert to list of tuples before passing it to PIL
    polygon_list = [tuple(p) for p in polygon]

    # Get bounding box of polygon
    bbox = cv2.boundingRect(polygon)
    if bbox is None:
        print(f"⚠️ Skipping image {filename}: Failed to compute bounding box.")
        return None

    out_image = create_output_image(input_image, polygon_list, bbox)
    if not out_image:
        print(f"⚠️ Skipping image {filename}: Failed to create output image.")
        return None
    
    out_image.save(output_image_path)
    save_crop_info(filename, polygon_list, bbox)
    print(f"✅ Processed image {filename}")
    
    return filename

def create_output_image(image, polygon, bbox):
    crop_box = create_crop_box(bbox)

    # draw polygon and bounding box on image
    if args.mode == ScriptMode.draw:
        out_image = Image.new("RGBA", image.size, (0, 0, 0, 0))
        out_image.paste(image, (0, 0))
        draw = ImageDraw.Draw(out_image)
        draw.rectangle(crop_box, outline="red")
        draw.polygon(polygon, outline="white")
        for i in range(len(polygon)):
            draw.point(polygon[i], fill="orange")
        return out_image
    
    # crop image to bounding box
    elif args.mode == ScriptMode.crop:
        return image.crop(crop_box)

def process_images():
    filenames = get_image_filenames(args.input_dir)
    real_num_threads = min(args.threads, len(filenames))
    global crop_info_file, stop_flag
    signal.signal(signal.SIGINT, handler)
    
    if args.info_file:
        crop_info_file = open(os.path.join(args.output_dir, args.info_file), "w")
    
    with ThreadPoolExecutor(max_workers=real_num_threads) as executor:
        future_to_filename = {executor.submit(process_image, filename): filename for filename in filenames}
        for future in future_to_filename:
            if stop_flag:
                break
            try:
                future.result()
            except Exception as e:
                print(f"Failed to process image {future_to_filename[future]}: {e}")

                if not args.ignore_exceptions:
                    stop_flag = True
    
    if crop_info_file:
        crop_info_file.close()

def main():
    global args
    parser = argparse.ArgumentParser(description="Crop a set of images using polygonal contours of the subject")
    parser.add_argument("input_dir", type=str, help="Directory containing input images")
    parser.add_argument("output_dir", type=str, help="Directory for output images")
    parser.add_argument("--mode", type=ScriptMode, choices=list(ScriptMode), default=ScriptMode.draw, help="Operation mode: draw or crop")
    parser.add_argument("--threads", type=int, default=16, help="Number of threads (default: 16)")
    parser.add_argument("--ignore-exceptions", action="store_true", help="Continue processing even if an error occurs")
    parser.add_argument("--info-file", type=str, help="File to save polygon information")
    args = parser.parse_args()
    
    if not os.path.exists(args.input_dir) or not os.path.isdir(args.input_dir):
        print("Input directory must exist and be a directory")
        return
    
    os.makedirs(args.output_dir, exist_ok=True)
    process_images()

if __name__ == "__main__":
    main()
