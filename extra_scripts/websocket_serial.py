#**********************************************************************************
# Copyright (C) 2023 Craig Setera
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this file,
# You can obtain one at https://mozilla.org/MPL/2.0/.
#*********************************************************************************/

#
# PlatformIO custom target to connect a to the Websocket
# serial port
#
# https://websocket-client.readthedocs.io/en/latest/index.html
#
import importlib
Import("env")
from pprint import pprint

def on_ws_error(wsapp, exception):
    print("Error: " + str(exception))

def on_ws_message(wsapp, message):
    print(message)

def on_ws_open(wsapp):
    print("Connection opened")

def websocket_serial(*args, **kwargs):
    projectConfig = env.GetProjectConfig()

    # Determine the right MDNS name to be used
    mdnsNameKey = "mdns_name"

    # Check baseline configuration
    if not projectConfig.has_section("wifi"): raise AssertionError("No `wifi` section configured")
    if not projectConfig.has_option("wifi", mdnsNameKey): raise AssertionError('No MDNS name configured')

    # Figure out the websocket URL
    mdns_name = projectConfig.get("wifi", mdnsNameKey) + ".local"
    ws_url = "ws://" + mdns_name + "/ws_serial"

    # Make sure the websocket module is installed and dynamically load it
    print("Installing websocket-client module")
    env.Execute("$PYTHONEXE -m pip install websocket-client")
    websocket = importlib.import_module('websocket')

    # Start up the listener
    print("Starting up Websocket connection on: " + ws_url)
    wsapp = websocket.WebSocketApp(ws_url, on_error=on_ws_error, on_message=on_ws_message, on_open=on_ws_open)
    wsapp.run_forever()

    return

#
# Wire up the custom targets
#
env.AddCustomTarget(
    name="websocket_serial",
    dependencies=None,
    actions=[
        websocket_serial
    ],
    title="Websocket Serial",
    description="Connect to Websocket serial on device"
)
