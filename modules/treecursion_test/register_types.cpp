#include "register_types.h"
#include "treecursion_test.h"
#include "engine.h"

_TreecursionTestStorage *treecursion_writer = nullptr;
TreecursionTestStorage *treecursion_thread = nullptr;
void register_treecursion_test_types(){
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

}
