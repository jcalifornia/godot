
/* register_types.cpp */

#include "register_types.h"
#include "class_db.h"
#include "talking_tree.h"
#include "sdl2_audiocapture.h"

static SDL2AudioCapture *SDL2_audio_capture = NULL;

void register_talkingtree_types() {
	ClassDB::register_class<TalkingTree>();
	//ClassDB::register_class<SDL2AudioCapture>();
	SDL2_audio_capture = memnew(SDL2AudioCapture);
	SDL2_audio_capture->init();
	SDL2_audio_capture->set_singleton();
}

void unregister_talkingtree_types() {
	//nothing to do here
	SDL2_audio_capture->finish();
}
