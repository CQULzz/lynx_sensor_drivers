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

#include "FFT_Buffer.h"

namespace Navtech::Navigation {

    FFT_Buffer::FFT_Buffer(Buffer_mode buffer_mode, std::size_t sample_sz) :
        mode { buffer_mode },
        samples { sample_sz },
        bytes_to_floats { 
            [this] (const std::vector<std::uint8_t>& fft_data) { return default_conversion(fft_data); }
        }
    {

    }


    FFT_Buffer::FFT_Buffer(Buffer_mode buffer_mode, std::size_t sample_sz, FFT_Buffer::Conversion_func bytes_to_floats) :
        mode { buffer_mode },
        samples { sample_sz },
        bytes_to_floats { bytes_to_floats }
    { }


    // -----------------------------------------------------------------------------------------------------------------
    // Public Member Functions
    //
    std::optional<std::vector<float>> FFT_Buffer::process_fft(
        const std::vector<float>& fft_data
    )
    {
        return buffer_data(fft_data);
    }


    std::optional<std::vector<float>> FFT_Buffer::process_fft(
        const std::vector<std::uint8_t>& fft_data
    )
    {
        return buffer_data(bytes_to_floats(fft_data));
    }


    // -----------------------------------------------------------------------------------------------------------------
    // Private member functions
    //
    std::vector<float> FFT_Buffer::default_conversion(const std::vector<std::uint8_t>& fft_data)
    {
        std::vector<float> out (fft_data.size());

        std::transform(fft_data.begin(), fft_data.end(), 
                        out.begin(), 
                        [] (std::uint8_t f) { return f * 0.5f; });

        return out;
    }


    std::optional<std::vector<float>> FFT_Buffer::buffer_data(
        const std::vector<float>& fft_data
    )
    {
        std::vector<float> adjusted_data { };
        switch (mode) {
            case Buffer_mode::off:
                // This case should be unreachable, but just in case:
                //
                return fft_data;
                break;
            case Buffer_mode::average:
                buffered_data.emplace_back(fft_data.begin(), fft_data.end());

                if (buffered_data.size() < samples) return { };

                adjusted_data.resize(fft_data.size());

                for (std::uint16_t bin { 0 }; bin < fft_data.size(); ++bin) {
                    float total { 0.0f };
                    for (auto& azimuth : buffered_data) {
                        float value { static_cast<float>(azimuth[bin]) / 2.0f };
                        total += std::pow(10.0f, value / 10.0f);
                    }
                    adjusted_data[bin] = std::log10(total / buffered_data.size()) * 10.0f;
                }

                buffered_data.clear();
                
                break;
            case Buffer_mode::max:
                float max { };

                adjusted_data.resize(fft_data.size());
                for (std::uint16_t bin = 0; bin < fft_data.size(); bin++) {
                    for (std::size_t counter = 0; counter < buffered_data.size(); counter++) {
                        float value { buffered_data[counter][bin] / 2.0f };
                        if (value > max) { max = value; }
                    }
                    adjusted_data[bin] = max;
                }

                break;
        }

        return adjusted_data;
    }
} // namespace Navtech::Navigation