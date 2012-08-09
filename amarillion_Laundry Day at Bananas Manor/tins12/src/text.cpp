#include "text.h"
#include <string>
#include <sstream>
#include "color.h"

#ifdef USE_ALFONT
#include <alfont.h>
#endif

using namespace std;

void Text::split(const string &s)
{
	stringstream ss(s);
	string item;
	while(std::getline(ss, item, '\n')) {
		elems.push_back(item);
	}
}

void Text::draw(const GraphicsContext &gc)
{
	if (!parent) return;

	IMotionPtr motion;
	motion = parent->getMotion();
	int counter = parent->getCounter();
	int xofst = motion ? motion->getdx(counter) : 0;
	int yofst = motion ? motion->getdy(counter) : 0;

#ifdef USE_ALFONT
	ALFONT_FONT *sfont = parent->getFont();
#else
		FONT *sfont = parent->getFont();
#endif
	if (!sfont) return;

	int yco = parent->gety();
	int lineh = alfont_text_height(sfont);
	int remain = showlines;
	for (vector<string>::iterator i = elems.begin(); i != elems.end() && remain > 0; ++i, remain--)
	{
#ifdef USE_ALFONT
		alfont_textout_centre_aa_ex (gc.buffer, sfont, (*i).c_str(), parent->getx() + xofst, yco + yofst, WHITE, -1);
#else
		textout_centre_ex (gc.buffer, sfont,  s.c_str(), gc.buffer->w / 2,
			hinty, WHITE, -1);
#endif
		yco += lineh;
	}

}
