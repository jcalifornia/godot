#include "object.h"
#include "os/file_access.h"
#include "ustring.h"


#include "ogg/ogg.h"


class TalkingTreeStorageWriter : public Object {
    GDCLASS(TalkingTreeStorageWriter, Object);

private:
	FileAccess *_fout;
    String _outfile_name;
    ogg_stream_state os;
    ogg_packet op;
    ogg_page og; /* one Ogg bitstream page. */
    int serialno;

protected:
    static void _bind_methods();

public:
    TalkingTreeStorageWriter();
    ~TalkingTreeStorageWriter();
	
};