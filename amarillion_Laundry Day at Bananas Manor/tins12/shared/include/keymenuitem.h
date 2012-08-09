#ifndef KEYMENUITEM_H_
#define KEYMENUITEM_H_

#include "menubase.h"
#include "button.h"

class KeyMenuItem : public MenuItem
{
	private:
		std::string btnName;
		const char* btnConfigName;
		Button &btn;
		bool waitForKey;
	public:
		KeyMenuItem (MenuBase *parent, std::string _btnName, const char* _btnConfigName, Button & _btn) :
			MenuItem (parent),
			btnName (_btnName),
			btnConfigName(_btnConfigName), btn(_btn), waitForKey (false) {}
		virtual int handleInput();
		virtual std::string getText();
		virtual std::string getHint();
};

#endif /* KEYMENUITEM_H_ */
