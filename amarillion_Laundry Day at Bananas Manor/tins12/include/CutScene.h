/*
 * CutScene.h
 *
 *  Created on: 5 Aug 2012
 *      Author: martijn
 */

#ifndef CUTSCENE_H_
#define CUTSCENE_H_

#include "component.h"
#include <vector>
#include "IContainer.h"

class CutScene : public IComponent
{
private:
	int exitCode;
	std::vector<IComponentPtr> sequence;
	std::vector<IComponentPtr>::iterator current;
	void addFrame (IComponentPtr comp);
public:
	IContainerPtr newPage ();
	CutScene(int exitCode);
	void handleMessage(int event);
	virtual void draw(const GraphicsContext &gc);
	virtual int update();
};

class Anim;

class AnimComponent : public IComponent
{
private:
	int exitCode;
	Anim *anim;
	int dir;
	int state;
public:
	AnimComponent (Anim *anim);
	virtual void draw(const GraphicsContext &gc);
	virtual int update();
};


#endif /* CUTSCENE_H_ */
