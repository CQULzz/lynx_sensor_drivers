/*
Copyright 2024 Navtech Radar Limited
This file is part of IASDK which is released under The MIT License (MIT).
See file LICENSE.txt in project root or go to https://opensource.org/licenses/MIT
for full license details.

Disclaimer:
Navtech Radar is furnishing this item "as is". Navtech Radar does not provide 
any warranty of the item whatsoever, whether express, implied, or statutory,
including, but not limited to, any warranty of merchantability or fitness
for a particular purpose or any warranty that the contents of the item will
be error-free.
In no respect shall Navtech Radar incur any liability for any damages, including,
but limited to, direct, indirect, special, or consequential damages arising
out of, resulting from, or any way connected to the use of the item, whether
or not based upon warranty, contract, tort, or otherwise; whether or not
injury was sustained by persons or property or otherwise; and whether or not
loss was sustained from, or arose out of, the results of, the item, or any
services that may be provided by Navtech Radar.
*/

namespace Navtech.IASDK.Enums
{
    /// <summary>
    /// Network message types
    /// Not all are relevant to the radar - client communications and some have
    /// been deprecated or are no longer used
    /// </summary>
    public enum TcpNetworkDataMessageType : byte
    {
        /// <summary>
        /// Invalid message type
        /// </summary>
        Invalid = 0,
        /// <summary>
        /// Keep alive message
        /// </summary>
        KeepAlive = 1,
        /// <summary>
        /// Configuration message
        /// </summary>
        Configuration = 10,
        /// <summary>
        /// Configuration request message
        /// </summary>
        ConfigurationRequest = 20,
        /// <summary>
        /// Request to start FFT Data message
        /// </summary>
        StartFftData = 21,
        /// <summary>
        /// Request to stop FFT Data message
        /// </summary>
        StopFftData = 22,
        /// <summary>
        /// Request to start health data message
        /// </summary>
        StartHealthMsgs = 23,
        /// <summary>
        /// Request to stop health data message
        /// </summary>
        StopHealthMsgs = 24,
        /// <summary>
        /// Request to recalibrate RF health settings
        /// </summary>
        ReCalibrateRfHealth = 25,
        /// <summary>
        /// Request to start tracks - only relevant with on-board tracker
        /// </summary>
        StartTracks = 26,
        /// <summary>
        /// Request to stop tracks - only relevant with on-board tracker
        /// </summary>
        StopTracks = 27,
        /// <summary>
        /// Request to switch head on and start transmitting
        /// </summary>
        TransmitOn = 28,
        /// <summary>
        /// Request to switch head off and stop transmitting
        /// </summary>
        TransmitOff = 29,
        /// <summary>
        /// FFT Data message
        /// </summary>
        FftData = 30,
        /// <summary>
        /// Request high precision FFT Data - Not yet implemented
        /// </summary>
        HighPrecisionFftData = 31,
        /// <summary>
        /// Health message
        /// </summary>
        Health = 40,
        /// <summary>
        /// Contour map update message
        /// </summary>
        ContourUpdate = 50,
        /// <summary>
        /// Track update message
        /// </summary>
        TrackUpdate = 60,
        /// <summary>
        /// Tracker configuration message - only relevant with on-board tracker
        /// </summary>
        TrackerConfiguration = 70,
        /// <summary>
        /// Tracker file playback message - only relevant with on-board tracker
        /// </summary>
        TrackerPlaybackCommand = 71,
        /// <summary>
        /// Tracker save clutter map message - only relevant with on-board tracker
        /// </summary>
        TrackerSaveClutterMap = 72,
        /// <summary>
        /// Legacy health message - used for older generation of radar - Deprecated
        /// </summary>
        TrackerLegacyHealthUnits = 73,
        /// <summary>
        /// Tracker source update message - only relevant with on-board tracker
        /// </summary>
        TrackerDataSourceUpdate = 74,
        /// <summary>
        /// Tracker distribution update message - only relevant with on-board tracker
        /// </summary>
        TrackerDistributionUpdate = 75,
        /// <summary>
        /// System restart message - Tracker only
        /// </summary>
        SystemRestart = 76,
        /// <summary>
        /// Change logging levels message - Deprecated
        /// </summary>
        LoggingLevels = 90,
        /// <summary>
        /// Request logging levels message - Deprecated
        /// </summary>
        LoggingLevelsRequest = 100,
        /// <summary>
        /// Sets auto tune params - Not used
        /// </summary>
        SetAutoTune = 110,
        /// <summary>
        /// Start navigation data message
        /// </summary>
        StartNavData = 120,
        /// <summary>
        /// Stop navigation data message
        /// </summary>
        StopNavData = 121,
        /// <summary>
        /// Set navigation threshold message
        /// </summary>
        SetNavThreshold = 122,
        /// <summary>
        /// Navigation data message
        /// </summary>
        NavigationData = 123,
        /// <summary>
        /// Set range gain and offset message
        /// </summary>
        SetNavRangeOffsetAndGain = 124,
        /// <summary>
        /// Calibrate Accelerometer
        /// </summary>
        CalibrateAccelerometer = 125,
        /// <summary>
        /// Start Accelerometer
        /// </summary>
        StartAccelerometer = 126,
        /// <summary>
        /// Stop Accelerometer
        /// </summary>
        StopAccelerometer = 127,
        /// <summary>
        /// Accelerometer Data
        /// </summary>
        AccelerometerData = 128,
        /// <summary>
        /// Navigation Alarm Data
        /// </summary>
        NavigationAlarmData = 143,
        /// <summary>
        /// Navigation Area Rules
        /// </summary>
        NavigationAreaRules = 144,
        /// <summary>
        /// Key exchange message - only relevant with on-board tracker
        /// </summary>
        KeyExchange = 200,
        /// <summary>
        /// Content key message - only relevant with on-board tracker
        /// </summary>
        ContentEncryptionKey = 201,
        /// <summary>
        /// Encrypted data message - only relevant with on-board tracker
        /// </summary>
        EncryptedData = 202,
        /// <summary>
        /// Navigation Configuration Request
        /// </summary>
        NavigationConfigurationRequest = 204,
        /// <summary>
        /// Set Navigation Configuration
        /// </summary>
        SetNavigationConfiguration = 205,
        /// <summary>
        /// Request Navigation Area Rules
        /// </summary>
        RequestNavigationAreaRules = 206,
        /// <summary>
        /// Request Time Server Status
        /// </summary>
        RequestTimeServerStatus = 207,
        /// <summary>
        /// Time Server Status
        /// </summary>
        TimeServerStatus = 208,
        /// <summary>
        /// Start Radar
        /// </summary>
        StartRadar = 209,
        /// <summary>
        /// Stop Radar
        /// </summary>
        StopRadar = 210,
        /// <summary>
        /// Change Profile
        /// </summary>
        ChangeProfile = 211
    }
}
