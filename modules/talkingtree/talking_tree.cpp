/* talking_tree.cpp */

#include "talking_tree.h"
#include "talking_tree_enum.h"

#include "TalkingTree.pb.h"
#include "io/marshalls.h"


void TalkingTree::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_network_peer", "peer"), &TalkingTree::set_network_peer);
	ClassDB::bind_method(D_METHOD("is_network_server"), &TalkingTree::is_network_server);
	ClassDB::bind_method(D_METHOD("has_network_peer"), &TalkingTree::has_network_peer);
	ClassDB::bind_method(D_METHOD("get_network_connected_peers"), &TalkingTree::get_network_connected_peers);
	ClassDB::bind_method(D_METHOD("send_text", "message"), &TalkingTree::send_text);
	ClassDB::bind_method(D_METHOD("poll"), &TalkingTree::poll);
	
	ADD_SIGNAL(MethodInfo("text_message", PropertyInfo(Variant::STRING, "message"), PropertyInfo(Variant::INT, "sender_id")));
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
		connected_peers.clear();
		last_send_cache_id = 1;
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
	}
}
Vector<int> TalkingTree::get_network_connected_peers() const {
	ERR_FAIL_COND_V(!network_peer.is_valid(), Vector<int>());
	Vector<int> ret;
	for (Set<int>::Element *E = connected_peers.front(); E; E = E->next()) {
		ret.push_back(E->get());
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
