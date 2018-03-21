#include "object.h"
#include "os/file_access.h"
#include "ustring.h"


#include "ogg/ogg.h"
#include "io/treecursion_types.h"






class TreecursionWriter : public Object {
    GDCLASS(TreecursionWriter, Object);

private:
	FileAccess *_fout;
    String _outfile_name;
    int64_t _sequence_number;
    int serialno;
    HashMap<String, Variant> last_value;
    ogg_stream_state ogg_os[HTOGG_END]; 

protected:
    static void _bind_methods();

public:
    void write_task(const TreecursionWriteTask & task);
    void close();
    TreecursionWriter(const String &fileName);
    ~TreecursionWriter();
	
};