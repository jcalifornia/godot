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
	ADD_SIGNAL(MethodInfo("text_message", PropertyInfo(Variant::STRING, "message"), PropertyInfo(Variant::INT, "sender_id")));
}

TalkingTree::TalkingTree() {
}
void TalkingTree::send_text(String msg) {
	TalkingTreeProto::TextMessage txtMsg;
	CharString m = msg.utf8();
	txtMsg.set_message(m.get_data(), m.length());
	_send_packet(0, PacketType::TEXTMESSAGE, txtMsg, NetworkedMultiplayerPeer::TRANSFER_MODE_RELIABLE);
}
void TalkingTree::_send_packet(int p_to, PacketType type, google::protobuf::Message &message, NetworkedMultiplayerPeer::TransferMode transferMode){
	Vector<uint8_t> packet;
	packet.resize(1 + 4 + message.ByteSize());
	packet[0] = (uint8_t)type;
	encode_uint32(message.ByteSize(), &packet[1]);
	message.SerializeToArray( &packet[5], message.ByteSize());
	network_peer->set_transfer_mode(transferMode);
	network_peer->set_target_peer(0);
	network_peer->put_packet(packet.ptr(), packet.size());
}
void TalkingTree::_serialize_packet(int p_from, const uint8_t *p_packet, int p_packet_len) {
	PacketType packet_type = (PacketType) p_packet[0];
	switch(packet_type){
		case PacketType::VERSION: {
		} break;
		case PacketType::UDPTUNNEL: {
		} break;
		case PacketType::TEXTMESSAGE: {
			TalkingTreeProto::TextMessage txtMsg;
			txtMsg.ParseFromArray( p_packet, p_packet_len );
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
