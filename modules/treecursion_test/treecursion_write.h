#include "object.h"
#include "os/file_access.h"
#include "ustring.h"

#include "io/treecursion_types.h"


class TreecursionTestWriter : public Object {
	GDCLASS(TreecursionTestWriter, Object);

private:
	FileAccess *_fout;
	String _outfile_name;
protected:
	static void _bind_methods();

public:
	void write_packet(TreecursionWriteTask & packet);
	void close();
	TreecursionTestWriter();
	~TreecursionTestWriter();
	
};
