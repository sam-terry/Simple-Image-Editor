#include <iostream>
#include <fstream>
#include <vector>
#include <cstdlib>
#include <string>
#include <cmath>

using namespace std;

/**
 * Sets a value to the char array starting at the offset using the size
 * specified by the bytes.
 * This is a helper function for write_image()
 * @param arr    Array to set values for
 * @param offset Starting index offset
 * @param bytes  Number of bytes to set
 * @param value  Value to set
 */
void set_bytes(unsigned char arr[], int offset, int bytes, int value) {
    for (int i = 0; i < bytes; i++) {
        arr[offset+i] = (unsigned char)(value>>(i*8));
    }
}

/**
 * Write the input image to a BMP file name specified
 * @param filename The BMP file name to save the image to
 * @param image    The input image to save
 * @return True if successful and false otherwise
 */
bool write_image(string filename, const vector<vector<vector<int> > >& image) {
    // Get the image width and height in pixels
    int width_pixels = image[0].size();
    int height_pixels = image.size();

    // Calculate the width in bytes incorporating padding (4 byte alignment)
    int width_bytes = width_pixels * 3;
    int padding_bytes = 0;
    padding_bytes = (4 - width_bytes % 4) % 4;
    width_bytes = width_bytes + padding_bytes;

    // Pixel array size in bytes, including padding
    int array_bytes = width_bytes * height_pixels;

    // Open a file stream for writing to a binary file
    fstream stream;
    stream.open(filename, ios::out | ios::binary);

    // If there was a problem opening the file, return false
    if (!stream.is_open()) {
        return false;
    }

    // Create the BMP and DIB Headers
    const int BMP_HEADER_SIZE = 14;
    const int DIB_HEADER_SIZE = 40;
    unsigned char bmp_header[BMP_HEADER_SIZE] = {0};
    unsigned char dib_header[DIB_HEADER_SIZE] = {0};

    // BMP Header
    set_bytes(bmp_header,  0, 1, 'B');              // ID field
    set_bytes(bmp_header,  1, 1, 'M');              // ID field
    set_bytes(bmp_header,  2, 4, BMP_HEADER_SIZE+DIB_HEADER_SIZE+array_bytes); // Size of BMP file
    set_bytes(bmp_header,  6, 2, 0);                // Reserved
    set_bytes(bmp_header,  8, 2, 0);                // Reserved
    set_bytes(bmp_header, 10, 4, BMP_HEADER_SIZE+DIB_HEADER_SIZE); // Pixel array offset

    // DIB Header
    set_bytes(dib_header,  0, 4, DIB_HEADER_SIZE);  // DIB header size
    set_bytes(dib_header,  4, 4, width_pixels);     // Width of bitmap in pixels
    set_bytes(dib_header,  8, 4, height_pixels);    // Height of bitmap in pixels
    set_bytes(dib_header, 12, 2, 1);                // Number of color planes
    set_bytes(dib_header, 14, 2, 24);               // Number of bits per pixel
    set_bytes(dib_header, 16, 4, 0);                // Compression method (0=BI_RGB)
    set_bytes(dib_header, 20, 4, array_bytes);      // Size of raw bitmap data (including padding)                     
    set_bytes(dib_header, 24, 4, 2835);             // Print resolution of image (2835 pixels/meter)
    set_bytes(dib_header, 28, 4, 2835);             // Print resolution of image (2835 pixels/meter)
    set_bytes(dib_header, 32, 4, 0);                // Number of colors in palette
    set_bytes(dib_header, 36, 4, 0);                // Number of important colors

    // Write the BMP and DIB Headers to the file
    stream.write((char*)bmp_header, sizeof(bmp_header));
    stream.write((char*)dib_header, sizeof(dib_header));

    // Initialize pixel and padding
    unsigned char pixel[3] = {0};
    unsigned char padding[3] = {0};

    // Pixel Array (Left to right, bottom to top, with padding)
    for (int h = 0; h < height_pixels; h++) {
        for (int w = 0; w < width_pixels; w++) {
            // Write the pixel (Blue, Green, Red)
            pixel[0] = image[h][w][0];
            pixel[1] = image[h][w][1];
            pixel[2] = image[h][w][2];
            stream.write((char*)pixel, 3);
        }
        // Write the padding bytes
        stream.write((char *)padding, padding_bytes);
    }

    // Close the stream and return true
    stream.close();
    return true;
}

/** 
 * Gets an integer from a binary stream.
 * @param stream the stream
 * @param offset the offset at which to read the integer
 * @return the integer starting at the given offset
 */ 
int get_int(fstream& stream, int offset)
{
    stream.seekg(offset);
    int result = 0;
    int base = 1;
    for (int i = 0; i < 4; i++)
    {   
        result = result + stream.get() * base;
        base = base * 256;
    }
    return result;
}

// Reads the BMP image specified and returns the resulting image
vector<vector<vector<int > > > read_image(string filename)
{
    vector<vector<vector< int > > > empty; // Initialize 3D vector

    fstream stream; // Create stream variable, read in binary file
    stream.open(filename, ios::in | ios::out | ios::binary);

    int file_size = get_int(stream, 2); // Get image dimensions
    int start = get_int(stream, 10);
    int width = get_int(stream, 18);
    int height = get_int(stream, 22);

    // Scan lines must occupy multiples of four bytes
    int scanline_size = width * 3;
    int padding = 0;
    if (scanline_size % 4 != 0)
    {
        padding = 4 - scanline_size % 4;
    }

    if (file_size != start + (scanline_size + padding) * height)
    {
        cout << "Not a 24-bit true color image file." << endl;
        return empty;
    }
    
    int pos = start;

    for (int i = 0; i < height; i++) // For each row
    {
        vector<vector<int> > w; // Create a vector for the row
        for (int j = 0; j < width; j++) // For each column
        {
            stream.seekg(pos); // Move to the start position

            // Extract blue, green, and red values for the pixel
            int blue = stream.get();
            int green = stream.get();
            int red = stream.get();

            vector<int> vals = {blue, green, red}; // Create vector for the pixel
            w.push_back(vals); // Add pixel to column
            
            pos += 3; // Move over to next pixel
        }

        empty.push_back(w); // Add row to the 3D output vector
        stream.seekg(padding, ios::cur); // Skip padding
        pos += padding;
    }

    return empty;
}

// Adds vignette effect to the input image and returns the resulting image
vector<vector<vector<int> > > process_1(const vector<vector<vector<int> > >& image)
{
    vector<vector<vector<int> > > empty; // Initialize 3D vector for result
    int height = image.size();
    int width = image[0].size();

    for (int row = 0; row < height; row++)
    {
        vector<vector<int> > w;
        for (int col = 0; col < width; col++)
        {
            vector<int> vals;
            double distance = sqrt(pow(col - (static_cast<double>(width) / 2), 2) + pow(static_cast<double>(row) - (height / 2), 2));
            double scaling_factor = (height - distance) / height;
            
            int newBlue = scaling_factor * image[row][col][0];
            int newGreen = scaling_factor * image[row][col][1];
            int newRed = scaling_factor * image[row][col][2];

            vals.push_back(newBlue);
            vals.push_back(newGreen);
            vals.push_back(newRed);

            w.push_back(vals);
        }
        empty.push_back(w);
    }
    return empty;
}

// Adds claredon effect to the input image and returns the resulting image
vector<vector<vector<int> > > process_2(const vector<vector<vector<int> > >& image)
{
    vector<vector<vector<int> > > empty; // Create output vector
    int height = image.size(); // Get height and width for image
    int width = image[0].size();
    const double SCALING_FACTOR = 0.3; // Set scaling factor

    for (int i = 0; i < height; i++) // For each row in image
    {
        vector<vector<int> > w; // Create a row for the output vector
        for (int j = 0; j < width; j++) // For each column in the row
        {
            vector<int> vals; // Create column for the output pixel

            // Store old RGB vals and declare new ones
            int oldBlue = image[i][j][0], oldGreen = image[i][j][1], oldRed = image[i][j][2]; 
            int newBlue, newGreen, newRed;

            double avg = (oldBlue + oldGreen + oldRed) / 3.0; // Compute average of rgb values

            if (avg >= 170) // If the pixel is light, make it lighter
            {
                newBlue = 255 - (255 - oldBlue) * SCALING_FACTOR;
                newGreen = 255 - (255 - oldGreen) * SCALING_FACTOR;
                newRed = 255 - (255 - oldRed) * SCALING_FACTOR;
            }

            else if (avg < 90) // If the pixel is dark, make it darker
            {
                newBlue = oldBlue * SCALING_FACTOR;
                newGreen = oldGreen * SCALING_FACTOR;
                newRed = oldRed * SCALING_FACTOR;
            }

            else // If the pixel is moderate, keep it as it is
            {
                newBlue = oldBlue;
                newGreen = oldGreen;
                newRed = oldRed;
            }
            
            vals.push_back(newBlue); // Add the new values to the pixel
            vals.push_back(newGreen);
            vals.push_back(newRed);

            w.push_back(vals); // Add the new pixel to the row

        }
        empty.push_back(w); // Add the row to the output vector
    }
    return empty;
}

// Adds grayscale effect to the input image and returns the resulting image
vector<vector<vector<int> > > process_3(const vector<vector<vector<int> > >& image)
{
    vector<vector<vector<int> > > empty; // Create output vector

    int height = image.size(); // Get dimensions of image
    int width = image[0].size();

    for (int i = 0; i < height; i++) // For each row in image
    {
        vector<vector<int> > w; // Create row for output vector
        for (int j = 0; j < width; j++) // For each pixel in image
        {
            vector<int> vals; // Create vector for the RGB values

            // Gray value is the average of the RGB values
            int gray = (image[i][j][0] + image[i][j][1] + image[i][j][2]) / 3;

            vals.push_back(gray); // RGB values for the new pixel are all the gray value
            vals.push_back(gray);
            vals.push_back(gray);

            w.push_back(vals); // Add new pixel to the row
        }
        empty.push_back(w); // Add new row to the output vector
    }

    return empty;
}

// Rotates the input image by 90 degrees clockwise and returns the resulting image
vector<vector<vector<int> > > process_4(const vector<vector<vector<int> > >& image)
{
    int height = image.size(); // Find dimensions of image
    int width = image[0].size();
    vector<vector<vector<int> > > empty; // Create 3D output vector

    for (int i = 0; i < width; i++) // For each column in image, create a row in the output vector
    {
        vector<vector<int > > row;
        for (int j = 0; j < height; j++) // For each row in image, create a column in the output vector
        {
            vector<int> vals = {0,0,0}; // Add an empty pixel at each column
            row.push_back(vals);
        }
        empty.push_back(row);
    }
    for (int x = 0; x < height; x++) // For each row in image 
        {
            for (int y = 0; y < width; y++) // For each pixel in row
            {
                empty[width - 1 - y][x] = image[x][y]; // Rotate pixel 90 degrees
            }
        }

    return empty;
}

// Rotates image by the specified multiple of 90 degrees clockwise and returns the resulting image
vector<vector<vector<int> > > process_5(const vector<vector<vector<int> > >& image, int number)
{
    vector<vector<vector<int> > > empty; // Create output vector

    if (number % 4 == 0) // If the number of rotations mod 4 is 0, the image isn't rotated
    {
        empty = image;
    }

    else if (number % 4 == 1) // If number mod 4 is 1, rotate once
    {
        empty = process_4(image);
    }

    else if (number % 4 == 2) // If number mod 4 is 2, rotate twice
    {
        empty = process_4(process_4(image));
    }

    else // If number mod 4 is 3, rotate thrice
    {
        empty = process_4(process_4(process_4(image)));
    }
    
    return empty;
}

// Enlarges the input image in the x and y direction by the scales specified and returns the resulting image
vector<vector<vector<int> > > process_6(const vector<vector<vector<int> > >& image, int x_scale, int y_scale)
{
    vector<vector<vector<int> > > empty; // Create output vector
    int height = image.size(); // Get image dimensions
    int width = image[0].size();
    for (int i = 0; i < (height * y_scale); i++) // For row in image, add y_scale rows to output
    {
        vector<vector<int> > w;
        for (int j = 0; j < (width * x_scale); j++) // For col in image, add x_scale pixels to output
        {
            vector<int> pxl;

            // Calculate RGB values for output
            int blueval = image[i / y_scale][j / x_scale][0];
            int greenval = image[i / y_scale][j / x_scale][1];
            int redval = image[i / y_scale][j / x_scale][2];

            // Add new values to pixel
            pxl.push_back(blueval);
            pxl.push_back(greenval);
            pxl.push_back(redval);

            w.push_back(pxl); // Add pixel to row

        }
        empty.push_back(w); // Add row to output
    }
    return empty;
}

// Converts the input image to high contrast and returns the resulting image
vector<vector<vector<int> > > process_7(const vector<vector<vector<int> > >& image)
{
    vector<vector<vector<int> > > empty; // Create output vector
    int height = image.size(); // Get image dimensions
    int width = image[0].size();

    for (int i = 0; i < height; i++) // For each row in image, add a row to output
    {
        vector<vector<int> > w;
        for (int j = 0; j < width; j++) // For each col in image, add pixel to output
        {
            vector<int> pxl;
            double avg = (image[i][j][0] + image[i][j][1] + image[i][j][2]) / 3; // Find average RGB value of pixel in image

            if (avg >= (255 / 2.0)) // If pixel is light, make it lighter
            {
                pxl = {255,255,255};
            }

            else // If pixel is dark, make it darker
            {
                pxl = {0,0,0};
            }
            
            w.push_back(pxl); // Add pixel to row
        }
        empty.push_back(w); // Add row to output
    }
    return empty;
}

// Lightens the input image and returns the resulting image
vector<vector<vector<int> > > process_8(const vector<vector<vector<int> > >& image, double scaling_factor)
{
    vector<vector<vector<int> > > empty;
    int height = image.size(); // Get image dimensions
    int width = image[0].size();

    for (int i = 0; i < height; i++) // For each row in image, add a row to output
    {
        vector<vector<int> > w;
        for (int j = 0; j < width; j++) // For each col in image, add pixel to output
        {
            vector<int> pxl;

            // Scale old RGB values with scaling factor
            double blueScaled = (255 - image[i][j][0]) * scaling_factor;
            double greenScaled = (255 - image[i][j][1]) * scaling_factor;
            double redScaled = (255 - image[i][j][2]) * scaling_factor;

            // Set new pixel to the lightened RGB values
            int newBlue = static_cast<int>(255 - blueScaled);
            int newGreen = static_cast<int>(255 - greenScaled);
            int newRed = static_cast<int>(255 - redScaled);

            pxl.push_back(newBlue);
            pxl.push_back(newGreen);
            pxl.push_back(newRed);

            w.push_back(pxl); // Add pixel to row
        }
        empty.push_back(w); // Add row to output
    }
    return empty;
}

// Darkens image the input image and returns the resulting image
vector<vector<vector<int> > > process_9(const vector<vector<vector<int> > >& image, double scaling_factor)
{
    vector<vector<vector<int> > > empty;
    int height = image.size(); // Get image dimensions
    int width = image[0].size();

    for (int i = 0; i < height; i++) // For each row in image, add a row to output
    {
        vector<vector<int> > w;
        for (int j = 0; j < width; j++) // For each col in image, add pixel to output
        {
            vector<int> pxl;

            // Reduce (darken) old RGB values by the scaling factor
            int newBlue = static_cast<int>(image[i][j][0] * scaling_factor);
            int newGreen = static_cast<int>(image[i][j][1] * scaling_factor);
            int newRed = static_cast<int>(image[i][j][2] * scaling_factor);

            pxl.push_back(newBlue); // Add darkened values to pixel
            pxl.push_back(newGreen);
            pxl.push_back(newRed);

            w.push_back(pxl); // Add pixel to row
        }
        empty.push_back(w); // Add row to output
    }
    return empty;
}

// Converts the input image to black, white, red, blue, and green only and returns the resulting image
vector<vector<vector<int> > > process_10(const vector<vector<vector<int> > >& image)
{
    vector<vector<vector<int> > > empty;
    int height = image.size(); // Get image dimensions
    int width = image[0].size();

    for (int i = 0; i < height; i++) // For each row in image, add a row to output
    {
        vector<vector<int> > w;
        for (int j = 0; j < width; j++) // For each col in image, add pixel to output
        {
            vector<int> pixel;

            int oldBlue = image[i][j][0]; // Store old RGB values
            int oldGreen = image[i][j][1];
            int oldRed = image[i][j][2];
            int totalRGB = oldBlue + oldGreen + oldRed; // Find total RGB value


            if (totalRGB >= 550) // If total is 550 or more, set pixel to white
            {
                pixel = {255,255,255};
            }

            else if (totalRGB <= 150) // If total is 150 or less, set pixel to black
            {
                pixel = {0,0,0};
            }

            else if (oldRed > oldBlue && oldRed > oldGreen) // Set pixel to whichever color has the highest value
            {
                pixel = {0,0,255};
            }
            
            else if (oldGreen > oldBlue && oldGreen > oldRed)
            {
                pixel = {0,255,0};
            }

            else
            {
                pixel = {255,0,0};
            }
            
            w.push_back(pixel); // Add pixel to row
        }
        empty.push_back(w); // Add row to output
    }
    return empty;
}


