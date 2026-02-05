using Navtech.IASDK.Utility;

namespace IASDK.Tests.Utility;

[TestFixture]
public class GivenAShapeFinder
{
    [Test]
    public void ASingleBalancedShapeShouldBeFound()
    {
        var data = new[,]
        {
            { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f },
            { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f },
            { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 12.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f },
            { 0.0f, 0.0f, 0.0f, 0.0f, 12.0f, 12.0f, 12.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f },
            { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 12.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f },
            { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f },
            { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f },
            { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f },
            { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f },
            { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f }
        };

        var shapeFinder = new ShapeFinder(0);

        var centre = shapeFinder.FindCentres(data);

        Assert.That(centre.Count, Is.EqualTo(1));
        Assert.That(centre[0].Item1, Is.EqualTo(3.0f).Within(float.Epsilon));
        Assert.That(centre[0].Item2, Is.EqualTo(5.0f).Within(float.Epsilon));
    }

    [Test]
    public void ASingleUnbalancedShapeShouldBeFound()
    {
        var data = new[,]
        {
            { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f },
            { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f },
            { 0.0f, 0.0f, 0.0f, 0.0f, 12.0f, 15.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f },
            { 0.0f, 0.0f, 0.0f, 0.0f, 12.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f },
            { 0.0f, 0.0f, 0.0f, 0.0f, 12.0f, 15.0f, 15.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f },
            { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f },
            { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f },
            { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f },
            { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f },
            { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f },
        };

        var shapeFinder = new ShapeFinder(0);

        var centre = shapeFinder.FindCentres(data);

        Assert.That(centre.Count, Is.EqualTo(1));
        Assert.That(centre[0].Item1, Is.EqualTo(3.185185f).Within(0.000001f));
        Assert.That(centre[0].Item2, Is.EqualTo(4.740741f).Within(0.000001f));
    }


    [Test]
    public void MultipleBalancedShapesShouldBeFound()
    {
        var data = new[,]
        {
            { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f },
            { 0.0f, 6.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f },
            { 6.0f, 6.0f, 6.0f, 0.0f, 0.0f, 12.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f },
            { 0.0f, 6.0f, 0.0f, 0.0f, 12.0f, 12.0f, 12.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f },
            { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 12.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f },
            { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f },
            { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 8.0f, 0.0f },
            { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 8.0f, 8.0f, 8.0f },
            { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 8.0f, 0.0f },
            { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f }
        };

        var shapeFinder = new ShapeFinder(0);

        var centre = shapeFinder.FindCentres(data);

        Assert.That(centre.Count, Is.EqualTo(3));
        Assert.That(centre[0].Item1, Is.EqualTo(2.0f).Within(float.Epsilon));
        Assert.That(centre[0].Item2, Is.EqualTo(1.0).Within(float.Epsilon));

        Assert.That(centre[1].Item1, Is.EqualTo(3.0f).Within(float.Epsilon));
        Assert.That(centre[1].Item2, Is.LessThanOrEqualTo(5.0).Within(float.Epsilon));

        Assert.That(centre[2].Item1, Is.EqualTo(7.0f).Within(float.Epsilon));
        Assert.That(centre[2].Item2, Is.EqualTo(10.0f).Within(float.Epsilon));
    }

    [Test]
    public void MultipleUnbalancedShapesShouldBeFound()
    {
        var data = new[,]
        {
            { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f },
            { 0.0f, 6.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f },
            { 0.0f, 6.0f, 6.0f, 0.0f, 15.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f },
            { 0.0f, 6.0f, 8.0f, 0.0f, 12.0f, 0.0f, 12.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f },
            { 0.0f, 0.0f, 8.0f, 0.0f, 12.0f, 15.0f, 12.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f },
            { 0.0f, 0.0f, 6.0f, 0.0f, 0.0f, 0.0f, 15.0f, 0.0f, 0.0f, 0.0f, 5.0f, 5.0f },
            { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 15.0f, 0.0f, 0.0f, 0.0f, 8.0f, 5.0f },
            { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 8.0f, 8.0f, 0.0f },
            { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 8.0f, 0.0f },
            { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f }
        };

        var shapeFinder = new ShapeFinder(0);

        var centre = shapeFinder.FindCentres(data);

        Assert.That(centre.Count, Is.EqualTo(3));
        Assert.That(centre[0].Item1, Is.EqualTo(2.913043f).Within(0.000001f));
        Assert.That(centre[0].Item2, Is.EqualTo(1.608696f).Within(0.000001f));

        Assert.That(centre[1].Item1, Is.EqualTo(3.916667f).Within(0.000001f));
        Assert.That(centre[1].Item2, Is.EqualTo(5.138889f).Within(0.000001f));

        Assert.That(centre[2].Item1, Is.EqualTo(6.468085f).Within(0.000001f));
        Assert.That(centre[2].Item2, Is.EqualTo(10.042553f).Within(0.000001f));
    }

    [Test]
    public void ShapesShouldWrapCorrectly()
    {
        var data = new[,]
        {
            { 0.0f, 2.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f },
            { 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f },
            { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f },
            { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f },
            { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f },
            { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f },
            { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f },
            { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f },
            { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f },
            { 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f }
        };

        var shapeFinder = new ShapeFinder(0);

        var centre = shapeFinder.FindCentres(data);

        Assert.That(centre.Count, Is.EqualTo(1));

        Assert.That(centre[0].Item1, Is.EqualTo(-0.2f).Within(float.Epsilon));
        Assert.That(centre[0].Item2, Is.EqualTo(1.2f).Within(float.Epsilon));
    }

    [Test]
    public void ShapesShouldOnlyBeFoundAfterMinimumColumn()
    {
        var data = new[,]
        {
            { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f },
            { 0.0f, 6.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f },
            { 0.0f, 6.0f, 6.0f, 0.0f, 15.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f },
            { 0.0f, 6.0f, 8.0f, 0.0f, 12.0f, 0.0f, 12.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f },
            { 0.0f, 0.0f, 8.0f, 0.0f, 12.0f, 15.0f, 12.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f },
            { 0.0f, 0.0f, 6.0f, 0.0f, 0.0f, 0.0f, 15.0f, 0.0f, 0.0f, 0.0f, 5.0f, 5.0f },
            { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 15.0f, 0.0f, 0.0f, 0.0f, 8.0f, 5.0f },
            { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 8.0f, 8.0f, 0.0f },
            { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 8.0f, 0.0f },
            { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f }
        };

        var shapeFinder = new ShapeFinder(4);

        var centre = shapeFinder.FindCentres(data);

        Assert.That(centre.Count, Is.EqualTo(2));
    }
}