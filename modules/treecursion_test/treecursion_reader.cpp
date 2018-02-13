#include "treecursion_reader.h"
#include "variant.h"
#include "variant_parser.h"
#include "io/json.h"
#include "dictionary.h"

#include "ustring.h"
#include "print_string.h"


void TreecursionTestReader::_bind_methods(){
}
Error TreecursionTestReader::set_file(const String &p_file) {
    file = p_file;
	Error err;
	_file = FileAccess::open(file, FileAccess::READ, &err);
	if (err) {
		ERR_FAIL_COND_V(err, err);
	}
	return OK;
}

Ref<TreecursionWriteTask> TreecursionTestReader::next(){
	if(this->_file -> eof_reached()){
		return Ref<TreecursionWriteTask>(nullptr);
	}
	String line = this->_file->get_line();
	String err_string;
	int err_line;
	JSON cmd;
	Variant ret;
	Error err = cmd.parse( line, ret, err_string, err_line);
	Dictionary dict  = Dictionary(ret);
	switch( int64_t(dict["type"])){
		case TreecursionWriteTask::CALL_TASK:{
			return Ref<TreecursionCallTask>(memnew(TreecursionCallTask(dict)));
			break;
		}
		case TreecursionWriteTask::SET_TASK:{
			return Ref<TreecursionSetTask>(memnew(TreecursionSetTask(dict)));
			break;
		}
		case TreecursionWriteTask::VOICE_TASK:{
			//return Ref<TreecursionVoiceTask>(memnew(TreecursionVoiceTask(dict)));
			return Ref<TreecursionWriteTask>(nullptr);
			break;
		}
		default : {
			break;
		}
	}
	return Ref<TreecursionWriteTask>(nullptr);

}


TreecursionTestReader::TreecursionTestReader() {
    
}
