//
// Created by ubushaevv on 17.03.24.
//
#pragma once
#include <vector>
#include "../image_exception/image_exception.h"
#include "cstdint"

struct Colour {
    float red_;
    float green_;
    float blue_;
    Colour();  // Default Constructor
    Colour(float red, float green, float blue);
    ~Colour();
};
class Image {
public:
    Image(int width, int height);
    Image();
    ~Image();
    int GetWidth() const {
        return width_;
    };
    int GetHeight() const {
        return height_;
    };
    Colour GetColour(int x, int y) const;
    void SetColour(const Colour& colour, int x, int y);

    void Read(const char* path);
    void Export(const char* path) const;

private:
    int32_t width_;
    int32_t height_;
    std::vector<Colour> data_;
};
