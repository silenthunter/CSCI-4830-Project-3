#include "GameKeyboard.h"

using OIS::KeyCode;
using OIS::Keyboard;

//Constructor
GameKeyboard::GameKeyboard()
{
	cBool = -1;
	resetColor();
}

//Destructor
GameKeyboard::~GameKeyboard()
{

}

//Initialise by setting up the unordered_map with keycodes
void GameKeyboard::init(OIS::KeyCode* kArr, int len)
{
	for(int i = 0; i < len; ++i)
	{
		KBM.insert(KeyBoolMap::value_type(kArr[i], false));
	}
}

//Set the keyboard
void GameKeyboard::setKeyboard(OIS::Keyboard *m_KB)
{
	m_Keyboard = m_KB;
}

//Check whether or not the state of the key is pressed. If so change it
void GameKeyboard::pressKey(OIS::KeyCode kc)
{
	KBM[kc] = true;
}

//Check whether or not the state of the key is released. If so change it
bool GameKeyboard::releaseKey(OIS::KeyCode kc)
{
	if(KBM[kc] == true)
	{
		KBM[kc] = false;
		return true;
	}
	return false;
}

bool GameKeyboard::handleColorInput()
{
	KeyCode kc = OIS::KC_R;
	if(m_Keyboard->isKeyDown(OIS::KC_R))
	{
		if(releaseKey(kc) == false) return false;
		pressKey(kc);
		cBool = 0;
		gc.r = 0;
	}
	else if(m_Keyboard->isKeyDown(OIS::KC_G))
	{
		kc = OIS::KC_G;
		if(releaseKey(kc) == false) return false;
		pressKey(kc);
		cBool = 1;
		gc.g = 0;
	}
	else if(m_Keyboard->isKeyDown(OIS::KC_B))
	{
		kc = OIS::KC_B;
		if(releaseKey(kc) == false) return false;
		pressKey(kc);
		cBool = 2;
		gc.b = 0;
	}
	else
	{
		if(m_Keyboard->isKeyDown(OIS::KC_0))
		{
			kc = OIS::KC_0;
			if(checkIfZero() == true) return false;
			if(releaseKey(kc) == false) return false;
			pressKey(kc);
			addColorValue(0);
		}
		else if(m_Keyboard->isKeyDown(OIS::KC_1))
		{
			kc = OIS::KC_1;
			if(releaseKey(kc) == false) return false;
			pressKey(kc);
			addColorValue(1);
		}
		else if(m_Keyboard->isKeyDown(OIS::KC_2))
		{
			kc = OIS::KC_2;
			if(releaseKey(kc) == false) return false;
			pressKey(kc);
			addColorValue(2);
		}
		else if(m_Keyboard->isKeyDown(OIS::KC_3))
		{
			kc = OIS::KC_3;
			if(releaseKey(kc) == false) return false;
			pressKey(kc);
			addColorValue(3);
		}
		else if(m_Keyboard->isKeyDown(OIS::KC_4))
		{
			kc = OIS::KC_4;
			if(releaseKey(kc) == false) return false;
			pressKey(kc);
			addColorValue(4);
		}
		else if(m_Keyboard->isKeyDown(OIS::KC_5))
		{
			kc = OIS::KC_5;
			if(releaseKey(kc) == false) return false;
			pressKey(kc);
			addColorValue(5);
		}
		else if(m_Keyboard->isKeyDown(OIS::KC_6))
		{
			kc = OIS::KC_6;
			if(releaseKey(kc) == false) return false;
			pressKey(kc);
			addColorValue(6);
		}
		else if(m_Keyboard->isKeyDown(OIS::KC_7))
		{
			kc = OIS::KC_7;
			if(releaseKey(kc) == false) return false;
			pressKey(kc);
			addColorValue(7);
		}
		else if(m_Keyboard->isKeyDown(OIS::KC_8))
		{
			kc = OIS::KC_8;
			if(releaseKey(kc) == false) return false;
			pressKey(kc);
			addColorValue(8);
		}
		else if(m_Keyboard->isKeyDown(OIS::KC_9))
		{
			kc = OIS::KC_9;
			if(releaseKey(kc) == false) return false;
			pressKey(kc);
			addColorValue(9);
		}
		else if(m_Keyboard->isKeyDown(OIS::KC_RETURN))
		{
			kc = OIS::KC_RETURN;
			if(checkColorValue() == false) return false;
			return true;
		}
		
		checkColorValue();
		return false;
	}
}

void GameKeyboard::resetColor()
{
	gc.r = 0;
	gc.g = 0;
	gc.b = 0;
}

bool GameKeyboard::checkColorValue()
{
	if(cBool == 0)
	{
		if(gc.r > 255)
		{
			gc.r = 0;
			return false;
		}
	}
	else if(cBool == 1)
	{
		if(gc.g > 255)
		{
			gc.g = 0;
			return false;
		}
	}
	else if(cBool == 2)
	{
		if(gc.b > 255)
		{
			gc.b = 0;
			return false;
		}
	}
	return true;
}

bool GameKeyboard::checkIfZero()
{
	if(cBool == 0)
	{
		if(gc.r == 0)
		{
			return true;
		}
	}
	else if(cBool == 1)
	{
		if(gc.g == 0)
		{
			return true;
		}
	}
	else if(cBool == 2)
	{
		if(gc.b == 0)
		{
			return true;
		}
	}
	return false;
}

void GameKeyboard::addColorValue(int newInput)
{
	if(cBool == 0)
	{
		if(gc.r == 0)
		{
			gc.r *= 10;
			gc.r += newInput;
		}
	}
	else if(cBool == 1)
	{
		if(gc.g == 0)
		{
			gc.g *= 10;
			gc.g += newInput;
		}
	}
	else if(cBool == 2)
	{
		if(gc.b == 0)
		{
			gc.b *= 10;
			gc.b += newInput;
		}
	}
}