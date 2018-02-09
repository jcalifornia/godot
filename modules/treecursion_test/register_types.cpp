#include "register_types.h"
#include "treecursion_test.h"

TreecursionTestStorage *treecursion_writer = NULL;
void register_treecursion_test_types(){
	treecursion_writer = memnew(TreecursionTestStorage);
	treecursion_writer->init();
	treecursion_writer->set_singleton();
}
void unregister_treecursion_test_types(){
	memdelete(treecursion_writer);
	treecursion_writer = NULL;
}
