#include "treecursion_writer.h"

#include "os/os.h"
#include "ogg_routines.h"

#include <skeleton/skeleton.h>
#include <skeleton/skeleton_query.h>

void TreecursionWriter::_bind_methods() {

}

void TreecursionWriter::write_header(){
	OggSkeleton *header = oggskel_new();
	oggskel_set_ptime_num(header, OS::get_singleton()->get_unix_time());
	oggskel_set_btime_num(header, 0);
	oggskel_set_start_granule(header, 0, 0);
	oggskel_set_preroll(header, 0, 0);
	oggskel_set_msg_header(header, 0, "i hope that i can read this");
	oggskel_encode_header(header, &op);
	ogg_stream_packetin(&os, &op);
	ogg_stream_flush(&os, &og);

	
	htogg_write_page(&og, _fout);
	//cleanup
	ogg_packet_clear(&op);

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
	//
	ogg_stream_flush( &os, &og);
	htogg_write_page(&og, _fout);
	_fout->close();
	//
}