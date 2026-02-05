namespace Navtech.IASDK.Enums;

/// <summary>
/// State of the alarm
/// </summary>
public enum AlarmState
{
    /// <summary>
    /// An inactive alarm
    /// </summary>
    Inactive = 0,

    /// <summary>
    /// The alarm is active
    /// </summary>
    Active = 1,

    /// <summary>
    /// The alarm is disabled (locally used only)
    /// </summary>
    Disabled = 2
}