
#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>

#include "SSound/SSound.h"

using namespace SSound;



int main(void)
{
    SSound::init();

    AudioFile *file = SoundSystem::createAudioFile(std::string("./assets/sample.wav"), std::string("wav"));
    file->init();
    //file->cache();

    SoundObject *so = new SoundObject(file);
    so->init();
    so->setRepeat(true);
    so->play();

    std::getchar();

    so->cleanup();
    SAFE_DELETE(so);

    file->cleanup();
    SAFE_DELETE(file);

    SSound::cleanup();

    exit(EXIT_SUCCESS);
}
