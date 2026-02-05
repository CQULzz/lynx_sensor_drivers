using System;
using System.Collections.Generic;
using Navtech.IASDK.Utility;

namespace Navtech.IASDK.Networking.Messages
{
    /// <summary>
    /// Represents a single Navigation Area Rule
    /// </summary>
    public class NavigationAreaRule
    {
        /// <summary>
        /// ID
        /// </summary>
        public byte Id { get; set; }

        /// <summary>
        /// Enabled
        /// </summary>
        public bool Enabled { get; set; }

        /// <summary>
        /// Invert Break Logic
        /// </summary>
        public bool InvertBreakLogic { get; set; }

        /// <summary>
        /// Threshold Delta (dB)
        /// </summary>
        public float ThresholdDelta { get; set; }

        /// <summary>
        /// Break Allowance
        /// </summary>
        public ushort BreakAllowance { get; set; }

        /// <summary>
        /// Allowance Curve Decrement
        /// </summary>
        public ushort AllowanceCurveDecrement { get; set; }

        /// <summary>
        /// Navigation Area Points
        /// </summary>
        public List<AreaPoint> AreaPoints { get; set; }


        /// <summary>
        /// Create a rule from the TCP message
        /// </summary>
        /// <param name="message">Message from radar as C# type.</param>
        /// <param name="payload">The navigation area rule as a byte array</param>
        public NavigationAreaRule(NavigationAreaRuleMessage message, byte[] payload)
        {
            Id = message.Id;
            Enabled = message.Enabled;
            InvertBreakLogic = message.InvertBreakLogic;
            ThresholdDelta = message.ThresholdDelta;
            BreakAllowance = message.BreakAllowance;
            AllowanceCurveDecrement = message.AllowanceCurveDecrement;

            AreaPoints = new List<AreaPoint>();

            for (var i = 0; i < message.PointCount; i++)
            {
                var xy = NetConversions.SwapUInt32(BitConverter.ToUInt32(payload, i * 4));
                AreaPoints.Add(new AreaPoint
                {
                    X = (short)((xy & 0xFFFF0000) >> 16) / 10.0f,
                    Y = (short)(xy & 0x0000FFFF) / 10.0f
                });
            }

        }

        /// <summary>
        /// Default constructor
        /// </summary>
        public NavigationAreaRule()
        {
        }

    }
}
