# -*- coding: utf-8 -*-
"""
Created on Mon Oct 19 18:10:59 2026

@author: h.s.webb
"""

# Define dimensions based on your array (16 wide * 6 deep = 96 columns, 6 tall crystals per column)
num_columns = 96
crystals_per_column = 6

# Create and write to the text file
filename = "crystalgammaoffsets_EMPTY.txt"

with open(filename, "w") as f:
    for col_id in range(num_columns):
        for crystal_id in range(crystals_per_column):
            # Write tab-delimited: column ID, crystal ID, and three zeros
            f.write(f"{col_id}\t{crystal_id}\t0\t0\t0\n")

print(f"Successfully generated '{filename}' with {num_columns * crystals_per_column} total rows.")
