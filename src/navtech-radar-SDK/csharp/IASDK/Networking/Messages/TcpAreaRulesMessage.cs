using Navtech.IASDK.Interfaces;
using System.Runtime.InteropServices;
using System;

namespace Navtech.IASDK.Networking.Messages;
/// <summary>
/// Colossus Protocol Navigation Area Rule
/// </summary>
[StructLayout(LayoutKind.Sequential, Size = 3, Pack = 1)]
public struct TcpAreaRulesMessage : IRawData
{
    /// <summary>
    /// Size
    /// </summary>
    public static int Size => 3;

    /// <summary>
    /// Rule Length
    /// </summary>
    public byte RuleCount { get; }

    #region EnableHealth

    private readonly byte _enableHealth;

    /// <summary>
    /// Enable or Disable using the 6th rule for health information
    /// </summary>
    public bool EnableHealth => _enableHealth != 0;

    #endregion

    #region FailSafeMode
    private readonly byte _failSafeMode;

    /// <summary>
    /// Enable or disable the fail safe mode
    /// </summary>
    public bool FailSafeMode => _failSafeMode != 0;
    #endregion

    /// <summary>
    /// Default constructor for creating a message
    /// </summary>
    /// <param name="ruleCount">The number of rules in the message</param>
    /// <param name="enableHealth">Enable or disable using the 6th rule for health information</param>
    /// <param name="failSafeMode">Enable or disable fail-safe mode</param>
    public TcpAreaRulesMessage(byte ruleCount, bool enableHealth, bool failSafeMode)
    {
        RuleCount = ruleCount;
        _enableHealth = Convert.ToByte(enableHealth);
        _failSafeMode = Convert.ToByte(failSafeMode);
    }


    /// <summary>
    /// Data in network order
    /// </summary>
    public byte[] Data
    {
        get
        {
            var data = new byte[Size];
            var value = BitConverter.GetBytes((short)RuleCount);
            Buffer.BlockCopy(value, 0, data, 0, 1);
            value = BitConverter.GetBytes(EnableHealth);
            Buffer.BlockCopy(value, 0, data, 1, 1);
            value = BitConverter.GetBytes(FailSafeMode);
            Buffer.BlockCopy(value, 0, data, 2, 1);

            return data;
        }
    }
}