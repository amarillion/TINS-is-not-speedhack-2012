#ifndef TEXT_H
#define TEXT_H

#include "component.h"
#include "DrawStrategy.h"
#include <vector>

class Text : public Vis
{
public:
	Text(const std::string &_s) : elems()
	{
		split (_s);
		showlines = elems.size();
		blink = false;
	}

	virtual void draw(const GraphicsContext &gc);
	int showlines;
	int lineh;
	bool blink;
	int size() { return elems.size(); }
private:
	std::vector<std::string> elems;
	void split (const std::string &s);
};


// for animated text
class TextComponent : public IComponent
{
public:
	TextComponent (const std::string &_s) {
		textPtr = boost::shared_ptr<Text>(new Text(_s));
		setVis (textPtr);
		textPtr->showlines = 1;
	}

	virtual int update()
	{
		IComponent::update();
		if (counter % 48 == 0)
		{
			textPtr->showlines++;
			if (textPtr->showlines > textPtr->size())
				return -1;
		}
		return 0;
	}
private:
	boost::shared_ptr<Text> textPtr;
};

typedef boost::shared_ptr<TextComponent> textComponentPtr;

#endif
