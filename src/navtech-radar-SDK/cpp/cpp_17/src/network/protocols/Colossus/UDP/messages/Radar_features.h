#ifndef RADAR_FEATURE_H
#define RADAR_FEATURE_H

#include <cstdint>


namespace Navtech {

    struct Radar_features {
        enum Point_data     { none, ca_cfar, nav_mode };
        enum Protocol       { undefined, colossus, cat_240 };
        enum Modbus_mode    { disabled, master, slave, relay };

        std::uint32_t auto_tune                     : 1;
        std::uint32_t secondary_processing_module   : 1;
        std::uint32_t non_contour_data              : 1;
        std::uint32_t contour_map_defined           : 1;
        std::uint32_t sector_blanking               : 1;
        std::uint32_t fft_protocol                  : 2;
        std::uint32_t low_precision_output          : 1;
        std::uint32_t high_precision_output         : 1;
        std::uint32_t point_data_output             : 2;
        std::uint32_t modbus_mode                   : 2;
        std::uint32_t motor_enabled                 : 1;
        std::uint32_t safeguard_enabled             : 1; 
        std::uint32_t                               : 17;


        Radar_features() = default;

        Radar_features(std::uint32_t init)       
        {
            union Convertor {
                Radar_features flags;
                std::uint32_t word;
            };

            Convertor convertor { };

            convertor.word = init;
            *this = convertor.flags;
        }

        std::uint32_t to_uint32() const         
        { 
            union Convertor {
                Radar_features flags;
                std::uint32_t word;
            };

            Convertor convertor { };

            convertor.flags = *this;
            return convertor.word; 
        }
    };

    static_assert(sizeof(Radar_features) == 4, "Radar feature packing bits incorrect"); 

} // Navtech::Core::Configuration::Radar


#endif // RADAR_FEATURE_H