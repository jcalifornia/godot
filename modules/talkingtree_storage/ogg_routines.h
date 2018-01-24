#include "ogg/ogg.h"
#include "os/file_access.h"


int htogg_write_page(ogg_page *page, FileAccess *fa);

int ogg_buffer(FileAccess *fa, ogg_sync_state *oy);

bool is_fishhead_packet(ogg_packet *packet);

bool is_fishbone_packet(ogg_packet *packet);