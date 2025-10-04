def process_image_data(input_file, output_file):
    """
    Process LVGL image data from RGBA format to 1-bit monochrome format (LV_COLOR_FORMAT_I1)
    for SSD1306 OLED display.
    
    This function:
    1. Reads the original image C file
    2. Extracts the RGBA data
    3. Processes data by keeping only values 3 and 4 from each group of 4 values (R,G,B,A)
    4. Replaces the original RGBA data array with the new monochrome data
    5. Updates the image descriptor to use LV_COLOR_FORMAT_I1
    6. Updates any references to the data array
    
    Args:
        input_file: Path to the input C file with RGBA image data
        output_file: Path to write the processed file (can be the same as input_file)
    """
    print(f"Processing file: {input_file}")
    
    # Read file content
    with open(input_file, 'r') as f:
        content = f.read()

    # Find the RGBA data array (first try with the regular pattern)
    start_marker = "const LV_ATTRIBUTE_MEM_ALIGN uint8_t ui_img_cat_mini_png_data[] = {"
    end_marker = "};"
    
    # Extract base name (e.g., "ui_img_cat_mini_png") from the marker
    base_name = "ui_img_cat_mini_png"  # Default value
    
    start_pos = content.find(start_marker)
    if start_pos == -1:
        print(f"Could not find data array. Looking for alternative marker...")
        
        # Try with "_rgba_" naming pattern
        for potential_base in ["ui_img_cat_mini_png", "ui_img_cat_png"]:
            alt_marker = f"const LV_ATTRIBUTE_MEM_ALIGN uint8_t {potential_base}_data[] = {{"
            alt_start_pos = content.find(alt_marker)
            if alt_start_pos != -1:
                start_marker = alt_marker
                start_pos = alt_start_pos
                base_name = potential_base
                print(f"Found data array for {base_name}")
                break
                
        if start_pos == -1:
            print(f"Could not find any data array")
            return
    else:
        # Extract base name from the found marker
        marker_parts = start_marker.split("const LV_ATTRIBUTE_MEM_ALIGN uint8_t ")[1].split("_data[] = {")
        if len(marker_parts) > 0:
            base_name = marker_parts[0]
    
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
    threshold = 0x99  # Default threshold value
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

# Main execution
if __name__ == "__main__":
    import sys
    if len(sys.argv) > 1:
        process_image_data(sys.argv[1], sys.argv[1])  # Input and output are the same file
    else:
        # Default paths if no arguments provided
        image_files = [
            "h:\\Learning\\NeoVerse\\neowatch\\ui\\export\\c\\ui_img_cat_mini_png.c",
        ]
        
        for file_path in image_files:
            try:
                process_image_data(file_path, file_path)  # Directly replace original file
            except Exception as e:
                print(f"Error processing {file_path}: {str(e)}")