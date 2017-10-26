/* talking_tree.cpp */

#include "talking_tree.h"
#include "talking_tree_enum.h"

#include "TalkingTree.pb.h"


void TalkingTree::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_network_peer", "peer"), &TalkingTree::set_network_peer);
	ClassDB::bind_method(D_METHOD("is_network_server"), &TalkingTree::is_network_server);
	ClassDB::bind_method(D_METHOD("has_network_peer"), &TalkingTree::has_network_peer);
}

TalkingTree::TalkingTree() {
}
void TalkingTree::sendText(String msg) {
	TalkingTreeProto::TextMessage txtMsg;
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
