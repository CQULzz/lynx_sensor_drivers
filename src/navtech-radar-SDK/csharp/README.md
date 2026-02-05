Using the SDK in C#
===================


## C# Radar Client API

The .NET API is based on C# 12 with .NET 8.0 and was developed in Visual Studio 2022.
There are two project within the repro:

1. **IASDK** - The API DLL for use within any 3rd party projects to assist with connecting to the radar
1. **TestClient** - This is a very simple console application that runs up, connects to a radar and then displays some information before auto-disconnecting and closing. This provides a simple example of the recommended steps to connect and consume data from the radar.


### Usage of the SDK

The steps involved in connecting and getting data are as follows:

Setup your radar client and hook up the message and connection events:

```C#
_radarTcpClient = new RadarTcpClient();

_radarTcpClient.OnConfigurationData += ConfigurationDataHandler;

_radarTcpClient.OnFftData += FftDataHandler;

_radarTcpClient.OnConnectionChanged += ConnectionChangedHandler;
```

Connect to the radar:
```
_radarTcpClient.Connect("192.168.0.1");
```

On successful connection you will receive a Configuration message with details of the radar's current configuration. So you must have the handler setup before you connect.
```C#
private void ConfigurationDataHandler(object sender, GenericEventArgs<TcpConfigurationDataMessage> configurationMessage)
{
	var rotationHz = configurationMessage.Payload.RotationSpeed / 1000.0	
}
```

Once connected and you have the config data, tell the radar to start sending FFT Data:
```
_radarTcpClient.StartFftData();
```

You must handle incoming FFT Data:
```C#
private static void FftDataHandler(object sender, GenericEventArgs<FftData> fftEventArgs)
{
	var azimuth = fftEventArgs.Payload.Message.Azimuth;	
}
```

When you need to disconnect, firstly stop the FFT Data:
```
_radarTcpClient.StopFftData();
```

Then disconnect:
```
_radarTcpClient.Disconnect();
```