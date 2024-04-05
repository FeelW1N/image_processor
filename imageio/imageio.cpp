#include "imageio.h"
#include <iostream>
#include <fstream>

#pragma pack(push, 1)
// Structure for BMP file header
struct FileHeader {
    char signature[2]{'B', 'M'};
    [[maybe_unused]] int32_t file_size;
    int32_t reserved{0};
    [[maybe_unused]] int32_t pixel_data_offset;
};

// Structure for BMP info header
struct InfoHeader {
    static const int16_t BMP_FORM = 24;
    static const int32_t SIZE_FORM = 40;
    [[maybe_unused]] int32_t header_size{SIZE_FORM};
    int32_t width;
    int32_t height;
    [[maybe_unused]] int16_t planes{1};
    int16_t bits_per_pixel{BMP_FORM};
    [[maybe_unused]] int32_t compression{0};
    [[maybe_unused]] int32_t image_size{0};
    [[maybe_unused]] int32_t x_pixels_per_meter{0};
    [[maybe_unused]] int32_t y_pixels_per_meter{0};
    [[maybe_unused]] int32_t total_colors{0};
    [[maybe_unused]] int32_t important_colors{0};
};

Image::Image(int width, int height) : width_(width), height_(height), data_(width * height) {
}

Colour Image::GetColour(int x, int y) const {
    return data_[y * width_ + x];
}

void Image::SetColour(const Colour& colour, int x, int y) {
    data_[y * width_ + x] = colour;
}

void Image::Export(const char* path) const {
    std::ofstream f(path, std::ios::out | std::ios::binary);
    if (!f.is_open()) {
        throw FileOpenException("File could not be opened");
    }

    // Calculate the necessary padding size to align pixels in a row to 4 bytes
    const int padding_amount = (4 - (width_ * 3) % 4) % 4;

    // Calculate the BMP file size
    const int pixel_data_offset = sizeof(FileHeader) + sizeof(InfoHeader);
    const int file_size = pixel_data_offset + (width_ * 3 + padding_amount) * height_;

    // Create the BMP file header
    FileHeader file_header;
    file_header.file_size = file_size;
    file_header.pixel_data_offset = pixel_data_offset;

    // Create the BMP info header
    InfoHeader info_header;
    info_header.width = width_;
    info_header.height = height_;

    // Write the headers to the file
    f.write(reinterpret_cast<const char*>(&file_header), sizeof(FileHeader));
    f.write(reinterpret_cast<const char*>(&info_header), sizeof(InfoHeader));

    // Write the pixels to the file with padding
    for (int y = height_ - 1; y >= 0; --y) {  // Start from the bottom row of the image
        for (int x = 0; x < width_; ++x) {
            const Colour& col = GetColour(x, y);
            const float convert = 255.0f;
            unsigned char colour[] = {static_cast<unsigned char>(col.blue_ * convert),
                                      static_cast<unsigned char>(col.green_ * convert),
                                      static_cast<unsigned char>(col.red_ * convert)};
            f.write(reinterpret_cast<const char*>(colour), 3);
        }
        // Write padding if necessary
        if (padding_amount > 0) {
            unsigned char bmp_pad[3] = {0, 0, 0};
            f.write(reinterpret_cast<const char*>(bmp_pad), padding_amount);
        }
    }

    f.close();
}

Colour::Colour() : red_(0), green_(0), blue_(0) {
}

Colour::Colour(float red, float green, float blue) : red_(red), green_(green), blue_(blue) {
}

Colour::~Colour() {
}

void Image::Read(const char* path) {
    std::ifstream f(path, std::ios::in | std::ios::binary);
    if (!f.is_open()) {
        throw FileOpenException("File could not be opened");
    }

    // Read the BMP file header
    FileHeader file_header;
    f.read(reinterpret_cast<char*>(&file_header), sizeof(FileHeader));
    if (file_header.signature[0] != 'B' || file_header.signature[1] != 'M') {
        throw InvalidBMPFormatException("The specified path is not BMP");
    }

    // Read the BMP info header
    InfoHeader info_header;
    f.read(reinterpret_cast<char*>(&info_header), sizeof(InfoHeader));
    const int bmp_form = 24;
    if (info_header.bits_per_pixel != bmp_form) {
        throw InvalidBMPFormatException("Wrong BMP format");
    }

    // Update the image size
    width_ = info_header.width;
    height_ = info_header.height;
    data_.assign(width_ * height_, Colour());

    // Read the image pixels
    const int padding_amount = (4 - (width_ * 3) % 4) % 4;
    for (int y = height_ - 1; y >= 0; --y) {  // Start from the bottom row of the image
        for (int x = 0; x < width_; ++x) {
            unsigned char colour[3];
            f.read(reinterpret_cast<char*>(colour), 3);
            const float convert = 255.0f;
            float red = static_cast<float>(colour[2]) / convert;
            float green = static_cast<float>(colour[1]) / convert;
            float blue = static_cast<float>(colour[0]) / convert;
            SetColour(Colour(red, green, blue), x, y);
        }
        // Skip padding if present
        if (padding_amount > 0) {
            f.seekg(padding_amount, std::ios::cur);
        }
    }

    f.close();
}

Image::Image() {
}

Image::~Image() {
}
#pragma pack(pop)
