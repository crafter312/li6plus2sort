def merge_data_files(file1_path, file2_path, output_path):
    # Read all lines from both files
    try:
        with open(file1_path, 'r', encoding='utf-8') as f1:
            lines1 = f1.readlines()
        with open(file2_path, 'r', encoding='utf-8') as f2:
            lines2 = f2.readlines()
    except FileNotFoundError as e:
        raise FileNotFoundError(f"Error: Could not find one of the input files. Details: {e}")

    # 1. Check if both files contain the same number of lines
    if len(lines1) != len(lines2):
        raise ValueError(
            f"Error: Line count mismatch! File 1 has {len(lines1)} lines, "
            f"while File 2 has {len(lines2)} lines."
        )

    output_lines = []

    # Loop through both files simultaneously (using zip)
    for idx, (line1, line2) in enumerate(zip(lines1, lines2), start=1):
        # Parse File 1 line (tab-delimited)
        parts1 = line1.strip().split('\t')
        # Parse File 2 line (space-delimited)
        parts2 = line2.strip().split()

        # Validate column counts
        if len(parts1) not in (9, 10):
            raise ValueError(
                f"Line {idx} in File 1 contains {len(parts1)} items "
                f"(expected 9 or 10 tab-delimited numbers)."
            )
        if len(parts2) != 4:
            raise ValueError(
                f"Line {idx} in File 2 contains {len(parts2)} items "
                f"(expected 4 space-delimited numbers)."
            )

        # Convert to float for accurate numeric comparison (handling potential formatting differences)
        try:
            f1_num1, f1_num2 = float(parts1[0]), float(parts1[1])
            f2_num1, f2_num2 = float(parts2[0]), float(parts2[1])
        except ValueError:
            raise ValueError(f"Line {idx}: Non-numeric data encountered in the first two columns.")

        # 2. Check if the first number matches across both files
        if f1_num1 != f2_num1:
            raise ValueError(
                f"Mismatch at line {idx}: First numbers do not match "
                f"(File 1: {parts1[0]}, File 2: {parts2[0]})."
            )

        # 3. Check if the second number matches across both files
        if f1_num2 != f2_num2:
            raise ValueError(
                f"Mismatch at line {idx}: Second numbers do not match "
                f"(File 1: {parts1[1]}, File 2: {parts2[1]})."
            )

        # Extract the third number from the second file (index 2)
        third_num_file2 = parts2[2]

        # 4. Handle appending or replacing based on the length of File 1's line
        if len(parts1) == 9:
            parts1.append(third_num_file2)
        else:  # len(parts1) == 10
            parts1[9] = third_num_file2

        # Reconstruct the line using tabs and preserve newline
        output_lines.append('\t'.join(parts1) + '\n')

    # Write the results to the output text file
    with open(output_path, 'w', encoding='utf-8') as f_out:
        f_out.writelines(output_lines)
    
    print(f"Successfully merged files! Output saved to '{output_path}'.")

# Example usage:
if __name__ == "__main__":
    merge_data_files("../config/barmap.txt", "TNTimecal137Cs.txt", "barmap_NEW.txt")
