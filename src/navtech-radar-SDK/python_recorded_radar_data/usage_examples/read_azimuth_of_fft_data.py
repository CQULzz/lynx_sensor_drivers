#####################################################################################################################
# Example to read an fft data from a (.colraw / .radar) recorded radar data file
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
import matplotlib.pyplot as plt

parser = argparse.ArgumentParser(
    description="Reads a single azimuth of FFT data from the file"
)

parser.add_argument("-f", "--filename", help="The location of the file to convert")

args = parser.parse_args()

# Create a radar file reader
# you can use either a .colraw file or a .radar file
recorded_data_reader = radar_recording_file_reader.RadarRecordingReader(args.filename)

# Read the next fft message
fft_message = recorded_data_reader.ReadNextFftMessage()

print("\n----------FFT Message----------\n")
print(f"Bit Depth:        {fft_message.bit_depth}")
print(f"FFT Data Offset:  {fft_message.fft_data_offset}")
print(f"Sweep Counter:    {fft_message.sweep_counter}")
print(f"Azimuth:          {fft_message.azimuth}")
print(f"Seconds:          {fft_message.seconds}")
print(f"Split Seconds:    {fft_message.split_seconds}")
print(f"Timestamp:        {fft_message.timestamp}")
print(f"FFT Data:         {fft_message.fft_data}")
print("\n")

# Plot the single azimuth of FFT data
if len(fft_message.fft_data) <= 0:
    print("No data points to plot")
    exit()
plt.figure("One Azimuth of {}bit FFT Radar Data", figsize=(20, 10)),format(fft_message.bit_depth)
bearing = 360 * fft_message.azimuth / recorded_data_reader.config.encoder_size
plt.title(f"{fft_message.bit_depth}bit FFT Radar Data from bearing {round(bearing,2)} at { fft_message.timestamp}", fontsize=12)
plt.plot(fft_message.fft_data, linewidth = 0.5)
plt.ylabel('Returned power', fontsize=12)
plt.xlabel('Reporting bin', fontsize=12)
plt.tight_layout()
plt.savefig(f"{recorded_data_reader.output_path}/one_azimuth_of_fft.png")