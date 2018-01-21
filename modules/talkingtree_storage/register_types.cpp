#include "register_types.h"
#include "class_db.h"
#include "talking_tree_storage.h"
#include "talkingtree_storage_loader.h"


static ResourceFormatTalkingTreeStorage *talkingtree_storage_loader = NULL;
static TalkingTreeStorage *talking_tree_storage_writer = NULL;
void register_talkingtree_storage_types(){


	talkingtree_storage_loader = memnew(ResourceFormatTalkingTreeStorage);
    ResourceLoader::add_resource_format_loader(talkingtree_storage_loader);

	talking_tree_storage_writer = memnew(TalkingTreeStorage);
	talking_tree_storage_writer->init();
	talking_tree_storage_writer->set_singleton();
	ClassDB::register_class<TreecursionWriter>();
}
void unregister_talkingtree_storage_types(){
	talking_tree_storage_writer->finish();
}