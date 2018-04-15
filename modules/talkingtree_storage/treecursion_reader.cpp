#include "treecursion_reader.h"
#include "print_string.h"
#include "variant.h"
#include "variant_parser.h"
#include "ogg_routines.h"
#include "io/marshalls.h"
#include "io/stream_peer.h"
//https://bluishcoder.co.nz/2009/06/24/reading-ogg-files-using-libogg.html

void TreecursionReader::_bind_methods(){
}
Error TreecursionReader::set_file(const String &p_file) {
	Error err;
	_file = FileAccess::open(p_file, FileAccess::READ, &err);
	if (err) {
		ERR_FAIL_COND_V(err, err);
		return err;
	}
	//buffer the first kilobytes
	int ret = ogg_sync_init(&state);
	if(ogg_buffer(_file, &state) <= 0)
		return ERR_FILE_CANT_READ;

	//load header pages
	int page_type = next_page();
	while(page_type == 1) {
		page_type = next_page();
	}
	if( page_type < 0 )
		return FAILED;
	return OK;
}

TreecursionWriteTask* TreecursionReader::variant2write_task(int64_t time, const Variant & cmd){
	Dictionary dict  = Dictionary(cmd).duplicate();
	switch( int64_t(dict["type"])){
		case TreecursionWriteTask::SET_TASK:{
			//dict["time"] = time;
			return memnew(TreecursionSetTask(dict));
			break;
		}
		case TreecursionWriteTask::CALL_TASK:{
			//dict["time"] = time;
			return memnew(TreecursionCallTask(dict));
			break;
		}
		case TreecursionWriteTask::ENGINE_HEADER_TASK: {
			return memnew(TreecursionEngineHeaderTask(time, dict));
			break;
		}
		default : {
			break;
		}
	}
	return nullptr;
}
uint64_t TreecursionReader::peek_time() {
	List<TreecursionWriteTask*>::Element *e = treecursion_buffer.front();
	return e->get()->get_time();
}

int TreecursionReader::next_page() {
	ogg_page og;
	ogg_page_init(&og);
	//loop until a page pops out
	while(ogg_sync_pageout(&state, &og) < 1){
		if(_file->eof_reached())
			return -1;
		
		if ( ogg_buffer(_file, &state) <= 0 ){
			return -1;
		}
	}
	return parse_page(&og);
}
// check libogg for memory leaks
int TreecursionReader::parse_page(ogg_page *og) {
	int serialno = ogg_page_serialno(og);
	//print_line( "page graulepos : " + itos(ogg_page_granulepos(og)));
	if(ogg_page_bos(og) > 0){
		HtoggStream *htogg_stream = memnew(HtoggStream(serialno));
		open_streams.set(serialno, htogg_stream);
		ogg_stream_pagein( &(htogg_stream->os), og);
		
		ogg_packet op;
		ogg_packet_init(&op);
		while(ogg_stream_packetout(&(htogg_stream->os), &op) > 0 ){
			parse_headers(serialno, &op);
			ogg_packet_clear(&op);
		}
		return 1;
	} else {
		HtoggStream *stream = open_streams.get(serialno);
		ogg_stream_pagein( &(stream->os), og);

		ogg_packet op;
		ogg_packet_init(&op);
		while(ogg_stream_packetout(&(stream->os), &op) > 0 ){
			parse_packet(serialno, &op);
			//stupid line just crashes even when the packet is valid
			//ogg_packet_clear(&op);
			ogg_packet_init(&op);
		}
		if( ogg_page_eos(og) > 0)
			return 2;
		else
			return 0;
	}
	return -1;
}
void TreecursionReader::parse_headers(int type, ogg_packet *op) {
	VariantParser::StreamString ss;
	VariantParser parser;
	String err_string;
	Variant ret;
	int err_line;
	switch(type){
		case HTOGG_ENGINE: {
			ss.s.parse_utf8((char *) op->packet, op->bytes);
			Error err_ret = parser.parse(&ss, ret, err_string, err_line);
			// nullptr would crash if nullptr are are pushed
			TreecursionWriteTask *cmd = memnew(TreecursionEngineHeaderTask(op->granulepos, ret));
			headers.push_back(cmd);
			break;
		}
		case HTOGG_VOIP :{
			//io/marshalls is usually little endian
			StreamPeerBuffer buffer;
			buffer.put_data(op->packet, op->bytes);
			//Stream peer buffer automatically
			//increaments the pointer when ever you add data.
			//seek it to zero so i can get the correct numbers
			buffer.seek(0);
			int codec = buffer.get_32();
			int sampleRate = buffer.get_32();
			int bitRate = buffer.get_32();
			int frameSize = buffer.get_32();
			
			TreecursionWriteTask *vt = memnew(TreecursionVoiceHeaderTask( codec, sampleRate, bitRate, frameSize));
			headers.push_back(vt);
			audio_codec_handler = Ref<AudioCodec>(memnew(OpusCodec(sampleRate, bitRate, frameSize)));
			break;
		}
		default:
			break;
	}

}
void TreecursionReader::parse_packet(int type, ogg_packet *op) {
	if(op->bytes == 0)
		return;
	TreecursionWriteTask * task = NULL;
	VariantParser::StreamString ss;
	VariantParser parser;
	String err_string;
	Variant ret;
	int err_line;
	switch(type){
		case HTOGG_ENGINE: {
			ss.s.parse_utf8((char *) op->packet, op->bytes);
			Error err_ret = parser.parse(&ss, ret, err_string, err_line);
			// nullptr would crash if nullptr are are pushed
			TreecursionWriteTask *cmd = variant2write_task(op->granulepos, ret);
			treecursion_buffer.push_back(cmd);
			current_granulepos = op->granulepos;
			break;
		}
		case HTOGG_VOIP: {

			uint8_t *packet_ptr = op->packet;
			uint64_t timestamp = decode_uint64(packet_ptr);
			packet_ptr += sizeof(uint64_t);
			uint32_t user_id = decode_uint32(packet_ptr);
			packet_ptr += sizeof(uint32_t);
			int16_t output[1024];
			const int samples = audio_codec_handler->decode( 
				packet_ptr, op->bytes-(packet_ptr - op->packet), 
				output, 1024);

			TreecursionWriteTask *talk = memnew(
					TreecursionVoiceTask(timestamp, user_id, (uint8_t*)output, samples*sizeof(int16_t))
				);
			treecursion_buffer.push_back(talk);
			current_granulepos = op->granulepos;
			break;
		}
		default:
			break;
	}
}

Ref<TreecursionWriteTask> TreecursionReader::pop() {
	Ref<TreecursionWriteTask> ret(treecursion_buffer.front()->get());
	treecursion_buffer.pop_front();
	if(treecursion_buffer.empty()) {
		int err = next_page();
		if(err > 0) {
			WARN_PRINTS("Error parsing page");
		}
	}
	return ret;
}
Ref<TreecursionWriteTask> TreecursionReader::next() {
	return Ref<TreecursionWriteTask>(nullptr);
}
bool TreecursionReader::has_next() {
	return !treecursion_buffer.empty();
}
Variant TreecursionReader::get_init_values() const {
	if(headers.empty())
		return Variant();
	TreecursionWriteTask *engineHeader = headers[0];
	TreecursionEngineHeaderTask *e = (TreecursionEngineHeaderTask * ) engineHeader;
	return e->get_vars();
}
void TreecursionReader::close() {
	ogg_sync_clear(&state);
	if(_file){
		if(_file->is_open()){
			//for some reason it closes on eof.
			//i have to research this behavior
			memdelete(_file);
		}
		_file = nullptr;
	}
	headers.clear();
	treecursion_buffer.clear();
	open_streams.clear();
}

TreecursionReader::TreecursionReader() : _file(nullptr) {
    headers.clear();
	treecursion_buffer.clear();
}
TreecursionReader::~TreecursionReader() {
	close();
}
