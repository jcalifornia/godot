/* talking_tree.cpp */

#include "talking_tree_enum.h"
#include "varint.h"
#include "talking_tree.h"

#include "TalkingTree.pb.h"
#include "io/marshalls.h"

#include "sdl2_audiocapture.h"

#include "os/os.h"
#include "pair.h"
void TalkingTree::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_network_peer", "peer"), &TalkingTree::set_network_peer);
	ClassDB::bind_method(D_METHOD("set_game_network_peer", "peer"), &TalkingTree::set_game_network_peer);
	ClassDB::bind_method(D_METHOD("is_network_server"), &TalkingTree::is_network_server);
	ClassDB::bind_method(D_METHOD("has_network_peer"), &TalkingTree::has_network_peer);
	ClassDB::bind_method(D_METHOD("get_network_connected_peers"), &TalkingTree::get_network_connected_peers);
	ClassDB::bind_method(D_METHOD("get_network_unique_id"), &TalkingTree::get_network_unique_id);
	ClassDB::bind_method(D_METHOD("send_user_info"), &TalkingTree::send_user_info);
	ClassDB::bind_method(D_METHOD("poll"), &TalkingTree::poll);
	
	ClassDB::bind_method(D_METHOD("_network_peer_connected"), &TalkingTree::_network_peer_connected);
	ClassDB::bind_method(D_METHOD("_network_peer_disconnected"), &TalkingTree::_network_peer_disconnected);
	ClassDB::bind_method(D_METHOD("_connected_to_server"), &TalkingTree::_connected_to_server);
	ClassDB::bind_method(D_METHOD("_connection_failed"), &TalkingTree::_connection_failed);
	ClassDB::bind_method(D_METHOD("_server_disconnected"), &TalkingTree::_server_disconnected);

	ADD_SIGNAL(MethodInfo("connected_to_server"));
	ADD_SIGNAL(MethodInfo("connection_failed"));
	ADD_SIGNAL(MethodInfo("server_disconncted"));
	ADD_SIGNAL(MethodInfo("network_peer_connected", PropertyInfo(Variant::INT, "id")));
	ADD_SIGNAL(MethodInfo("network_peer_disconnected", PropertyInfo(Variant::INT, "id")));
	//Text
	ClassDB::bind_method(D_METHOD("send_text", "message"), &TalkingTree::send_text);
	ADD_SIGNAL(MethodInfo("text_message", PropertyInfo(Variant::STRING, "message"), PropertyInfo(Variant::INT, "sender_id")));
	//VOIP
	ClassDB::bind_method(D_METHOD("_create_audio_frame", "audio_frame"), &TalkingTree::_create_audio_frame);

	ClassDB::bind_method(D_METHOD("get_audio_stream_peer", "p_id"), &TalkingTree::get_audio_stream_peer);
	ClassDB::bind_method(D_METHOD("talk"), &TalkingTree::talk);
	ClassDB::bind_method(D_METHOD("mute"), &TalkingTree::mute);
}

TalkingTree::TalkingTree() : last_sent_audio_timestamp(0){
	int error;
	opusDecoder = opus_decoder_create(TalkingTree::SAMPLE_RATE, 1, &error);
	if(error != OPUS_OK){
		ERR_PRINTS( "failed to initialize OPUS decoder: " + String( opus_strerror(error)));
	}
	opusEncoder = opus_encoder_create(TalkingTree::SAMPLE_RATE, 1, OPUS_APPLICATION_VOIP, &error);
	if(error != OPUS_OK){
		ERR_PRINTS( "failed to initialize OPUS encoder: " + String( opus_strerror(error)));
	}
	opus_encoder_ctl(opusEncoder, OPUS_SET_VBR(0));
	error = opus_encoder_ctl(opusEncoder, OPUS_SET_BITRATE(TalkingTree::BIT_RATE));
	if (error != OPUS_OK) {
        ERR_PRINTS("failed to initialize bitrate to " + itos(TalkingTree::BIT_RATE) + "B/s: " + String(opus_strerror(error)));
	}
	reset_encoder();
}
TalkingTree::~TalkingTree(){
    if (opusDecoder) {
        opus_decoder_destroy(opusDecoder);
    }

    if (opusEncoder) {
        opus_encoder_destroy(opusEncoder);
	}
}
void TalkingTree::reset_encoder(){
	int status = opus_encoder_ctl(opusEncoder, OPUS_RESET_STATE, nullptr);
	
	if (status != OPUS_OK) {
		ERR_PRINTS("failed to reset encoder: " + String( opus_strerror(status)));
	}
	outgoing_sequence_number = 0;
}
void TalkingTree::send_text(String msg) {
	TalkingTreeProto::TextMessage txtMsg;
	CharString m = msg.utf8();
	txtMsg.set_message(m.get_data(), m.length());
	_send_packet(0, PacketType::TEXTMESSAGE, txtMsg, NetworkedMultiplayerPeer::TRANSFER_MODE_RELIABLE);
}
void TalkingTree::send_user_info(){
	_send_user_info(0);
}
void TalkingTree::_send_user_info(int p_to){
	if(game_peer.is_valid()){
		TalkingTreeProto::UserInfo usrInfo;
		usrInfo.set_user_id(game_peer->get_unique_id());
		usrInfo.set_tree_id(network_peer->get_unique_id());
		_send_packet(p_to, PacketType::USERINFO, usrInfo, NetworkedMultiplayerPeer::TRANSFER_MODE_RELIABLE);
	}
}

Ref<AudioStreamTalkingTree> TalkingTree::get_audio_stream_peer(int pid) {
	const int talkingtree_id = connected_peers.getForward(pid);
	return connected_audio_stream_peers.get(talkingtree_id);
}
void TalkingTree::_create_audio_peer_stream(int p_id){
	connected_audio_stream_peers[p_id].instance();
	connected_audio_stream_peers[p_id]->set_pid(p_id);
	connected_audio_stream_peers[p_id]->set_mix_rate(TalkingTree::SAMPLE_RATE);
	connected_audio_stream_peers[p_id]->set_format(AudioStreamTalkingTree::FORMAT_16_BITS);
	
}

void TalkingTree::_send_packet(int p_to, PacketType type, google::protobuf::Message &message, NetworkedMultiplayerPeer::TransferMode transferMode){
	Vector<uint8_t> packet;
	//incorrect
	packet.resize(1 + message.ByteSize());
	packet[0] = (uint8_t)type;
	encode_uint32(message.ByteSize(), &packet[1]);
	message.SerializeToArray( &packet[1], message.ByteSize());
	network_peer->set_transfer_mode(transferMode);
	network_peer->set_target_peer(p_to);
	network_peer->put_packet(packet.ptr(), packet.size());
}

void TalkingTree::_network_process_packet(int p_from, const uint8_t *p_packet, int p_packet_len) {
	PacketType packet_type = (PacketType) p_packet[0];
	const uint8_t * proto_packet = &p_packet[1];
	int proto_packet_len = p_packet_len - 1;
	switch(packet_type){
		case PacketType::VERSION: {
		} break;
		case PacketType::UDPTUNNEL: {
			_process_audio_packet(p_from, proto_packet, proto_packet_len);
		} break;
		case PacketType::TEXTMESSAGE: {
			TalkingTreeProto::TextMessage txtMsg;
			txtMsg.ParseFromArray( proto_packet, proto_packet_len );
			String msg;
			msg.parse_utf8(txtMsg.message().c_str(), txtMsg.message().length());
			this->emit_signal("text_message", msg, p_from);
		} break;
		case PacketType::USERINFO: {
			TalkingTreeProto::UserInfo usrInfo;
			usrInfo.ParseFromArray( proto_packet, proto_packet_len );
			int game_id = usrInfo.user_id();
			int tree_id = usrInfo.tree_id();
			//create audio stream peer
			_create_audio_peer_stream(tree_id);
			if(network_peer->is_server()){
				//send everybody else
				const int *k=NULL;
				while((k=connected_peers.next(k))){
					TalkingTreeProto::UserInfo otherUsr;
					otherUsr.set_user_id(*k);
					otherUsr.set_tree_id(connected_peers.getForward(*k));
					_send_packet(p_from, PacketType::USERINFO, otherUsr, NetworkedMultiplayerPeer::TRANSFER_MODE_RELIABLE);
					
				}
				//send yourself
				_send_user_info(p_from);
			}
			connected_peers.add(game_id, tree_id);
			
		}
		default:
			break;
	}
}

bool TalkingTree::is_network_server() const {

	ERR_FAIL_COND_V(!network_peer.is_valid(), false);
	return network_peer->is_server();
}

bool TalkingTree::has_network_peer() const {
	return network_peer.is_valid();
}
void TalkingTree::set_game_network_peer(const Ref<NetworkedMultiplayerPeer> &p_network_peer){
	game_peer = p_network_peer;
	if (game_peer.is_valid()) {
	}

}
void TalkingTree::set_network_peer(const Ref<NetworkedMultiplayerPeer> &p_network_peer) {
	if (network_peer.is_valid()) {
		network_peer->disconnect("peer_connected", this, "_network_peer_connected");
		network_peer->disconnect("peer_disconnected", this, "_network_peer_disconnected");
		network_peer->disconnect("connection_succeeded", this, "_connected_to_server");
		network_peer->disconnect("connection_failed", this, "_connection_failed");
		network_peer->disconnect("server_disconnected", this, "_server_disconnected");
		connected_audio_stream_peers.clear();
		connected_peers.clear();
		last_send_cache_id = 1;
		SDL2AudioCapture::get_singleton()->disconnect("get_pcm", this, "_create_audio_frame");
	}

	ERR_EXPLAIN("Supplied NetworkedNetworkPeer must be connecting or connected.");
	ERR_FAIL_COND(p_network_peer.is_valid() && p_network_peer->get_connection_status() == NetworkedMultiplayerPeer::CONNECTION_DISCONNECTED);
	network_peer = p_network_peer;
	if (network_peer.is_valid()) {
		network_peer->connect("peer_connected", this, "_network_peer_connected");
		network_peer->connect("peer_disconnected", this, "_network_peer_disconnected");
		network_peer->connect("connection_succeeded", this, "_connected_to_server");
		network_peer->connect("connection_failed", this, "_connection_failed");
		network_peer->connect("server_disconnected", this, "_server_disconnected");
		SDL2AudioCapture::get_singleton()->connect("get_pcm", this, "_create_audio_frame");
	}
}
void TalkingTree::_connection_failed() {
	emit_signal("connection_failed");
}
void TalkingTree::_connected_to_server() {
	emit_signal("connected_to_server");
}
void TalkingTree::_network_peer_connected(int p_id) {
	emit_signal("network_peer_connected", p_id);
}
void TalkingTree::_network_peer_disconnected(int p_id) {
	connected_audio_stream_peers.erase(p_id);
	connected_peers.eraseData(p_id);
	emit_signal("network_peer_disconnected", p_id);
}
void TalkingTree::_server_disconnected(){
	connected_audio_stream_peers.clear();
	emit_signal("server_disconnected");
}
int TalkingTree::get_network_unique_id() const {
	ERR_FAIL_COND_V(!network_peer.is_valid(), 0);
	return network_peer->get_unique_id();
}
Vector<int> TalkingTree::get_network_connected_peers() const {
	ERR_FAIL_COND_V(!network_peer.is_valid(), Vector<int>());
	Vector<int> ret;
	const int *k=NULL;
	while( (k=connected_audio_stream_peers.next(k)) ) {
		ret.push_back(*k);
	}
	return ret;
}

void TalkingTree::_network_poll() {
	
	if (!network_peer.is_valid() || network_peer->get_connection_status() == NetworkedMultiplayerPeer::CONNECTION_DISCONNECTED)
		return;

	network_peer->poll();

	if (!network_peer.is_valid()) //it's possible that polling might have resulted in a disconnection, so check here
		return;

	while (network_peer->get_available_packet_count()) {

		int sender = network_peer->get_packet_peer();
		const uint8_t *packet;
		int len;

		Error err = network_peer->get_packet(&packet, len);
		if (err != OK) {
			ERR_PRINT("Error getting packet!");
		}

		_network_process_packet(sender, packet, len);

		if (!network_peer.is_valid()) {
			break; //it's also possible that a packet or RPC caused a disconnection, so also check here
		}
	}
}

void TalkingTree::poll(){
	while(true){
		//_timer->set_wait_time(500.0/1000.0);
		_network_poll();
		//_timer->start();
	}
}
void TalkingTree::mute(){
	SDL2AudioCapture::get_singleton()->mute();
}

void TalkingTree::talk(){
	SDL2AudioCapture::get_singleton()->talk();
}


Pair<int, bool> TalkingTree::_decode_opus_frame(const uint8_t *in_buf, int in_len, int16_t *pcm_buf, int buf_len){
	VarInt varint(in_buf);
	int16_t opus_length = varint.getValue();
	int pointer = varint.getEncoded().size();
	bool endFrame = opus_length != 0x2000;
	opus_length = opus_length & 0x1fff;
	int decoded_samples = opus_decode(opusDecoder, (const unsigned char *) &in_buf[pointer], opus_length, pcm_buf, buf_len, 0);
	return Pair<int,bool>(decoded_samples, endFrame);
}

void TalkingTree::_process_audio_packet(int p_from, const uint8_t *p_packet, int p_packet_len){
	//print_line("from id: "+ itos(p_from));
	int pointer = 1;
	VarInt seqNum(&p_packet[pointer]);
	pointer += seqNum.getEncoded().size();
	uint64_t sequenceNumber = seqNum.getValue();

	if(pointer >= p_packet_len){
		ERR_PRINTS("invalid incoming audio packet " + itos(p_packet_len) + "B : header : " + itos(pointer));
	}
	AudioCodingType codingType = static_cast<AudioCodingType>((p_packet[0] & 0xC0) >> 6);
	bool facial_flag = (p_packet[0] & 0x20); //anything greater than 0 is true;
	uint8_t target = p_packet[0] & 0x1F;

	if(facial_flag){
		ERR_PRINTS("Facial data not supported");
	}
	int payloadLength = p_packet_len - pointer;
	const uint8_t *payload = &p_packet[pointer];
	int16_t pcm_buf[50000];
	//print_line("talking: " + itos(p_from) + " size: " + itos(payloadLength));
	Pair<int, bool> out_len;
	switch( codingType ){
		case AudioCodingType::OPUS: {
				out_len = _decode_opus_frame(payload, payloadLength, pcm_buf, 50000);
				break;
			}
		default:
			ERR_PRINTS("Unsupported Audio format: " + itos((uint32_t) codingType));
			return;
	}
	
	connected_audio_stream_peers[p_from]->append_data((uint8_t *) pcm_buf, sizeof(int16_t) * out_len.first);
	this->emit_signal("audio_message", p_from);
}

void TalkingTree::_create_audio_frame(PoolVector<uint8_t> pcm){
	_encode_audio_frame(1, pcm);
}
int TalkingTree::_encode_audio_frame(int target, PoolVector<uint8_t> &pcm){
	int now = OS::get_singleton()->get_ticks_msec();
	if( (now-last_sent_audio_timestamp) > 5000 ){
		reset_encoder();
	}
	int16_t pcm_buf[TalkingTree::FRAME_SIZE];
	PoolVector<uint8_t>::Read  tmp = pcm.read();
	copymem( pcm_buf, tmp.ptr(), pcm.size());
	//https://www.opus-codec.org/docs/html_api/group__opusencoder.html#ga88621a963b809ebfc27887f13518c966
	//in_len most be multiples of 120
	uint8_t opus_buf[1024];
	const int output_size = opus_encode(opusEncoder, pcm_buf, TalkingTree::FRAME_SIZE, opus_buf, 1024);


	Vector<uint8_t> encoded_size = VarInt(output_size).getEncoded();
	Vector<uint8_t> encoded_seq = VarInt( outgoing_sequence_number ).getEncoded();

	int seqNum = 100 * pcm.size()/2 / TalkingTree::SAMPLE_RATE;	
	outgoing_sequence_number += seqNum;

	Vector<uint8_t> audiobuf;
	audiobuf.resize(1 + 1 + encoded_seq.size() + encoded_size.size() + output_size);
	audiobuf[1] = 0x00 | target;
	copymem( &audiobuf[2],  encoded_seq.ptr(), encoded_seq.size() );
	copymem( &audiobuf[2 + encoded_seq.size()],  encoded_size.ptr(), encoded_size.size() );
	copymem( &audiobuf[2 + encoded_seq.size() + encoded_size.size()],  opus_buf, output_size );

	
	last_sent_audio_timestamp = OS::get_singleton()->get_ticks_msec();
	audiobuf[0] = (uint8_t) PacketType::UDPTUNNEL;
	network_peer->set_transfer_mode(NetworkedMultiplayerPeer::TRANSFER_MODE_UNRELIABLE_ORDERED);
	network_peer->set_target_peer(0);
	network_peer->put_packet(audiobuf.ptr(), audiobuf.size());
	return audiobuf.size();
}
