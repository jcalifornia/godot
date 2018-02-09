#include "object.h"
#include "os/file_access.h"
#include "ustring.h"

class TreecursionTestWriter : public Object {
	GDCLASS(TreecursionTestWriter, Object);

private:
	FileAccess *_fout;
	String _outfile_name;
protected:
	static void _bind_methods();

public:
	void close();
	TreecursionTestWriter();
	~TreecursionTestWriter();
	
};
