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
#ifndef CFAR_PEAK_FINDER_H
#define CFAR_PEAK_FINDER_H

#include <string_view>

#include "Buffer_mode.h"
#include "Colossus_protocol.h"
#include "Units.h"
#include "File_writer.h"
#include "CFAR_algorithms.h"
#include "configurationdata.pb.h"
// #include "Active.h"

namespace Navtech::Navigation {

    enum class Subresolution_mode { curve_fit, centre_of_mass, centre_of_mass_2d };

    enum class Peak_mode { max, first };

    struct CFAR_Target {
        CFAR_Target(float bearing, float range) :
            bearing(bearing), range(range) 
            {}
        
        float bearing;
        float range;
    };


    class CFAR_Peak_finder : public Navtech::Utility::Active {
    public:
        using float_it = std::vector<float>::iterator;

        CFAR_Peak_finder();
        
        void configure(
            const Networking::Colossus_protocol::TCP::Configuration&        cfg_msg,
            Unit::Bin                                                       min_bin,
            Unit::Bin                                                       max_peaks,
            Subresolution_mode                                              subresolution_mode,
            Peak_mode                                                       peak_type = Peak_mode::max
        );
    
        void set_target_callback(std::function<void(const CFAR_Target&)> fn);
        
        void find_peaks(
            Unit::Azimuth               azimuth,
            const std::vector<float>&   cfar_data
        );
        
    private:
        float           range_gain                      { 0.0f };
        Unit::Metre     range_offset                    { 0.0f };
        Unit::Metre     range_resolution                { 0.1752f };
        Unit::Metre     min_range                       { 0.0 };
        Unit::Metre     max_range                       { 50.0 };
        
        static const    Unit::Bin   max_bins_to_operate_on   { 15 };

        float           steps_per_azimuth               { 5600.0f / 400.0f };
        Unit::Azimuth   azimuth_samples                 { 400 };
        Unit::Bin       range_in_bins                   { 2856 };

        float           azimuth_to_bearing              { 360.0f * 400.f/5600.f };
        Unit::Bin       minimum_bin                     { 0 };
        Unit::Bin       max_peaks                       { 5 };

        Subresolution_mode              mode            { Subresolution_mode::curve_fit };
        Peak_mode                       peak_mode       { Peak_mode::max };

        std::function<float(float)>                 to_degrees      { nullptr };
        std::function<Unit::Metre(float)>           to_metre        { nullptr };
        std::function<void(const CFAR_Target&)>     target_callback { nullptr };

        std::vector<std::vector<float>>     rotation_data   { };
        std::uint16_t                       last_azimuth    { 0 };
        std::uint32_t                       counter         { 0 };

        void on_find_peaks(Unit::Azimuth azimuth, const std::vector<float>& cfar_data);

        void buffer_data(Unit::Azimuth azi_idx, const std::vector<float>& data);
        void process_data(Unit::Azimuth azi_idx, const std::vector<float>& data);

        std::vector<float>::const_iterator select_peak(
            std::vector<float>::const_iterator begin,
            std::vector<float>::const_iterator end
        ) const;

        void send_target(float resolved_bin, float resolved_bearing);
    
        float peak_resolve(
            const std::vector<float>& data,
            Unit::Bin peak_bin,
            Unit::Bin window_sz
        );

        void find_shapes(
            const std::vector<std::vector<float>> rotation_data
        );

   };

} // namespace Navtech::Navigation
#endif