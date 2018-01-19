#include "talkingtree_storage_loader.h"
#include "treecursion_reader.h"


ResourceFormatTalkingTreeStorage::ResourceFormatTalkingTreeStorage() {
}
RES ResourceFormatTalkingTreeStorage::load(const String &p_path, const String &p_original_path, Error *r_error) {
	if (r_error)
		*r_error = OK;

	TreecursionReader *treecursion = memnew(TreecursionReader);
	treecursion->set_file(p_path);
	return Ref<TreecursionReader>(treecursion);
}

void ResourceFormatTalkingTreeStorage::get_recognized_extensions(List<String> *p_extensions) const {

	p_extensions->push_back("htogg");
}
String ResourceFormatTalkingTreeStorage::get_resource_type(const String &p_path) const {

	if (p_path.get_extension().to_lower() == "htogg")
		return "TreecursionReader";
	return "";
}

bool ResourceFormatTalkingTreeStorage::handles_type(const String &p_type) const {
	return (p_type == "TreecursionReader");
}