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
#include "CFAR_Peak_finder.h"
#include "Vector_maths.h"
#include "Log.h"
#include "float_equality.h"
#include "Centre_of_mass.h"
#include "Shape_finder.h"

using namespace Navtech::Networking::Colossus_protocol;

namespace Navtech::Navigation {

    CFAR_Peak_finder::CFAR_Peak_finder() : 
       Active { "CFAR Peak Finder "}
    {
    }

    
    // -----------------------------------------------------------------------
    // Public methods
    //
    void CFAR_Peak_finder::configure(
        const TCP::Configuration&       cfg_msg,
        Unit::Bin                       min_bin,
        Unit::Bin                       max_pks,
        Subresolution_mode              subresolution_mode,
        Peak_mode                       peak_type
    ) 
    {
        range_in_bins           = cfg_msg.range_in_bins();
        range_gain              = cfg_msg.range_gain();
        range_offset            = cfg_msg.range_offset();
        range_resolution        = cfg_msg.bin_size() / 10000.0f;

        minimum_bin             = min_bin;
        max_peaks               = max_pks;
        min_range               = minimum_bin * range_resolution;
        max_range               = range_in_bins * range_resolution;
        azimuth_samples         = cfg_msg.azimuth_samples();
        steps_per_azimuth       = static_cast<float>(cfg_msg.encoder_size() / azimuth_samples);

        mode                    = subresolution_mode;
        peak_mode               = peak_type;

        if (mode == Subresolution_mode::centre_of_mass_2d) {
            rotation_data.resize(azimuth_samples);
        }

        to_degrees = [this] (float a)  { 
            return fmod(a * 360.0f / static_cast<float>(azimuth_samples) + 360.0f, 360.0f); 
        };
        
        to_metre        = [this] (float b)      { return (b * range_gain * range_resolution) + range_offset; };
    }


    void CFAR_Peak_finder::find_peaks(
        Unit::Azimuth               azimuth,
        const std::vector<float>&   cfar_data
    )
    {
        async_call(
            &CFAR_Peak_finder::on_find_peaks,
            this,
            azimuth,
            cfar_data
        );
    }


    void CFAR_Peak_finder::set_target_callback(std::function<void(const CFAR_Target&)> fn)
    {
        target_callback = std::move(fn);
    }

    
    // ------------------------------------------------------------------------
    // Private members
    //
    void CFAR_Peak_finder::on_find_peaks(            
        Unit::Azimuth azimuth,
        const std::vector<float>& cfar_data
    )
    {
        process_data(azimuth, cfar_data);       
    }


    void CFAR_Peak_finder::process_data(Unit::Azimuth azi_idx, const std::vector<float>& cfar_data)
    {
        // It's possible for the data to be contoured
        //
        std::vector<float> resized_data { cfar_data };
        resized_data.resize(range_in_bins);

        auto min_it = resized_data.cbegin() + minimum_bin;
        auto max_it = select_peak(min_it, resized_data.end());

        // This is so rows aren't skipped, there's certainly a better way of doing this
        //
        if (Utility::essentially_equal(*max_it, 0.0f) && mode != Subresolution_mode::centre_of_mass_2d) return;

        auto forward_it = max_it;
        auto reverse_it = max_it;

        // Create a window for subresolution
        // by walking two pointers forward, and backward, respectively, until 
        // The first non-zero point/below threshold point is found.
        //
        while (forward_it < resized_data.end() - 1) {
            if (*(forward_it + 1) <= 0) break;
            ++forward_it;
        } 
        while (reverse_it >= (min_it + 1)) {
            if (*(reverse_it - 1) <= 0) break;
            --reverse_it;
        }

        auto window_sz = static_cast<Unit::Bin>(std::distance(reverse_it, forward_it));
        auto peak_bin = static_cast<Unit::Bin>(std::distance(resized_data.cbegin(), max_it));
        
        float           resolved_bin    { };

        switch (mode) {
            case Subresolution_mode::curve_fit:
                resolved_bin = peak_resolve(resized_data, peak_bin, window_sz);
                send_target(resolved_bin, azi_idx);
                break;
            case Subresolution_mode::centre_of_mass: 
                {
                    auto first_bin = std::distance(resized_data.cbegin(), reverse_it);
                    auto window_start = resized_data.begin() + first_bin;
                    auto window_end = window_start + window_sz;
                    resolved_bin = first_bin + Utility::centre_of_mass(window_start, window_end);
                    send_target(resolved_bin, azi_idx);
                }
                break;
            case Subresolution_mode::centre_of_mass_2d:
                if (azi_idx < last_azimuth) {
                    counter++;
                    if (counter >= 2) {
                        find_shapes(rotation_data);
                    }
                }

                if (counter >= 1) {
                    std::vector<Unit::dB> reduced_points(resized_data.size(), 0.0);
                    auto peaks { 0 };
                    for (auto i { minimum_bin }; i < resized_data.size(); i++) {
                        if (Utility::essentially_equal(resized_data[i], 0.0f)) continue;
                        reduced_points[i] = resized_data[i];
                        peaks++;
                        if (peaks >= max_peaks) break;
                    }

                    rotation_data[azi_idx] = reduced_points;
                }

                last_azimuth = azi_idx;
                return;
        }
    }


    std::vector<float>::const_iterator CFAR_Peak_finder::select_peak(
            std::vector<float>::const_iterator begin,
            std::vector<float>::const_iterator end
    ) const
    {
        switch (peak_mode) {
            case (Peak_mode::max) :
                return std::max_element(begin, end);
            case (Peak_mode::first): {
                // Find the first non-zero value
                //
                auto first_non_zero = std::find_if(begin, end, [] (float f) { return f > 0.0f; });
                if (first_non_zero == end) return end;

                // Continue until finished rising 
                //
                for (auto itr { first_non_zero }; itr < end - 1; ++itr) {
                    if (*(itr + 1) > *itr) continue;
                    return itr;
                }

                return end - 1;
            }
        }
        
        return end - 1;
    }


    void CFAR_Peak_finder::send_target(float resolved_bin, float resolved_azimuth)
    {
        auto range = to_metre(resolved_bin);
        auto bearing = to_degrees(resolved_azimuth);

        if (std::isinf(range) || range < min_range || range > max_range) return;

        if (target_callback) {
            target_callback({ bearing, range });
        }
    }


    float CFAR_Peak_finder::peak_resolve(
        const std::vector<float>& data,
        Unit::Bin peak_bin,
        Unit::Bin window_sz
    )
    {   
        if (window_sz == 0) return peak_bin;
        window_sz = (window_sz <= 5) ? 5 : window_sz; 
        const std::uint8_t bins_to_offset   { static_cast<uint8_t>((window_sz - 1) / 2) };
        float x[max_bins_to_operate_on]     { 0.0 };
        float y[max_bins_to_operate_on]     { 0.0 };
        auto index                          { 0 };
        auto startValue                     { 0 - bins_to_offset };

        for (index = 0; index < window_sz; index++) {
            x[index] = static_cast<float>(startValue++);
        }

        auto startBin { peak_bin - bins_to_offset };

        for (index = 0; index < window_sz; index++) {
            y[index] = data[startBin + index];
        }

        float Sx  { };
        float Sx2 { };
        float Sx3 { }; 
        float Sx4 { };
        float x2[max_bins_to_operate_on] { };
        float x3[max_bins_to_operate_on] { };
        float x4[max_bins_to_operate_on] { };

        Vector_maths::scalar_sum(x, window_sz, Sx);
        Vector_maths::scalar_square(x, window_sz, Sx2);
        Vector_maths::vector_cube(x, window_sz, x3);
        Vector_maths::scalar_sum(x3, window_sz, Sx3);
        Vector_maths::vector_square(x, window_sz, x2);
        Vector_maths::vector_multiply(x2, x2, window_sz, x4);
        Vector_maths::scalar_sum(x4, window_sz, Sx4);

        float Sy   { }; 
        float Sxy  { };
        float Sx2y { };
        float xy[max_bins_to_operate_on]  { };
        float x2y[max_bins_to_operate_on] { };

        Vector_maths::scalar_sum(y, window_sz, Sy);
        Vector_maths::vector_multiply(x, y, window_sz, xy);
        Vector_maths::scalar_sum(xy, window_sz, Sxy);
        Vector_maths::vector_multiply(x2, y, window_sz, x2y);
        Vector_maths::scalar_sum(x2y, window_sz, Sx2y);

        float A[4] { Sx2, Sx3, Sx4, Sx2y };
        float B[4] { Sx, Sx2, Sx3, Sxy };
        float C[4] { (float)window_sz, Sx, Sx2, Sy };

        float F = C[0] / A[0];

        for (index = 0; index <= 3; index++) {
            C[index] = C[index] - (F * A[index]);
        }

        F = B[0] / A[0];

        for (index = 0; index <= 3; index++) {
            B[index] = B[index] - (F * A[index]);
        }

        F = C[1] / B[1];

        for (index = 1; index <= 3; index++) {
            C[index] -= F * B[index];
        }

        float b2  { C[3] / C[2] };
        float b1  { (B[3] - B[2] * b2) / B[1] };

        return -b1 / (2 * b2) + startBin + (float)bins_to_offset;
    }


    void CFAR_Peak_finder::find_shapes(const std::vector<std::vector<float>> rotation_data) {

        using namespace Navtech::Utility;

        Shape_finder<float>    shape_finder { minimum_bin };

        auto shape_centres = shape_finder.find_centres(rotation_data);

        for (auto& centre : shape_centres) {
            send_target(
                centre.first,
                centre.second
            );
        }
    }
} // namespace Navtech::Navigation