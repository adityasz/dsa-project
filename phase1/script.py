import os
import sys
import subprocess

def print_files_with_prefix(directory_path='.', file_prefix=''):
    try:
        files = os.listdir(directory_path)
        filtered_files = [file for file in files if file.startswith(file_prefix)]
        file_list = ' '.join(filtered_files)
        print(file_list)

    except Exception as e:
        print(f"An error occurred: {e}")

if len(sys.argv) != 2:
    print("Please provide a prefix as an argument.")
    sys.exit(1)

file_prefix = sys.argv[1]
directory_path = os.path.join(os.path.dirname(os.path.abspath(__file__)), 'inputs')

print_files_with_prefix(directory_path, 'input_' + file_prefix)

n = input('Enter input number: ')
input_file = 'inputs/input_' + sys.argv[1] + '_' + n
output_file = 'input.txt'

subprocess.run(['cp', input_file, output_file], check=True)
