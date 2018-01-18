#include "talkingtree_storage_writer.h"

#include "os/os.h"
#include "ogg_routines.h"

void TalkingTreeStorageWriter::_bind_methods() {

}

TalkingTreeStorageWriter::TalkingTreeStorageWriter(){
		serialno=0;
	ogg_stream_init(&os, serialno);
	//create a file using unix time as file name by default
	int current_time = OS::get_singleton()->get_unix_time();


	_outfile_name = itos(current_time) + ".htogg";
	_fout = FileAccess::open(_outfile_name, FileAccess::WRITE);
}

TalkingTreeStorageWriter::~TalkingTreeStorageWriter(){
	//
	ogg_stream_flush( &os, &og);
	htogg_write_page(&og, _fout);
	_fout->close();
	//
}