// ReSharper disable InconsistentNaming

using Navtech.IASDK.Utility;

namespace IASDK.Tests.Utility;

[TestFixture]
public class GivenANetConversionFunction
{
    [Test]
    public void GivenAnInt32ShouldSwapCorrectly()
    {
        const int testVal = -123456;
        var swapped = Navtech.IASDK.Utility.NetConversions.SwapInt32(testVal);

        Assert.That(swapped, Is.EqualTo(-1071776001));

        Assert.That(NetConversions.SwapInt32(swapped), Is.EqualTo(-123456));
    }

    [Test]
    public void GivenAUInt32ShouldSwapBytesCorrectly()
    {
        const uint testVal = 0xEFBEADDE;
        var swapped = Navtech.IASDK.Utility.NetConversions.SwapUInt32(testVal);

        Assert.That(swapped, Is.EqualTo(0xDEADBEEF));

        Assert.That(NetConversions.SwapUInt32(swapped), Is.EqualTo(0xEFBEADDE));
    }

    [Test]
    public void GivenAUInt16ShouldSwapBytesCorrectly()
    {
        const ushort testVal = 0xEFBE;
        var swapped = NetConversions.SwapUInt16(testVal);

        Assert.That(swapped, Is.EqualTo(0xBEEF));

        Assert.That(NetConversions.SwapUInt16(swapped), Is.EqualTo(0xEFBE));
    }
}