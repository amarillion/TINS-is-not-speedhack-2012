#include "sound.h"
#include <allegro.h>
#include "resources.h"
#include <assert.h>

#ifdef USE_DUMB
#include <aldumb.h>
#endif
#ifdef USE_ALSPC
#include <alspc.h>
#endif

Sound::Sound(): inited(false), soundInstalled(true), fSound(true), fMusic(true)
{
}

void Sound::initSound()
{
#ifdef USE_ALSPC
    hifi = true;
    stereo = true;
    currentMusic = NULL;
    alspc_install();
    alspc_player = NULL;
#endif
#ifdef USE_DUMB
	atexit(&dumb_exit); // dumb cleanup
    dumb_register_stdfiles();
	dp = NULL;
	currentMusic = NULL;
#endif
	inited = true;
}

void Sound::getSoundFromConfig()
{
	fMusic = get_config_int ("twist", "music", fMusic);
	fSound = get_config_int ("twist", "sound", fSound);
}

void Sound::playSample (SAMPLE *s)
{
	if (!(isSoundOn() && isSoundInstalled())) return;
	assert (s);
	play_sample (s, 127, 127, 1000, 0);
}

#ifdef USE_DUMB
void Sound::pollMusic()
{
	assert (inited);
	al_poll_duh(dp);
}

void Sound::playMusic (DUH *duh, float volume)
{
	if (!isSoundInstalled()) return;
	currentMusic = duh;
    if (!(isSoundOn() && isMusicOn())) return;
    if (dp)
    {
        al_stop_duh (dp);
        dp = NULL;
    }
    dp = al_start_duh (duh, 2, 0, volume, 4096, 22050);
}

void Sound::stopMusic ()
{
    if (dp)
    {
        al_stop_duh (dp);
        dp = NULL;
    }
}
#endif


void Sound::propertyChanged ()
{
	if (!isSoundInstalled()) return;
#if defined(USE_DUMB) || defined (USE_ALSPC)
	if (isMusicOn() && isSoundOn())
	{
		if (currentMusic != NULL)
			playMusic (currentMusic);
	}
	else
	{
		stopMusic();
	}
#endif
}

void Sound::setSoundOn(bool value)
{
	fSound = value;
	propertyChanged();
}
void Sound::setMusicOn(bool value)
{
	fMusic = value;
	propertyChanged();
}

#ifdef USE_ALSPC
void Sound::playMusic (ALSPC_DATA *alspc_data)
{
	if (!isSoundInstalled()) return;
	currentMusic = alspc_data;
    if (!(isSoundOn() && isMusicOn())) return;
    assert (alspc_data); // fails if requested SPC is not in datafile.
    stopMusic();
    alspc_player = alspc_start (alspc_data, hifi ? 44100 : 22050, 255, 128, stereo, hifi);
}

void Sound::pollMusic()
{
	assert (inited);
	if (alspc_player) alspc_poll (alspc_player);
}

void Sound::setHifiOn(bool value)
{
    if (value != hifi)
    {
        hifi = value;
        //TODO: reset music playing
    }
}

void Sound::setStereoOn(bool value)
{
    if (value != stereo)
    {
        stereo = value;
        //TODO: reset music playing
    }
}

void Sound::stopMusic()
{
    if (alspc_player)
    {
        alspc_stop(alspc_player);
        alspc_player = NULL;
    }
}

#endif

void Sound::doneSound()
{
#ifdef USE_DUMB
	stopMusic();
#endif
#ifdef USE_ALSPC
	stopMusic();
    alspc_uninstall();
#endif
}

