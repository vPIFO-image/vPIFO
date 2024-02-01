import pandas as pd
from prettytable import PrettyTable
import argparse

parser = argparse.ArgumentParser(description='Read data from file and print a PrettyTable.')
parser.add_argument('input_file_path', type=str, help='Path to the input file')
parser.add_argument('output_file', type=str, help='Path to the output file for the PrettyTable')

args = parser.parse_args()

input_file_path = args.input_file_path
output_file_path = args.output_file

with open(input_file_path, 'r') as file:
    lines = file.readlines()

data = {
    'leafNodeId': [],
    'path': []
}

for line in lines:
    parts = line.strip().split(': ')
    leaf_node_id = parts[0].strip()
    path = parts[1].strip()
    data['leafNodeId'].append(leaf_node_id)
    data['path'].append(path)

df = pd.DataFrame(data)

table = PrettyTable()
table.field_names = df.columns

for row in df.itertuples(index=False):
    table.add_row(row)

with open(output_file_path, 'w') as output_file:
    output_file.write(str(table))

print(f'Table has been saved to {output_file_path}')
