#####################################################################################################################
# A python example to perform cell averaging CFAR peak detection on one azimuth of fft data
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
import matplotlib.pyplot as plt
import numpy as np
import sys, os, argparse
sys.path.insert(0, os.path.abspath(os.path.join(os.path.dirname( __file__ ), '..')))
sys.path.insert(0, os.path.abspath(os.path.join(os.path.dirname( __file__ ), '../..')))
from utils import cfar

parser = argparse.ArgumentParser(
    description="Detects a peak from one azimuth of data using CFAR"
)

parser.add_argument(
    "-f", "--filename",
    nargs='?', default = "data/one_azimuth_of_fft_data.csv",
    help="The location of the file to convert"
)

parser.add_argument(
    "-w", "--window_size",
    nargs='?', type=int,
    default=50,
    help="The window sized used for training"
)

parser.add_argument(
    "-g", "--guard_cells",
    nargs='?', type=int,
    default=2,
    help="Number of cells to ignore close to a test cell"
)

parser.add_argument(
    "-t", "--threshold_delta",
    nargs='?', type=float,
    default=30.0,
    help="Detection threshold delta to local average"
)

args = parser.parse_args()

# CFAR settings
num_train_cells = args.window_size
num_guard_cells = args.guard_cells
threshold_delta = args.threshold_delta


# Open the file containing the FFT data
filename = args.filename
try:
    data = np.genfromtxt(filename, delimiter=',')
except Exception as e:
    print("Cannot open data file {} - {}".format(filename, e))
    exit()

print ("\n######### CFAR SETTINGS #########")
print ("Train cells:            {}".format(num_train_cells))
print ("Guard cells:            {}".format(num_guard_cells))
print ("Threshold delta:        {}".format(threshold_delta))
print ("############################\n")

# Plot the data
if len(data) <= 0:
    print("No data points to plot")
    exit()

plt.figure("One Azimuth Analysis - Cell averaging CFAR peak detection")
plt.title("Single Azimuth of FFT Radar Data - CA-CFAR Peak Detection\nPeakBins marked in purple")
plt.plot(data, ".", markersize=5, color='b')

# Run CA-CFAR peak detection
# Generally, more train cells/higher threshold will result in less peaks returned
cfar_peaks = cfar.get_peak_indexes(data, num_train_cells, num_guard_cells, threshold_delta)
print("Peaks detected at:")
for cfar_peak in cfar_peaks:
    print("Bin index: {}, returned power: {}".format(cfar_peak, data[cfar_peak]))
    plt.plot(cfar_peak, data[cfar_peak],".-.", color='purple', markersize=12)
print ("\n############################")

plt.ylabel('Returned power', fontsize=12)
plt.xlabel('Reporting bin', fontsize=12)
plt.tight_layout()
plt.savefig("data/one_azimuth_of_fft_data_cfar_peaks.png")