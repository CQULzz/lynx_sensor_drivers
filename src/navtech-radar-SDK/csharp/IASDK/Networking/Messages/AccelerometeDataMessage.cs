using Navtech.IASDK.Interfaces;
using System.Runtime.InteropServices;
using System;
using Navtech.IASDK.Utility;

namespace Navtech.IASDK.Networking.Messages;

/// <summary>
/// Colossus Protocol Accelerometer Data Message
/// </summary>
[StructLayout(LayoutKind.Sequential, Size = 12, Pack = 1)]
public struct AccelerometerDataMessage : IRawData
{
    private readonly uint _theta;
    private readonly uint _phi;
    private readonly uint _psi;

    /// <summary>
    /// Size
    /// </summary>
    public static int Size => 12;

    /// <summary>
    /// Theta
    /// </summary>
    public float Theta => BitConverter.ToSingle(BitConverter.GetBytes(NetConversions.SwapUInt32(_theta)));

    /// <summary>
    /// Phi
    /// </summary>
    public float Phi => BitConverter.ToSingle(BitConverter.GetBytes(NetConversions.SwapUInt32(_theta)));

    /// <summary>
    /// Psi
    /// </summary>
    public float Psi => BitConverter.ToSingle(BitConverter.GetBytes(NetConversions.SwapUInt32(_theta)));

    /// <summary>
    /// Data
    /// </summary>
    public byte[] Data
    {
        get
        {
            var data = new byte[Size];
            var value = BitConverter.GetBytes(_theta);
            Buffer.BlockCopy(value, 0, data, 0, 4);
            value = BitConverter.GetBytes(_phi);
            Buffer.BlockCopy(value, 0, data, 4, 4);
            value = BitConverter.GetBytes(_psi);
            Buffer.BlockCopy(value, 0, data, 8, 4);

            return data;
        }
    }
}