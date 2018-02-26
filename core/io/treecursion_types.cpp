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
	VariantParser::StreamFile sf;
	sf.f = f;
	while(!sf.is_eof()){
		VariantParser parser;
		Variant ret;
		String err_string;
		int err_line;
		Error err_ret = parser.parse(&sf, ret, err_string, err_line);
		// nullptr would crash if nullptr are are pushed
		Ref<TreecursionWriteTask> cmd = variant2write_task(ret);
		if(cmd.is_valid()){
			commands.push_back(cmd);
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
Ref<TreecursionWriteTask> TreecursionTestData::variant2write_task(const Variant & cmd){
	Dictionary dict  = Dictionary(cmd);
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