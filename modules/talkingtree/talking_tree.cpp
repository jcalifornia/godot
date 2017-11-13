/* talking_tree.cpp */

#include "talking_tree.h"
#include "talking_tree_enum.h"

#include "TalkingTree.pb.h"
#include "io/marshalls.h"

#include "sdl2_audiocapture.h"

void TalkingTree::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_network_peer", "peer"), &TalkingTree::set_network_peer);
	ClassDB::bind_method(D_METHOD("is_network_server"), &TalkingTree::is_network_server);
	ClassDB::bind_method(D_METHOD("has_network_peer"), &TalkingTree::has_network_peer);
	ClassDB::bind_method(D_METHOD("get_network_connected_peers"), &TalkingTree::get_network_connected_peers);
	ClassDB::bind_method(D_METHOD("get_network_unique_id"), &TalkingTree::get_network_unique_id);
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
	ClassDB::bind_method(D_METHOD("_encode_audio_frame", "audio_frame"), &TalkingTree::_encode_audio_frame);

	ClassDB::bind_method(D_METHOD("get_audio_stream_peer", "p_id"), &TalkingTree::get_audio_stream_peer);
	ClassDB::bind_method(D_METHOD("create_audio_peer_stream", "p_id"), &TalkingTree::_create_audio_peer_stream);
	ClassDB::bind_method(D_METHOD("talk"), &TalkingTree::talk);
	ClassDB::bind_method(D_METHOD("mute"), &TalkingTree::mute);
}

TalkingTree::TalkingTree(){
}
void TalkingTree::send_text(String msg) {
	TalkingTreeProto::TextMessage txtMsg;
	CharString m = msg.utf8();
	txtMsg.set_message(m.get_data(), m.length());
	_send_packet(0, PacketType::TEXTMESSAGE, txtMsg, NetworkedMultiplayerPeer::TRANSFER_MODE_RELIABLE);
}
void TalkingTree::_send_packet(int p_to, PacketType type, google::protobuf::Message &message, NetworkedMultiplayerPeer::TransferMode transferMode){
	Vector<uint8_t> packet;
	//incorrect
	packet.resize(1 + 4 + message.ByteSize());
	packet[0] = (uint8_t)type;
	encode_uint32(message.ByteSize(), &packet[1]);
	message.SerializeToArray( &packet[5], message.ByteSize());
	network_peer->set_transfer_mode(transferMode);
	network_peer->set_target_peer(0);
	network_peer->put_packet(packet.ptr(), packet.size());
}
void TalkingTree::_network_process_packet(int p_from, const uint8_t *p_packet, int p_packet_len) {
	PacketType packet_type = (PacketType) p_packet[0];
	const uint8_t * proto_packet = &p_packet[5];
	int proto_packet_len = p_packet_len - 5;
	switch(packet_type){
		case PacketType::VERSION: {
		} break;
		case PacketType::UDPTUNNEL: {
		} break;
		case PacketType::TEXTMESSAGE: {
			TalkingTreeProto::TextMessage txtMsg;
			txtMsg.ParseFromArray( proto_packet, proto_packet_len );
			String msg;
			msg.parse_utf8(txtMsg.message().c_str(), txtMsg.message().length());
			this->emit_signal("text_message", msg, p_from);
		} break;
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

void TalkingTree::set_network_peer(const Ref<NetworkedMultiplayerPeer> &p_network_peer) {
	if (network_peer.is_valid()) {
		network_peer->disconnect("peer_connected", this, "_network_peer_connected");
		network_peer->disconnect("peer_disconnected", this, "_network_peer_disconnected");
		network_peer->disconnect("connection_succeeded", this, "_connected_to_server");
		network_peer->disconnect("connection_failed", this, "_connection_failed");
		network_peer->disconnect("server_disconnected", this, "_server_disconnected");
		connected_audio_stream_peers.clear();
		last_send_cache_id = 1;
		SDL2AudioCapture::get_singleton()->disconnect("audio_frame", this, "_encode_audio_frame");
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
		SDL2AudioCapture::get_singleton()->connect("get_pcm", this, "_encode_audio_frame");
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

Ref<AudioStreamTalkingTree> TalkingTree::get_audio_stream_peer(int pid) {
	return connected_audio_stream_peers[pid];
}
void TalkingTree::_create_audio_peer_stream(int p_id){
	connected_audio_stream_peers[p_id].instance();
	connected_audio_stream_peers[p_id]->set_pid(p_id);
	connected_audio_stream_peers[p_id]->set_mix_rate(TalkingTree::SAMPLE_RATE);
	connected_audio_stream_peers[p_id]->set_format(AudioStreamTalkingTree::FORMAT_16_BITS);
	
}
void TalkingTree::_encode_audio_frame(PoolVector<uint8_t> pcm){

}