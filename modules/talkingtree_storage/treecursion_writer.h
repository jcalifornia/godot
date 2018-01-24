#include "object.h"
#include "os/file_access.h"
#include "ustring.h"


#include "ogg/ogg.h"





class TreecursionWriter : public Object {
    GDCLASS(TreecursionWriter, Object);

private:
	FileAccess *_fout;
    String _outfile_name;
    ogg_stream_state os;
    ogg_page og; /* one Ogg bitstream page. */
    int serialno;

protected:
    static void _bind_methods();

public:
    void write_header();
    void close();
    TreecursionWriter();
    ~TreecursionWriter();
	
};