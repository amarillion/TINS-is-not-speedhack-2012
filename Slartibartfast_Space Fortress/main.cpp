#include "Game.h"
int main(int iArgc, char **ppszArgv)
{
	Game gGame;
	int iLevel = 1;

	if (2 == iArgc)
	{
		iLevel = atoi(ppszArgv[1]);
	}

	gGame.Start(iLevel);

	return 0;
}