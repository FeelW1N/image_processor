#include "imagefilter.h"
#include <cmath>
Image CropFilter::Apply(const Image& image) const {
    int new_width = std::min(width_, image.GetWidth());
    int new_height = std::min(height_, image.GetHeight());

    Image cropped_image(new_width, new_height);
    for (int y = 0; y < new_height; ++y) {
        for (int x = 0; x < new_width; ++x) {
            cropped_image.SetColour(image.GetColour(x, y), x, y);
        }
    }

    return cropped_image;
}

Image GrayscaleFilter::Apply(const Image& image) const {
    Image grayscale_image(image.GetWidth(), image.GetHeight());

    for (int y = 0; y < image.GetHeight(); ++y) {
        for (int x = 0; x < image.GetWidth(); ++x) {
            const float grayscale_red = 0.299f;
            const float grayscale_green = 0.587f;
            const float grayscale_blue = 0.114f;
            Colour pixel = image.GetColour(x, y);
            float gray = grayscale_red * pixel.red_ + grayscale_green * pixel.green_ + grayscale_blue * pixel.blue_;
            grayscale_image.SetColour(Colour(gray, gray, gray), x, y);
        }
    }

    return grayscale_image;
}

Image NegativeFilter::Apply(const Image& image) const {
    Image negative_image(image.GetWidth(), image.GetHeight());

    for (int y = 0; y < image.GetHeight(); ++y) {
        for (int x = 0; x < image.GetWidth(); ++x) {
            Colour pixel = image.GetColour(x, y);
            negative_image.SetColour(Colour(1.0f - pixel.red_, 1.0f - pixel.green_, 1.0f - pixel.blue_), x, y);
        }
    }

    return negative_image;
}

Image SharpeningFilter::Apply(const Image& image) const {
    Image sharpened_image(image.GetWidth(), image.GetHeight());

    // Sharpening matrix (Kernel)
    const std::vector<std::vector<float>> sharp_mat = {{0, -1, 0}, {-1, 5, -1}, {0, -1, 0}};

    // Apply the sharpening filter to the image
    for (int y = 0; y < image.GetHeight(); ++y) {
        for (int x = 0; x < image.GetWidth(); ++x) {
            // Initialize the sum of the products of the kernel and pixel values to 0
            float sum_red = 0;
            float sum_green = 0;
            float sum_blue = 0;

            // Iterate over the neighborhood of the current pixel
            for (int ky = -1; ky <= 1; ++ky) {
                for (int kx = -1; kx <= 1; ++kx) {
                    // Get the coordinates of the pixel in the neighborhood
                    int nx = x + kx;
                    int ny = y + ky;

                    // Check if the coordinates are within the bounds of the image
                    if (nx >= 0 && nx < image.GetWidth() && ny >= 0 && ny < image.GetHeight()) {
                        // Get the pixel value from the image
                        const Colour& neighbour_colour = image.GetColour(nx, ny);

                        // Multiply the pixel value by the corresponding kernel value
                        float kernel_value = sharp_mat[ky + 1][kx + 1];
                        sum_red += kernel_value * neighbour_colour.red_;
                        sum_green += kernel_value * neighbour_colour.green_;
                        sum_blue += kernel_value * neighbour_colour.blue_;
                    } else {
                        // For pixels outside the image bounds, we'll just use the original pixel value
                        const Colour& original_colour = image.GetColour(x, y);
                        float kernel_value = sharp_mat[ky + 1][kx + 1];
                        sum_red += kernel_value * original_colour.red_;
                        sum_green += kernel_value * original_colour.green_;
                        sum_blue += kernel_value * original_colour.blue_;
                    }
                }
            }

            // Clamp values to [0, 1] and set the sharpened pixel value in the output image
            sharpened_image.SetColour(
                Colour(std::min(1.0f, std::max(0.0f, sum_red)), std::min(1.0f, std::max(0.0f, sum_green)),
                       std::min(1.0f, std::max(0.0f, sum_blue))),
                x, y);
        }
    }
    return sharpened_image;
}

Image EdgeDetectionFilter::Apply(const Image& image) const {
    // Applying GrayscaleFilter
    Image grayscale_image = GrayscaleFilter().Apply(image);

    // Creating new Image
    Image edge_image(grayscale_image.GetWidth(), grayscale_image.GetHeight());

    // EdgeDetection matrix (Kernel)
    const std::vector<std::vector<float>> edge_mat = {{0, -1, 0}, {-1, 4, -1}, {0, -1, 0}};

    // Apply the edge detection filter to the grayscale image
    for (int y = 0; y < grayscale_image.GetHeight(); ++y) {
        for (int x = 0; x < grayscale_image.GetWidth(); ++x) {
            // Initialize the sum of the products of the kernel and pixel values to 0
            float sum = 0;
            // Iterate over the neighborhood of the current pixel
            for (int ky = -1; ky <= 1; ++ky) {
                for (int kx = -1; kx <= 1; ++kx) {
                    // Get the coordinates of the pixel in the neighborhood
                    int nx = x + kx;
                    int ny = y + ky;

                    // Check if the coordinates are within the bounds of the image
                    if (!(nx >= 0 && nx < grayscale_image.GetWidth() && ny >= 0 && ny < grayscale_image.GetHeight())) {
                        nx = x;
                        ny = y;
                    }
                    // Get the pixel value from the grayscale image
                    const Colour& neighbour_colour = grayscale_image.GetColour(nx, ny);
                    // Multiply the pixel value by the corresponding kernel value
                    float kernel_value = edge_mat[(ky + 1)][(kx + 1)];
                    sum += kernel_value *
                           neighbour_colour.red_;  // Since the image is grayscale, we only need to consider one channel
                }
            }
            // Apply threshold
            Colour edge_colour = (sum > threshold_) ? Colour(1.0f, 1.0f, 1.0f) : Colour(0.0f, 0.0f, 0.0f);
            // Set the edge pixel value in the output image
            edge_image.SetColour(edge_colour, x, y);
        }
    }

    return edge_image;
}

Image GaussianBlurFilter::Apply(const Image& image) const {
    // Creating new Image
    Image blurred_image(image.GetWidth(), image.GetHeight());

    // Creating Kernel for Blur
    const int kernel_size = static_cast<int>(6 * sigma_ + 1);  // Kernel size
    std::vector<float> gaussian_kernel;
    gaussian_kernel.reserve(kernel_size);

    // Filling gaussian kernel
    float sum = 0.0f;
    for (int x = -kernel_size / 2; x <= kernel_size / 2; ++x) {
        const float two = 2.0f;
        float value = static_cast<float>(std::exp(static_cast<float>(-(x * x)) / (two * sigma_ * sigma_)) /
                                         (std::sqrt(two * M_PI) * sigma_));
        gaussian_kernel.push_back(value);
        sum += value;
    }

    // normalizing gaussian kernel
    for (float& value : gaussian_kernel) {
        value /= sum;
    }

    // Applying gaussian blur horizontally
    for (int y = 0; y < image.GetHeight(); ++y) {
        for (int x = 0; x < image.GetWidth(); ++x) {
            float new_red = 0.0f;
            float new_green = 0.0f;
            float new_blue = 0.0f;

            for (int kx = -kernel_size / 2; kx <= kernel_size / 2; ++kx) {
                // Getting new colour for current pixel and neighbours
                if (x + kx >= 0 && x + kx < image.GetWidth()) {
                    const Colour& neighbour = image.GetColour(x + kx, y);
                    float kernel_value = gaussian_kernel[kx + kernel_size / 2];
                    new_red += kernel_value * neighbour.red_;
                    new_green += kernel_value * neighbour.green_;
                    new_blue += kernel_value * neighbour.blue_;
                }
            }

            // Setting colour for blurred image
            blurred_image.SetColour(Colour(new_red, new_green, new_blue), x, y);
        }
    }

    // Applying gaussian blur vertically
    for (int y = 0; y < image.GetHeight(); ++y) {
        for (int x = 0; x < image.GetWidth(); ++x) {
            float new_red = 0.0f;
            float new_green = 0.0f;
            float new_blue = 0.0f;
            for (int ky = -kernel_size / 2; ky <= kernel_size / 2; ++ky) {
                // Getting new colour for current pixel and neighbours
                if (y + ky >= 0 && y + ky < image.GetHeight()) {
                    const Colour& neighbour = blurred_image.GetColour(x, y + ky);
                    float kernel_value = 0.0f;
                    if (ky + kernel_size / 2 >= 0 && ky + kernel_size / 2 < kernel_size) {
                        kernel_value = gaussian_kernel[ky + kernel_size / 2];
                    }
                    new_red += kernel_value * neighbour.red_;
                    new_green += kernel_value * neighbour.green_;
                    new_blue += kernel_value * neighbour.blue_;
                }
            }

            // Setting colour for blurred image
            blurred_image.SetColour(Colour(new_red, new_green, new_blue), x, y);
        }
    }

    return blurred_image;
}

Image PixelationFilter::Apply(const Image& image) const {
    // Create a new image with the same dimensions as the original
    Image pixelated_image(image.GetWidth(), image.GetHeight());

    // Parameters for pixelation
    const int pixel_size = pixel_size_;

    // Apply pixelation
    for (int y = 0; y < image.GetHeight(); y += pixel_size) {
        for (int x = 0; x < image.GetWidth(); x += pixel_size) {
            // Compute the average color for each pixel block
            float avg_red = 0.0f;
            float avg_green = 0.0f;
            float avg_blue = 0.0f;

            // Calculate the average color value in the block
            int pixel_count = 0;
            for (int dy = 0; dy < pixel_size; ++dy) {
                for (int dx = 0; dx < pixel_size; ++dx) {
                    int nx = x + dx;
                    int ny = y + dy;

                    // Check if the coordinates are within the bounds of the image
                    if (nx < image.GetWidth() && ny < image.GetHeight()) {
                        const Colour& pixel_colour = image.GetColour(nx, ny);
                        avg_red += pixel_colour.red_;
                        avg_green += pixel_colour.green_;
                        avg_blue += pixel_colour.blue_;
                        ++pixel_count;
                    }
                }
            }

            // Compute the average color value in the block
            avg_red /= static_cast<float>(pixel_count);
            avg_green /= static_cast<float>(pixel_count);
            avg_blue /= static_cast<float>(pixel_count);

            // Set this average color for the entire block
            for (int dy = 0; dy < pixel_size; ++dy) {
                for (int dx = 0; dx < pixel_size; ++dx) {
                    int nx = x + dx;
                    int ny = y + dy;

                    // Check if the coordinates are within the bounds of the image
                    if (nx < image.GetWidth() && ny < image.GetHeight()) {
                        pixelated_image.SetColour(Colour(avg_red, avg_green, avg_blue), nx, ny);
                    }
                }
            }
        }
    }

    return pixelated_image;
}
PixelationFilter::PixelationFilter() {
}
