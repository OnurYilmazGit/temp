from PIL import Image, ImageEnhance, ImageFilter
import numpy as np

# Replace 'image.jpeg' with your image file
image_path = 'pandaa.jpeg'

# Open the image and convert to 1-bit black and white
image = Image.open(image_path)

contrast_enhancer = ImageEnhance.Contrast(image)
image = contrast_enhancer.enhance(2.0)  # Increase the contrast

# Apply a sharpening filter to make the image details more pronounced
image = image.filter(ImageFilter.SHARPEN)

image = image.resize((200, 200))

# Convert to 1-bit black and white
image = image.convert('1')

image.save('output.jpg')


# Convert image to numpy array and make sure it is binary (0 for white, 1 for black)
image_array = np.array(image).astype(np.uint8)

# Invert the image (make 1 for white and 0 for black) if necessary
# This is needed because some displays consider 0 as white and 1 as black.
# Uncomment the following line if you need to invert the binary values.
# image_array = 1 - image_array

# Flatten the array and pack 8 bits into each byte
packed_image = np.packbits(image_array.flatten())

# Convert to a list of hex values
hex_values = ['0X{:02X}'.format(byte) for byte in packed_image]

# Set the desired width of the C array output (number of values per line)
output_width = 16

# Create the C array string representation
hex_array_str = ',\n'.join(', '.join(hex_values[i:i+output_width])
                            for i in range(0, len(hex_values), output_width))

# Format the array as a C array
c_array = f'const unsigned char IMAGE_BLACK[] PROGMEM = {{\n{hex_array_str}\n}};'

# Print the result or write to a .cpp file
with open('imagedata.cpp', 'w') as f:
    f.write(c_array)
