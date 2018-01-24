#include "treecursion_reader.h"
#include "print_string.h"
#include "ogg_routines.h"
#include <skeleton/skeleton.h>
//https://bluishcoder.co.nz/2009/06/24/reading-ogg-files-using-libogg.html

void TreecursionReader::_bind_methods(){
	ClassDB::bind_method(D_METHOD("print_fishhead"), &TreecursionReader::print_fishhead);
}
Error TreecursionReader::set_file(const String &p_file) {
    file = p_file;
	Error err;
	_file = FileAccess::open(file, FileAccess::READ, &err);
	if (err) {
		ERR_FAIL_COND_V(err, err);
	}
	int ret = ogg_sync_init(&state);
	
	return OK;
}
void TreecursionReader::print_fishhead(){
	ogg_page og;
	ogg_packet op;
	ogg_stream_state os;
	OggSkeleton *skeleton = oggskel_new();
	int sk_headers = -1;

	int ret = ogg_buffer(_file, &state);
	if (ret == 0) {
		print_line("error cannot buffer ogg file");
		return;
	}
	int got_packet;
	if(ogg_sync_pageout (&state, &og) > 0){
		ogg_stream_init (&os, ogg_page_serialno (&og));
		ogg_stream_pagein (&os, &og);
		got_packet = ogg_stream_packetpeek (&os, &op);
		if((sk_headers = oggskel_decode_header (skeleton, &op)) > 0){
			char **msg_header;
			oggskel_get_msg_header(skeleton, 0, msg_header);
			int64_t ptime;
			oggskel_get_ptime_num(skeleton, &ptime);
			String message(*msg_header);
			print_line("ptime: " + itos(ptime));
			print_line("attempting to print message");
			print_line(message);
		}else{
			print_line("not skeleton header");
		}
	}else{
		print_line("error reading page");
	}

}

TreecursionReader::TreecursionReader() {
    
}
