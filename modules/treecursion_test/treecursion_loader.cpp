#include "treecursion_loader.h"
#include "treecursion_reader.h"


ResourceFormatTreecursionTestStorage::ResourceFormatTreecursionTestStorage() {
}
RES ResourceFormatTreecursionTestStorage::load(const String &p_path, const String &p_original_path, Error *r_error) {
	if (r_error)
		*r_error = OK;

	TreecursionTestReader *treecursion = memnew(TreecursionTestReader);
	treecursion->set_file(p_path);
	return Ref<TreecursionTestReader>(treecursion);
}

void ResourceFormatTreecursionTestStorage::get_recognized_extensions(List<String> *p_extensions) const {

	p_extensions->push_back("tjson");
}
String ResourceFormatTreecursionTestStorage::get_resource_type(const String &p_path) const {

	if (p_path.get_extension().to_lower() == "tjson")
		return "TreecursionTest";
	return "";
}

bool ResourceFormatTreecursionTestStorage::handles_type(const String &p_type) const {
	return (p_type == "TreecursionTest");
}