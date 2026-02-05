#####################################################################################################################
# Example to read an entire rotation of fft data from a (.colraw / .radar) recorded radar data file
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
import cv2

parser = argparse.ArgumentParser(
    description="Reads a single rotation of FFT data"
)

parser.add_argument("-f", "--filename", help="The location of the file to convert")

args = parser.parse_args()

# Create a radar file reader
# you can use either a .colraw file or a .radar file
recorded_data_reader = radar_recording_file_reader.RadarRecordingReader(args.filename)

# Read the next rotation of fft data
# this is presented as a list of lists of fft data
rotation_of_fft_data = recorded_data_reader.ReadNextRotationOfFftData()

print("\n----------Rotation of FFT Data----------\n")
print(f"Number of FFT Data: {rotation_of_fft_data.fft_messages_read}")
print(f"Start Azimuth:      {rotation_of_fft_data.start_azimmuth}")
print(f"End Azimuth:        {rotation_of_fft_data.end_azimuth}")
print(f"Max FFT Length:     {rotation_of_fft_data.max_fft_length}")
print(f"Max Azimuths:       {rotation_of_fft_data.max_azimuths}")
print(f"Max Bins:           {rotation_of_fft_data.max_bins}")
print("\n")

# Convert the fft data to a bscan image
bscan_image = rotation_of_fft_data.ConvertToBscanImage()

# Save the bscan image to disk
cv2.imwrite("data/bscan_image.png", bscan_image)

# Convert the fft data to a ppi image
ppi_image = rotation_of_fft_data.ConvertToPpiImage()

# Save the ppi image to disk
cv2.imwrite("data/ppi_image.png", ppi_image)