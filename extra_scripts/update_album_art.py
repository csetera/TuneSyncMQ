#**********************************************************************************
# Copyright (C) 2023 Craig Setera
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this file,
# You can obtain one at https://mozilla.org/MPL/2.0/.
#*********************************************************************************/

#
# Publish an MQTT message to update the album art.
#
import importlib
import io
import math
import struct
import time
Import("env")
from pprint import pprint


"""
// 1 + 4 + 4 + 4 + 25 = 38 bytes header info
struct AlbumArtHeader {
	uint8_t 	type;
	uint32_t	id;
	uint32_t	crc;
	uint32_t	totalDataSize;
	char			filename[25];
	uint8_t 	*data;
};
"""
ALBUM_ART_HEADER_STRUCT_FORMAT = '!BIII25s'

"""
// 1 + 1 + 4 + 4 = 10 bytes chunk info
struct AlbumArtChunk {
	uint8_t 	type;
	uint8_t 	chunk;
	uint32_t	id;
	uint32_t	offset;
	uint8_t 	*data;
};
"""
ALBUM_ART_CHUNK_STRUCT_FORMAT = '!BBII'

MAX_PACKET_SIZE = 2 * 1024
MAX_DATA_SIZE = MAX_PACKET_SIZE - 100 # Accounting for payload header length as well as protocol headers

def on_connect(client, userdata, flags, rc):
    print("Connected with result code "+str(rc))

def update_album_art(*args, **kwargs):
    projectConfig = env.GetProjectConfig()

    # Check baseline configuration
    if not projectConfig.has_section("mqtt"): raise AssertionError("No `mqtt` section configured")
    if not projectConfig.has_option("mqtt", "host"): raise AssertionError('No MQTT host configured')
    if not projectConfig.has_option("mqtt", "user"): raise AssertionError('No MQTT user configured')
    if not projectConfig.has_option("mqtt", "password"): raise AssertionError('No MQTT password configured')

    # Install necessary modules
    print("Installing paho-mqtt module")
    env.Execute("$PYTHONEXE -m pip install paho-mqtt")

    print("Installing Pillow module")
    env.Execute("$PYTHONEXE -m pip install Pillow")
    Image = importlib.import_module('PIL.Image')

    jpegBytesIO = io.BytesIO()
    with Image.open(env['PROJECT_DIR'] + "/test/assets/images/coverimage1.jpg") as coverimage:
        resized = coverimage.resize((160, 160))
        resized.save(jpegBytesIO, format='JPEG', quality=50)
        jpegBytesIO.seek(0)

    # Connect up the MQTT client
    mqttHost = projectConfig.get("mqtt", "host")
    mqttUser = projectConfig.get("mqtt", "user")
    mqttPass = projectConfig.get("mqtt", "password")

    mqtt = importlib.import_module('paho.mqtt.client')
    client = mqtt.Client()
    client.on_connect = on_connect
    client.username_pw_set(mqttUser, mqttPass)
    client.connect(mqttHost, 1883, 60)

    client.loop_start()

    jpegBytes = jpegBytesIO.getvalue()
    print("Publishing jpeg bytes with length: " + str(len(jpegBytes)))

    chunkCount = math.ceil(len(jpegBytes) / MAX_DATA_SIZE)
    print("Chunk count: " + str(chunkCount))

    id = 456
    crc = 789

    for i in range(chunkCount):
        offset = i * MAX_DATA_SIZE
        dataChunk = jpegBytes[offset:offset+MAX_DATA_SIZE]

        if (i == 0):
            meta = struct.pack(ALBUM_ART_HEADER_STRUCT_FORMAT, 1, id, crc, len(jpegBytes), b'example.jpg')
            client.publish("tunesyncmq/update/albumart", meta + dataChunk)
        else:
            meta = struct.pack(ALBUM_ART_CHUNK_STRUCT_FORMAT, 2, i, id, offset)
            client.publish("tunesyncmq/update/albumart", meta + dataChunk)

    time.sleep(5)
    client.loop_stop()

    return

#
# Wire up the custom targets
#
env.AddCustomTarget(
    name="update_album_art",
    dependencies=None,
    actions=[
        update_album_art
    ],
    title="Update Album Art",
    description="Send a new image to update the album art",
)
