#include "register_types.h"
#include "class_db.h"
#include "talking_tree_storage.h"
#include "talkingtree_storage_loader.h"

static ResourceFormatTalkingTreeStorage *talkingtree_storage_loader = NULL;

void register_talkingtree_storage_types(){
	talkingtree_storage_loader = memnew(ResourceFormatTalkingTreeStorage);
    ResourceLoader::add_resource_format_loader(talkingtree_storage_loader);
}
void unregister_talkingtree_storage_types(){

}