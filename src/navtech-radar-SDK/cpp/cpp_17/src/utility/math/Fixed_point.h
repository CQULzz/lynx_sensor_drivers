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
#ifndef FIXED_POINT_H
#define FIXED_POINT_H

#include <cstdint>

namespace Navtech::Utility {

    namespace Internal {

        template <unsigned bits>
        struct Traits {
        };

        template <>
        struct Traits<8> {
            using type      = std::uint8_t;
            using calc_type = std::uint16_t;
        };

        template <>
        struct Traits<16> {
            using type      = std::uint16_t;
            using calc_type = std::uint32_t;
        };

        template <>
        struct Traits<32> {
            using type      = std::uint32_t;
            using calc_type = std::uint64_t;
        };

        // NB - No 64-bit fixed types!

    } // namespace Internal


    template <unsigned bits, unsigned dec_places>
    class Fixed {
    public:
        using Type      = typename Internal::Traits<bits>::type;
        using Calc_type = typename Internal::Traits<bits>::calc_type;

        static constexpr Fixed from_raw(Type src);

        constexpr Fixed() = default;
        constexpr Fixed(float f);

        ~Fixed()                        = default; 
        Fixed(const Fixed&)             = default;
        Fixed(Fixed&&)                  = default;
        Fixed& operator=(const Fixed&)  = default;
        Fixed& operator=(Fixed&&)       = default;
        Fixed& operator=(float rhs);

        constexpr Fixed operator+(const Fixed& rhs) const;
        constexpr Fixed operator-(const Fixed& rhs) const;
        constexpr Fixed operator*(const Fixed& rhs) const;
        constexpr Fixed operator/(const Fixed& rhs) const;
        
        Fixed& operator+=(const Fixed& rhs);
        Fixed& operator-=(const Fixed& rhs);
        Fixed& operator*=(const Fixed& rhs);
        Fixed& operator/=(const Fixed& rhs);

        constexpr bool operator==(const Fixed& rhs) const;
        constexpr bool operator!=(const Fixed& rhs) const;
        constexpr bool operator>(const Fixed& rhs) const;
        constexpr bool operator<(const Fixed& rhs) const;
        constexpr bool operator>=(const Fixed& rhs) const;
        constexpr bool operator<=(const Fixed& rhs) const;

        constexpr float to_float() const;
        template <typename Fixed_Ty> constexpr Fixed_Ty to_fixed() const;
        constexpr Type raw() const;

    private:
        Type value { };

        static_assert(dec_places < bits, "Fixed type cannot accommodate number of decimal places");
    };

    template <unsigned bits, unsigned dec_places>
    constexpr Fixed<bits, dec_places> Fixed<bits, dec_places>::from_raw(Type src)
    {
        Fixed<bits, dec_places> instance { };
        instance.value = src;
        return instance;
    }


    template <unsigned bits, unsigned dec_places>
    constexpr Fixed<bits, dec_places>::Fixed(float f) : 
        value { static_cast<Type>(f * (1 << dec_places)) }
    {
    }


    template <unsigned bits, unsigned dec_places>
    constexpr Fixed<bits, dec_places> Fixed<bits, dec_places>::operator+(const Fixed<bits, dec_places>& rhs) const
    {
        Fixed<bits, dec_places> result { };
        
        result.value = this->value + rhs.value;
        
        return result;
    }


    template <unsigned bits, unsigned dec_places>
    constexpr Fixed<bits, dec_places> Fixed<bits, dec_places>::operator-(const Fixed<bits, dec_places>& rhs) const
    {
        Fixed<bits, dec_places> result { };
        
        result.value = this->value - rhs.value;
        
        return result;
    }


    template <unsigned bits, unsigned dec_places>
    constexpr Fixed<bits, dec_places> Fixed<bits, dec_places>::operator*(const Fixed<bits, dec_places>& rhs) const
    {
        Fixed<bits, dec_places> result  { };
        Calc_type               product { };
  
        product      = (static_cast<Calc_type>(value) * rhs.value) / (1 << dec_places);
        result.value = static_cast<Type>(product);

        return result;
    }


    template <unsigned bits, unsigned dec_places>
    constexpr Fixed<bits, dec_places> Fixed<bits, dec_places>::operator/(const Fixed<bits, dec_places>& rhs) const
    {
        Fixed<bits, dec_places> result   { };
        Calc_type               quotient { };

        quotient = (static_cast<Calc_type>(value) * (1 << dec_places) / static_cast<Calc_type>(rhs.value));
        result.value = static_cast<Type>(quotient);
        
        return result;
    }


    template <unsigned bits, unsigned dec_places>
    Fixed<bits, dec_places>& Fixed<bits, dec_places>::operator+=(const Fixed<bits, dec_places>& rhs)
    {
        value += rhs.value;
        return *this;
    }


    template <unsigned bits, unsigned dec_places>
    Fixed<bits, dec_places>& Fixed<bits, dec_places>::operator-=(const Fixed<bits, dec_places>& rhs)
    {
        value -= rhs.value;
        return *this;
    }


    template <unsigned bits, unsigned dec_places>
    Fixed<bits, dec_places>& Fixed<bits, dec_places>::operator*=(const Fixed<bits, dec_places>& rhs)
    {
        Calc_type product = (static_cast<Calc_type>(value) * rhs.value) / (1 << dec_places);
        value             = static_cast<Type>(product);

        return *this;
    }


    template <unsigned bits, unsigned dec_places>
    Fixed<bits, dec_places>& Fixed<bits, dec_places>::operator/=(const Fixed<bits, dec_places>& rhs)
    {
        Calc_type quotient = (static_cast<Calc_type>(value) * (1 << dec_places) / static_cast<Calc_type>(rhs.value));
        value              = static_cast<Type>(quotient);
        
        return *this;
    }


    template <unsigned bits, unsigned dec_places>
    Fixed<bits, dec_places>& Fixed<bits, dec_places>::operator=(float rhs)
    {
        value = static_cast<Type>(rhs * (1 << dec_places));
        return *this;
    }


    template <unsigned bits, unsigned dec_places>
    constexpr bool Fixed<bits, dec_places>::operator==(const Fixed<bits, dec_places>& rhs) const
    {
        return this->value == rhs.value;
    }


    template <unsigned bits, unsigned dec_places>
    constexpr bool Fixed<bits, dec_places>::operator!=(const Fixed<bits, dec_places>& rhs) const
    {
        return !(*this == rhs);
    }

    
    template <unsigned bits, unsigned dec_places>
    constexpr bool Fixed<bits, dec_places>::operator>(const Fixed<bits, dec_places>& rhs) const
    {
        return this->value > rhs.value;
    }


    template <unsigned bits, unsigned dec_places>
    constexpr bool Fixed<bits, dec_places>::operator<(const Fixed<bits, dec_places>& rhs) const
    {
        return this->value < rhs.value;
    }
    

    template <unsigned bits, unsigned dec_places>
    constexpr bool Fixed<bits, dec_places>::operator>=(const Fixed<bits, dec_places>& rhs) const
    {
        return (*this > rhs) || (*this == rhs);
    }


    template <unsigned bits, unsigned dec_places>
    constexpr bool Fixed<bits, dec_places>::operator<=(const Fixed<bits, dec_places>& rhs) const
    {
        return (*this < rhs) || (*this == rhs);
    }


    template <unsigned bits, unsigned dec_places>
    constexpr float Fixed<bits, dec_places>::to_float() const
    {
        return static_cast<float>(value) / (1 << dec_places);
    }


    template <unsigned bits, unsigned dec_places>
    template <typename Fixed_Ty> 
    constexpr Fixed_Ty Fixed<bits, dec_places>::to_fixed() const
    {
        return Fixed_Ty { to_float() };
    }


    template <unsigned bits, unsigned dec_places>
    constexpr typename Fixed<bits, dec_places>::Type Fixed<bits, dec_places>::raw() const
    {
        return value;
    }

} // namespace Navtech::Utility


#endif // FIXED_POINT_H