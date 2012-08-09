#ifndef RESOURCES_IMPL_H
#define RESOURCES_IMPL_H

#include "resources.h"

#ifdef USE_ALFONT
// wrapper class for management of ALFONT memory
class AlFont
{
private:
	ALFONT_FONT* font;
	AlFont(ALFONT_FONT *_font) : font (_font) {}
public:
	~AlFont() { alfont_destroy_font (font); }
	
	static AlFont *load (const char *filename)
	{
		return new AlFont (alfont_load_font(filename));
	}
	
	void setFontSize (int size)
	{
		alfont_set_font_size (font, size);
	}
	
	ALFONT_FONT *get() { return font; }
};

#endif

class ResourcesImpl
{
	char errorMsg[1024];
	std::vector<DATAFILE*> data;
	std::map <std::string, RLE_SPRITE*> sprites; // seperately loaded bitmaps	
	std::map <std::string, Anim*> animlist;
#ifdef USE_TEGEL
	std::vector<TEG_MAP*> maps; // seperately loaded maps
	std::map <std::string, TEG_MAP*> maplist;	
#endif
	std::vector <RLE_SPRITE*> generated; // run-time generated RLE's
	std::map <std::string, SAMPLE *> samples;
#ifdef USE_ALFONT
	std::map <std::string, boost::shared_ptr<AlFont> > alfonts;
#endif
#ifdef USE_DUMB
	std::map <std::string, DUH*> duhlist;
#endif
#ifdef USE_LUA
	lua_State *L;
#endif

	Resources &parent;
public:
	ResourcesImpl(Resources &_parent) : parent(_parent) {};
	~ResourcesImpl();
	
	int addFiles(const char *pattern);
	
	Anim *getAnim (const std::string &id);
	void putAnim (const std::string &id, Anim *val);
	const std::map<std::string, Anim*> &getAnims() const;
	
	FONT *getFont (const std::string &id);
	BITMAP *getBitmap (const std::string &id);
	RLE_SPRITE *getRle (const std::string &id);
	void putRle(const std::string &id, RLE_SPRITE *rle);
	SAMPLE *getSample (const std::string &id);
	const char *getErrorMsg() const;
#ifdef USE_ALFONT
	ALFONT_FONT *getAlfont (const std::string &id, int size);
#endif
#ifdef USE_TEGEL
	int addMapFile(const std::string &id, const std::string &filename, const std::string &tilelist);
	int addMapFiles(const std::string &pattern, const std::string &tilelist);
	TEG_MAP *getMap (const std::string &id);
	TEG_TILELIST *getTilelist (const std::string &id);
#endif
#ifdef USE_DUMB
	DUH *getDuh (const std::string &id);
#endif
#ifdef USE_ALSPC
	ALSPC_DATA *getSPC (const std::string &id);
#endif
#ifdef USE_LUA
	void setLuaState (lua_State *val);
#endif
};

#endif
