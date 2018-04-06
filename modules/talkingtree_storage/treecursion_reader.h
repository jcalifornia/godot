#ifndef TREECURSION_H
#define TREECURSION_H
#include "resource.h"
#include "reference.h"

#include "io/treecursion_types.h"
#include "os/file_access.h"
#include "list.h"
#include "hash_map.h"
#include "variant.h"
#include "audio_codec.h"
#include <ogg/ogg.h>


class HtoggStream : public Resource {
    GDCLASS(HtoggStream, Resource);
public:
    ogg_stream_state os;
    ogg_int64_t serial_no;
    HtoggStream( int num ) : serial_no(num) {
        ogg_stream_init(&os, num);
    }
    ~HtoggStream() {
        ogg_stream_clear(&os);
    }
};

class TreecursionReader : public Resource {
    GDCLASS(TreecursionReader, Resource);
    
public:
    Error set_file(const String &p_file);
    Ref<TreecursionWriteTask> next();
    Ref<TreecursionWriteTask> pop();
    bool has_next();
    uint64_t peek_time();
    Variant get_init_values() const;
    TreecursionReader();
    ~TreecursionReader();
    
protected:
    static void _bind_methods();

private:
    List<TreecursionWriteTask *> headers;
    List<TreecursionWriteTask *> treecursion_buffer;
    HashMap<int, HtoggStream *> open_streams;
    FileAccess *_file;
    ogg_sync_state state;

    Ref<AudioCodec> audio_codec_handler;
    ogg_int64_t current_unit;
    ogg_int64_t current_granulepos;
    /* Read positioning */
    long current_page_bytes;
    /* Calculation of position */
    //oggz_off_t current_packet_begin_page_offset;
    int current_packet_pages;
    int current_packet_begin_segment_index;
    TreecursionWriteTask* variant2write_task(int64_t time, const Variant & cmd);
    void parse_packet(int type, ogg_packet *op);
    void parse_headers(int type, ogg_packet *op);
    int parse_page(ogg_page *);
    int next_page();
    void close();
};

class Treecursion : public Resource {
	GDCLASS(Treecursion, Resource)
	String file;
public:
	Ref<TreecursionReader> instance_playback() const {
		Ref<TreecursionReader> pb(memnew(TreecursionReader));
		pb->set_file(file);
		return pb;
	}
	void set_file(const String &p_file) { file = p_file; }
}; 
#endif