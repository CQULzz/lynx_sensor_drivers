#####################################################################################################################
# Example to read a health data from a (.colraw / .radar) recorded radar data file
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
    description="Reads health data from the file"
)

parser.add_argument("-f", "--filename", help="The location of the file to convert")

args = parser.parse_args()

# Create a radar file reader
# you can use either a .colraw file or a .radar file
recorded_data_reader = radar_recording_file_reader.RadarRecordingReader(args.filename)

# Read the next health message
health_message = recorded_data_reader.ReadNextHealthMessage()

print("\n----------Health Message----------\n")
print("Die Temperature:         {} \u2103".format(health_message.dietemperature))
print("Soc Temperature:         {} \u2103".format(health_message.soctemperature))
print("Vco Temperature:         {} \u2103".format(health_message.vcotemperature))
print("Ambient Temperature:     {} \u2103".format(health_message.ambienttemperature))
print("Rotation:                {} mHz".format(health_message.rotation))
print("Packet Rate:             {} mHz".format(health_message.packetrate))
print("Rfhealthcheck:           {} ".format(health_message.rfhealthcheck))
print("Transmitting:            {} ".format(health_message.transmitting))
print("Expected Rotation:       {} mHz".format(health_message.expectedrotation))
print("Expected Packet Rate:    {} mHz".format(health_message.expectedpacketrate))
print("Mac Address:             {} ".format(health_message.macaddress))
print("Encoder Error Count:     {} ".format(health_message.encodererrorcount))
print("System Uptime:           {} ".format(health_message.systemuptime))
print("Motor Current:           {} ".format(health_message.motorcurrent))
print("Software Uptime:         {} ".format(health_message.softwareuptime))
print("Total Uptime:            {} ".format(health_message.totaluptime))
print("Network State:           {} ".format(health_message.networkstate))
print("Max Clients Allowed:     {} ".format(health_message.maxclientsallowed))
print("IP Clients:              {} ".format(health_message.ipclients))
print("Expected RX Packet Rate: {} ".format(health_message.expectedrxpacketrate))
print("Uplink Errors:           {} ".format(health_message.uplinkerrors))
print("Downlink Errors:         {} ".format(health_message.downlinkerrors))
print("Uplink Missed:           {} ".format(health_message.uplinkmissed))
print("Downloank Missed:        {} ".format(health_message.downlinkmissed))
print("\n")