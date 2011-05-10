#pragma once

#include <unordered_map>
#include <OIS.h>

typedef std::unordered_map<OIS::KeyCode, bool> KeyBoolMap;

struct gameColor
{
	int r;
	int g;
	int b;
};

class GameKeyboard
{
public:
	//Functions
	GameKeyboard();
	~GameKeyboard();
	void init(OIS::KeyCode* kArr, int len);
	void resetColor();
	void setKeyboard(OIS::Keyboard *m_KB);
	bool pressKey(OIS::KeyCode kc);
	void releaseKey(OIS::KeyCode kc);
	bool handleColorInput();

	//Variables
	KeyBoolMap KBM;
	gameColor gc;
	OIS::Keyboard *m_Keyboard;

private:
	//Functions
	bool checkColorValue();
	bool checkIfZero();
	void addColorValue(int newInput);
	void checkReleaseKeys();

	//Variables
	int cBool;
};