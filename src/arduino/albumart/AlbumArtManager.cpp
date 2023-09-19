#include "AlbumArtManager.h"
#include <arduino/logging/Logger.h>
#include <shared/misc/Utils.h>

enum {
	ALBUM_ART_HEADER = 1,
	ALBUM_ART_CHUNK = 2
} AlbumArtPayloadType;

#pragma pack(1)
struct AlbumArtHeader {
	uint8_t 	type;
	uint32_t	id;
	uint32_t	crc;
	uint32_t	totalDataSize;
	char			filename[25];
	uint8_t 	*data;
};
#pragma pack(4)

#pragma pack(1)
struct AlbumArtChunk {
	uint8_t 	type;
	uint8_t 	chunk;
	uint32_t	id;
	uint32_t	offset;
	uint8_t 	*data;
};
#pragma pack(4)

/**
 * Handles an album art chunk.
 *
 * @param chunk a pointer to the album art chunk
 * @param length the length of the album art chunk
 */
void handleAlbumArtChunk(AlbumArtChunk *chunk, unsigned int length) {
	Logger::get().printf("Album art chunk received\n");

	chunk->id = Utils::reverseByteOrder(chunk->id);
	chunk->offset = Utils::reverseByteOrder(chunk->offset);

	Logger::get().printf("id: %d; offset: %d\n", chunk->id, chunk->offset);

	// Logger::get().hexDump(chunk, length);
}

/**
 * Handles the album art header.
 *
 * @param header Pointer to the album art header
 * @param length The length of the album art header
 */
void handleAlbumArtHeader(AlbumArtHeader *header, unsigned int length) {
	Logger::get().printf("Album art header received\n");

	header->id = Utils::reverseByteOrder(header->id);
	header->crc = Utils::reverseByteOrder(header->crc);
	header->totalDataSize = Utils::reverseByteOrder(header->totalDataSize);

	Logger::get().printf("id: %d; crc: %d; totalDataSize: %d, filename: %s\n", header->id, header->crc, header->totalDataSize, header->filename);
	// Logger::get().hexDump(header, length);
}

/**
 * Handles a received album art message.
 *
 * @param payload a pointer to the byte array containing the payload
 * @param length the length of the payload array
 */
void AlbumArtManager::albumArtMessageReceived(byte *payload, unsigned int length) {
	switch (payload[0]) {
		case ALBUM_ART_HEADER:
			handleAlbumArtHeader((AlbumArtHeader *)payload, length);
			break;

		case ALBUM_ART_CHUNK:
			handleAlbumArtChunk((AlbumArtChunk *)payload, length);
			break;
	}
}
