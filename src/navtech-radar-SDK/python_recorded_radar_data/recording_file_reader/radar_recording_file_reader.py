#####################################################################################################################
# A python recorded radar data reader. Used to read recorded radar messages from a .colraw or a .radar file
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
import sys, os
sys.path.insert(0, os.path.abspath(os.path.join(os.path.dirname( __file__ ), '../..')))
import zlib
import gzip
from utils import logger
from messages_from_file import configuration_message
from messages_from_file import metadata_message
from messages_from_file import fft_message_8_bit
from messages_from_file import health_message
from custom_messages import rotation_of_fft
import cv2
import numpy as np

# Radar client class
class RadarRecordingReader:

    # Constructor
    def __init__(self, filepath, log_directory = "logs", log_filename = "radar_recording_reader_log"):
        self.filepath = filepath
        self.log = logger.Logger(log_directory, log_filename)
        self.file_extension = self.filepath.split(".")[-1]
        self.use_new_file_format = False
        if (self.file_extension == "colraw"):
            self.use_new_file_format = False
            self.log.write_to_log(f"Filetype is .{self.file_extension}")
        elif (self.file_extension == "radar"):
            self.use_new_file_format = True
            self.log.write_to_log(f"Filetype is .{self.file_extension}")
        else:
            self.log.write_to_log(f"File extension: ({self.file_extension}) is invalid. Quitting")
            exit()

        self.log.write_to_log("Created radar recording file reader")
        self.file = open(self.filepath, mode='rb')
        self.end_of_file = False

        # Create the data output folder, if it does not exist already
        self.output_path = "data"
        if (not os.path.exists(self.output_path)):
            self.log.write_to_log(f"Created output directory: {self.output_path}")
            os.mkdir(self.output_path)

        # First thing we do is read the config data and store it
        try:
            self.metadata, self.config = self.ReadConfigMessage()
        except Exception as ex:
            self.log.write_to_log(f"Failed to read config message - {ex}")
        

    # Read bytes from the file but check if end of file
    def read_bytes(self, num_bytes):
        try:
            bytes_read = self.file.read(num_bytes)
            if not bytes_read:
                self.end_of_file = True
            return bytes_read
        except Exception as ex:
            self.log.write_to_log(f"Failed to read bytes from file - {ex}")


    # Read the metadata/config data from the file
    # returns a config message for a .colraw recording
    # returns a metadata plus a config message for a .radar recording
    def ReadConfigMessage(self):

        # If this is a new style recording, read the metadata record first
        metadata = None
        if (self.use_new_file_format):
            payload_size = int.from_bytes(self.read_bytes(4), byteorder='big')
            metadata = metadata_message.MetadataMessage(self.read_bytes(payload_size - 4))
            self.log.write_to_log("Read metadata message")

        # Read the config message
        if (not self.use_new_file_format):
            self.read_bytes(4)
        message_type = int.from_bytes(self.read_bytes(1), byteorder='little')
        
        if (message_type != 10):
            self.log.write_to_log("\nFirst message is not a config")
            exit()
        self.log.write_to_log("Read config")

        if (self.use_new_file_format):
            self.read_bytes(29)
            message_type = int.from_bytes(self.read_bytes(1), byteorder='little')
        else:
            self.read_bytes(26)

        payload_size = int.from_bytes(self.read_bytes(4), byteorder='big')

        config = configuration_message.ConfigurationMessage(self.read_bytes(payload_size))

        return metadata, config


    # Read the next message from the file
    def ReadNextMessage(self):

        try:
            if (self.use_new_file_format):
                message_type = int.from_bytes(self.read_bytes(1), byteorder='little')
                self.read_bytes(8)
                message_length = int.from_bytes(self.read_bytes(4), byteorder='big')
            else:
                message_length = int.from_bytes(self.read_bytes(4), byteorder='little')
                message_type = int.from_bytes(self.read_bytes(1), byteorder='little')
                self.read_bytes(8)
            
            message_data = self.read_bytes(message_length)

        except Exception as ex:
            self.log.write_to_log(f"Failed to read next message - {ex}")

        # If the message type is compressed data, then uncompress it
        try:
            if (message_type == 255):
                if (self.use_new_file_format):
                    message_data = gzip.decompress(message_data)
                    message_type = message_data[0]
                else:
                    message_data = zlib.decompress(message_data, -15)
                    message_type = message_data[4]
            return message_type, message_data
        except Exception as ex:
            self.log.write_to_log(f"Failed to uncompress next message - {ex}")


    # Read the next FFT message from the file
    def ReadNextFftMessage(self):
        message_type, message_data = self.ReadNextMessage()

        while (message_type != 30 and not self.end_of_file):
            message_type, message_data = self.ReadNextMessage()
        
        # FFT data
        if (message_type == 30):
            fft = fft_message_8_bit.FftMessage8Bit(message_data[35:])
            return fft

    # Read the next FFT message from the file
    def ReadNextHealthMessage(self):
        message_type, message_data = self.ReadNextMessage()

        while (message_type != 40 and not self.end_of_file):
            message_type, message_data = self.ReadNextMessage()
        
        # Health message
        if (message_type == 40):
            health = health_message.HealthMessage(message_data[35:])
            return health

    # Read the next rotation of FFT messages from the file
    def ReadNextRotationOfFftData(self):

        try:
            fft_messages_read = 0
            start_azimuth = 0
            end_azimuth = 0
            max_fft_length = 0
            max_azimuths = 0
            max_bins = 0
            azimuth_data = [None] * self.config.azimuth_samples
            previous_azimuth_index = 0

            message_type, message_data = self.ReadNextMessage()
            while (message_type != 30 and not self.end_of_file):
                message_type, message_data = self.ReadNextMessage()
            fft = fft_message_8_bit.FftMessage8Bit(message_data[35:])
            fft_messages_read += 1
            start_azimuth = fft.azimuth
            end_azimuth = fft.azimuth
            max_azimuths = self.config.azimuth_samples
            max_bins = self.config.range_in_bins
            azimuth_index = int(fft.azimuth/self.config.encoder_size * self.config.azimuth_samples)
            azimuth_data[azimuth_index] = fft.fft_data
            previous_azimuth_index = azimuth_index
            if (len(fft.fft_data)) > max_fft_length:
                max_fft_length = len(fft.fft_data)

            while (fft_messages_read < self.config.azimuth_samples and not self.end_of_file):
                message_type, message_data = self.ReadNextMessage()
                if (message_type == 30):
                    fft = fft_message_8_bit.FftMessage8Bit(message_data[35:])
                    fft_messages_read += 1
                    azimuth_data[azimuth_index] = fft.fft_data
                    end_azimuth = fft.azimuth

                    azimuth_index = int(fft.azimuth/self.config.encoder_size * self.config.azimuth_samples)
                    if (previous_azimuth_index == self.config.azimuth_samples - 1):
                        previous_azimuth_index = -1
                    if (azimuth_index != previous_azimuth_index+1):
                        self.log.write_to_log(f"No data for azimuth {azimuth_index+1}")
                        fft_messages_read += 1
                    previous_azimuth_index = azimuth_index

                    if (max_bins == 0):
                        max_bins = self.config.range_in_bins
                    if (max_azimuths == 0):
                        max_azimuths = self.config.azimuth_samples
                    if (len(fft.fft_data)) > max_fft_length:
                        max_fft_length = len(fft.fft_data)
            
            return rotation_of_fft.RotationOfFft(fft_messages_read, start_azimuth, end_azimuth, max_fft_length, max_azimuths, max_bins, azimuth_data)

        except Exception as ex:
            self.log.write_to_log(f"Failed to read next rotation of FFT data - {ex}")

    # Read all FFT data from the file, and save to a bscan video
    def ConvertRecordingToBscanVideo(self):
        try:
            first_frame = True
            video_writer = None

            while (not self.end_of_file):
                rotation_of_fft = self.ReadNextRotationOfFftData()
                bscan_image = rotation_of_fft.ConvertToBscanImage()
                
                if (first_frame):
                    video_writer = cv2.VideoWriter(f"{self.output_path}/radar_data_bscan_video.avi", cv2.VideoWriter_fourcc(*'XVID'), int(self.config.rotation_speed / 1000), (bscan_image.shape[1], bscan_image.shape[0]))
                    first_frame = False

                else:
                    video_writer.write(bscan_image.astype(np.uint8))

            video_writer.release()
        except Exception as ex:
            self.log.write_to_log(f"Failed to create bscan video from radar data - {ex}")

    # Read all FFT data from the file, and save to a ppi video
    def ConvertRecordingToPpiVideo(self, scale_to = None):
        try:
            first_frame = True
            video_writer = None

            while (not self.end_of_file):
                rotation_of_fft = self.ReadNextRotationOfFftData()
                ppi = rotation_of_fft.ConvertToPpiImage(scale_to)
                
                if (first_frame):
                    video_writer = cv2.VideoWriter(f"{self.output_path}/radar_data_ppi_video.avi", cv2.VideoWriter_fourcc(*'XVID'), int(self.config.rotation_speed / 1000), (ppi.shape[1], ppi.shape[0]))
                    first_frame = False

                else:
                    video_writer.write(ppi.astype(np.uint8))

            video_writer.release()
        except Exception as ex:
            self.log.write_to_log(f"Failed to create ppi video from radar data - {ex}")