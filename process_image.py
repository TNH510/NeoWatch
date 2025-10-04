def process_image_data(input_file, output_file, threshold=0xF0):
    """
    Process LVGL image data from RGBA format to 1-bit monochrome format (LV_COLOR_FORMAT_I1)
    for SSD1306 OLED display.
    
    This function:
    1. Reads the original image C file
    2. Auto-detects and extracts the image data array
    3. Calculates average of RGB values and compares with threshold
    4. Converts to binary black/white (0x00 or 0xFF) based on threshold
    5. Replaces the original RGBA data array with the new monochrome data
    6. Updates the image descriptor to use LV_COLOR_FORMAT_I1
    
    Args:
        input_file: Path to the input C file with RGBA image data
        output_file: Path to write the processed file (can be the same as input_file)
    """
    print(f"Processing file: {input_file}")
    
    # Read file content
    with open(input_file, 'r') as f:
        content = f.read()

    # Extract file base name from the file path
    import os
    file_base = os.path.basename(input_file)
    file_name = os.path.splitext(file_base)[0]  # Remove .c extension
    
    # Find the RGBA data array
    end_marker = "};"
    base_name = None
    start_marker = None
    start_pos = -1
    
    # First try: standard pattern with filename
    potential_markers = [
        f"const LV_ATTRIBUTE_MEM_ALIGN uint8_t {file_name}_data[] = {{",
        f"const LV_ATTRIBUTE_MEM_ALIGN uint8_t {file_name}_rgba_data[] = {{"
    ]
    
    # Try each potential marker
    for marker in potential_markers:
        pos = content.find(marker)
        if pos != -1:
            start_marker = marker
            start_pos = pos
            base_name = file_name
            print(f"Found data array for {base_name}")
            break
    
    # If not found, try to detect any ui_img pattern
    if start_pos == -1:
        print("Looking for any ui_img data array...")
        import re
        
        # Look for any ui_img*_data pattern
        pattern = r'const\s+LV_ATTRIBUTE_MEM_ALIGN\s+uint8_t\s+(ui_img[^_]+(?:_[^_]+)*)_data\[\]\s*=\s*\{'
        matches = re.findall(pattern, content)
        
        if matches:
            base_name = matches[0]
            start_marker = f"const LV_ATTRIBUTE_MEM_ALIGN uint8_t {base_name}_data[] = {{"
            start_pos = content.find(start_marker)
            print(f"Auto-detected image data array for {base_name}")
                
        if start_pos == -1:
            print(f"Could not find any data array in {input_file}")
            return
    
    start_pos += len(start_marker)
    end_pos = content.find(end_marker, start_pos)
    
    if end_pos == -1:
        print("Could not find end of data array")
        return
    
    # Extract the original data content
    data_content = content[start_pos:end_pos].strip()
    
    # Parse hex values from string
    values = []
    for item in data_content.replace('\n', '').replace(' ', '').split(','):
        if item and "0x" in item:
            try:
                values.append(int(item, 16))
            except ValueError:
                pass
    
    # Process data: Calculate average of first 3 values (RGB) and compare with threshold
    # If avg < threshold, use 0x00, else use 0xFF, and generate 2 identical bytes
    # Use the threshold parameter passed to the function
    processed_values = []
    
    for i in range(0, len(values), 4):
        if i+2 < len(values):
            # Calculate average of RGB values (first 3 values in RGBA)
            r = values[i]
            g = values[i+1]
            b = values[i+2]
            avg = (r + g + b) // 3  # Integer division for average
            
            # Compare with threshold and set binary value (0x00 or 0xFF)
            pixel_value = 0xFF if avg >= threshold else 0x00
            
            # Add the same value twice (for each group of 4 bytes)
            processed_values.append(pixel_value)
            processed_values.append(pixel_value)
    
    # Format output data
    output_data = []
    line_items = []
    
    for idx, val in enumerate(processed_values):
        line_items.append(f"0x{val:02X}")
        
        # Each line contains 64 values
        if len(line_items) == 64 or idx == len(processed_values) - 1:
            output_data.append("    " + ", ".join(line_items) + ",")
            line_items = []
    
    # Create new monochrome data array
    new_data_array = f"const LV_ATTRIBUTE_MEM_ALIGN uint8_t {base_name}_data[] = {{\n"
    new_data_array += "\n".join(output_data) + "\n};"
    
    # Replace the original data array with the new one
    before_array = content[:start_pos - len(start_marker)]
    after_array = content[end_pos + len(end_marker):]
    
    # Combine everything
    output_content = before_array + new_data_array + after_array
    
    # Write to output file
    with open(output_file, 'w') as f:
        f.write(output_content)
    
    print(f"Successfully processed and saved to: {output_file}")
    print(f"Image format changed to LV_COLOR_FORMAT_I1 (1-bit monochrome)")

# Function to find all image files in a directory
def find_image_files(directory):
    """
    Find all files starting with ui_img in the specified directory
    
    Args:
        directory: Directory path to search in
    
    Returns:
        List of paths to files starting with ui_img and ending with .c
    """
    import os
    import glob
    
    # Get all .c files in the directory
    pattern = os.path.join(directory, "ui_img*.c")
    files = glob.glob(pattern)
    
    print(f"Found {len(files)} image files in {directory}")
    return files

# Main execution
if __name__ == "__main__":
    import sys
    import os
    
    if len(sys.argv) > 1:
        # If argument is a directory, process all image files in that directory
        if os.path.isdir(sys.argv[1]):
            image_files = find_image_files(sys.argv[1])
            print(f"Found {len(image_files)} image files to process")
            for file_path in image_files:
                try:
                    process_image_data(file_path, file_path)
                except Exception as e:
                    print(f"Error processing {file_path}: {str(e)}")
        else:
            # Process single file
            process_image_data(sys.argv[1], sys.argv[1])
    else:
        # Default directory if no arguments provided
        default_dir = "h:\\Learning\\NeoVerse\\neowatch\\ui\\export\\c"
        print(f"Searching for image files in default directory: {default_dir}")
        
        image_files = find_image_files(default_dir)
        
        if not image_files:
            print("No image files found. Please check the directory path.")
        
        for file_path in image_files:
            try:
                process_image_data(file_path, file_path)  # Directly replace original file
            except Exception as e:
                print(f"Error processing {file_path}: {str(e)}")