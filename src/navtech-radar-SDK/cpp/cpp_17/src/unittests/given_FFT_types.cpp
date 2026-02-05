#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <array>

#include "FFT_types.h"

using namespace Navtech;
using namespace Navtech::Utility;
using namespace Navtech::Unit;
using namespace std;


class GivenFFTTypes : public ::testing::Test {
protected:
};


TEST_F(GivenFFTTypes, FFTIn8bitTodB)
{
    FFT_8bit fft_8bit { };

    fft_8bit = 96.5_dB;

    dB result = fft_8bit.to_dB();

    ASSERT_FLOAT_EQ(result, 96.5_dB);

    std::uint8_t byte_val = fft_8bit.raw();

    ASSERT_EQ(byte_val, 193);
}


TEST_F(GivenFFTTypes, FFTIn16bitTodB)
{
    FFT_16bit fft_16bit { };

    ASSERT_EQ(sizeof(fft_16bit), 2);

    fft_16bit = 96.5_dB;

    dB result = fft_16bit.to_dB();

    ASSERT_FLOAT_EQ(result, 96.5_dB);

    std::uint16_t word_val = fft_16bit.raw();

    ASSERT_EQ(word_val, 36224);
}


TEST_F(GivenFFTTypes, Overlay16bit)
{
    array<uint8_t, 2> raw_data { 0xBC, 0x7D };

    auto fft_16bit = FFT_16bit::overlay_at(raw_data.data());

    ASSERT_EQ(*(uint16_t*)fft_16bit, 0x7DBC);
    ASSERT_EQ(fft_16bit->raw(), 0x7DBC);
    ASSERT_FLOAT_EQ(fft_16bit->to_dB(), 85.748184_dB);
}


TEST_F(GivenFFTTypes, Overlay8bit)
{
    array<uint8_t, 2> raw_data { 0xAF };

    auto fft_8bit = FFT_8bit::overlay_at(raw_data.data());

    ASSERT_EQ(*(uint8_t*)fft_8bit, 0xAF);
    ASSERT_EQ(fft_8bit->raw(), 0xAF);
    ASSERT_FLOAT_EQ(fft_8bit->to_dB(), 87.5_dB);
}




