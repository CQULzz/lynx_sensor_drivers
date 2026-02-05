#####################################################################################################################
# Example to read all radar data from a (.colraw / .radar) recorded radar data file, and save as a bscan video
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
    description="Converts a radar recording to a ppi video"
)

parser.add_argument("-f", "--filename", help="The location of the file to convert")

args = parser.parse_args()

# Create a radar file reader
# you can use either a .colraw file or a .radar file
recorded_data_reader = radar_recording_file_reader.RadarRecordingReader(args.filename)

# Save the radar data to a ppi video
# this will create a huge file, so scale the resultion to 1000 pixels
recorded_data_reader.ConvertRecordingToPpiVideo(scale_to = 1000)