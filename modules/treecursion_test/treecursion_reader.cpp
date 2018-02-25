#include "treecursion_reader.h"
#include "variant.h"
#include "variant_parser.h"
#include "io/json.h"
#include "dictionary.h"

#include "ustring.h"
#include "print_string.h"


/*
 * just a test module.
 */
Variant TreecursionTestInit::get_dict(){
	return Variant(dict);
}

TreecursionTestInit::TreecursionTestInit() : dict(){

}
void TreecursionTestInit::_bind_methods(){
	ClassDB::bind_method(D_METHOD("get_dictionary"), &TreecursionTestInit::get_dict);
}

TreecursionTestInit::TreecursionTestInit(const Dictionary &d) : dict(d){

}
TreecursionTestInit::~TreecursionTestInit(){

}


