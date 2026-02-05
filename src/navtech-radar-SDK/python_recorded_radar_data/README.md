# Python 3 Radar SDK

This folder contains a Python 3 Radar SDK. Which facilitates the conenction to a radar, bi directional message handling, and examples of common functionality of the SDK.

--------------------------------------------------------------------------------------------------------------------

# Requirements

This version of the SDK was developed on Ubuntu 22.04, using Python version 3.11.3. The SDK has been designed with backwards compatability in mind, so previous Python versions may also work.

Additional Python package requirements to run all SDK code and examples:

matplotlib==3.5.3
numpy==1.17.4
opencv_python==4.6.0.66
protobuf==3.19.4

--------------------------------------------------------------------------------------------------------------------

# Directory structure
.<br>
├── custom_messages<br>
│   ├── __init__.py<br>
│   └── rotation_of_fft.py<br>
├── LICENSE.txt<br>
├── message_base<br>
│   ├── base_radar_message.py<br>
│   └── __init__.py<br>
├── messages_from_file<br>
│   ├── configuration_message.py<br>
│   ├── fft_message_8_bit.py<br>
│   ├── health_message.py<br>
│   ├── __init__.py<br>
│   └── metadata_message.py<br>
├── README.md<br>
├── recording_file_reader<br>
│   ├── __init__.py<br>
│   └── radar_recording_file_reader.py<br>
└── usage_examples<br>
    ├── convert_radar_recording_to_bscan_video.py<br>
    ├── convert_radar_recording_to_ppi_video.py<br>
    ├── read_azimuth_of_fft_data.py<br>
    ├── read_configuration_data.py<br>
    ├── read_health_data.py<br>
    └── read_rotation_off_fft_data.py<br>

--------------------------------------------------------------------------------------------------------------------

# custom_messages
Contains class definitions for custom messages, which are not based on standard radar (Colossus) messages, and are unique to the Python radar SDK.

# message_base
Contains the base class from which the standard radar messages inherit from.

# messages_from_file
Contains message creating/handling functions

# recording_file_reader
Contains the radar recording file reader, which reads messages from a .colraw or .radar file

# usage_examples
Contains examples of how to use the key functionality that the Python radar SDK provides.

--------------------------------------------------------------------------------------------------------------------

# Explanation of key SDK functionality

The below worked example shows how to import the radar recording file reader into your Python script and use it to read messages

## Importing the SDK

The Python radar SDK can be imported into your project from another location by appending the location of the SDK to your Python system path. The following example allows the Python SDK from location iasdk/python_recorded_radar_data to be accessed from an example python script within the examples folder at iasdk/python_recorded_radar_data/usage_examples.

```
import sys, os
sys.path.insert(0, os.path.abspath(os.path.join(os.path.dirname( __file__ ), '..')))
```

## Creating a recorded radar data reader object

A radar data reader object can be created by calling the RadarRecordingReader constructor from the radar_recording_file_reader package, and specifying the relative path to the recording file to open.

```
recorded_data_reader = radar_recording_file_reader.RadarRecordingReader("Test-Radar-1_192-168-11-12_20240704080044073.radar")
```

## Reading messages from the radar recording file

Messages in the radar recording file are in the order that the radar sent them when they were originally recorded, and messages can only be read from the file (you cannot request fft data, nav data etc). Radar recordings generally contain several basic types of message data, which are: meta data (.radar files only), configuration data, fft data and health data. A radar recording always contains a metadata (if .radar filetype supplied) and a configuration message (present in both .radar and .colraw) at the start, and this is read automatically and stored in the RadarRecordingReader object. Access the metadata and configuration data as follows:

```
metadata_message, config_message = recorded_data_reader.metadata, recorded_data_reader.config
```
### NOTE - metadata message type will be None for an old style .colraw file

Read the next fft data in the file as follows:

```
fft_message = recorded_data_reader.ReadNextFftMessage()
```

Read the next health data in the file as follows:

```
fft_message = recorded_data_reader.ReadNextHealthMessage()
```

--------------------------------------------------------------------------------------------------------------------

# Example projects
# See "usage_examples" folder
The SDK comes with six example projects, designed to demonstrate the key functionality of the Python radar recording file SDK.

## read_configuration_data
This example opens up a radar recording file, and demonstrates how to access and print out the metadata (for a .radar file) and the configuration data (for both a .radar and a .colraw file)

## read_azimuth_of_fft_data
This example opens up a radar recording file, and demonstrates how to access, print out, and save a plot of the first available fft data in the file

## read_health_data
This example opens up a radar recording file, and demonstrates how to access and print out the first available health data in the file

## read_rotation_off_fft_data
This example opens up a radar recording file, and demonstrates how to access the first complete rotation of fft data

## convert_radar_recording_to_bscan_video
This example opens up a radar recording file, and demonstrates how to read every rotation of fft data in the file, and save it as a bscan style video of the radar data

## convert_radar_recording_to_ppi_video
This example opens up a radar recording file, and demonstrates how to read every rotation of fft data in the file, and save it as a ppi style (previously known as cartesian) video of the radar data