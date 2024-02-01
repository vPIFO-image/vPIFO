import os
import math
from prettytable import PrettyTable
import argparse

DEGREE = 2

parser = argparse.ArgumentParser(description='Process tree data and print PrettyTable.')
parser.add_argument('file_path', type=str, help='Path to the input file')
args = parser.parse_args()

file_path = args.file_path

base_path, file_name = os.path.split(file_path)
file_name_without_ext, file_ext = os.path.splitext(file_name)

readable_output_file_path = os.path.join(base_path, f'{file_name_without_ext}.tb')
mem_output_file_path = os.path.join(base_path, f'{file_name_without_ext}.mem')

with open(file_path, 'r') as file:
    text = file.read()

lines = text.split('\n')

tree_data = []

for line in lines:
    if line.strip():
        parts = line.split(', ')
        tree_id = int(parts[0].split(': ')[1])
        level = int(parts[1].split(': ')[1])
        tree_data.append((tree_id, level))

sorted_tree_data = sorted(tree_data, key=lambda x: x[0])

table = PrettyTable()
table.field_names = ["tree_id", "level"]

for tree_id, level in sorted_tree_data:
    table.add_row([tree_id, level])

print(table)

max_tree_id = max(tree_data, key=lambda x: x[0])[0]
print(f"max_tree_id: {max_tree_id}")

max_level = max(tree_data, key=lambda x: x[1])[1]
print(f"max_level: {max_level}")

SRAM_NUM = int(input(f"pelase input SRAM num(geq max_level({max_level})): "))


start_addr_dict = {}

# start_addr of each level
start_addr = [0] * SRAM_NUM
for tree_id, level in sorted_tree_data:
    cur_level_size = 1
    for i in range(level):
        SRAM_id = (tree_id+i) % SRAM_NUM
        start_addr_dict[(tree_id, i)] = (SRAM_id, start_addr[SRAM_id])
        start_addr[SRAM_id] += cur_level_size
        cur_level_size *= DEGREE

max_SRAM_addr = max(start_addr)

SRAM_addr_bits = math.ceil(math.log2(max_SRAM_addr))
level_bits = math.ceil(math.log2(max_level))
tree_id_bits = math.ceil(math.log2(max_tree_id+1))
SRAM_id_bits = math.ceil(math.log2(SRAM_NUM))

print(f"max_SRAM_addr: {max_SRAM_addr}")
print(f"max_level: {max_level} \n")

print(f"SRAM_addr_bits: {SRAM_addr_bits}")
print(f"level_bits: {level_bits}")
print(f"tree_id_bits: {tree_id_bits}")
print(f"SRAM_NUM_bits: {SRAM_id_bits}")

table = PrettyTable()
table.field_names = ["tree_id", "level", "SRAM_id", "start_address"]

with open(readable_output_file_path, 'w') as output_file:
    for key, value in start_addr_dict.items():
        tree_id, i = key
        SRAM_id, start_address = value
        table.add_row([tree_id, i, SRAM_id, start_address])
    
    print(table, file=output_file)

with open(mem_output_file_path, 'w') as output_file:
    for tree_id in range(2 ** tree_id_bits):
        for level in range(2 ** level_bits):
            if (tree_id, level) in start_addr_dict:
                SRAM_id, start_address = start_addr_dict[(tree_id, level)]
                binary_SRAM_id = format(SRAM_id, f'0{SRAM_id_bits}b')
                binary_start_address  = format(start_address, f'0{SRAM_addr_bits}b')
            else:
                binary_SRAM_id = format((1 << SRAM_id_bits) - 1, f'0{SRAM_id_bits}b')
                binary_start_address  = format((1 << SRAM_addr_bits) - 1, f'0{SRAM_addr_bits}b')

            result_binary = binary_SRAM_id + binary_start_address
            print(f"{result_binary}", file=output_file)
            
