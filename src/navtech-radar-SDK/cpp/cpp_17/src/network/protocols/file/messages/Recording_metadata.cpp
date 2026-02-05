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
#include "Recording_metadata.h"
#include "net_conversion.h"
#include <utility>

namespace Navtech::Networking::Offline {
    
    using Networking::to_uint64_host;
    using Networking::to_uint32_host;

    // Constructors
    //
    Metadata::Metadata(const std::vector<std::uint8_t>& message_buffer)
    {
        data = message_buffer;
    }


    Metadata::Metadata(std::vector<std::uint8_t>&& message_buffer) :
    data { std::move(message_buffer) }
    { }


    // Accessors
    //
    std::uint64_t   Metadata::start_date()
    {
        auto header = Header::overlay_onto(data.data());
        return to_uint64_host(header->start_date);
    }


    std::uint64_t   Metadata::end_date()
    {
        auto header = Header::overlay_onto(data.data());
        return to_uint64_host(header->end_date);
    }


    std::uint64_t   Metadata::start_ticks()
    {
        auto header = Header::overlay_onto(data.data());
        return to_uint64_host(header->start_ticks);
    }


    std::uint64_t   Metadata::end_ticks()
    {
        auto header = Header::overlay_onto(data.data());
        return to_uint64_host(header->end_ticks);
    }


    Networking::IP_address  Metadata::ip_address()
    {
        auto header = Header::overlay_onto(data.data());
        return Networking::IP_address { to_uint32_host(header->radar_ip) };
    }


}