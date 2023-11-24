import sys
import subprocess

n = input('Enter input number: ')
input_file = 'inputs/input_' + sys.argv[1] + '_' + n
output_file = 'input.txt'

subprocess.run(['cp', input_file, output_file], check=True)
