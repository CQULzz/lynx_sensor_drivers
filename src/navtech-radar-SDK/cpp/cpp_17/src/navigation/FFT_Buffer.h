#ifndef FFT_BUFFER_H
#define FFT_BUFFER_H
// ---------------------------------------------------------------------------------------------------------------------
// Copyright 2025 Navtech Radar Limited
// This file is part of IASDK which is released under The MIT License (MIT).
// See file LICENSE.txt in project root or go to https://opensource.org/licenses/MIT
// for full license details.
//
// Disclaimer:
// Navtech Radar is furnishing this item "as is". Navtech Radar does not provide 
// any warranty of the item whatsoever, whether express, implied, or statutory,
// including, but not limited to, any warranty of merchantability or fitness
// for a particular purpose or any warranty that the contents of the item will
// be error-free.
// In no respect shall Navtech Radar incur any liability for any damages, including,
// but limited to, direct, indirect, special, or consequential damages arising
// out of, resulting from, or any way connected to the use of the item, whether
// or not based upon warranty, contract, tort, or otherwise; whether or not
// injury was sustained by persons or property or otherwise; and whether or not
// loss was sustained from, or arose out of, the results of, the item, or any
// services that may be provided by Navtech Radar.
// ---------------------------------------------------------------------------------------------------------------------

#include <cstdint>
#include <deque>
#include <functional>
#include <optional>
#include <vector>

#include "Buffer_mode.h"
#include "Units.h"

// This file defines a class for buffering incoming fft data in one of two modes: Average or Max Buffering.
// In both modes, samples of data are taken for each azimuth up to sample_sz. Once collection has finished,
// it is then processed in one of the two following ways:
//  1. Average buffering
//     For reach azimuth, the average of each bin is calculated from the samples.
//  2. Max buffering
//     For each azimuth, only the maximum value of each bin across all samples is taken.
//
// A row of FFT is returned if and only if the minimum number of samples has been reached.
//
// The averaging mode will work best with a staring radar, or a radar producing only a few azimuths of data.
//
namespace Navtech::Navigation {

class FFT_Buffer {
public:
    using Conversion_func   = std::function<std::vector<float>(const std::vector<std::uint8_t>&)>;

    FFT_Buffer(Buffer_mode mode, std::size_t sample_sz);

    // Overload, in case some other conversion function is desired
    //
    FFT_Buffer(Buffer_mode mode, std::size_t sample_sz, Conversion_func bytes_to_floats);

    // Process FFT...
    // as floats
    //
    std::optional<std::vector<float>> process_fft(const std::vector<float>& fft_data);
    // As raw bytes (with a simple conversion function)
    //
    std::optional<std::vector<float>> process_fft(const std::vector<std::uint8_t>& fft_data);

private:
    Buffer_mode                     mode             { Buffer_mode::off };
    std::size_t                     samples          { };
    std::deque<std::vector<float>>  buffered_data    { };

    std::function<std::vector<float>(const std::vector<std::uint8_t>&)> bytes_to_floats { };

    std::vector<float> default_conversion(const std::vector<std::uint8_t>& fft_data);

    std::optional<std::vector<float>> buffer_data(const std::vector<float>& fft_data);
};


} // namespace Navtech::Navigation
#endif