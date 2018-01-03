/* talking_tree_storage.h */
#ifndef TALKING_TREE_STORAGE_H
#define TALKING_TREE_STORAGE_H

#include "reference.h"
#include "os/file_access.h"

#include "ogg/ogg.h"
#include <kate/oggkate.h>


#include <opus.h>
#include <opus_multistream.h>

class TalkingTreeStorage : public Reference {
    GDCLASS(TalkingTreeStorage, Reference);


protected:
    static void _bind_methods();

public:
    TalkingTreeStorage();
    ~TalkingTreeStorage();
private:
    enum{
        //https://mf4.xiph.org/jenkins/view/opus/job/opus/ws/doc/html/group__opus__multistream.html
        SAMPLE_RATE = 48000,

    };
    OpusMSEncoder *opusEncoder;
    FileAccess *_fa;
    ogg_stream_state os;
    ogg_packet op;

};

#endif