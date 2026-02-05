using System;
using System.Collections.Generic;

namespace Navtech.IASDK.Utility;

/// <summary>
/// Finds shapes in a 2D set of radar data
/// </summary>
public class ShapeFinder
{
    private readonly ushort _minBin;
    private readonly HashSet<(int, int)> _exploredCells;
    private readonly Queue<(int, int)> _testCells;
    private int _rows;
    private int _cols;

    private static readonly List<(int, int)> Directions = new() { (0, 1), (1, 0), (-1, 0), (0, -1) };

    /// <summary>
    /// Initialise a shape finder with a minimum bin
    /// </summary>
    /// <param name="minBin"></param>
    public ShapeFinder(ushort minBin)
    {
        _minBin = minBin;
        _exploredCells = new HashSet<(int, int)>();
        _testCells = new Queue<(int, int)> ();
    }

    private int WrapRow(int r)
    {
        var wrapped = r;
        if (wrapped < 0) wrapped = _rows + r;
        else if (wrapped >= _cols) wrapped = r - _rows;

        return wrapped;
    }

    private Tuple<float, float> DepthFirstSearch(float[,] inputData)
    {
        if (inputData == null) return null;

        var shapeSize = 0;
        var totalMass = 0.0f;
        var rowMomentSum = 0.0f;
        var colMomentSum = 0.0f;

        while (_testCells.Count > 0)
        {
            var (r, c) = _testCells.Dequeue();

            var r2 = WrapRow(r);

            if (inputData[r2, c].Equals(default) || _exploredCells.Contains( (r2, c))) continue;

            shapeSize++; 
            var currentMass = inputData[r2, c];
            totalMass += currentMass;
            rowMomentSum += currentMass * r;
            colMomentSum += currentMass * c;

            _exploredCells.Add( (r2, c) );

            foreach (var (dr, dc) in Directions)
            {
                var nr = r + dr;
                var nr2 = WrapRow(nr);

                var nc = c + dc;

                if (nc >= 0 && nc < _cols && !_exploredCells.Contains((nr2, nc))) _testCells.Enqueue((nr, nc));
            }
        }

        return shapeSize == 1 ? null : new Tuple<float, float>(rowMomentSum / totalMass, colMomentSum / totalMass);
    }


    /// <summary>
    /// Finds the centres of shapes present in the data
    /// </summary>
    /// <param name="inputData"></param>
    /// <returns></returns>
    public List<Tuple<float, float>> FindCentres(float[,] inputData)
    {
        var output = new List<Tuple<float, float>>();

        _rows = inputData.GetLength(0);
        _cols = inputData.GetLength(1);

        for (var r = 0; r < _rows; ++r)
        {
            for (var c = 0; c < _cols; ++c)
            {
                if (c < _minBin || Math.Abs(inputData[r,c]) < float.Epsilon || _exploredCells.Contains((r, c))) continue;

                _testCells.Enqueue((r, c));

                var centre = DepthFirstSearch(inputData);

                if (centre == null) continue;

                output.Add(centre);
            }
        }
        return output;
    }
}