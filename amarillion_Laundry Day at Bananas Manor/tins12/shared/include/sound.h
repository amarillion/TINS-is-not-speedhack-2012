#ifndef SOUND_H_
#define SOUND_H_

#ifdef USE_DUMB
struct DUH;
struct AL_DUH_PLAYER;
#endif

#ifdef USE_ALSPC
struct ALSPC_DATA;
struct ALSPC_PLAYER;
#endif

struct SAMPLE;

class Resources;

/**
 * Global sound manager
 */
class Sound
{
protected:
	void getSoundFromConfig();
	void initSound();
#ifdef USE_DUMB
	void pollMusic();
#endif
#ifdef USE_ALSPC
	void pollMusic();
#endif
private:
	bool inited;
	void propertyChanged ();
	bool soundInstalled;
	bool fSound;
	bool fMusic;
#ifdef USE_DUMB
	AL_DUH_PLAYER *dp;
	DUH *currentMusic;
#endif
#ifdef USE_ALSPC
    bool stereo;
    bool hifi;
    ALSPC_DATA *currentMusic;
    ALSPC_PLAYER *alspc_player;
#endif
public:
	Sound();
	bool isSoundOn() { return fSound; }
	bool isSoundInstalled() { return soundInstalled; }
	void setSoundInstalled(bool value) { soundInstalled = value; }
	void playSample (SAMPLE *s);
	bool isMusicOn() { return fMusic; }
	void setSoundOn(bool value);
	void setMusicOn(bool value);
	void doneSound();
#ifdef USE_DUMB
	void playMusic (DUH *duh, float volume = 1.0f);
	void stopMusic ();
#endif
#ifdef USE_ALSPC
	void playMusic (ALSPC_DATA *alspc_data);
	void stopMusic ();
    bool isStereoOn() { return stereo; }
	void setStereoOn (bool value);
    bool isHifiOn() { return hifi; }
	void setHifiOn (bool value);
#endif
};

#endif
