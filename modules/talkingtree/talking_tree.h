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
	bool is_network_server() const;
	bool has_network_peer() const;
	void sendText(String msg);
private:
	Set<int> connected_peers;
	int last_send_cache_id;
	Ref<NetworkedMultiplayerPeer> network_peer;
	void sendPacket(PacketType type, google::protobuf::Message &message);
};

#endif
