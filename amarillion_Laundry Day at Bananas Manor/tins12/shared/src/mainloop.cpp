#include <assert.h>
#include "mainloop.h"
#include "color.h"

#ifdef USE_ALFONT
#include <alfont.h>
#endif

#ifdef USE_TEGEL
#include <tegel.h>
#endif

using namespace std;

volatile int MainLoop::counterMsec = 0;

void MainLoop::timer_handler()
{
	MainLoop::counterMsec += 5;
}

END_OF_FUNCTION(timer_handler);

MainLoop::MainLoop(const char *configFilename, const char *title, int _bufw, int _bufh) :
	configFilename(configFilename), title(title), stretch (false), bufw(_bufw), bufh (_bufh),
	logicIntervalMsec(20)
{
	quit = false;
	buffer = NULL;
	last_fps = 0;
	lastUpdateMsec = 0;
	frame_count = 0;
	frame_counter = 0;

#ifdef DEBUG
	// default in debug version
	windowed = true;
	fpsOn = true;
#else
	// default in release version
	windowed = false;
	fpsOn = false;
#endif
}

void MainLoop::getFromConfig()
{
	getSoundFromConfig();
	fpsOn = get_config_int ("twist", "fps", fpsOn);
	windowed = get_config_int ("twist", "windowed", windowed);
}

void MainLoop::getFromArgs(int argc, const char *const *argv)
{
	// parse command line arguments
	int i;
	for (i = 1; i < argc; i++)
	{
		if (strcmp (argv[i], "-nosound") == 0)
		{
			setSoundInstalled(false);
		}
		else if (strcmp (argv[i], "-windowed") == 0)
		{
			windowed = true;
		}
		else if (strcmp (argv[i], "-fullscreen") == 0)
		{
			windowed = false;
		}
		else if (strcmp (argv[i], "-showfps") == 0)
		{
			fpsOn = true;
		}
		else
		{
			options.push_back (string(argv[i]));
		}
	}	
}

int MainLoop::init(int argc, const char *const *argv)
// returns 1 on success, 0 on failure
{
	LOCK_VARIABLE(Main::counterMsec);
	LOCK_FUNCTION(timer_handler);
	if (allegro_init () < 0)
	{
		allegro_message ("init failed");
		return 0;
	}

	override_config_file (configFilename);
	getFromConfig();
	getFromArgs (argc, argv);
	parseOpts(options);
	
	if (install_keyboard () < 0)
	{
		allegro_message ("install keyboard failed");
		return 0;
	}
		
	if (install_timer() < 0)
	{
		allegro_message ("install timer failed");
		return 0;
	}

	set_volume_per_voice (1);	
	if (isSoundInstalled() && install_sound(DIGI_AUTODETECT, MIDI_NONE, NULL) < 0)
	{
		// could not get sound to work
		setSoundInstalled(false);
		allegro_message ("Could not initialize sound. Sound is turned off.\n%s\n", allegro_error);
	}
	if (install_int (timer_handler, 5) < 0)
	{
		allegro_message ("installation of timer handler failed");
		return 0;
	}
	initSound();
#ifdef USE_ALFONT
	int result = alfont_init ();
	if (result != 0)
	{
		allegro_message ("Installation of alfont failed with error code %i", result);
		return 0;
	}	
#endif
	const int DEFAULT_DEPTH = 16;
	int depth = DEFAULT_DEPTH;
	// in windowed mode, use the desktop depth if possible
	if (windowed)
	{
		int depth = desktop_color_depth();
		if (depth == 0) depth = DEFAULT_DEPTH; // could not be determined, use default.
	}
	set_color_depth(depth);

	bool success = false;
	int gfx_driver = windowed ? GFX_AUTODETECT_WINDOWED : GFX_AUTODETECT_FULLSCREEN;
	int resNum = resX.size();

	// if there is no resolution defined, use default of bufw x bufh
	if (resNum == 0)
	{
		addResolution (bufw, bufh);
		resNum = 1;
	}

	for (int i = 0; i < resNum; ++i)
	{
		int rx = resX[i];
		int ry = resY[i];
		if (set_gfx_mode(gfx_driver, rx, ry, 0, 0)==0)
		{
			success = true;
			if (rx != bufw || ry != bufh) stretch = true;
			break;
		}
	}

	if (!success)
	{
		allegro_message("Unable initialize graphics module\n%s\n", allegro_error);
		return 0;
	}

	// use the first resolution as the primary game resolution.
	// not necessarily the same size as the actual game resolution
	buffer = create_bitmap(bufw, bufh);
	if (!buffer)
	{
		allegro_message ("Error creating background buffer");
		return 0;
	}
#ifdef USE_MOUSE
	if (install_mouse() == -1)
	{
		allegro_message ("could not install mouse");
		set_gfx_mode (GFX_TEXT, 0, 0, 0, 0);
		allegro_exit();
		return 0;
	}
#endif
	set_window_title (title);
#ifdef USE_TEGEL
	teg_install();
#endif
	return postInit();
}

void MainLoop::run()
{
	while (isAlive() && !quit)
	{
		// wait until it's time for the next logic update
		while ((counterMsec - lastUpdateMsec) < logicIntervalMsec)
		{
#if defined(USE_DUMB) || defined (USE_ALSPC)
			pollMusic();
#endif
			rest(1);
		}

		while ((counterMsec - lastUpdateMsec) >= logicIntervalMsec)
		{
#if defined(USE_DUMB) || defined (USE_ALSPC)
			pollMusic();
#endif
			lastUpdateMsec += logicIntervalMsec;
			int update_start = counterMsec;

			if (isAlive()) update();
#ifdef DEBUG
			if (key[KEY_F10]) quit = true; // emergency bail out
#endif

			// check to see if update takes too long
			if (counterMsec - update_start > 40)
			{
				// we probably can't catch up anymore.
				// move update_counter ahead, or we have to
				// catch up even more next loop
				lastUpdateMsec = counterMsec;
				break;
			}
		}

		GraphicsContext gc;
		gc.buffer = buffer;
		gc.xofst = 0;
		gc.yofst = 0;

		if (isVisible()) draw(gc);
#if defined(USE_DUMB) || defined (USE_ALSPC)
			pollMusic();
#endif

        if (fpsOn)
		{
//			textprintf_ex (buffer, font, 0, 0,
//				  WHITE, -1, "fps: %d", last_fps);
		}

        if (stretch)
    		stretch_blit (buffer, screen, 0, 0, buffer->w, buffer->h, 0, 0, SCREEN_W, SCREEN_H);
        else
        	blit (buffer, screen, 0, 0, 0, 0, buffer->w, buffer->h);

		if ((counterMsec - frame_counter) > 1000)
		{
			last_fps = frame_count;
			frame_count = 0;
			frame_counter = counterMsec;
		}
		frame_count++;
	}
}

MainLoop::~MainLoop()
{
	if (buffer) destroy_bitmap (buffer);

	doneSound();
#ifdef USE_ALFONT
	alfont_exit();
#endif
}


void MainLoop::addResolution (int w, int h)
{
	resX.push_back (w);
	resY.push_back (h);
}
