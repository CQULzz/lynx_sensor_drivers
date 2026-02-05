using Navtech.IASDK.Interfaces;

namespace Navtech.IASDK.Networking;

/// <summary>
/// A basic object used to carry raw data (bytes)
/// </summary>
public class RawData : IRawData
{
    /// <summary>
    /// This constructor will allocated a buffer of dataLength
    /// </summary>
    /// <param name="dataLength">Length of buffer</param>
    public RawData(int dataLength)
    {
        Data = new byte[dataLength];
    }

    /// <summary>
    /// Default constructor - takes message data as bytes
    /// </summary>
    /// <param name="rawData">Byte array of message data</param>
    public RawData(byte[] rawData)
    {
        Data = rawData;
    }

    /// <summary>
    /// The raw message data
    /// </summary>
    public byte[] Data { get; }
}