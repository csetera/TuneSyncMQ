#pragma once

#include <Arduino.h>

namespace AlbumArtManager {
	void albumArtMessageReceived(byte *payload, unsigned int length);
}
