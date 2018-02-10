#include "register_types.h"
#include "treecursion_test.h"
#include "engine.h"

_TreecursionTestStorage *treecursion_writer = NULL;
void register_treecursion_test_types(){
	treecursion_writer = memnew(_TreecursionTestStorage);
	ClassDB::register_class<_TreecursionTestStorage>();
	Engine::get_singleton()->add_singleton(Engine::Singleton("TreecursionTestStorage", _TreecursionTestStorage::get_singleton()));
}
void unregister_treecursion_test_types(){
	memdelete(treecursion_writer);
	treecursion_writer = NULL;
}
