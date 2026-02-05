#ifndef SHAPE_FINDER_H
#define SHAPE_FINDER_H
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

#include <algorithm>
#include <array>
#include <deque>
#include <optional>
#include <utility>
#include <vector>
#include "Log.h"

#include "float_equality.h"

#ifdef __GNUC__
    #include <ext/pb_ds/assoc_container.hpp>
    // When using GNU compilers, a policy hash map becomes available. I found out about it here:
    // https://codeforces.com/blog/entry/60737
    // Using this with its default hashing function provides a roughly 4x speed increase. 
    // On a 400*2856 grid containing 8 shapes, unordered_map will take ~300ms to complete 
    // (~220 if using unordered set and reserving 2048 bytes). 
    // With gp_hash_table, the same grid could be processed in ~75-78 ms.
    // The trade off is higher memory usage: according to a comment left here 
    // https://github.com/kth-competitive-programming/kactl/blob/main/content/data-structures/HashMap.h,
    // gp_hash_table uses ~50% more memory than unordered_map. 
    // Compared to unordered_set, both gp_hash_table and unordered_map will require more memory.
    template <typename Key_Ty, typename Value_Ty>
    using Hash_map = __gnu_pbds::gp_hash_table<Key_Ty, Value_Ty>;

#else
    #include <unordered_map>
    template <typename Key_Ty, typename Value_Ty>
    using Hash_map = std::unordered_map<Key_Ty, Value_Ty>;
#endif

namespace Navtech::Utility {

template<typename T>
class Shape_finder {
    
public:
    Shape_finder(int minimum_col);

    std::vector<std::pair<float, float>>    find_centres(const std::vector<std::vector<T>>& data);
private:
    Hash_map<int, bool>     explored_cells   { };

    // Depth-first search directions
    //
    static constexpr std::array<std::array<int, 2>, 4>  directions { {{0, 1}, {1, 0}, {0, -1}, {-1, 0}} };

    // Cells to test next
    //
    std::deque<std::pair<int, int>>     test_cells  { };
    
    int     min_col { 0 };
    int     rows    { 0 };
    int     cols    { 0 };
    
    int    wrap_row(int x);
    
    std::optional<std::pair<float, float>>     depth_first_search(const std::vector<std::vector<T>>& data);
};

template<typename T>
Shape_finder<T>::Shape_finder(int minimum_col) : min_col {minimum_col}
{

}


template<typename T>
std::vector<std::pair<float, float>> Shape_finder<T>::find_centres(const std::vector<std::vector<T>>& data)
{
    std::vector<std::pair<float, float>>    output_pairs    { };
    rows = static_cast<int>(data.size());
    cols = static_cast<int>(data[0].size());

    for (auto r { 0 }; r < rows; ++r) {
        for (auto c { 0 }; c < cols; ++c) {
            int placement { r * (rows - 1) + c };
            if (c < min_col || essentially_equal(static_cast<float>(data[r][c]), 0.0f) || explored_cells[placement])
                continue;

            test_cells.emplace_back(r, c);

            auto shape_centre = depth_first_search(data);
            
            if (!shape_centre.has_value()) continue;

            output_pairs.emplace_back(shape_centre.value());
        }
    }
    return output_pairs;
}


template<typename T>
int Shape_finder<T>::wrap_row(int r)
{
    auto wrapped { r };
    if (wrapped < 0) {
        wrapped = rows + r;
    }
    else if (wrapped >= rows) {
        wrapped = r - rows;
    }

    return wrapped;
}


template<typename T>
std::optional<std::pair<float, float>> Shape_finder<T>::depth_first_search(const std::vector<std::vector<T>>& data)
{
    using Utility::essentially_equal;

    auto    shape_size      { 0 };
    float   total_mass      { 0.0f };
    float   row_moment_sum  { 0.0f };
    float   col_moment_sum  { 0.0f };

    while (!test_cells.empty()) {
        auto [r, c] = test_cells.front();
        test_cells.pop_front();

        auto r2 = wrap_row(r);
        int cell_location { r2 * (rows - 1) + c };

        // MSVC will not cast to a floating point type any integer passed to std::isnan
        // It should do this, according to the standard, but it doesn't.
        // Explicitly casting to a float prevents headaches on Windows.
        // Compiler optimisations should convert the std::isnan(int) to false
        // So this fix really only applies to Debug builds.
        //
        auto data_as_float = static_cast<float>(data[r2][c]);

        if (
            data[r2].empty() || 
            std::isnan(data_as_float) ||
            essentially_equal(data_as_float, 0.0f) ||
            explored_cells[cell_location]
        )
        {
            continue;
        }

        shape_size++;
        float current_mass = static_cast<float>(data[r2][c]);
        total_mass += current_mass;
        col_moment_sum += current_mass * c;
        row_moment_sum += current_mass * r;

        explored_cells[cell_location] = true;

        for (auto [dr, dc] : directions) {
            int nr = r + dr;
            int nr2 = wrap_row(nr);

            int nc = c + dc;

            if (nc >= 0 && nc < cols && !explored_cells[nr2 * (rows - 1) + nc]) {
                test_cells.emplace_back(nr, nc);
            }
        }
    }

    if (shape_size == 1) return { };

    return std::make_pair<float, float>(row_moment_sum / total_mass, col_moment_sum / total_mass);
}
} // namespace Navtech::Utility
#endif