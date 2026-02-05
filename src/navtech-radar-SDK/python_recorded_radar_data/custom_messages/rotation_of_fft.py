#####################################################################################################################
# A python recorded radar data message handling class. Used to handle a rotation of FFT data
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
from message_base import base_radar_message
import datetime
import numpy as np
import cv2

# Radar FFT message
class RotationOfFft(base_radar_message.BaseRadarMessage):

    # Constructor
    def __init__(self, fft_messages_read, start_azimuth, end_azimuth, max_fft_length, max_azimuths, max_bins, azimuth_data):

        self.fft_messages_read = fft_messages_read
        self.start_azimmuth = start_azimuth
        self.end_azimuth = end_azimuth
        self.max_fft_length = max_fft_length
        self.max_azimuths = max_azimuths
        self.max_bins = max_bins
        self.azimuth_data = azimuth_data

    # Convert the rotation of fft to a bscan image
    def ConvertToBscanImage(self):
        bscan_image = np.zeros((self.max_azimuths, self.max_bins, 3), dtype = np.uint8)
        for azimuth_index in range(0, len(self.azimuth_data)):
            if (self.azimuth_data[azimuth_index] is not None):
                bscan_image[azimuth_index, :len(self.azimuth_data[azimuth_index]), 1] = np.asarray(self.azimuth_data[azimuth_index])
        return bscan_image.astype(np.uint8)

    # Convert the rotation of fft to a ppi image
    def ConvertToPpiImage(self, scale_to = None):
        bscan_image = self.ConvertToBscanImage()
        output_size = bscan_image.shape[1]*2

        if (scale_to is not None):
            if type(scale_to) is int and scale_to < output_size:
                output_size = scale_to

        ppi_image = cv2.warpPolar(src=bscan_image, center=(int(output_size / 2), int(output_size / 2)), maxRadius=int(output_size / 2), dsize=(output_size, output_size),flags=cv2.WARP_INVERSE_MAP + cv2.WARP_POLAR_LINEAR + cv2.WARP_FILL_OUTLIERS)
        return ppi_image.astype(np.uint8)