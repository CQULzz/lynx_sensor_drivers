namespace Navtech.IASDK.Networking.Messages;

using Interfaces;
using Navtech.IASDK.Utility;
using System;
using System.Runtime.InteropServices;

/// <summary>
/// Colossus Protocol Navigation Area Rule Message
/// </summary>
[StructLayout(LayoutKind.Sequential, Size = 15, Pack = 1)]
public struct NavigationAreaRuleMessage : IRawData
{
    private readonly uint _ruleLength;
    private readonly byte _id;
    private readonly byte _enabled;
    private readonly byte _invertBreakLogic;
    private readonly ushort _thresholdDelta;
    private readonly ushort _breakAllowance;
    private readonly ushort _allowanceCurveDecrement;
    private readonly ushort _pointCount;

    /// <summary>
    /// Fixed size of the header
    /// </summary>
    public static int Size => 15;

    /// <summary>
    /// The complete size of the rule, including header + point count
    /// </summary>
    public uint RuleLength => NetConversions.SwapUInt32(_ruleLength);

    /// <summary>
    /// ID
    /// </summary>
    public byte Id => _id;

    /// <summary>
    /// Enabled?
    /// </summary>
    public bool Enabled => _enabled != 0;

    /// <summary>
    /// Invert Break Logic?
    /// </summary>
    public bool InvertBreakLogic => _invertBreakLogic != 0;

    /// <summary>
    /// Threshold Delta
    /// </summary>
    public float ThresholdDelta => NetConversions.SwapUInt16(_thresholdDelta) / 10.0f;

    /// <summary>
    /// Break Allowance
    /// </summary>
    public ushort BreakAllowance => NetConversions.SwapUInt16(_breakAllowance);

    /// <summary>
    /// Allowance Curve Decrement
    /// </summary>
    public ushort AllowanceCurveDecrement => NetConversions.SwapUInt16(_allowanceCurveDecrement);

    /// <summary>
    /// Point Count
    /// </summary>
    public ushort PointCount => NetConversions.SwapUInt16(_pointCount);

    /// <summary>
    /// Constructor
    /// </summary>
    public NavigationAreaRuleMessage(uint ruleLength, byte id, bool enabled, bool invertBreakLogic,
        float thresholdDelta, ushort breakAllowance, ushort allowanceCurveDecrement, ushort pointCount)
    {
        _ruleLength = NetConversions.SwapUInt32(_ruleLength);
        _id = id;
        _enabled = Convert.ToByte(enabled);
        _invertBreakLogic = Convert.ToByte(invertBreakLogic);
        _thresholdDelta = NetConversions.SwapUInt16((ushort)(thresholdDelta * 10.0));
        _breakAllowance = NetConversions.SwapUInt16(breakAllowance);
        _allowanceCurveDecrement = NetConversions.SwapUInt16(allowanceCurveDecrement);
        _pointCount = NetConversions.SwapUInt16(pointCount);
    }

    /// <summary>
    /// Fixed-size components as byte array
    /// </summary>
    public byte[] Data
    {
        get
        {
            var data = new byte[Size];
            var value = BitConverter.GetBytes(_ruleLength);
            Buffer.BlockCopy(value, 0, data, 0, 4);
            value = BitConverter.GetBytes((short)Id);
            Buffer.BlockCopy(value, 0, data, 4, 1);
            value = BitConverter.GetBytes((short)_enabled);
            Buffer.BlockCopy(value, 0, data, 5, 1);
            value = BitConverter.GetBytes((short)_invertBreakLogic);
            Buffer.BlockCopy(value, 0, data, 6, 1);
            value = BitConverter.GetBytes(_thresholdDelta);
            Buffer.BlockCopy(value, 0, data, 7, 2);
            value = BitConverter.GetBytes(_breakAllowance);
            Buffer.BlockCopy(value, 0, data, 9, 2);
            value = BitConverter.GetBytes(_allowanceCurveDecrement);
            Buffer.BlockCopy(value, 0, data, 11, 2);
            value = BitConverter.GetBytes(_pointCount);
            Buffer.BlockCopy(value, 0, data, 13, 2);
            return data;
        }
    }
}