#include "treecursion_writer.h"
#include "os/os.h"
#include "ogg_routines.h"
#include "os/copymem.h"

void TreecursionWriter::_bind_methods() {
	ClassDB::bind_method(D_METHOD("close"), &TreecursionWriter::close);
}

//TODO: Check if its little endian
//TODO: mime type should be in the msg header field
// https://xiph.org/ogg/doc/rfc3533.txt
void TreecursionWriter::write_task( const TreecursionWriteTask & task ){
	ogg_packet op;
	PoolByteArray packet_data;

	ogg_packet_init(&op);
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
			last_value.set(st->get_node_path(), st -> get_value());
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
	op.granulepos = (ogg_int64_t) task.get_time();
	op.packetno = _sequence_number;
	op.bytes = packet_data.size();
	_sequence_number += 100;

	ogg_stream_packetin(&ogg_os[HTOGG_ENGINE], &op);
	//ogg_packet_clear(&op);

	ogg_page og;
	ogg_page_init(&og);
	//print_line(" state granuelopos: " + itos(op.granulepos) + " : " + itos(ogg_os[HTOGG_ENGINE].granulepos));
	//state and current page granuelopos is the same
	if(ogg_stream_pageout(&ogg_os[HTOGG_ENGINE], &og) > 0){
		htogg_write_page(&og, _fout);
		//seems to crash
		//ogg_page_clear(&og);
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
		ogg_packet_init(&op);
		op.granulepos = (ogg_int64_t) OS::get_singleton()->get_ticks_usec();
		op.e_o_s = 1;
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