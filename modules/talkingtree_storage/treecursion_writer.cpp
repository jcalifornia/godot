#include "treecursion_writer.h"
#include "os/os.h"
#include "ogg_routines.h"
#include "os/copymem.h"
#include <skeleton/skeleton.h>
#include <skeleton/skeleton_query.h>

void TreecursionWriter::_bind_methods() {
	ClassDB::bind_method(D_METHOD("close"), &TreecursionWriter::close);
}

//TODO: mime type should be in the msg header field
// https://xiph.org/ogg/doc/rfc3533.txt
/*
void TreecursionWriter::write_header(){
	OggSkeleton *header = oggskel_new();
	ogg_packet fishbone_op;
	ogg_packet fishhead_op;
	//the only way to add an bone
	
	
	
	int64_t pna = OS::get_singleton()->get_unix_time();
	oggskel_set_ptime_num(header, pna);
	oggskel_set_btime_num(header, 0);
	

	oggskel_encode_add_stream(header,1);
	oggskel_set_start_granule(header, 1, 0);
	oggskel_set_preroll(header, 1, 0);
	oggskel_set_num_headers(header, 1, 1);
	oggskel_set_msg_header(header, 1, "i hope that i can read this");

	//encode to write
	//I need to call it twice because encode header have three states
	oggskel_encode_header(header, &fishhead_op);
	oggskel_encode_header(header, &fishbone_op);
	ogg_stream_packetin(&engine_os, &fishhead_op);
	ogg_stream_packetin(&engine_os, &fishbone_op);

	ogg_stream_flush(&engine_os, &og);

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
}*/
//TODO: Check if its little endian
void TreecursionWriter::write_task( const TreecursionWriteTask & task ){
	ogg_packet op;
	PoolByteArray packet_data;

	op.b_o_s = 0;

	//create packet data
	switch(task.get_type()){
		case TreecursionWriteTask::ENGINE_HEADER_TASK: {
			CharString s = task.toString().utf8();
			packet_data.resize(s.size());
			PoolByteArray::Write wptr = packet_data.write();
			copymem(wptr.ptr(), s.ptr(), s.size());
			op.b_o_s = 1;
			break;
		}
		case TreecursionWriteTask::CALL_TASK: {
			CharString s = task.toString().utf8();
			packet_data.resize(s.size());
			PoolByteArray::Write wptr = packet_data.write();
			copymem(wptr.ptr(), s.ptr(), s.size());
			break;
		}
		case TreecursionWriteTask::SET_TASK: {
			TreecursionSetTask *st = (TreecursionSetTask *) &task;

			//test for duplicated set values
			if(last_value.has(st->get_node_path())){
				Variant value = last_value.get(st->get_node_path());
				//skip when set values are duplicated
				if(value == st->get_value()){
					return; 
				}
			}
			last_value[st->get_node_path()] = st -> get_value();
			CharString s = task.toString().utf8();
			op.bytes = s.size();
			packet_data.resize(s.size());
			PoolByteArray::Write wptr = packet_data.write();
			copymem(wptr.ptr(), s.ptr(), s.size());
			break;
		}
		default:
			ERR_PRINTS(String("Treecursion Scenetree - incorrect treecursionwritetask type: ") + itos(task.get_type()) );
			return;
	}
	unsigned char *rptr = (unsigned char *) packet_data.read().ptr();
	op.packet = rptr;
	op.granulepos = task.get_time();
	op.packetno = _sequence_number;
	op.bytes = packet_data.size();
	_sequence_number += 100;

	ogg_stream_packetin(&ogg_os[HTOGG_ENGINE], &op);

	ogg_page og;
	if(ogg_stream_pageout(&ogg_os[HTOGG_ENGINE], &og) > 0){
		htogg_write_page(&og, _fout);
	}
}

void TreecursionWriter::close(){
	if(_fout){
		//write empty packets for the last page
		/*
		for(int64_t i = 0; i < HTOGG_END; i++){
			ogg_packet op;
			op.e_o_s = 1;
			ogg_stream_packetin(&ogg_os[i], &op);

			ogg_page og;
			ogg_stream_flush( &ogg_os[i], &og);
			htogg_write_page(&og, _fout);
		}*/
		//only handle engine for now
		ogg_packet op;
		op.granulepos = OS::get_singleton()->get_ticks_usec();
		op.e_o_s = 1;
		op.bytes = 0;
		op.b_o_s = 0;
		ogg_stream_packetin(&ogg_os[HTOGG_ENGINE], &op);
		ogg_page og;
		ogg_stream_flush( &ogg_os[HTOGG_ENGINE], &og);
		htogg_write_page( &og, _fout);
		

		for(int64_t i = 0; i < HTOGG_END; i++){
			ogg_stream_clear(&ogg_os[i]);
		}

		_fout->close();
		_fout = nullptr;
	}
}

TreecursionWriter::TreecursionWriter(const String &fileName): _sequence_number(0) {
	
	for(int64_t i = 0; i < HTOGG_END; i++){
		zeromem(&ogg_os[i], sizeof(ogg_stream_state));
		ogg_stream_clear(&ogg_os[i]);
		ogg_stream_init(&ogg_os[i], i);
	}

	_outfile_name = fileName + ".htogg";
	_fout = FileAccess::open(_outfile_name, FileAccess::WRITE);
}

TreecursionWriter::~TreecursionWriter(){
	close();
}