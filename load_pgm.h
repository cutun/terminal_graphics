#ifndef PGM_H
#define PGM_H

#include <fstream>
#include <string>
#include <sstream>
#include <stdexcept>
#include <limits>
#include <vector>
#include <span>
#include <concepts>
#include <ranges>
#include <format>
#include "terminal_graphics.h"

// Simple function to load an ascii-encoded PGM grayscale image.
// This returns an object of type TG::Image<ValueType>

template<typename T>
concept PixelType = std::unsigned_integral<T> || std::floating_point<T>;

namespace detail {
    [[nodiscard]] inline std::stringstream preprocess_pgm_file(const std::string_view filename) {
        std::ifstream file{filename.data()};
        if (!file) {
            throw std::runtime_error(std::format("Failed to open PGM file: {}", filename));
        }

        std::stringstream clean_content;
        std::string line;
        
        while (std::getline(file, line)) {
            if (const auto comment_pos = line.find('#'); comment_pos != std::string::npos) {
                line.resize(comment_pos);
            }
            if (!line.empty()) {
                clean_content << line << '\n';
            }
        }
        return clean_content;
    }

    inline void validate_pgm_header(std::string_view magic, int width, int height, 
                                  int max_val, std::string_view filename) {
        if (magic != "P2") {
            throw std::runtime_error(std::format("Invalid PGM format in file: {}", filename));
        }

        if (max_val >= 65536 || max_val <= 0) {
            throw std::runtime_error(std::format("Invalid maxVal {} in PGM file: {}", 
                                               max_val, filename));
        }

        if (width <= 0 || height <= 0) {
            throw std::runtime_error(std::format("Invalid dimensions {}x{} in PGM file: {}", 
                                               width, height, filename));
        }
    }
}

template <PixelType ValueType = unsigned char>
[[nodiscard]] TG::Image<ValueType> load_pgm(const std::string_view filename) {
    auto content = detail::preprocess_pgm_file(filename);

    // Read header
    std::string magic;
    int width, height, max_val;
    content >> magic >> width >> height >> max_val;

    detail::validate_pgm_header(magic, width, height, max_val, filename);

    if (max_val > std::numeric_limits<ValueType>::max()) {
        throw std::runtime_error(
            std::format("Maximum intensity {} exceeds data type range in file: {}", 
                       max_val, filename)
        );
    }

    TG::Image<ValueType> image(width, height);
    
    for (const auto y : std::views::iota(0, height)) {
        for (const auto x : std::views::iota(0, width)) {
            int val;
            content >> val;
            image(x, y) = static_cast<ValueType>(val);
        }
    }

    return image;
}

#endif // PGM_H