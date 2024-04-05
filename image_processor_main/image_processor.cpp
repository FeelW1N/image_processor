#include <iostream>
#include <vector>
#include "../imagefilter/imagefilter.h"

class ImageProcessor {
public:
    ImageProcessor() = default;

    // Process the image based on the command-line arguments
    void ProcessImage(int argc, char* argv[]) {
        try {
            if (argc < 3) {
                throw std::invalid_argument("Insufficient command-line arguments.");
            }

            // Load the input image
            Image input_image;
            input_image.Read(argv[1]);
            // Apply filters
            for (int i = 3; i < argc; ++i) {
                std::string filter_name = argv[i];
                if (filter_name[0] == '-') {
                    // Parse filter name and parameters
                    std::vector<std::string> filter_args;
                    while (++i < argc && argv[i][0] != '-') {
                        filter_args.push_back(argv[i]);
                    }
                    --i;

                    // Apply filter
                    ApplyFilter(filter_name.substr(1), filter_args, input_image);
                } else {
                    throw std::invalid_argument("Invalid argument: " + filter_name);
                }
            }

            // Export the processed image
            input_image.Export(argv[2]);
        } catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << std::endl;
            PrintUsage();
        }
    }

private:
    // Apply filter to the image
    void ApplyFilter(const std::string& filter_name, const std::vector<std::string>& args, Image& image) {
        if (filter_name == "crop") {
            if (args.size() != 2) {
                throw std::invalid_argument("Invalid number of arguments for crop filter.");
            }
            int width = std::stoi(args[0]);
            int height = std::stoi(args[1]);
            CropFilter crop_filter(width, height);
            image = crop_filter.Apply(image);
        } else if (filter_name == "gs") {
            GrayscaleFilter grayscale_filter;
            image = grayscale_filter.Apply(image);
        } else if (filter_name == "blur") {
            if (args.size() != 1) {
                throw std::invalid_argument("Invalid number of arguments for blur filter.");
            }
            float sigma = std::stof(args[0]);
            GaussianBlurFilter blur_filter(sigma);
            image = blur_filter.Apply(image);
        } else if (filter_name == "edge") {
            if (args.size() != 1) {
                throw std::invalid_argument("Invalid number of arguments for EdgeDetection filter.");
            }
            float threshold = std::stof(args[0]);
            EdgeDetectionFilter edge_filter(threshold);
            image = edge_filter.Apply(image);
        } else if (filter_name == "neg") {
            NegativeFilter negative_filter;
            image = negative_filter.Apply(image);
        } else if (filter_name == "sharp") {
            SharpeningFilter sharpening_filter;
            image = sharpening_filter.Apply(image);

        } else if (filter_name == "pixel") {
            if (args.size() != 1) {
                throw std::invalid_argument("Invalid number of arguments for Pixelation filter.");
            }
            int pixel_size = std::stoi(args[0]);
            PixelationFilter pixelation_filter(pixel_size);
            image = pixelation_filter.Apply(image);
        } else {
            throw std::invalid_argument("Unknown filter: " + filter_name);
        }
    }

    // Print usage information
    void PrintUsage() {
        std::cout << "Usage: ./image_processor input_file output_file [-filter1 [args...]] [-filter2 [args...]] ..."
                  << std::endl;
    }
};

int main(int argc, char* argv[]) {
    ImageProcessor processor;
    processor.ProcessImage(argc, argv);
    return 0;
}
