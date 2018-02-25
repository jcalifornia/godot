#include "treecursion_types.h"

#include "treecursion_types.h"
#include "variant.h"
#include "variant_parser.h"
#include "io/json.h"
#include "dictionary.h"

#include "ustring.h"
#include "print_string.h"


void TreecursionTestData::_bind_methods(){
}
Error TreecursionTestData::set_file(const String &p_file) {
    file = p_file;
	Error err;
	FileAccess *f = FileAccess::open(file, FileAccess::READ, &err);
	if (err) {
		ERR_FAIL_COND_V(err, err);
	}
	
	while(!f->eof_reached()){
		// nullptr would crash if nullptr are are pushed
		Ref<TreecursionWriteTask> cmd = next_command(f);
		if(cmd.is_valid()){
			commands.push_back(cmd);
			print_line(cmd->toJson());
		}
	}
	f-> close();
	return OK;
}

/*
 *  set_file and open and close file
 *  doesnt really make much sense really.
 *  Needs cleanup
 */
Ref<TreecursionWriteTask> TreecursionTestData::next_command(FileAccess *f){
	String line =  f->get_line();
	String err_string;
	int err_line;
	JSON cmd;
	Variant ret;
	Error err = cmd.parse( line, ret, err_string, err_line);
	Dictionary dict  = Dictionary(ret);
	switch( int64_t(dict["type"])){
		case TreecursionWriteTask::SET_TASK:{
			return Ref<TreecursionSetTask>(memnew(TreecursionSetTask(dict)));
			break;
		}
		case TreecursionWriteTask::CALL_TASK:{
			return Ref<TreecursionCallTask>(memnew(TreecursionCallTask(dict)));
			break;
		}
		case TreecursionWriteTask::VOICE_TASK:{
			//return Ref<TreecursionVoiceTask>(memnew(TreecursionVoiceTask(dict)));
			//return Ref<TreecursionWriteTask>(nullptr);
			break;
		}
		default : {
			break;
		}
	}
	return Ref<TreecursionWriteTask>(nullptr);

}
Ref<TreecursionWriteTask> TreecursionTestData::peek(){
	Ref<TreecursionWriteTask> ret = commands[counter];
	return ret;
}
Ref<TreecursionWriteTask> TreecursionTestData::next(){
	return Ref<TreecursionWriteTask>(commands[counter++]);
}
bool TreecursionTestData::has_next(){
	return counter < commands.size();
}
TreecursionTestData::~TreecursionTestData(){
}
TreecursionTestData::TreecursionTestData(): counter(0) {
    
}