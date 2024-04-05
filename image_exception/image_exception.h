#ifndef IMAGE_EXCEPTION_H
#define IMAGE_EXCEPTION_H

#include <stdexcept>
#include <string>

class ImageException : public std::runtime_error {
public:
    explicit ImageException(const std::string& message) : std::runtime_error(message) {
    }
};

class FileOpenException : public ImageException {
public:
    explicit FileOpenException(const std::string& message) : ImageException(message) {
    }
};

class InvalidBMPFormatException : public ImageException {
public:
    explicit InvalidBMPFormatException(const std::string& message) : ImageException(message) {
    }
};

#endif  // IMAGE_EXCEPTION_H
