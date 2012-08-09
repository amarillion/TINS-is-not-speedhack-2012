#include <assert.h>
#include "engine.h"
#include "main.h"
#include "color.h"
#include "screenshot.h"
#include "motion.h"
#include "motionimpl.h"
#include "ComponentBuilder.h"
#include "CutScene.h"
#include "text.h"

using namespace boost;

int Engine::getCounter ()
{
	return parent->getCounter();
}

Engine::Engine (Main *p) : event (E_NONE)
{
	parent = p;
	debug = false;
	resources = p->getResources();
	settings = p->getSettings();

	menu = shared_ptr<Menu>(new Menu(*this));
	game = shared_ptr<Game>(new Game(this));
	
	btnScreenshot.setScancode (KEY_F12);

#ifdef DEBUG
	btnAbort.setScancode (KEY_F10);
	btnDebugMode.setScancode (KEY_F11);
#endif

	setEvent(E_MAINMENU);
}

Button* Engine::getButton()
{
	return settings->getButton();
}

void Engine::init()
{
 	srand(time(0));
 	state.clear();

	add (dynamic_pointer_cast<IComponent>(menu), false);
	add (dynamic_pointer_cast<IComponent>(game), false);

 	// override menu font before initing
	menu->setFont(resources->getAlfont("GP32", 40));

 	menu->init();
 	game->init();

	setFocus(dynamic_pointer_cast<IComponent>(menu));
}

int Engine::update ()
{
	if (btnScreenshot.justPressed())
	{
		screenshot();
	}

#ifdef DEBUG
	if (btnDebugMode.justPressed())
	{
		debug = !debug;
	}
	if (btnAbort.justPressed())
	{
		setEvent(E_QUIT);
	}
#endif

	if (event != E_NONE)
		handleMessage(event);

	int result = IContainer::update();
	if (result != 0)
		handleMessage(result);

	return 0;
}

void Engine::playSample (const char *name)
{
	parent->playSample (resources->getSample(string(name)));
}

void Engine::done()
{
}

void Engine::initStartSequence(CutScene *cutscene)
{
	IContainerPtr p1 = cutscene->newPage();

	IComponentPtr a1 = IComponentPtr(new AnimComponent(resources->getAnim("Open")));
	a1->setx(200);
	a1->sety(0);
	p1->add(a1);

	IComponentPtr t1 = IComponentPtr(new TextComponent(
			"One day, we Find Fole and Raul\n"
			"sitting in their living room\n"
			"What shall we do tonight, Fole?\n"
			"The same we do every night, Raul\n"
			"Try to take over the world\n"
			"\n"
			"But it's raining...\n"
			"\n"
			"Ok, let's do the laundry then.\n"
			"\n"
			"\n"));
	t1->setx(400);
	t1->sety(320);
	p1->add(t1);

	{
	IContainerPtr p = cutscene->newPage();
	IComponentPtr a = IComponentPtr(new AnimComponent(resources->getAnim("Laundry room 1")));
	a->setx(200);
	a->sety(0);
	p->add(a);

	IComponentPtr t = IComponentPtr(new TextComponent(
			"\n"
			"\n"
			"\n"));
	t->setx(400);
	t->sety(320);
	p->add(t);
	}

	{
	IContainerPtr p = cutscene->newPage();
	IComponentPtr a = IComponentPtr(new AnimComponent(resources->getAnim("Laundry room 2")));
	a->setx(200);
	a->sety(0);
	p->add(a);

	IComponentPtr t = IComponentPtr(new TextComponent(
			"\n"
			"\n"
			"\n"));
	t->setx(400);
	t->sety(320);
	p->add(t);
	}

	{
	IContainerPtr p = cutscene->newPage();
	IComponentPtr a = IComponentPtr(new AnimComponent(resources->getAnim("Laundry room 3")));
	a->setx(200);
	a->sety(0);
	p->add(a);

	IComponentPtr t = IComponentPtr(new TextComponent(
			"Hello, technical support?\n"
			"\n"
			"... Have you tried turning it off ...\n"
			"and then on again?'\n"
			"\n"));
	t->setx(400);
	t->sety(320);
	p->add(t);
	}

	{
	IContainerPtr p = cutscene->newPage();
	IComponentPtr a = IComponentPtr(new AnimComponent(resources->getAnim("Laundry room 4")));
	a->setx(200);
	a->sety(0);
	p->add(a);

	IComponentPtr t = IComponentPtr(new TextComponent(
			"\n"
			"\n"
			"\n"));
	t->setx(400);
	t->sety(320);
	p->add(t);
	}

	{
	IContainerPtr p = cutscene->newPage();
	IComponentPtr a = IComponentPtr(new AnimComponent(resources->getAnim("Laundry room 5")));
	a->setx(200);
	a->sety(0);
	p->add(a);

	IComponentPtr t = IComponentPtr(new TextComponent(
			"Fole? \n"
			"\n"
			"Fole ???\n"
			"\n"
			"\n"));
	t->setx(400);
	t->sety(320);
	p->add(t);
	}

	{
	IContainerPtr p = cutscene->newPage();
	IComponentPtr a = IComponentPtr(new AnimComponent(resources->getAnim("Laundry room 6")));
	a->setx(200);
	a->sety(0);
	p->add(a);

	IComponentPtr t = IComponentPtr(new TextComponent(
			"Meh...\n"
			"\n"
			"\n"
			"\n"
			"\n"));
	t->setx(400);
	t->sety(320);
	p->add(t);
	}

}

void Engine::initEndSequence(CutScene *cutscene)
{
	{
	IContainerPtr p = cutscene->newPage();
	IComponentPtr a = IComponentPtr(new AnimComponent(resources->getAnim("Close")));
	a->setx(200);
	a->sety(0);
	p->add(a);

	IComponentPtr t = IComponentPtr(new TextComponent(
			"My laundry machine accidentally my cat.\n"
			"The whole of it.\n"
			"\n"
			"Will it be OK?\n"
			"\n"
			"\n"
			"\n"
			"\n"
			"\n"
			"\n"
			"\n"));
	t->setx(400);
	t->sety(320);
	p->add(t);
	}
	{
	IContainerPtr p = cutscene->newPage();
	IComponentPtr a = IComponentPtr(new AnimComponent(resources->getAnim("Close")));
	a->setx(200);
	a->sety(0);
	p->add(a);

	IComponentPtr t = IComponentPtr(new TextComponent(
			"\n"
			"\n"
			"Thank you for playing!\n"
			"\n"
			"Max and Amarillion\n"
			"\n"
			"\n"
			"\n"
			"\n"));
	t->setx(400);
	t->sety(320);
	p->add(t);
	}
}

void Engine::handleMessage(int code)
{
	switch (code)
	{
		case E_NEWGAME:
			game->initGame(); // reset game state
			{
				CutScene *cutscene = new CutScene(E_CHOOSELEVEL);
				IComponentPtr ptr = IComponentPtr(cutscene);
				add (ptr);
				cutscene->setFont(resources->getAlfont("megaman_2", 16));

				initStartSequence (cutscene);
				setFocus(ptr);
			}
			clear_keybuf();
			break;
		case E_CHOOSELEVEL:
			menu->prepare(2);
			setFocus(dynamic_pointer_cast<IComponent>(menu));
			clear_keybuf();
			break;
		case E_STARTLEVEL:
			setFocus(dynamic_pointer_cast<IComponent>(game));
			clear_keybuf();
			break;
		case E_RESUME:
			setFocus(dynamic_pointer_cast<IComponent>(game));
			clear_keybuf();
			break;
		case E_PAUSE:
			menu->prepare(1);
			setFocus(dynamic_pointer_cast<IComponent>(menu));
			clear_keybuf();
			break;
		case E_FINISHED:
			menu->prepare(0);
			setFocus(dynamic_pointer_cast<IComponent>(menu));
			clear_keybuf();
			break;
		case E_QUIT:
			parent->stop();
			clear_keybuf();
			break;
		case E_MAINMENU:
			game->initGame();
			menu->prepare(0);
			setFocus(dynamic_pointer_cast<IComponent>(menu));
			clear_keybuf();
			break;
		case E_STOPGAME:
			game->killAll();
			menu->prepare(0);
			setFocus(dynamic_pointer_cast<IComponent>(menu));
			clear_keybuf();
			break;
		case E_LEVEL_INTRO:
			game->initLevel();
			{
				IContainerPtr intro = IContainerPtr(new IContainer());
				add(intro);
				intro->buildComponent().clearScreen(BLACK);
				intro->buildComponent()
					.textf("LEVEL %i", state.getSelectedLevel() + 1)
					.xy(SCREEN_W / 2, 50);
				intro->buildComponent()
					.textf("LIFE %02i", game->getLives())
					.xy(SCREEN_W / 2, 130);
				intro->buildComponent()
					.text("GET READY!")
					.center();
				intro->setTimer (50, MSG_KILL);
				setFocus(intro);

				setTimer(50, Engine::E_STARTLEVEL);
			}
			clear_keybuf();
			break;
		case E_PASS_INVALID:
			{
				IContainerPtr intro = IContainerPtr(new IContainer());
				add(intro);
				intro->buildComponent().clearScreen(RED);
				intro->buildComponent()
					.text("INVALID PASS CODE!")
					.center();
				intro->setTimer (50, MSG_KILL);
				setFocus(intro);

				setTimer(50, Engine::E_MAINMENU);
			}
			clear_keybuf();
			break;
		case E_LEVEL_CLEAR:
			{
				state.setLevelCleared();

				if (state.allClear())
				{
					CutScene *cutscene = new CutScene(E_FINISHED);
					IComponentPtr ptr = IComponentPtr(cutscene);
					add (ptr);
					cutscene->setFont(resources->getAlfont("megaman_2", 16));

					initEndSequence (cutscene);
					setFocus(ptr);
				}
				else
				{
					IContainerPtr intro = IContainerPtr(new IContainer());
					add(intro);
					intro->buildComponent().clearScreen(BLACK);
					intro->buildComponent()
						.text("LEVEL CLEAR!")
						.center();
					intro->buildComponent()
						.text("NEW WEAPON\n\nUse DOWN button to change")
						.xy(SCREEN_W / 2, SCREEN_H * 3 / 4);
					intro->setTimer (100, MSG_KILL);
					setFocus(intro);

					menu->prepare(2);
					setTimer(100, E_CHOOSELEVEL);
				}
			}
			clear_keybuf();
			break;
		case E_GAME_OVER:
			game->killAll();
			{
				IContainerPtr intro = IContainerPtr(new IContainer());
				add(intro);
				intro->buildComponent().clearScreen(BLACK);
				intro->buildComponent()
					.text("GAME OVER")
					.center();
				int passcode = state.calculateCode();
				intro->buildComponent()
					.textf("PASSCODE: %i", passcode)
					.xy(SCREEN_W / 2, 50);
				intro->setTimer (200, MSG_KILL);
				setFocus(intro);

				setTimer(200, E_MAINMENU);
			}
			clear_keybuf();
			break;
	}
	// clear event...
	event = E_NONE;
}
