#pragma once

#include "Interface.h"

#include "Task.h"
#include "EventRecorder.h"
#include "BuildOrder.h"
#include "GA/GA.h"

class Cromartie : public BWAPI::AIModule
{
public:
	Cromartie();
	~Cromartie(void);

	virtual void onStart();
	virtual void onEnd(bool isWinner);

	virtual void onFrame();

	virtual void onSendText(std::string text);
	virtual void onPlayerLeft(Player player);
	virtual void registerListeners();

private:
	bool mOnBegin;
	GA _ga;
	BuildOrder loadedBO; 
	int mLeavingGame;
	bool mSaidGG;
	EventRecorder* eventRecorderListener;
};