
#include "ogg_routines.h"

#include "ustring.h"
#include <string.h>

//https://dismaldenizen.wordpress.com/2010/08/20/the-ogg-container-format-explained/
int htogg_write_page(ogg_page *page, FileAccess *fa){
	fa->store_buffer(page->header, page->header_len);
	fa->store_buffer(page->body, page->body_len);
	return page->header_len + page->body_len;
}

int ogg_buffer(FileAccess *fa, ogg_sync_state *oy){
  uint8_t * buffer  = (uint8_t *) ogg_sync_buffer (oy, 4096);
  int bytes = fa->get_buffer(buffer, 4096);
  ogg_sync_wrote (oy,bytes);
  return (bytes);
}

bool is_fishhead_packet(ogg_packet *packet){
	return packet &&
			packet->bytes > 8 &&
			memcmp(packet->packet, "fishead", 8) == 0;
}

bool is_fishbone_packet(ogg_packet *packet){
	return packet->packet &&
			packet->bytes >= 52 &&
			memcmp(packet->packet, "fisbone", 8) == 0;
}

unsigned char * write_le32(unsigned char* p, const ogg_uint32_t num){
  ogg_int32_t n = num;
  ogg_int32_t i;
  for (i=0; i<4; i++) {
	 p[i] = (unsigned char)(n & 0xff);
	 n >>= 8;
  }
  return p + 4;
}

unsigned char * write_le16(unsigned char* p, const ogg_uint32_t num){
  ogg_uint16_t n = num;
  p[0] = (unsigned char)(n & 0xff);
  p[1] = (unsigned char)((n >> 8) & 0xff);
  return p + 2;
}