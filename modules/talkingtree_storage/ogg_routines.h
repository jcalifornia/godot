#include "ogg/ogg.h"
#include "os/file_access.h"
#include "ustring.h"


int htogg_write_page(ogg_page *page, FileAccess *fa){
	fa->store_buffer(page->header, page->header_len);
	fa->store_buffer(page->body, page->body_len);
	return page->header_len + page->body_len;
}

