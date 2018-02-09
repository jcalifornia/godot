#include "treecursion_write.h"

#include "os/os.h"


void TreecursionTestWriter::_bind_methods() {
	ClassDB::bind_method(D_METHOD("close"), &TreecursionTestWriter::close);
}
void TreecursionTestWriter::close(){
	_fout->close();
}
void TreecursionTestWriter::write_packet(TreecusionWriteTask & packet){
	_fout->store_line(packet.toJson());
}
TreecursionTestWriter::TreecursionTestWriter(){
	int current_time = OS::get_singleton()->get_unix_time();
	_outfile_name = itos(current_time) + ".tjson";
	_fout = FileAccess::open(_outfile_name, FileAccess::WRITE);
}

TreecursionTestWriter::~TreecursionTestWriter(){
	
}
