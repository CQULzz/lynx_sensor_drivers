/*
Copyright 2024 Navtech Radar Limited
This file is part of IASDK which is released under The MIT License (MIT).
See file LICENSE.txt in project root or go to https://opensource.org/licenses/MIT
for full license details.

Disclaimer:
Navtech Radar is furnishing this item "as is". Navtech Radar does not provide 
any warranty of the item whatsoever, whether express, implied, or statutory,
including, but not limited to, any warranty of merchantability or fitness
for a particular purpose or any warranty that the contents of the item will
be error-free.
In no respect shall Navtech Radar incur any liability for any damages, including,
but limited to, direct, indirect, special, or consequential damages arising
out of, resulting from, or any way connected to the use of the item, whether
or not based upon warranty, contract, tort, or otherwise; whether or not
injury was sustained by persons or property or otherwise; and whether or not
loss was sustained from, or arose out of, the results of, the item, or any
services that may be provided by Navtech Radar.
*/


using Navtech.IASDK.Extensions;
using Navtech.IASDK.Networking.Messages;

namespace IASDK.Tests.Extensions;

[TestFixture]
public class GivenARawStructExtension
{
    [Test]
    public void ARadarMessageShouldCorrectlyMarshalToFftData()
    {
        byte[] rawMessage =
        {
            0x00, 0x00, // data offset
            0x01, 0x90, // Sweep counter (400)
            0x00, 0xC8, // Azimuth (200)
            0x66, 0x69, 0xA3, 0x4E, // NTP seconds (1718199118)
            0x07, 0x5B, 0xCD, 0x15, // Split seconds (123,456,789)
            0xFF, 0xFF, 0xFF, 0xFF, // Data
            0xFF, 0xFF, 0xFF, 0xFF
        };

        var fftMessage = rawMessage.MarshalToObject<TcpFftDataMessage>();

        Assert.That(fftMessage.FftDataOffset, Is.EqualTo(0));
        //Assert.That(fftMessage.SweepCounter, Is.EqualTo(400));
        Assert.That(fftMessage.Azimuth, Is.EqualTo(200));
        Assert.That(fftMessage.Seconds, Is.EqualTo(1718199118));
        Assert.That(fftMessage.SplitSeconds, Is.EqualTo(123_456_789));
    }

    [Test]
    public void ARadarMessageShouldCorrectlyMarshalToNavigationData()
    {
        byte[] rawMessage =
        {
            0x00, 0xC8, // Azimuth (200)
            0x66, 0x69, 0xA3, 0x4E, // NTP Seconds (1718199118)
            0x07, 0x5B, 0xCD, 0x15, // Split seconds (123,456,789)
            0xFF, 0xFF, 0xFF, 0xFF, // Data
            0xFF, 0xFF, 0xFF, 0xFF
        };

        var navDataMessage = rawMessage.MarshalToObject<TcpNavigationDataMessage>();

        Assert.That(navDataMessage.Azimuth, Is.EqualTo(200));
        Assert.That(navDataMessage.Seconds, Is.EqualTo(1718199118));
        Assert.That(navDataMessage.SplitSeconds, Is.EqualTo(123_456_789));
    }

    [Test]
    public void ARadarMessageShouldCorrectlyMarshalToAConfigurationMessage()
    {
        byte[] rawMessage =
        {
            0x01, 0x90, // Azimuth Samples (400)
            0x06, 0xD8, // Bin Size * 10000 (1752)
            0x0B, 0x28, // Range in Bins (2856)
            0x15, 0xE0, // Encoder Size (5600)
            0x0F, 0xA0, // Rotation Speed in ms (4000)
            0x06, 0x40, // Packet rate (1600)
            0x3F, 0x7F, 0xEB, 0xC0, // Range Gain (0.999691)
            0xBE, 0xA3, 0xCB, 0x4B, // Range offset (-0.319910377)
        };

        var configMessage = rawMessage.MarshalToObject<TcpConfigurationDataMessage>();

        Assert.That(configMessage.AzimuthSamples, Is.EqualTo(400));
        Assert.That(configMessage.BinSize, Is.EqualTo(1752));
        Assert.That(configMessage.RangeInBins, Is.EqualTo(2856));
        Assert.That(configMessage.EncoderSize, Is.EqualTo(5600));
        Assert.That(configMessage.RotationSpeed, Is.EqualTo(4000));
        Assert.That(configMessage.PacketRate, Is.EqualTo(1600));
        Assert.That(Math.Abs(configMessage.RangeGain - 0.999691f), Is.LessThanOrEqualTo(float.Epsilon));
        Assert.That(Math.Abs(configMessage.RangeOffset - -0.319910377f), Is.LessThanOrEqualTo(float.Epsilon));
    }

}