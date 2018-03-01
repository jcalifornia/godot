#include "treecursion_write.h"

#include "os/os.h"


void TreecursionTestWriter::_bind_methods() {
	ClassDB::bind_method(D_METHOD("close"), &TreecursionTestWriter::close);
}
void TreecursionTestWriter::close(){
	_fout->close();
}
void TreecursionTestWriter::write_packet(TreecursionWriteTask & packet){
	//remove duplicate rset commands
	switch(packet.get_type()){
		case TreecursionWriteTask::SET_TASK:{
			TreecursionSetTask *st = (TreecursionSetTask *) &packet;
			if(last_value.has(st->get_node_path())){
				Variant value = last_value.get(st->get_node_path());
				if(value == st->get_value()){
					break;
				}
			}
			last_value[st->get_node_path()] = st -> get_value();
			_fout->store_line(packet.toString());
			break;
		}
		default: {
			_fout->store_line(packet.toString());
			break;
		}
	}
	
}
TreecursionTestWriter::TreecursionTestWriter(){
	int current_time = OS::get_singleton()->get_unix_time();
	_outfile_name = itos(current_time) + ".tcommand";
	_fout = FileAccess::open(_outfile_name, FileAccess::WRITE);
}
TreecursionTestWriter::TreecursionTestWriter(String fname){
	_outfile_name = fname + ".tcommand";
	_fout = FileAccess::open(_outfile_name, FileAccess::WRITE);
}

TreecursionTestWriter::~TreecursionTestWriter(){
	close();
	
}


void TreecursionInitWriter::_bind_methods(){
	ClassDB::bind_method(D_METHOD("write_header", "header"), &TreecursionInitWriter::write_packet);
}


void TreecursionInitWriter::write_packet(const Variant & var){
	Dictionary *packet = memnew(Dictionary(var));
	(*packet)["p_time"] = OS::get_singleton()->get_ticks_usec();
	Variant *t = packet->getptr("os_time");
	FileAccess *file = FileAccess::open( itos(uint64_t(*t))+".tmeta", FileAccess::WRITE);
	JSON a;
	String ret = a.print(*packet);
	file->store_line(ret);
	file->close();
	memdelete(packet);
}
TreecursionInitWriter::TreecursionInitWriter(){

}
TreecursionInitWriter::~TreecursionInitWriter(){

}
	
