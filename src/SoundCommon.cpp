
#include "SSound/SoundCommon.h"

#include "SSound/SoundSystem.h"



namespace VS{
	namespace Sound {

        void init()
        {
            SoundSystem::init();
        }
        
        void cleanup()
        {
            SoundSystem::cleanup();
        }
        
	}; //namespace Sound
}; //namespace VS
