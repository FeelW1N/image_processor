#ifndef IMAGEFILTER_H
#define IMAGEFILTER_H

#include "../imageio/imageio.h"

class ImageFilter {
public:
    virtual ~ImageFilter() {
    }

    // Method for applying the filter to an image
    virtual Image Apply(const Image& image) const = 0;
};

// Class for cropping an image
class CropFilter : public ImageFilter {
public:
    CropFilter(int width, int height) : width_(width), height_(height) {
    }

    // Applying the crop filter
    Image Apply(const Image& image) const override;

private:
    int32_t width_;
    int32_t height_;
};

// Class for converting an image to grayscale
class GrayscaleFilter : public ImageFilter {
public:
    // Applying the grayscale conversion filter
    Image Apply(const Image& image) const override;
};

// Class for obtaining the negative of an image
class NegativeFilter : public ImageFilter {
public:
    // Applying the negative filter
    Image Apply(const Image& image) const override;
};

// Class for sharpening an image
class SharpeningFilter : public ImageFilter {
public:
    // Applying the sharpening filter
    Image Apply(const Image& image) const override;
};

// Class for edge detection on an image
class EdgeDetectionFilter : public ImageFilter {
public:
    explicit EdgeDetectionFilter(float threshold) : threshold_(threshold) {
    }

    // Applying the edge detection filter
    Image Apply(const Image& image) const override;

private:
    float threshold_;
};

// Class for Gaussian blur on an image
class GaussianBlurFilter : public ImageFilter {
public:
    explicit GaussianBlurFilter(float sigma) : sigma_(sigma) {
    }

    // Applying the Gaussian blur filter
    Image Apply(const Image& image) const override;

private:
    float sigma_;
};
// Class for pixelation of an image
class PixelationFilter : public ImageFilter {
public:
    PixelationFilter();
    explicit PixelationFilter(int pixel_size) : pixel_size_(pixel_size) {
    }

    // Applying the pixelation filter
    Image Apply(const Image& image) const override;

private:
    int pixel_size_;
};
#endif  // IMAGEFILTER_H
