using Navtech.IASDK.Extensions;

namespace IASDK.Tests.Extensions;

[TestFixture]
public class GivenAnArrayExtension
{
    [Test]
    public void ASliceShouldProvideOnlyRequestedIndices()
    {
        int[] testIntArr = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
        double[] testDoubleArr = { 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0 };

        var intSlice = testIntArr.Slice(4, 8);

        Assert.That(intSlice.Length, Is.EqualTo(4));
        Assert.That(intSlice[0], Is.EqualTo(5));
        Assert.That(intSlice[1], Is.EqualTo(6));
        Assert.That(intSlice[2], Is.EqualTo(7));

        Assert.That(testIntArr.Length, Is.EqualTo(10));

        var doubleSlice = testDoubleArr.Slice(4, 8);
        Assert.That(doubleSlice.Length, Is.EqualTo(4));
        Assert.That(Math.Abs(doubleSlice[0] - 5.0), Is.LessThanOrEqualTo(double.Epsilon));
        Assert.That(Math.Abs(doubleSlice[1] - 6.0), Is.LessThanOrEqualTo(double.Epsilon));
        Assert.That(Math.Abs(doubleSlice[2] - 7.0), Is.LessThanOrEqualTo(double.Epsilon));

        Assert.That(testDoubleArr.Length, Is.EqualTo(10));
    }
}