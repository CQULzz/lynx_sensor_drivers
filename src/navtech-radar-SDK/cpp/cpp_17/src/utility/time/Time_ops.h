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
#ifndef TIME_OPS_H
#define TIME_OPS_H

#include "Time_core.h"
#include "Duration.h"

// Basic mathematical operations for time types.
//
// Durations:
// - Durations may be compared for equality/inequality and greater-than/less-than.
// - Addition represents lengthening the duration. Two Durations may be added to yield a new 
//   Duration whose value is the simple sum of the two Durations.
// - Subtraction represents shortening the duration. Subtracting a larger Duration from a 
//   smaller Duration will yield a negative Duration (which is not physically realizable).
// - If the difference between two Durations is required, prefer abs_diff.
// - Two Durations cannot be multiplied since this has no physical meaning.
// - Two Durations may be divided. The result will be a positive integer, representing the 
//   number of times the divisor Duration goes into the numerator.
// - Durations may be scaled by multiplying or dividing by a scale factor; either integer or 
//   floating-point.
//
// Observations:
// - Observations may be compared for equality/inequality. Equality indicates that both Observations 
//   mark the same moment in time.
// - Greater-than indicates that one Observation is later the other; less-than indicates that one 
//   Observation is earlier than the other.
// - Adding two Observations has no meaning.
// - A Duration may be added to, or subtracted from, an Observation. The result is a new Observation.
// - To find the absolute difference between two Observations use abs_diff.
// - If a later Observation (t1) is subtracted from an earlier Observation (t0) the result will yield 
//   a negative Duration. That is, t0 – t1 => -d.
// - Since it cannot be guaranteed that two subsequent Observations will yield increasing time points, 
//   subtracting two Observations may lead to a negative Duration.
//
namespace Navtech::Time {

    template <typename Dur_Ty1, typename Dur_Ty2, 
        is_duration<Dur_Ty1> = true, 
        is_duration<Dur_Ty2> = true>
    constexpr Duration operator+(const Dur_Ty1& lhs, const Dur_Ty2& rhs)
    {
        // NOTE: Overflow behaviour is undefined

        return Duration { lhs.ticks() + rhs.ticks() };
    }


    template <typename Obs_Ty, typename Dur_Ty, 
        is_observation<Obs_Ty> = true,
        is_duration<Dur_Ty>    = true>
    constexpr Obs_Ty operator+(const Obs_Ty& lhs, const Dur_Ty& rhs)
    {
        auto result = lhs.since_epoch().ticks() + rhs.ticks();

        // Cannot have a time observation before the epoch (zero)
        //
        if (result < 0) return Obs_Ty { Time::Duration { 0 } };
        else            return Obs_Ty { Time::Duration { result } };
    }


    template <typename Obs_Ty, typename Dur_Ty, 
        is_observation<Obs_Ty> = true,
        is_duration<Dur_Ty>    = true>
    constexpr Obs_Ty operator+(const Dur_Ty&  lhs, const Obs_Ty&rhs)
    {
        auto result = rhs.since_epoch().ticks() + lhs.ticks();

        // Cannot have a time observation before the epoch (zero)
        //
        if (result < 0) return Obs_Ty { Time::Duration { 0 } };
        else            return Obs_Ty { Time::Duration { result } };
    }


    template <typename Dur_Ty1, typename Dur_Ty2, 
        is_duration<Dur_Ty1> = true, 
        is_duration<Dur_Ty2> = true>
    constexpr Duration operator-(const Dur_Ty1& lhs, const Dur_Ty2& rhs)
    {
        // NOTE: Underflow behaviour is undefined

        return Duration { lhs.ticks() - rhs.ticks() };
    }


    template <typename Obs_Ty, typename Dur_Ty, 
        is_observation<Obs_Ty> = true,
        is_duration<Dur_Ty>    = true>
    constexpr Obs_Ty operator-(const Obs_Ty& lhs, const Dur_Ty& rhs)
    {
        auto result = lhs.since_epoch().ticks() - rhs.ticks();

        // Cannot have a time observation before the epoch (zero)
        //
        if (result < 0) return Obs_Ty { Time::Duration { 0 } };
        else            return Obs_Ty { Time::Duration { result } };
    }


    template <typename Obs_Ty1, typename Obs_Ty2, 
        is_observation<Obs_Ty1> = true,
        is_observation<Obs_Ty2> = true>
    constexpr Duration operator-(const Obs_Ty1& lhs, const Obs_Ty2& rhs)
    {
        return Duration { lhs.since_epoch().ticks() - rhs.since_epoch().ticks() };
    }


    template <typename Dur_Ty,
        is_duration<Dur_Ty> = true>
    constexpr Duration operator*(const Dur_Ty& lhs, double rhs)
    {
        // NOTE:
        // -Ofast or -ffast-math optimisation disables NaN
        //
        if (std::isnan(rhs)) throw std::invalid_argument { "RHS operand is not a number" };

        return Duration { static_cast<Tick_type>(lhs.ticks() * rhs) };
    }
    

    template <typename Dur_Ty,
        is_duration<Dur_Ty> = true>
    constexpr Duration operator*(double lhs, const Dur_Ty& rhs)
    {
        // NOTE:
        // -Ofast or -ffast-math optimisation disables NaN
        //
        if (std::isnan(lhs)) throw std::invalid_argument { "RHS operand is not a number" };

        return Duration { static_cast<Tick_type>(lhs * rhs.ticks()) };
    }


    template <typename Dur_Ty1, typename Dur_Ty2, 
        is_duration<Dur_Ty1> = true, 
        is_duration<Dur_Ty2> = true>
    constexpr int operator/(const Dur_Ty1& lhs, const Dur_Ty2& rhs)
    {
        if (rhs.ticks() == 0) throw std::overflow_error { "divide by zero!" };

        return static_cast<int>(lhs.ticks() / rhs.ticks());
    }


    template <typename Dur_Ty,
        is_duration<Dur_Ty> = true>
    constexpr Duration operator/(const Dur_Ty& lhs, double rhs)
    {
        if (rhs == 0.0) throw std::overflow_error { "divide by zero!" };

        return Duration { static_cast<Tick_type>(lhs.ticks() / rhs) };
    }


    template <typename Dur_Ty1, typename Dur_Ty2, 
        is_duration<Dur_Ty1> = true, 
        is_duration<Dur_Ty2> = true>
    constexpr bool operator==(const Dur_Ty1& lhs, const Dur_Ty2& rhs)
    {
        return (lhs.ticks() == rhs.ticks());
    }


    template <typename Obs_Ty1, typename Obs_Ty2, 
        is_observation<Obs_Ty1> = true,
        is_observation<Obs_Ty2> = true>
    constexpr bool operator==(const Obs_Ty1& lhs, const Obs_Ty2& rhs)
    {
        return (lhs.since_epoch() == rhs.since_epoch());
    }


    template <typename Dur_Ty1, typename Dur_Ty2, 
        is_duration<Dur_Ty1> = true, 
        is_duration<Dur_Ty2> = true>
    constexpr bool operator!=(const Dur_Ty1& lhs, const Dur_Ty2& rhs)
    {
        return (lhs.ticks() != rhs.ticks());
    }


    template <typename Obs_Ty1, typename Obs_Ty2, 
        is_observation<Obs_Ty1> = true,
        is_observation<Obs_Ty2> = true>
    constexpr bool operator!=(const Obs_Ty1& lhs, const Obs_Ty2& rhs)
    {
        return (lhs.since_epoch() != rhs.since_epoch());
    }


    template <typename Dur_Ty1, typename Dur_Ty2, 
        is_duration<Dur_Ty1> = true, 
        is_duration<Dur_Ty2> = true>
    constexpr bool operator>(const Dur_Ty1& lhs, const Dur_Ty2& rhs)
    {
        return (lhs.ticks() > rhs.ticks());
    }


    template <typename Obs_Ty1, typename Obs_Ty2, 
        is_observation<Obs_Ty1> = true,
        is_observation<Obs_Ty2> = true>
    constexpr bool operator>(const Obs_Ty1& lhs, const Obs_Ty2& rhs)
    {
        return (lhs.since_epoch() > rhs.since_epoch());
    }


    template <typename Dur_Ty1, typename Dur_Ty2, 
        is_duration<Dur_Ty1> = true, 
        is_duration<Dur_Ty2> = true>
    constexpr bool operator<(const Dur_Ty1& lhs, const Dur_Ty2& rhs)
    {
        return (lhs.ticks() < rhs.ticks());
    }


    template <typename Obs_Ty1, typename Obs_Ty2, 
        is_observation<Obs_Ty1> = true,
        is_observation<Obs_Ty2> = true>
    constexpr bool operator<(const Obs_Ty1& lhs, const Obs_Ty2& rhs)
    {
        return (lhs.since_epoch() < rhs.since_epoch());
    }


    template <typename Dur_Ty1, typename Dur_Ty2, 
        is_duration<Dur_Ty1> = true, 
        is_duration<Dur_Ty2> = true>
    constexpr bool operator>=(const Dur_Ty1& lhs, const Dur_Ty2& rhs)
    {
        return (lhs.ticks() >= rhs.ticks());
    }


    template <typename Obs_Ty1, typename Obs_Ty2, 
        is_observation<Obs_Ty1> = true,
        is_observation<Obs_Ty2> = true>
    constexpr bool operator>=(const Obs_Ty1& lhs, const Obs_Ty2& rhs)
    {
        return (lhs.since_epoch() >= rhs.since_epoch());
    }


    template <typename Dur_Ty1, typename Dur_Ty2, 
        is_duration<Dur_Ty1> = true, 
        is_duration<Dur_Ty2> = true>
    constexpr bool operator<=(const Dur_Ty1& lhs, const Dur_Ty2& rhs)
    {
        return (lhs.ticks() <= rhs.ticks());
    }


    template <typename Obs_Ty1, typename Obs_Ty2, 
        is_observation<Obs_Ty1> = true,
        is_observation<Obs_Ty2> = true>
    constexpr bool operator<=(const Obs_Ty1& lhs, const Obs_Ty2& rhs)
    {
        return (lhs.since_epoch() <= rhs.since_epoch());
    }


    template <typename Dur_Ty1, typename Dur_Ty2, 
        is_duration<Dur_Ty1> = true, 
        is_duration<Dur_Ty2> = true>
    constexpr Duration abs_diff(const Dur_Ty1& lhs, const Dur_Ty2& rhs)
    {
        using std::abs;

        auto lhs_ticks = lhs.ticks();
        auto rhs_ticks = rhs.ticks();

        if (lhs_ticks >= rhs_ticks) return Duration { abs(lhs_ticks - rhs_ticks) };
        else                        return Duration { abs(rhs_ticks - lhs_ticks) };
    }


    template <typename Obs_Ty, typename Dur_Ty, 
        is_observation<Obs_Ty> = true,
        is_duration<Dur_Ty>    = true>
    constexpr Duration abs_diff(const Obs_Ty& lhs, const Dur_Ty& rhs)
    {
        using std::abs;

        auto lhs_ticks = lhs.since_epoch().ticks();
        auto rhs_ticks = rhs.ticks();

        if (lhs_ticks >= rhs_ticks) return Duration { abs(lhs_ticks - rhs_ticks) };
        else                        return Duration { abs(rhs_ticks - lhs_ticks) };
    }


    template <typename Obs_Ty1, typename Obs_Ty2, 
        is_observation<Obs_Ty1> = true,
        is_observation<Obs_Ty2> = true>
    constexpr Duration abs_diff(const Obs_Ty1& lhs, const Obs_Ty2& rhs)
    {
        using std::abs;

        auto lhs_ticks = lhs.since_epoch().ticks();
        auto rhs_ticks = rhs.since_epoch().ticks();

        if (lhs_ticks >= rhs_ticks) return Duration { abs(lhs_ticks - rhs_ticks) };
        else                        return Duration { abs(rhs_ticks - lhs_ticks) };
    }

} // namespace Navtech::Time

#endif //TIME_OPS_H