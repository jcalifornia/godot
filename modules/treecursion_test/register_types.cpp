#include "register_types.h"
#include "treecursion_test.h"
#include "treecursion_loader.h"
#include "treecursion_reader.h"
#include "engine.h"

static _TreecursionTestStorage *treecursion_writer = nullptr;
static TreecursionTestStorage *treecursion_thread = nullptr;

static ResourceFormatTreecursionTestStorage *treecursion_test_loader = nullptr;
static ResourceFormatTreecursionTestInitStorage *treecursion_test_init_loader = nullptr;


void register_treecursion_test_types(){

	treecursion_test_init_loader = memnew(ResourceFormatTreecursionTestInitStorage);
    ResourceLoader::add_resource_format_loader(treecursion_test_init_loader);

	ClassDB::register_class<TreecursionTestInit>();
	ClassDB::register_class<TreecursionInitWriter>();
	treecursion_test_loader = memnew(ResourceFormatTreecursionTestStorage);
    ResourceLoader::add_resource_format_loader(treecursion_test_loader);

	treecursion_thread = memnew(TreecursionTestStorage);;
	treecursion_thread->set_singleton();
	
	treecursion_writer = memnew(_TreecursionTestStorage);
	ClassDB::register_class<_TreecursionTestStorage>();
	Engine::get_singleton()->add_singleton(Engine::Singleton("TreecursionTestStorage", _TreecursionTestStorage::get_singleton()));
}
void unregister_treecursion_test_types(){
	memdelete(treecursion_thread);
	treecursion_thread = nullptr;

	memdelete(treecursion_writer);
	treecursion_writer = nullptr;

	memdelete(treecursion_test_loader);
	treecursion_test_loader = nullptr;

}
