/* talking_tree_storage.cpp */

#include "talking_tree_storage.h"

void TalkingTreeStorage::_bind_methods() {

}

TalkingTreeStorage::TalkingTreeStorage() {
	//opusEncoder = opus_multistream_encoder_create();
}
TalkingTreeStorage::~TalkingTreeStorage() {
	if(opusEncoder){
		opus_multistream_encoder_destroy(opusEncoder);
	}
}