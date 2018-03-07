#include "treecursion_loader.h"
#include "treecursion_reader.h"
#include "io/treecursion_types.h"
#include "os/file_access.h"

ResourceFormatTreecursionTestStorage::ResourceFormatTreecursionTestStorage() {
}
RES ResourceFormatTreecursionTestStorage::load(const String &p_path, const String &p_original_path, Error *r_error) {
	if (r_error)
		*r_error = OK;

	TreecursionTestData *treecursion = memnew(TreecursionTestData);
	treecursion->set_file(p_path);
	return Ref<TreecursionTestData>(treecursion);
}

void ResourceFormatTreecursionTestStorage::get_recognized_extensions(List<String> *p_extensions) const {

	p_extensions->push_back("tcommand");
}
String ResourceFormatTreecursionTestStorage::get_resource_type(const String &p_path) const {

	if (p_path.get_extension().to_lower() == "tcommand")
		return "TreecursionTest";
	return "";
}

bool ResourceFormatTreecursionTestStorage::handles_type(const String &p_type) const {
	return (p_type == "TreecursionTest");
}

ResourceFormatTreecursionTestInitStorage::ResourceFormatTreecursionTestInitStorage() {
}
RES ResourceFormatTreecursionTestInitStorage::load(const String &p_path, const String &p_original_path, Error *r_error) {
	if (r_error)
		*r_error = OK;
	
	Error error_file;
	FileAccess *file = FileAccess::open(p_path, FileAccess::READ, &error_file);

	String line = file->get_line();
	String err_string;
	int err_line;
	JSON cmd;
	Variant ret;
	Error err = cmd.parse( line, ret, err_string, err_line);
	file -> close();
	return Ref<TreecursionTestInit>(memnew(TreecursionTestInit(ret)));
}

void ResourceFormatTreecursionTestInitStorage::get_recognized_extensions(List<String> *p_extensions) const {

	p_extensions->push_back("tmeta");
}
String ResourceFormatTreecursionTestInitStorage::get_resource_type(const String &p_path) const {

	if (p_path.get_extension().to_lower() == "tmeta")
		return "TreecursionTestMeta";
	return "";
}

bool ResourceFormatTreecursionTestInitStorage::handles_type(const String &p_type) const {
	return (p_type == "TreecursionTestMeta");
}