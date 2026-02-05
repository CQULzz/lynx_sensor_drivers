#####################################################################################################################
# A python recorded radar data message handling class. Used to handle a metadata message from a file
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

# Radar metadata message
class MetadataMessage(base_radar_message.BaseRadarMessage):

    # Constructor
    def __init__(self, payload_data):

        # Standard metadata message data
        self.start_date = int.from_bytes(payload_data[0:8], byteorder='big')
        self.end_date = int.from_bytes(payload_data[8:16], byteorder='big')
        self.start_ticks = int.from_bytes(payload_data[16:24], byteorder='big')
        self.end_ticks = int.from_bytes(payload_data[24:32], byteorder='big')
        self.radar_ip = f"{payload_data[32]}.{payload_data[33]}.{payload_data[34]}.{payload_data[35]}"