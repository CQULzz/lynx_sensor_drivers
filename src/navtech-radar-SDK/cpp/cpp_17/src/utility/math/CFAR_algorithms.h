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
#ifndef CA_CFAR_ALGORITHMS_H
#define CA_CFAR_ALGORITHMS_H

#include <vector>
#include <cstdint>
#include <functional>
#include <algorithm>
#include <numeric>
#include <iterator>

#include "Units.h"

namespace Navtech::CA_CFAR {

    // -----------------------------------------------------------------------------------------------------------------
    // CFAR window (input)
    //
    //    T T T T T T T T x x | x x T T T T T T T T
    //    ^               ^   ^                   ^
    //    |               |   |                   |
    //    |               |   cell-under-test     |
    //    |               guard cells(2)          |
    //    |<----          window size (21)     -->|

    struct Window {
        using Range_fn = std::function<Unit::Metre(Unit::Bin)>;

        Unit::Bin   size;                                               // *Total* window size
        Unit::Bin   guard_cells;                                        // Guard cells on each side of cell-under-test
        Unit::dB    threshold_delta { 0.0_dB };                         // Signal level *above* local average

        Window() = default;

        Window(
            Unit::Bin window_sz,
            Unit::Bin num_guard_cells
        ) :
            size            { window_sz },
            guard_cells     { num_guard_cells }
        {
            resize();
        }

        Window(
            Unit::Bin window_sz,
            Unit::Bin num_guard_cells,
            Unit::dB  delta
        ) :
            size            { window_sz },
            guard_cells     { num_guard_cells },
            threshold_delta { delta }
        {
            resize();
        }


    private:
        static constexpr Unit::Bin force_odd(Unit::Bin sz)
        {
            return (sz % 2) != 0 ? sz : sz + 1;
        }

        constexpr void resize() 
        {
            constexpr Unit::Bin min_training_cells { 1 };

            Unit::Bin min_window_sz = (2 * guard_cells) + (2 * min_training_cells) + 1;
            Unit::Bin adj_sz        = force_odd(size);
            size                    = std::max(adj_sz, min_window_sz);
        };
    };


    // -----------------------------------------------------------------------------------------------------------------
    // Bin-to-metre conversion function
    // 
    using Range_fn = std::function<Unit::Metre(Unit::Bin)>;


    // -----------------------------------------------------------------------------------------------------------------
    // Range - defines the start bin and end bin to process
    //         Non-inclusive set: start <= bin < end
    //         The struct will swap to ensure start <= end
    //
    struct Range {
        Unit::Bin start { };
        Unit::Bin end   { };

        Range() = default;

        Range(Unit::Bin first, Unit::Bin last) :
            start { std::min(first, last) },
            end   { std::max(first, last) }
        {
        }


        Range(Unit::Bin last) : 
            end { last }
        {
        }

        Range(std::size_t last) : 
            end { static_cast<Unit::Bin>(last) }
        {
        }

        std::size_t size() const
        {
            return static_cast<std::size_t>(end - start);
        }
    };

    // -----------------------------------------------------------------------------------------------------------------
    // Point (output)
    //
    struct Point {
        Unit::Metre range { };
        Unit::dB    power { };

        Point() = default;

        Point(Unit::Metre rng, Unit::dB pwr) : 
            range { rng }, 
            power { pwr }
        {
        }
    };


    // -----------------------------------------------------------------------------------------------------------------
    // Algorithms
    //
    // All points given a pointer-to-data
    //
    template <typename Iterator_Ty>    
    std::vector<Point> points(
        Iterator_Ty     data, 
        const Range&    range, 
        const Window&   window,
        const Range_fn& to_metre = [](Unit::Bin b) { return static_cast<Unit::Metre>(b * 0.175238f); }
    );


    // All points from range, given a vector
    //
    template <typename T>    
    std::vector<Point> points(
        const std::vector<T>&   data, 
        const Range&            range, 
        const Window&           window,
        const Range_fn&         to_metre = [](Unit::Bin b) { return static_cast<Unit::Metre>(b * 0.175238f); }
    );


    // All points from entire vector
    //
    template <typename T>    
    std::vector<Point> points(
        const std::vector<T>&   data, 
        const Window&           window,
        const Range_fn&         to_metre = [](Unit::Bin b) { return static_cast<Unit::Metre>(b * 0.175238f); }
    );
    
    // First n points from range, given a pointer-to-data
    //
    template <typename Iterator_Ty>
    std::vector<Point> first_n_points(
        Iterator_Ty     data, 
        const Range&    range, 
        const Window&   window, 
        std::size_t     max_points,
        const Range_fn& to_metre = [](Unit::Bin b) { return static_cast<Unit::Metre>(b * 0.175238f); }
    );


    // First n points from a range, given a vector of data
    //
    template <typename T>
    std::vector<Point> first_n_points(
        const std::vector<T>&   data,
        const Window&           window, 
        std::size_t             max_points,
        const Range_fn&         to_metre = [](Unit::Bin b) { return static_cast<Unit::Metre>(b * 0.175238f); }
    );
    

    // Run CA_CFAR on a range of values and produce a processed output
    // 
    template <typename Iterator_Ty>
    std::vector<Unit::dB> process(Iterator_Ty start, Iterator_Ty finish, const Window& window);


    // Process an entire azimuth
    //
    template <typename T>
    std::vector<Unit::dB> process(const std::vector<T>& vec, const Window& Window);


    // Process an azimuth but do not convert the output to dB; that is,
    // leave the output as 'raw' azimuth data
    //
    template <typename Iterator_Ty>
    auto process_as_raw(Iterator_Ty start, Iterator_Ty finish, const Window& window);


    // Process an entire azimuth as raw data
    //
    template <typename T>
    std::vector<T> process_as_raw(const std::vector<T>& vec, const Window& window);

    // -----------------------------------------------------------------------------------------------------------------
    // Implementation
    //
    namespace Implementation {

        // Traits.
        // When adding, for example, std::uint8_t values, overflow is a real possibility,
        // therefore use a type that can hold the largest sum.
        //
        template <typename T> 
        class Slider_traits {
        public:
            using Value_Ty  = T;
            using Sum_Ty    = T;

            static T from_dB(Unit::dB power)
            {
                return static_cast<T>(power);
            }

            static Unit::dB to_dB(T power)
            {
                return static_cast<Unit::dB>(power);
            }
        };


        template <>
        class Slider_traits<std::uint8_t> {
        public:
            using Value_Ty  = std::uint8_t;
            using Sum_Ty    = std::uint32_t;

            static std::uint8_t from_dB(Unit::dB power)
            {
                return static_cast<std::uint8_t>(power * 2);
            }

            static Unit::dB to_dB(std::uint8_t power)
            {
                return static_cast<Unit::dB>(power) / 2.0f;
            }
        };


        template <>
        class Slider_traits<std::uint16_t> {
        public:
            using Value_Ty  = std::uint16_t;
            using Sum_Ty    = std::uint32_t;

            // 16-bit FFT data is quantized differently to 8-bit.
            // The dynamic range is the same (96.5dB) but that value
            // gives a full-scale output of 141.5.  Therefore, the
            // fixed-point value has to be re-scaled to give the correct
            // actual dB value.
            //
            constexpr static float full_scale      { 141.5f };
            constexpr static float max_dB          { 96.5f };
            constexpr static float scale_factor    { max_dB / full_scale };

            static std::uint16_t from_dB(Unit::dB power)
            {
                return static_cast<std::uint16_t>(power / scale_factor);
            }

            static Unit::dB to_dB(std::uint16_t power)
            {
                return static_cast<Unit::dB>(power) * scale_factor;
            }
        };


        // ------------------------
        //
        template <typename Iterator_Ty>
        class Window_slider {
        public:
            using Traits    = Slider_traits<typename std::iterator_traits<Iterator_Ty>::value_type>;
            using Sum_Ty    = typename Traits::Sum_Ty;
            using Value_Ty  = typename Traits::Value_Ty;

            Window_slider() = default;
            Window_slider(const Window& window_defn, Iterator_Ty min_bin, Iterator_Ty max_bin);

            std::pair<Unit::dB, std::size_t> process_cell(Iterator_Ty elem_ptr);
        
        private:
            Window      window;
            Iterator_Ty range_start { };
            Iterator_Ty range_end   { };

            std::size_t threshold_exceeded_count { };
        };


        template <typename Iterator_Ty>
        Window_slider<Iterator_Ty>::Window_slider(const Window& window_defn, Iterator_Ty min_bin, Iterator_Ty max_bin) :
            window      { window_defn },
            range_start { min_bin },
            range_end   { max_bin }
        {
        }


        template <typename Iterator_Ty>
        std::pair<Unit::dB, std::size_t> Window_slider<Iterator_Ty>::process_cell(Iterator_Ty const elem_ptr)
        {
            //                               elem_ptr
            //             guard_sz <----->  v
            // _____________________________________________________________
            // |   |   |   |   |   | X | X |   | X | X |   |   |   |   |   |
            // -------------------------------------------------------------
            //   ^                   ^                   ^                    ^
            // lower_begin        lower_end         upper_begin       upper_end   
            //
            // At the beginning and end of the range the window must be 'slewed'
            // to ensure that all elements (elem_ptr) are checked.
            // If the window extends below the start of the range, truncate the
            // lower set of training cells and extend the upper set.  That is, 
            // the training cell set always remains the same size. Similarly,
            // if the upper training cells extend beyond the end of the range,
            // truncate the upper cells and extend the lower set to maintain the
            // window size.
            // Another way to think about this: Start with the cell-under-test (CUT)
            // at the left-most part of the window.  Increment the CUT until the
            // middle of the window is reached; after which the entire window can slide
            // up the azimuth. When the window reaches the end of the azimuth, continue
            // moving the CUT until the last cell in the window is tested.                    
            
            using namespace std;
            using namespace Unit;

            Iterator_Ty lower_begin { elem_ptr - (window.size / 2) };
            Iterator_Ty lower_end   { elem_ptr - window.guard_cells };

            Iterator_Ty upper_begin { elem_ptr + window.guard_cells + 1 };
            Iterator_Ty upper_end   { elem_ptr + (window.size / 2) + 1 };


            if (lower_begin < range_start) {
                lower_begin = range_start;
                upper_end   = lower_begin + window.size;
            }

            if (upper_end > range_end) {
                upper_end = range_end;
                lower_begin = upper_end - window.size;
            }

            if (lower_end < range_start) {
                lower_end = range_start;
            }

            if (lower_begin > lower_end) {
                lower_begin = lower_end;
            }

            if (upper_begin > upper_end) {
                upper_begin = upper_end;
            }
            
            Sum_Ty      lower_sum    { accumulate(lower_begin, lower_end, Value_Ty { }) };
            Sum_Ty      upper_sum    { accumulate(upper_begin, upper_end, Value_Ty { }) };
            auto        elems        { std::distance(lower_begin, lower_end) + std::distance(upper_begin, upper_end) };
            Unit::dB    average      { static_cast<Unit::dB>(lower_sum + upper_sum) / elems };
            Unit::dB    cell_value   { Traits::to_dB(*elem_ptr) };

            if (cell_value > (average + window.threshold_delta)) {
                ++threshold_exceeded_count;
                return std::make_pair(cell_value, threshold_exceeded_count);
            }
            else {
                return std::make_pair(0.0f, threshold_exceeded_count);
            }
        }


        // ------------------------
        // As above, but works only with the 'raw' type - either 8-bit or 16-bit FFT
        // data. That is, this window slider does not convert the output of the CFAR
        // to dB
        //
        template <typename Iterator_Ty>
        class Raw_window_slider {
        public:
            using Traits    = Slider_traits<typename std::iterator_traits<Iterator_Ty>::value_type>;
            using Sum_Ty    = typename Traits::Sum_Ty;
            using Value_Ty  = typename Traits::Value_Ty;

            Raw_window_slider() = default;
            Raw_window_slider(const Window& window_defn, Iterator_Ty min_bin, Iterator_Ty max_bin);

            std::pair<Raw_window_slider::Value_Ty, std::size_t> process_cell(Iterator_Ty elem_ptr);
        
        private:
            Window      window;
            Iterator_Ty range_start { };
            Iterator_Ty range_end   { };
            Value_Ty    threshold   { Traits::from_dB(window.threshold_delta) };

            std::size_t threshold_exceeded_count { };
        };


        template <typename Iterator_Ty>
        Raw_window_slider<Iterator_Ty>::Raw_window_slider(const Window& window_defn, Iterator_Ty min_bin, Iterator_Ty max_bin) :
            window      { window_defn },
            range_start { min_bin },
            range_end   { max_bin }
        {
        }


        template <typename Iterator_Ty>
        std::pair<typename Raw_window_slider<Iterator_Ty>::Value_Ty, std::size_t> 
        Raw_window_slider<Iterator_Ty>::process_cell(Iterator_Ty const elem_ptr)
        {
            using namespace std;
            using namespace Unit;

            Iterator_Ty lower_begin { elem_ptr - (window.size / 2) };
            Iterator_Ty lower_end   { elem_ptr - window.guard_cells };

            Iterator_Ty upper_begin { elem_ptr + window.guard_cells + 1 };
            Iterator_Ty upper_end   { elem_ptr + (window.size / 2) + 1 };


            if (lower_begin < range_start) {
                lower_begin = range_start;
                upper_end   = lower_begin + window.size;
            }

            if (upper_end > range_end) {
                upper_end = range_end;
                lower_begin = upper_end - window.size;
            }

            if (lower_end < range_start) {
                lower_end = range_start;
            }

            if (lower_begin > lower_end) {
                lower_begin = lower_end;
            }

            if (upper_begin > upper_end) {
                upper_begin = upper_end;
            }
            
            Sum_Ty      lower_sum    { accumulate(lower_begin, lower_end, Value_Ty { }) };
            Sum_Ty      upper_sum    { accumulate(upper_begin, upper_end, Value_Ty { }) };
            auto        elems        { std::distance(lower_begin, lower_end) + std::distance(upper_begin, upper_end) };
            Value_Ty    average      { static_cast<Value_Ty>((lower_sum + upper_sum) / elems) };
            Value_Ty    cell_value   { *elem_ptr };

            if (cell_value > (average + threshold)) {
                ++threshold_exceeded_count;
                return std::make_pair(cell_value, threshold_exceeded_count);
            }
            else {
                return std::make_pair(0.0f, threshold_exceeded_count);
            }
        }

    } // namespace Implementation


    template <typename Iterator_Ty>    
    std::vector<Point> points(
        Iterator_Ty     data, 
        const Range&    range, 
        const Window&   window,
        const Range_fn& to_metre
    )
    {
        return first_n_points(data, range, window, range.size(), to_metre);
    }


    template <typename T>    
    std::vector<Point> points(
        const std::vector<T>&   data, 
        const Range&            range, 
        const Window&           window,
        const Range_fn&         to_metre
    )
    {
        return first_n_points(data.begin(), range, window, range.size(), to_metre);
    }


    template <typename T>    
    std::vector<Point> points(
        const std::vector<T>&   data,
        const Window&           window,
        const Range_fn&         to_metre
    )
    {
        return first_n_points(data.begin(), Range { data.size() }, window, data.size(), to_metre);
    }


    template <typename T>
    std::vector<Point> first_n_points(
        const std::vector<T>&   data, 
        const Window&           window,
        std::size_t             max_points,
        const Range_fn&         to_metre
    )
    {
        return first_n_points(data.begin(), Range { data.size() }, window, max_points, to_metre);
    }
    

    template <typename Iterator_Ty>
    std::vector<Point> first_n_points(
        Iterator_Ty     data, 
        const Range&    range, 
        const Window&   window, 
        std::size_t     max_points,
        const Range_fn& to_metre
    )
    {
        using namespace Unit;

        std::vector<Point> output { };
        output.reserve(max_points);

        Iterator_Ty start  { data + range.start };
        Iterator_Ty finish { data + range.end };

        Implementation::Window_slider<Iterator_Ty> slider {
            window,
            start,
            finish
        };

        for (Bin bin { range.start }; bin < range.end; ++bin) {
            auto [power, count] = slider.process_cell(data + bin);

            if (power > 0_dB)        output.emplace_back(to_metre(bin), power);
            if (count == max_points) break;
        }

        return output;
    }
    
    
    
    template <typename Iterator_Ty>
    std::vector<Unit::dB> process(Iterator_Ty first, Iterator_Ty last, const Window& window)
    {
        using namespace Unit;

        Implementation::Window_slider<Iterator_Ty> slider {
            window,
            first,
            last
        };

        std::vector<dB> output { };
        output.resize(std::distance(first, last));

        auto out_itr = output.begin();

        for (auto itr { first }; itr != last; ++itr) {
            *out_itr = slider.process_cell(itr).first;
            ++out_itr;
        }

        return output;
    }


    template <typename T>
    std::vector<Unit::dB> process(const std::vector<T>& vec, const Window& window)
    {
        return process(vec.begin(), vec.end(), window);
    }


    template <typename Iterator_Ty>
    auto process_as_raw(Iterator_Ty first, Iterator_Ty last, const Window& window)
    {
        using Value_Ty = typename std::iterator_traits<Iterator_Ty>::value_type;

        Implementation::Raw_window_slider<Iterator_Ty> slider {
            window,
            first,
            last
        };

        std::vector<Value_Ty> output { };
        output.resize(std::distance(first, last));

        auto out_itr = output.begin();

        for (auto itr { first }; itr != last; ++itr) {
            *out_itr = slider.process_cell(itr).first;
            ++out_itr;
        }

        return output;
    }


    template <typename T>
    std::vector<T> process_as_raw(const std::vector<T>& vec, const Window& window)
    {
        return process_as_raw(vec.begin(), vec.end(), window);
    }

} // namespace Navtech::CA_CFAR


#endif // CA_CFAR_ALGORITHMS_H