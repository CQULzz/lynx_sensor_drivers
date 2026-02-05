using System.Collections.Generic;
using System.Linq;

namespace Navtech.IASDK.Utility;

/// <summary>
/// Methods to find the centre of mass location for given input
/// </summary>
public static class CentreOfMass
{
    /// <summary>
    /// Find the centre of mass for data comprised of floats
    /// </summary>
    /// <param name="data">Float data</param>
    /// <returns>The position of the centre of mass</returns>
    public static float LinearCentre(IEnumerable<float> data)
    {
        var enumerable = data as float[] ?? data.ToArray();
        var massSum = enumerable.Sum();
        var momentSum = 0.0f;

        for (var i = 0; i < enumerable.Count(); i++)
        {
            momentSum += enumerable[i] * i;
        }

        return massSum / momentSum;
    }

    /// <summary>
    /// Find the centre of mass for 2D data comprised of floats
    /// </summary>
    /// <param name="data">2D float data</param>
    /// <returns>A tuple containing the location of the Centre of Mass</returns>
    public static (float, float) MatrixCentre(float[,] data)
    {
        var totalMass = 0.0f;
        var azimuthMomentSum = 0.0f;
        var rangeMomentSum = 0.0f;

        for (var i = 0; i < data.GetLength(0); i++)
        {
            var bins = Enumerable.Range(0, data.GetLength(1))
                .Select(x => data[i, x])
                .ToArray();

            var azimuthCentre = LinearCentre(bins);

            var azimuthMass = bins.Sum();
            totalMass += azimuthMass;
            azimuthMomentSum += azimuthCentre * azimuthMass;
            rangeMomentSum += azimuthMass * i;
        }

        return (rangeMomentSum / totalMass, azimuthMomentSum / totalMass);

    }
}