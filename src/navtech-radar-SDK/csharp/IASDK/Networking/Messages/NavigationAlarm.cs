using Navtech.IASDK.Enums;

namespace Navtech.IASDK.Networking.Messages;

/// <summary>
/// Data structure representing an Alarm, along with its area number
/// </summary>
public record NavigationAlarm
{
    /// <summary>
    /// Default Constructor
    /// </summary>
    /// <param name="area">The area ID</param>
    /// <param name="state">True if the alarm is state</param>
    public NavigationAlarm(int area, AlarmState state)
    {
        Area = area;
        State = state;
    }

    /// <summary>
    /// The Area
    /// </summary>
    public int Area { get; }

    /// <summary>
    /// The alarm activity
    /// </summary>
    public AlarmState State { get; set; }
}