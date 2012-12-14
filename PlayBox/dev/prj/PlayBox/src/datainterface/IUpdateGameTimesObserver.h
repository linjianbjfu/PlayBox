#ifndef _UPDATE_GAME_TIMES_OBSERVER_H
#define _UPDATE_GAME_TIMES_OBSERVER_H

#include "IMessageManager.h"
#include "../src/module/PlayedGamePanel/LMCStructs.h"

class IUpdateGameTimesObserver : public IMessageObserver
{
public:
	virtual void UpdateGameTimes ( string& strGameID, GameType type) = 0;
};

#endif