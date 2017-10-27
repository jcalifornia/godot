/* talking_tree.h */
#ifndef TALKING_TREE_H
#define TALKING_TREE_H

#include "reference.h"
#include "io/networked_multiplayer_peer.h"
#include "ustring.h"
#include "talking_tree_enum.h"
#include <google/protobuf/message.h>

class TalkingTree : public Reference {
	GDCLASS(TalkingTree, Reference);

protected:
	static void _bind_methods();

public:
	TalkingTree();
	void set_network_peer(const Ref<NetworkedMultiplayerPeer> &p_network_peer);
	Vector<int> get_network_connected_peers() const;
	bool is_network_server() const;
	bool has_network_peer() const;
	void send_text(String msg);
private:
	Set<int> connected_peers;
	int last_send_cache_id;
	Ref<NetworkedMultiplayerPeer> network_peer;
	void _send_packet(PacketType type, google::protobuf::Message &message);
	void _serialize_packet(int p_from, const uint8_t *p_packet, int p_packet_len);
};

#endif
