#####################################################################################################################
# Example to read the configuration data from a (.colraw / .radar) recorded radar data file
#####################################################################################################################

# ---------------------------------------------------------------------------------------------------------------------
# Copyright 2024 Navtech Radar Limited
# This file is part of IASDK which is released under The MIT License (MIT).
# See file LICENSE.txt in project root or go to https:#opensource.org/licenses/MIT
# for full license details.
#
# Disclaimer:
# Navtech Radar is furnishing this item "as is". Navtech Radar does not provide 
# any warranty of the item whatsoever, whether express, implied, or statutory,
# including, but not limited to, any warranty of merchantability or fitness
# for a particular purpose or any warranty that the contents of the item will
# be error-free.
# In no respect shall Navtech Radar incur any liability for any damages, including,
# but limited to, direct, indirect, special, or consequential damages arising
# out of, resulting from, or any way connected to the use of the item, whether
# or not based upon warranty, contract, tort, or otherwise; whether or not
# injury was sustained by persons or property or otherwise; and whether or not
# loss was sustained from, or arose out of, the results of, the item, or any
# services that may be provided by Navtech Radar.
# ---------------------------------------------------------------------------------------------------------------------

# Imports
import sys, os, argparse
sys.path.insert(0, os.path.abspath(os.path.join(os.path.dirname( __file__ ), '..')))
from recording_file_reader import radar_recording_file_reader

parser = argparse.ArgumentParser(
    description="Reads the configuration data from the file"
)

parser.add_argument("-f", "--filename", help="The location of the file to convert")

args = parser.parse_args()

# Create a radar file reader
# you can use either a .colraw file or a .radar file
recorded_data_reader = radar_recording_file_reader.RadarRecordingReader(args.filename)

# The metadata/config messages will always be read automatically - this is how you access them
# NOTE - metadata message type will be None for an old style .colraw file
metadata_message, config_message = recorded_data_reader.metadata, recorded_data_reader.config

# Print out the metadata message
if (metadata_message != None):
    print("\n----------Metadata Message----------\n")
    print(f"Start Date:  {metadata_message.start_date}")
    print(f"End Date:    {metadata_message.end_date}")
    print(f"Start Ticks: {metadata_message.start_ticks}")
    print(f"End Ticks:   {metadata_message.end_ticks}")
    print(f"Radar IP:    {metadata_message.radar_ip}")
    print("\n")

# Print out the config message
print("\n----------Config Message----------\n")
print(f"Azimuth Samples:  {config_message.azimuth_samples} samples/rotation")
print(f"Bin Size:         {config_message.bin_size/10} mm/bin")
print(f"Range in Bins:    {config_message.range_in_bins} bins")
print(f"Encoder Size:     {config_message.encoder_size} counts/rotation")
print(f"Rotation Speed:   {config_message.rotation_speed} mHz")
print(f"Packet Rate:      {config_message.packet_rate} azimuths/second")
print(f"Range Gain:       {config_message.range_gain} ")
print(f"Range Offset:     {config_message.range_offset} m")
print("\n")