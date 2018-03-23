#include "ogg/ogg.h"
#include "os/file_access.h"

#ifndef HTOGG_ROUTES_H
#define HTOGG_ROUTES_H
int htogg_write_page(ogg_page *page, FileAccess *fa);

int ogg_buffer(FileAccess *fa, ogg_sync_state *oy);

void ogg_page_init(ogg_page *page);

void ogg_page_clear(ogg_page * page);

void ogg_packet_init(ogg_packet *packet);

bool is_fishhead_packet(ogg_packet *packet);

bool is_fishbone_packet(ogg_packet *packet);

unsigned char * write_le32(unsigned char* p, const ogg_uint32_t num);

unsigned char * write_le16(unsigned char* p, const ogg_uint32_t num);

unsigned char * write_le64(unsigned char* p, const ogg_int64_t num);

#endif HTOGG_ROUTES_H

