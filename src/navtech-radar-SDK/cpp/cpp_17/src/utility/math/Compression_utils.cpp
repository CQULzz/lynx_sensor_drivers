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

#include "Compression_utils.h"

#include <cstring>
#include <algorithm>
#include <zlib.h>


namespace Navtech::Utility::Compression {
    
    namespace GZIP 
    {
        std::vector<std::uint8_t> decompress(std::vector<std::uint8_t>& compressed)
        {
            return Compression::decompress(compressed, -MAX_WBITS);
        }
    } // namespace ZLIB
    

    namespace ZLIB
    {
        std::vector<std::uint8_t> decompress(std::vector<std::uint8_t>& compressed)
        {
            return Compression::decompress(compressed, 16 + MAX_WBITS);
        }        
    } // namespace ZLIB


    std::vector<std::uint8_t> decompress(std::vector<std::uint8_t>& compressed, int wbits) 
    {
        // Adapted from https://www.zlib.net/zlib_how.html
        //
        const std::uint32_t chunk_size { 
            std::min(std::uint32_t { 1024 }, static_cast<std::uint32_t>(compressed.size())) 
        };
        std::vector<std::uint8_t> decompressed { };

        z_stream stream { };
        std::memset(&stream, 0, sizeof(z_stream));

        stream.next_in = reinterpret_cast<Bytef*>(compressed.data());
        stream.avail_in = static_cast<std::uint32_t>(compressed.size());

        if (::inflateInit2(&stream, wbits) != Z_OK) return decompressed;
        
        std::vector<std::uint8_t> out_buff(chunk_size);

        int ret { Z_OK };

        do {
            stream.avail_out = chunk_size;
            stream.next_out = reinterpret_cast<Bytef*>(out_buff.data());

            ret = ::inflate(&stream, Z_NO_FLUSH);

            decompressed.insert(decompressed.end(), out_buff.begin(), out_buff.end());
        } while (ret == Z_OK);
        

        ::inflateEnd(&stream);
        
        decompressed.resize(stream.total_out);
        decompressed.erase(decompressed.begin() + stream.total_out, decompressed.end());

        return decompressed;
    }

} // namespace Navtech::Utility::Compression