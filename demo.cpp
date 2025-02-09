#include <random>
#include <cmath>
#include <vector>
#include <stdexcept>
#include <iostream>
#include <fstream>
#include <format>

#include "terminal_graphics.h"
#include "load_pgm.h"



/**
 * This function calculates the frequency distribution of pixel values 
 * (ranging from 0 to 255) in a grayscale image and visualizes it as a histogram.
 */
// void plot_histogram(const TG::Image<unsigned char>& image) {
//     std::vector<int> histogram(256, 0);

//     for (int y = 0; y < image.height(); ++y) {
//         for (int x = 0; x < image.width(); ++x) {
//             int pixel_value = image(x, y);
//             if (pixel_value >= 0 && pixel_value < 256) {
//                 histogram[pixel_value]++;
//             }
//         }
//     }

//     std::vector<int> cropped_histogram(histogram.begin() + 1, histogram.begin() + 255);

//     int max_value = *std::max_element(cropped_histogram.begin(), cropped_histogram.end());

//     std::cout << "Plotting the histogram of pixel values as a bar plot:\n";

//     auto plot = TG::plot(768, 256)
//                     .set_xlim(1, 255)
//                     .set_ylim(0, max_value);

//     for (int i = 0; i < cropped_histogram.size(); ++i) {
//         float x = 51 + i;
//         float y = static_cast<float>(cropped_histogram[i]);
//         plot.add_line(static_cast<float>(x), 0.0f, static_cast<float>(x), y, 2);
//     }
// }


int main (int argc, char* argv[])
{
  try {
    // demonstrate use of TG::imshow():

    const std::string image_filename = "brain.pgm";

    const auto image = load_pgm (image_filename);
    std::cout << std::format ("Showing image \"{}\", size: {} x {}\n", image_filename, image.width(), image.height());
    TG::imshow (image, 0, 255);

    // plot_histogram(image);

    std::cout << "Same image magnified by a factor of 2:\n";
    TG::imshow (TG::magnify (image, 2), 0, 255);
 // ToDO: TG::imshow(TG::Rotate (image, TG::AN))
    TG::imshow(TG::Rotate_90(image, TG::ANGLE::D_180), 0, 255);


    const auto histogram_data = TG::get_histogram_data(image);

    int max_value = *std::max_element(histogram_data.begin(), histogram_data.end());

    auto plot = TG::plot(768, 256)
                    .set_xlim(1, 255)
                    .set_ylim(0, max_value);

    for (int i = 0; i < static_cast<int>(histogram_data.size()); ++i) {
      float x = 1 + i; 
      float y = static_cast<float>(histogram_data[i]);

      // non-zero
      if (y > 0) {
        plot.add_line(static_cast<float>(x), 0.0f, static_cast<float>(x), y, 2);
      }
    }


    auto image_char = load_pgm(image_filename);
    auto image_short = TG::convert_image_to_unsigned_short<unsigned char, unsigned short>(image_char);



    std::cout << "Displaying original image:\n";
    TG::imshow(image_char, 0, 255);

    std::cout << "Displaying converted image:\n";
    TG::imshow(image_short, 0, 65535);

    int block_size = 35;
    auto binary_image = TG::adaptive_threshold_blockwise(image_char, block_size);

    std::cout << "Displaying binary thresholded image:\n";
    TG::imshow(binary_image, 0, 255);
    // demonstate use of TG::plot():
    int kernel_size = 5; // Must be odd
    double sigma = 1.0;
    auto smoothed_image = TG::apply_gaussian_filter(image, kernel_size, sigma);

    std::cout << "Displaying Gaussian filtered image:\n";
    TG::imshow(smoothed_image, 0, 255);

    std::cout << "Displaying original image:\n";
    TG::imshow(image, 0, 255);

    std::vector<std::vector<float>> sobel_x = {
        {-1,  0,  1},
        {-2,  0,  2},
        {-1,  0,  1}
    };

    auto edge_image = TG::convolve(image, sobel_x, TG::PaddingType::REPLICATE);


    std::cout << "Displaying edge-detected image using Sobel-X filter:\n";
    TG::imshow(edge_image, 0, 255);

    // Run-Length Encoding
    auto encoded_data = TG::run_length_encode(image);
    std::cout << "Image encoded using Run-Length Encoding\n";
    
    // Save to file
    const std::string encoded_filename = "encoded_image.rle";
    TG::save_encoded_to_file(encoded_data, encoded_filename);
    std::cout << "Encoded data saved to file: " << encoded_filename << "\n";
    
    // Load encoded data from file
    auto loaded_encoded_data = TG::load_encoded_from_file<unsigned char>(encoded_filename);
    std::cout << "Encoded data loaded from file\n";
    
    // Decode the image
    auto decoded_image = TG::run_length_decode(loaded_encoded_data, image.width(), image.height());
    std::cout << "Decoded image constructed\n";

    std::cout << "Displaying decoded image:\n";
    TG::imshow(decoded_image, 0, 255);

    auto polar_image = TG::cartesian_to_polar(image);
    std::cout << "Image converted to Polar coordinates with Gaussian filtering\n";
    
    // Convert back to Cartesian
    auto reconstructed_image = TG::polar_to_cartesian(polar_image, image.width(), image.height());
    std::cout << "Image converted back to Cartesian coordinates\n";

    std::cout << "Displaying polar image with Gaussian smoothing:\n";
    TG::imshow(polar_image, 0, 255);
    
    std::cout << "Displaying reconstructed Cartesian image:\n";
    TG::imshow(reconstructed_image, 0, 255);

    std::vector<float> x (50);
    std::vector<float> y (50);



    for (unsigned int x = 0; x < y.size(); ++x)
      y[x] = exp (-0.1*x) - 1.5*exp (-0.4*x);

    std::cout << "A simple one-line plot:\n";
    TG::plot().add_line (y); 




    for (std::size_t n = 0; n < x.size();++n) {
      y[n] = std::sin (0.2*n) + 0.3*std::cos (0.33*n);
      x[n] = 20.0+10.0*std::cos (0.41*n) + 5.0*std::sin(0.21*n);
    }

    std::cout << "Plotting arbitrary lines:\n";
    TG::plot (768, 256)
      .add_line (y, 4, 10)
      .add_line (x, y, 3)
      .add_text ("sinusoids", (x.size()-1)/2.0, 1.2, 0.5, 0.0, 6);




    // a plot of random numbers:

    // set up random number generator for Normal distribution:
    std::random_device rd;
    std::mt19937 gen (rd());
    std::normal_distribution normal (5.0, 2.0);

    // generate vector of Normally-distributed random numbers:
    std::vector<float> noise (256);
    for (auto& x : noise)
      x = normal (gen);

    std::cout << "Plotting Normally distributed random variables:\n";
    TG::plot (768, 256)
      .set_ylim (-1, 11)
      .set_grid (50, 2)
      .add_line (noise,2);

  }
  catch (std::exception& e) {
    std::cerr << "error: " << e.what() << std::endl;
    return 1;
  }

  return 0;
}

