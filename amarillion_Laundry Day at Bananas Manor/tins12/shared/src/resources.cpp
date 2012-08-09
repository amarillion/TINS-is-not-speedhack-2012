#include <assert.h>
#include "resources.h"
#include "resourcesimpl.h"
#include "color.h"
#include "dom.h"
#include <boost/algorithm/string.hpp>

#ifdef USE_OGG
#include <logg.h>
#endif

#ifdef USE_LUA
extern "C" {
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}
#endif

using namespace std;
using namespace xdom;
using namespace boost;

Resources::Resources() : pImpl (new ResourcesImpl(*this)) {}

// adds all filenames that match pattern to the vector
void glob(const char *pattern, vector<string> &result)
{
	char buf[1024];
	al_ffblk info;
	int temp = al_findfirst (pattern, &info, FA_RDONLY | FA_ARCH);
	
	while (temp == 0)
	{
		replace_filename(buf, pattern, info.name, sizeof(buf));
		result.push_back (std::string(buf));
		temp = al_findnext(&info);
	}
			
	al_findclose(&info);
}

int Resources::addFiles(const char *pattern)
{
	return pImpl->addFiles(pattern);
}

int ResourcesImpl::addFiles(const char *pattern)
{
	vector<string> filenames;
	glob (pattern, filenames);
	for (vector<string>::iterator i = filenames.begin();
			i != filenames.end(); ++i)
	{		
		unsigned int start = i->find_last_of ("\\/");
		if (start == string::npos) start = 0;
		else start += 1;
			
		unsigned int end = i->find_last_of ('.');
		// must have an extension, to be able to recognize file type.
		//TODO: error message?
		if (end == string::npos) return 0;
		string extension = i->substr (end + 1);
		string basename = i->substr(start, end-start);
		
		if (boost::iequals (extension, "dat"))
		{
#ifdef USE_TEGEL
			DATAFILE *temp;
			if (!(temp = teg_load_datafile (i->c_str())))
			{
				snprintf(errorMsg, sizeof(errorMsg), "error teg_load_datafile %s", i->c_str());
				return 0;
			}
			else
				data.push_back (temp);
#else
			DATAFILE *temp;
			if (!(temp = load_datafile (i->c_str())))
			{
				snprintf(errorMsg, sizeof(errorMsg), "error load_datafile %s", i->c_str());
				return 0;
			}
			else
				data.push_back (temp);
#endif
		}
#ifdef USE_ALFONT
		else if (boost::iequals (extension, "ttf"))
		{			
			shared_ptr<AlFont> temp;
			// generate fonts with sizes 8, 16, 24, etc. to 48
			for (int k = 8; k < 48; k += 8)
			{
				shared_ptr<AlFont> temp;
				if (!(temp = shared_ptr<AlFont>(AlFont::load(i->c_str()))))
				{
					snprintf(errorMsg, sizeof(errorMsg), "error load AlFont %s", i->c_str());
					return 0;
				} else {
					char namebuf[128];
					snprintf(namebuf, sizeof(namebuf), "%s_%i", basename.c_str(), k);
					string name = string(namebuf);
					temp->setFontSize(k);
					alfonts.insert (pair<string, shared_ptr<AlFont> >(name, temp));
				}
			}
		}
#endif
#ifdef USE_DUMB
		else if (boost::iequals (extension, "mod"))
		{
			DUH *temp;
			if (!(temp = dumb_load_mod (i->c_str())))
			{
				snprintf(errorMsg, sizeof(errorMsg), "error loading DUMB %s", i->c_str());
				return 0;
			}
			else
				duhlist.insert (pair<string, DUH*>(basename, temp));
		}
#endif
#ifdef USE_OGG
		else if (boost::iequals (extension, "ogg"))
		{
			SAMPLE *s;
			s = logg_load(i->c_str());
			if (!s)
			{
				snprintf(errorMsg, sizeof(errorMsg), "error loading OGG %s", i->c_str());
				return 0;
			}
			else
				samples.insert (pair<string, SAMPLE*>(basename, s));
		}
#endif
#ifdef USE_LUA
		else if (boost::iequals (extension, "lua"))
		{
			assert (L != NULL);
			int result = luaL_loadfile (L, i->c_str());
			if (result == LUA_ERRFILE)
			{
				snprintf(errorMsg, sizeof(errorMsg), "error reading lua script %s", i->c_str());
			}
		}
#endif
		else if (boost::iequals (extension, "bmp"))
		{
			BITMAP *bmp;
			bmp = load_bitmap (i->c_str(), NULL);
			if (!bmp)
			{
				snprintf(errorMsg, sizeof(errorMsg), "error loading BMP %s", i->c_str());
				return 0;
			}
			else
			{
				sprites.insert (pair<string, RLE_SPRITE*>(basename, get_rle_sprite(bmp)));
				destroy_bitmap (bmp);
			}
		}
		else if (boost::iequals (extension, "xml"))
		{
			DomNode node = xmlParseFile(*i);
			
			vector<DomNode>::iterator n;
			for (n = node.children.begin(); n != node.children.end(); ++n)
			{
				if (n->name == "anim")
					Anim::loadFromXml (*n, &parent, animlist);
			}
		}
	}
	return 1;
}

ResourcesImpl::~ResourcesImpl()
{
	{
		map<string, RLE_SPRITE*>::iterator i;
		for (i = sprites.begin(); i != sprites.end(); i++)
		{
			destroy_rle_sprite (i->second);
		}
	}
	{
		map<string, Anim*>::iterator i;
		for (i = animlist.begin(); i != animlist.end(); i++)
		{
			delete (i->second);
		}
	}
	{
		vector<DATAFILE*>::iterator i;
		for (i = data.begin(); i != data.end(); ++i)
		{
#ifdef USE_TEGEL
			teg_unload_datafile (*i);
#else
			unload_datafile (*i);
#endif
			*i = NULL;
		}
#ifdef USE_DUMB
		{
			map<string, DUH*>::iterator i;
			for (i = duhlist.begin(); i != duhlist.end(); i++)
			{
				unload_duh (i->second);
			}
		}
#endif
#ifdef USE_TEGEL
		{
			vector<TEG_MAP*>::iterator i;
			for (i = maps.begin(); i != maps.end(); i++)
			{
				teg_destroymap (*i);
			}
		}
#endif
#ifdef USE_OGG
		{
			map<string, SAMPLE *>::iterator i;
			for (i = samples.begin(); i != samples.end(); i++)
			{
				destroy_sample (i->second);
			}
		}
#endif
	}	
}

BITMAP *Resources::getBitmap (const string &id)
{
	return pImpl->getBitmap (id);
}

BITMAP *ResourcesImpl::getBitmap (const string &id)
{
	vector<DATAFILE*>::const_iterator i;
	DATAFILE *temp = NULL;
	for (i = data.begin(); i != data.end() && temp == NULL; ++i)
	{
			assert (*i);
			temp = find_datafile_object (*i, id.c_str());				
			// check if object is of the right type
			if (temp && temp->type != DAT_BITMAP)
					temp = NULL;
	}
	if (temp)
		return (BITMAP*)temp->dat;
	else
	{
		allegro_message ("Couldn't find bitmap '%s'", id.c_str());			
		exit(1);
	}
	return NULL;
}

FONT *Resources::getFont (const string &id)
{
	return pImpl->getFont(id);
}

FONT *ResourcesImpl::getFont (const string &id)
{
	vector<DATAFILE*>::const_iterator i;
	DATAFILE *temp = NULL;
	for (i = data.begin(); i != data.end() && temp == NULL; ++i)
	{
			assert (*i);
			temp = find_datafile_object (*i, id.c_str());				
			// check if object is of the right type
			if (temp && temp->type != DAT_FONT)
					temp = NULL;
	}
	if (temp)
		return (FONT*)temp->dat;
	else
	{
		allegro_message ("Couldn't find FONT '%s'", id.c_str());			
		exit(1);
	}
	return NULL;	
}

RLE_SPRITE *Resources::getRle (const string &id)
{
	return pImpl->getRle (id);
}

RLE_SPRITE *ResourcesImpl::getRle (const string &id)
{
	vector<DATAFILE*>::const_iterator i;
	DATAFILE *temp = NULL;
	for (i = data.begin(); i != data.end() && temp == NULL; ++i)
	{
			assert (*i);
			temp = find_datafile_object (*i, id.c_str());				
			// check if object is of the right type
			if (temp && temp->type != DAT_RLE_SPRITE)
					temp = NULL;
	}
	
	if (temp)
			return (RLE_SPRITE*)temp->dat;
	else
	{
		if (sprites.find (id) == sprites.end())
		{
			allegro_message ("Couldn't find RLE sprite '%s'", id.c_str());			
			exit(1);
		}
		
		return sprites[id];
	}
	
	assert (false);
	return NULL;
}

SAMPLE *Resources::getSample (const string &id)
{
	return pImpl->getSample(id);
}

SAMPLE *ResourcesImpl::getSample (const string &id)
{
	vector<DATAFILE*>::const_iterator i;
	DATAFILE *temp = NULL;
	for (i = data.begin(); i != data.end() && temp == NULL; ++i)
	{
			assert (*i);
			temp = find_datafile_object (*i, id.c_str());				
			// check if object is of the right type
			if (temp && temp->type != DAT_SAMPLE)
					temp = NULL;
	}
	if (temp)
			return (SAMPLE*)temp->dat;
	else
	{
		if (samples.find (id) == samples.end())
		{
			allegro_message ("Couldn't find RLE sprite '%s'", id.c_str());
			exit(1);
		}

		return samples[id];
	}		
	return NULL;
}
#ifdef USE_ALSPC

ALSPC_DATA *Resources::getSPC (const string &id)
{
	return pImpl->getSPC(id);
}

ALSPC_DATA *ResourcesImpl::getSPC (const string &id)
{
	vector<DATAFILE*>::const_iterator i;
	DATAFILE *temp = NULL;
	for (i = data.begin(); i != data.end() && temp == NULL; ++i)
	{
		assert (*i);
		temp = find_datafile_object (*i, id.c_str());				
		// check if object is of the right type
		if (temp && temp->type != DAT_ALSPC)
			temp = NULL;
	}
    if (temp)
    	return (ALSPC_DATA*)temp->dat;    
	else
	{
		allegro_message ("Couldn't find ALSPC '%s'", id.c_str());			
		exit(1);
	}		
	return NULL;
}

#endif
#ifdef USE_ALFONT
ALFONT_FONT *Resources::getAlfont (const string &id, int size)
{
	return pImpl->getAlfont (id, size);
}

ALFONT_FONT *ResourcesImpl::getAlfont (const string &id, int size)
{
	char buf[128];
	snprintf (buf, sizeof(buf), "%s_%i", id.c_str(), size);
	string key = string(buf);
	if (alfonts.find (key) == alfonts.end())
	{
		allegro_message ("Couldn't find ttf font '%s' with size %i", id.c_str(), size);
		exit(1);
	}
	else
	{
		return alfonts[key].get()->get();
	}
}
#endif

Anim *Resources::getAnim (const string &id)
{
	return pImpl->getAnim(id);
}

Anim *ResourcesImpl::getAnim (const string &id)
{
	if (animlist.find (id) == animlist.end())
	{
		allegro_message ("Couldn't find Animation '%s'", id.c_str());			
		exit(1);
	}
	else
	{
		return animlist[id];
	}
}

#ifdef USE_DUMB
DUH *Resources::getDuh (const string &id)
{
	return pImpl->getDuh(id);
}

DUH *ResourcesImpl::getDuh (const string &id)
{
    if (duhlist.find (id) == duhlist.end())
    {
		allegro_message ("Couldn't find DUH (Music) '%s'", id.c_str());	
		exit(1);
    }
    return duhlist[id];
}
#endif

#ifdef USE_TEGEL
TEG_MAP *Resources::getMap (const string &id)
{
	return pImpl->getMap (id);
}

TEG_MAP *ResourcesImpl::getMap (const string &id)
{
	assert (maplist.find (id) != maplist.end());
	return maplist[id];
}

TEG_TILELIST *Resources::getTilelist (const string &id)
{
	return pImpl->getTilelist (id);
}

TEG_TILELIST *ResourcesImpl::getTilelist (const string &id)
{
	vector<DATAFILE*>::iterator i;
	DATAFILE *temp = NULL;
	for (i = data.begin(); i != data.end() && temp == NULL; ++i)
	{
			assert (*i);
			temp = find_datafile_object (*i, id.c_str());				
			// check if object is of the right type
			if (temp && temp->type != DAT_TEG_TILELIST)
					temp = NULL;
	}
	if (temp)
			return (TEG_TILELIST*)temp->dat;
	else
	{
		allegro_message ("Couldn't find TILELIST '%s'", id.c_str());	
		exit(1);
	}
}

int Resources::addMapFile(const string &id, const string &filename, const string &tilesname)
{
	return pImpl->addMapFile (id, filename, tilesname);
}

int ResourcesImpl::addMapFile(const string &id, const string &filename, const string &tilesname)
{
	TEG_MAP *tempmap;
	TEG_TILELIST *temptiles = getTilelist (tilesname);
	if (!temptiles)
	{
		snprintf(errorMsg, sizeof(errorMsg), "Could not find tiles named [%s]", tilesname.c_str());
	}

	if (!(tempmap = teg_loadmap (filename.c_str(), temptiles)))
	{
		snprintf(errorMsg, sizeof(errorMsg), "Error while loading map: [%s]", teg_error);
		return 0;
	}
	else 
	{
		maps.push_back (tempmap);
		maplist.insert (pair<string, TEG_MAP*>(id, tempmap));
	}
	return 1;
}

int Resources::addMapFiles(const string &pattern, const string &tilesname)
{
	return pImpl->addMapFiles (pattern, tilesname);
}

int ResourcesImpl::addMapFiles(const string &pattern, const string &tilesname)
{
	vector <string> mapfilenames;	
	glob (pattern.c_str(), mapfilenames);
	
	for (vector<string>::iterator i = mapfilenames.begin();
		i != mapfilenames.end(); ++i)
	{
		unsigned int start = i->find_last_of ("\\/");
		if (start == string::npos) start = 0;
		else start += 1;
			
		unsigned int end = i->rfind (".");
		if (end == string::npos) end = i->size();
		else end -= 1;
			
		TEG_MAP *tempmap;
		if (!(tempmap = teg_loadmap (i->c_str(), getTilelist (tilesname))))
		{
			return 0;
		} else {
			maps.push_back (tempmap);
			string name = i->substr(start, end-start+1);
			maplist.insert (pair<string, TEG_MAP*>(name, tempmap));
		}
	}
	return 1;
}
#endif

const map<string, Anim*> &Resources::getAnims() const
{
	return pImpl->getAnims();
}

const map<string, Anim*> &ResourcesImpl::getAnims() const
{
	return animlist;
}

void Resources::putRle(const string &id, RLE_SPRITE *rle)
{
	pImpl->putRle (id, rle);
}

void ResourcesImpl::putRle(const string &id, RLE_SPRITE *rle)
{
	sprites.insert (pair<string, RLE_SPRITE *>(id, rle));
}

void Resources::putAnim (const string &id, Anim *val)
{
	pImpl->putAnim (id, val);
}

void ResourcesImpl::putAnim (const string &id, Anim *val)
{
	animlist.insert (pair<string, Anim*>(id, val));
}

const char *Resources::getErrorMsg() const
{
	return pImpl->getErrorMsg();
}

const char *ResourcesImpl::getErrorMsg() const
{
	return errorMsg;
}

#ifdef USE_LUA
void Resources::setLuaState (lua_State *val)
{
	pImpl->setLuaState (val);
}

void ResourcesImpl::setLuaState (lua_State *val)
{
	L = val;
}

#endif
