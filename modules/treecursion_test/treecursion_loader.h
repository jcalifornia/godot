#ifndef TREECURSION_TEST_LOADER_H
#define TREECURSION_TEST_LOADER_H

#include "io/resource_loader.h"


class ResourceFormatTreecursionTestStorage : public ResourceFormatLoader {
public:
	virtual RES load(const String &p_path, const String &p_original_path, Error *r_error = NULL);
	virtual void get_recognized_extensions(List<String> *p_extensions) const;
	virtual bool handles_type(const String &p_type) const;
	virtual String get_resource_type(const String &p_path) const;

	ResourceFormatTreecursionTestStorage();
	virtual ~ResourceFormatTreecursionTestStorage() {}
};


#endif // TALKINGTREE_STORAGE_LOADER_H