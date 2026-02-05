using System.Collections.Generic;
using System;
using System.Linq;
using Navtech.IASDK.Interfaces;

namespace Navtech.IASDK.Networking.Messages;


/// <summary>
/// Class to represent radar model attributes
/// </summary>
public class RadarModel : ICloneable
{
    /// <summary>
    /// Legacy model Id for backward compatability
    /// </summary>
    public uint LegacyId { get; set; }

    /// <summary>
    /// Unique Model Id
    /// </summary>
    public Guid Id { get; set; }

    /// <summary>
    /// Model name
    /// </summary>
    public string Name { get; set; }

    /// <summary>
    /// Models designated operating range
    /// </summary>
    public double RangeInMetres { get; set; }


    /// <inheritdoc cref="ICloneable"/>
    public object Clone()
    {
        return MemberwiseClone();
    }
}

/// <summary>
/// Features available on the radar
/// </summary>
public enum RadarFeatureFlag
{
    /// <summary>
    /// None
    /// </summary>
    None = 0,
    /// <summary>
    /// Autotune (depricated)
    /// </summary>
    AutoTune = 1,
    /// <summary>
    /// Onboard processing installed
    /// </summary>
    SecondaryProcessingModulePresent = 2
}

/// <summary>
/// Radar discovery data
/// </summary>
public class DiscoveryData : IExpirable, ICloneable
{
    /// <summary>
    /// Default Constructor
    /// </summary>
    public DiscoveryData()
    {
        IPClients = new List<string>();
    }
    /// <summary>
    /// Unique Id of sesnor
    /// </summary>
    public Guid UniqueId { get; set; }
    /// <summary>
    /// IP address of sensor
    /// </summary>
    public string TCPIPAddress { get; set; }
    /// <summary>
    /// TCP port of sensor TCP server
    /// </summary>
    public ushort TCPPort { get; set; }
    /// <summary>
    /// MAC address of sensor
    /// </summary>
    public string MACAddress { get; set; }
    /// <summary>
    /// Sensor serial number
    /// </summary>
    public ushort RadarSerial { get; set; }
    /// <summary>
    /// Radar model type
    /// <see cref="RadarModel"/>
    /// </summary>
    public RadarModel Model { get; set; }
    /// <summary>
    /// Radar features
    /// <see cref="RadarFeatureFlag"/>
    /// </summary>
    public RadarFeatureFlag RadarFeatures { get; set; }

    /// <summary>
    /// Staring Mode Enabled
    /// </summary>
    public bool StaringMode { get; set; }

    /// <summary>
    /// Transmitter Enabled
    /// </summary>
    public bool TransmitterEnabled { get; set; }

    /// <summary>
    /// Onboard MAC Adddress
    /// </summary>
    public string OnBoardMacAddress { get; set; }

    /// <summary>
    /// Max Clients Allowed
    /// </summary>
    public uint MaxClientsAllowed { get; set; }

    /// <summary>
    /// IP Clients
    /// </summary>
    public List<string> IPClients { get; set; }

    /// <inheritdoc cref="IExpireable"/>
    public int ExpirePeriod { get; set; }

    /// <inheritdoc cref="IExpireable"/>
    public bool Expired => (DateTime.UtcNow - LastSeen).TotalMilliseconds > ExpirePeriod;

    /// <inheritdoc cref="IExpireable"/>
    public DateTime LastSeen { get; set; }
    /// <summary>
    /// Range resolution in metres
    /// </summary>
    public double RangeResolutionMetres { get; set; }
    /// <summary>
    /// Range in bins (number of cells)
    /// </summary>
    public ushort RangeInBins { get; set; }
    /// <summary>
    /// Number of azimuths per rotation
    /// </summary>
    public ushort NumAzimuths { get; set; }

    /// <inheritdoc cref="ICloneable" />
    public object Clone()
    {
        var dd = (DiscoveryData)MemberwiseClone();
        dd.IPClients = IPClients.Select(i => i).ToList();
        dd.Model = (RadarModel)Model?.Clone();
        return dd;
    }
}

/// <summary>
/// A collection of disovery data instances
/// </summary>
public class DiscoveryDataUpdate
{
    /// <summary>
    /// List of discovery data instances
    /// </summary>
    public IEnumerable<DiscoveryData> DiscoveryData { get; set; }
}