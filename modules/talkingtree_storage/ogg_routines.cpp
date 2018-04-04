
#include "ogg_routines.h"

#include "ustring.h"
#include "os/copymem.h"
#include <string.h>
#include <stdlib.h>
//https://dismaldenizen.wordpress.com/2010/08/20/the-ogg-container-format-explained/
int htogg_write_page(ogg_page *page, FileAccess *fa){
	fa->store_buffer(page->header, page->header_len);
	fa->store_buffer(page->body, page->body_len);
	return page->header_len + page->body_len;
}

void ogg_page_init(ogg_page *page){
	page ->header = NULL;
	page ->header_len = 0;
	page ->body = NULL;
	page -> body_len = 0;
}

void ogg_page_clear(ogg_page * page){
	if(page->header && page->body){
		_ogg_free(page->header);
	}
	ogg_page_init(page);
}

void ogg_packet_init(ogg_packet * packet){
	zeromem(packet, sizeof(ogg_packet));
}

int ogg_buffer(FileAccess *fa, ogg_sync_state *oy){
  uint8_t * buffer  = (uint8_t *) ogg_sync_buffer (oy, 4096);
  int bytes = fa->get_buffer(buffer, 4096);
  ogg_sync_wrote (oy,bytes);
  return bytes;
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

unsigned char * write_le64(unsigned char* p, const ogg_int64_t num){
  ogg_int64_t n = num;
  ogg_int64_t i;
  for (i=0; i<8; i++) {
	 p[i] = (unsigned char)(n & 0xff);
	 n >>= 8;
  }
  return p + 8;
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