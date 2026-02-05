namespace Navtech.IASDK.Networking.Messages;

/// <summary>
/// Accelerometer Data
/// </summary>
public class AccelerometerData
{
    /// <summary>
    /// Theta
    /// </summary>
    public float Theta { get; set; }

    /// <summary>
    /// Phi
    /// </summary>
    public float Phi { get; set; }

    /// <summary>
    /// Psi
    /// </summary>
    public float Psi { get; set; }

    /// <summary>
    /// Default Constructor
    /// </summary>
    /// <param name="message"></param>
    public AccelerometerData(AccelerometerDataMessage message)
    {
        Theta = message.Theta;
        Phi = message.Phi;
        Psi = message.Psi;
    }
}