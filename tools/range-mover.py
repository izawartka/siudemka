import argparse
import os
import shutil as fs
from PIL import Image, ImageDraw

args = None


def process_images():
    global args

    filenames = os.listdir(args.input_dir)
    filenames.sort()
    filenames = [f for f in filenames if f.startswith(args.old_prefix)]

    name = filenames[0].split('.')[0].replace(args.old_prefix, '')
    old_start_id = int(name)
    old_leading_zeros = len(name)

    new_start = args.new_start if args.new_start >= 0 else old_start_id
    leading_zeros = args.leading_zeros if args.leading_zeros >= 0 else old_leading_zeros

    for filename in filenames:
        ext = filename.split('.')[1]
        old_id = int(filename.split('.')[0].replace(args.old_prefix, ''))
        new_id = new_start + (old_id - old_start_id)
        new_name = str(new_id).zfill(leading_zeros if leading_zeros > 0 else old_leading_zeros)

        if args.prefix:
            new_name = args.prefix + new_name
        elif args.old_prefix:
            new_name = args.old_prefix + new_name

        new_name += '.' + ext

        input_path = os.path.join(args.input_dir, filename)
        output_path = os.path.join(args.output_dir, new_name)

        fs.copy(input_path, output_path)

    print("Done")


def main():
    global args

    parser = argparse.ArgumentParser(description="Batch rename range of files with a new range")
    parser.add_argument("input_dir", type=str, help="Directory containing the input files")
    parser.add_argument("output_dir", type=str, help="Directory to save the output files")
    parser.add_argument("--ignore-exceptions", action="store_true", help="Stop the script if an exception occurs")
    parser.add_argument("--new-start", type=int, default=-1, help=" (default: -1, auto) Start of the new range")
    parser.add_argument("--prefix", type=str, default="", help="Prefix to add to the new range")
    parser.add_argument("--old-prefix", type=str, default="", help="Affect only files with this prefix")
    parser.add_argument("--leading-zeros", type=int, default=-1, help="Number of leading zeros to add to the new range (default: -1, leave as is)")
    args = parser.parse_args()

    if args.new_start < -1:
        print("Ranges cannot start at a negative number")
        return

    if args.leading_zeros < -1:
        print("Number of leading cannot be less than 0 (except -1)")
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
