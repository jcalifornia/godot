#include "treecursion_writer.h"

#include "os/os.h"
#include "ogg_routines.h"

#include <skeleton/skeleton.h>
#include <skeleton/skeleton_query.h>

void TreecursionWriter::_bind_methods() {
	ClassDB::bind_method(D_METHOD("write_header"), &TreecursionWriter::write_header);
	ClassDB::bind_method(D_METHOD("close"), &TreecursionWriter::close);
}
//TODO: mime type should be in the msg header field
// https://xiph.org/ogg/doc/rfc3533.txt
void TreecursionWriter::write_header(){
	OggSkeleton *header = oggskel_new();
	ogg_packet fishbone_op;
	ogg_packet fishhead_op;
	//the only way to add an bone
	oggskel_encode_add_stream(header,0);

	int64_t pna = OS::get_singleton()->get_unix_time();
	oggskel_set_ptime_num(header, pna);
	oggskel_set_btime_num(header, 0);
	oggskel_set_start_granule(header, 0, 0);
	oggskel_set_preroll(header, 0, 0);
	oggskel_set_msg_header(header, 0, "i hope that i can read this");

	//encode to write
	//I need to call it twice because encode header have three states
	oggskel_encode_header(header, &fishhead_op);
	oggskel_encode_header(header, &fishbone_op);

	ogg_stream_packetin(&os, &fishhead_op);
	ogg_stream_packetin(&os, &fishbone_op);
	ogg_stream_flush(&os, &og);

	char **msg;
	int64_t ptime;
	OggSkeletonError err =  oggskel_get_ptime_num(header, &ptime);
	print_line("get error oggskelton: "+ itos((int)err));
	oggskel_get_msg_header(header, 0, msg);
	print_line("msg" + String(*msg));

	print_line("current2 time " + itos(pna));
	print_line("current time " + itos(ptime));

	print_line("header size" + itos(og.header_len));
	print_line("body size" + itos(og.body_len));

	htogg_write_page(&og, _fout);
	
	//cleanup
	ogg_packet_clear(&fishhead_op);
	ogg_packet_clear(&fishbone_op);
	oggskel_destroy(header);
}
void TreecursionWriter::close(){
	ogg_stream_flush( &os, &og);
	htogg_write_page(&og, _fout);
	_fout->close();
}
TreecursionWriter::TreecursionWriter(){
	serialno=0;
	ogg_stream_init(&os, serialno);
	//create a file using unix time as file name by default
	int current_time = OS::get_singleton()->get_unix_time();
	_outfile_name = itos(current_time) + ".htogg";
	_fout = FileAccess::open(_outfile_name, FileAccess::WRITE);
}

TreecursionWriter::~TreecursionWriter(){
	
}