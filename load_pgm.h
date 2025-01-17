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
    static constexpr char MAGIC_NUMBER[] = "P2";
    static constexpr int MAX_ALLOWED_VALUE = 65535;
    static constexpr char COMMENT_CHAR = '#';

    constexpr bool is_valid_dimensions(int width, int height) noexcept {
        return width > 0 && height > 0;
    }

    constexpr bool is_valid_max_value(int max_val) noexcept {
        return max_val > 0 && max_val <= MAX_ALLOWED_VALUE;
    }

    template<PixelType T>
    constexpr bool is_within_type_limits(int value) noexcept {
        return value <= std::numeric_limits<T>::max();
    }

    [[nodiscard]] inline std::stringstream preprocess_pgm_file(const std::string_view filename) {
        std::ifstream file{filename.data()};
        if (!file) {
            throw std::runtime_error(std::format("Failed to open PGM file: {}", filename));
        }

        std::stringstream clean_content;
        std::string line;
        
        while (std::getline(file, line)) {
            const auto comment_pos = line.find(COMMENT_CHAR);
            if (comment_pos != 0 && !line.empty()) {
                clean_content.write(line.data(), 
                    comment_pos != std::string::npos ? comment_pos : line.length());
                clean_content.put('\n');
            }
        }
        return clean_content;
    }

    constexpr void validate_pgm_header(std::string_view magic, 
                                     int width, int height, 
                                     int max_val, 
                                     std::string_view filename) {
        if (magic != MAGIC_NUMBER) {
            throw std::runtime_error(std::format("Invalid PGM format in file: {}", filename));
        }

        if (!is_valid_max_value(max_val)) {
            throw std::runtime_error(std::format("Invalid maxVal {} in PGM file: {}", 
                                               max_val, filename));
        }

        if (!is_valid_dimensions(width, height)) {
            throw std::runtime_error(std::format("Invalid dimensions {}x{} in PGM file: {}", 
                                               width, height, filename));
        }
    }
}

template <PixelType ValueType = unsigned char>
[[nodiscard]] TG::Image<ValueType> load_pgm(const std::string_view filename) {
    auto content = detail::preprocess_pgm_file(filename);

    std::string magic;
    int width, height, max_val;
    content >> magic >> width >> height >> max_val;

    detail::validate_pgm_header(magic, width, height, max_val, filename);

    if (!detail::is_within_type_limits<ValueType>(max_val)) {
        throw std::runtime_error(
            std::format("Maximum intensity {} exceeds data type range in file: {}", 
                       max_val, filename)
        );
    }

    TG::Image<ValueType> image(width, height);
    
    constexpr auto convert_value = [](int val) constexpr -> ValueType {
        return static_cast<ValueType>(val);
    };

    for (const auto y : std::views::iota(0, height)) {
        for (const auto x : std::views::iota(0, width)) {
            int val;
            content >> val;
            image(x, y) = convert_value(val);
        }
    }

    return image;
}


#endif // PGM_H