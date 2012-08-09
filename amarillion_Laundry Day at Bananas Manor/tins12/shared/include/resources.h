#include <allegro.h>
#include <string>
#include <map>
#include "anim.h"
#include <boost/shared_ptr.hpp>
#include <vector>

#ifdef USE_ALFONT
#include <alfont.h>
#endif

#ifdef USE_DUMB
#include <aldumb.h>
#endif

#ifdef USE_TEGEL
#include <tegel.h>
#endif

#ifdef USE_ALSPC
#include <alspc.h>
#endif

#ifdef USE_LUA
struct lua_State;
#endif

#ifndef RESOURCES_H
#define RESOURCES_H

class Anim;
class ResourcesImpl;

class Resources : public IRleProvider
{
private:
	boost::shared_ptr<ResourcesImpl> pImpl;
public:
	Resources();
	int addFiles(const char *pattern);

	void putRle(const std::string &id, RLE_SPRITE *rle);

	Anim *getAnim (const std::string &id);
	void putAnim (const std::string &id, Anim *val);
	const std::map<std::string, Anim*> &getAnims() const;
	const char *getErrorMsg() const;

	FONT *getFont (const std::string &id);
	BITMAP *getBitmap (const std::string &id);
	virtual RLE_SPRITE *getRle (const std::string &id);
	SAMPLE *getSample (const std::string &id);
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
	virtual ~Resources() {}
};

void glob(const char *pattern, std::vector<std::string> &result);

#endif
