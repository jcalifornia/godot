#include "ogg/ogg.h"
#include "os/file_access.h"
#include "ustring.h"


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
