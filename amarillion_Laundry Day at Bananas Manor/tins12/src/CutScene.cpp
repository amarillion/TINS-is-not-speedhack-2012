/*
 * CutScene.cpp
 *
 *  Created on: 5 Aug 2012
 *      Author: martijn
 */

#include "CutScene.h"
#include "color.h"
#include <allegro.h>
#include "anim.h"

CutScene::CutScene(int exitCode) : exitCode(exitCode)
{
}

void CutScene::addFrame (IComponentPtr comp)
{
	sequence.push_back (comp);
	comp->setFont(sfont);
	comp->setViewport(viewPort);
}

IContainerPtr CutScene::newPage ()
{
	IContainerPtr scene = IContainerPtr(new IContainer());
	addFrame(scene);
	scene->buildComponent().clearScreen(BLACK);
	return scene;
}


void CutScene::handleMessage(int event)
{
	IComponent::handleMessage(event);
	if (event == MSG_FOCUS)
	{
		current = sequence.begin();
	}
}

void CutScene::draw(const GraphicsContext &gc)
{
	if (current != sequence.end())
		(*current)->draw(gc);
}

int CutScene::update()
{
	IComponent::update();
	int result = -1;
	if (current != sequence.end())
		result = (*current)->update();

	if (result != 0)
	{
		if (current == sequence.end())
		{
			return exitCode;
		}
		else
			current++;
	}
	return 0;
}


AnimComponent::AnimComponent (Anim *anim) : anim(anim), state(0), dir(0) {}

void AnimComponent::draw(const GraphicsContext &gc)
{
	anim->drawFrame(gc.buffer, state, dir, (counter * 20), getx(), gety());
}

int AnimComponent::update()
{
	IComponent::update();
	return 0;
}
