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
bool GameKeyboard::pressKey(OIS::KeyCode kc)
{
	if(!KBM[kc])
	{
		KBM[kc] = true;
		return true;
	}
	return false;
}

//Check whether or not the state of the key is released. If so change it
void GameKeyboard::releaseKey(OIS::KeyCode kc)
{
	if(!(m_Keyboard->isKeyDown(kc)))
	{
		KBM[kc] = false;
	}
}

bool GameKeyboard::handleColorInput()
{
	checkReleaseKeys();

	KeyCode kc = OIS::KC_R;
	if(m_Keyboard->isKeyDown(OIS::KC_R))
	{
		if(pressKey(kc) == false) return false;
		cBool = 0;
		gc.r = -1;
	}
	else if(m_Keyboard->isKeyDown(OIS::KC_G))
	{
		kc = OIS::KC_G;
		if(pressKey(kc) == false) return false;
		cBool = 1;
		gc.g = -1;
	}
	else if(m_Keyboard->isKeyDown(OIS::KC_B))
	{
		kc = OIS::KC_B;
		if(pressKey(kc) == false) return false;
		cBool = 2;
		gc.b = -1;
	}
	else
	{
		if(m_Keyboard->isKeyDown(OIS::KC_0))
		{
			kc = OIS::KC_0;
			if(checkIfZero() == true) return false;
			if(pressKey(kc) == false) return false;
			addColorValue(0);
		}
		else if(m_Keyboard->isKeyDown(OIS::KC_1))
		{
			kc = OIS::KC_1;
			if(pressKey(kc) == false) return false;
			addColorValue(1);
		}
		else if(m_Keyboard->isKeyDown(OIS::KC_2))
		{
			kc = OIS::KC_2;
			if(pressKey(kc) == false) return false;
			addColorValue(2);
		}
		else if(m_Keyboard->isKeyDown(OIS::KC_3))
		{
			kc = OIS::KC_3;
			if(pressKey(kc) == false) return false;
			addColorValue(3);
		}
		else if(m_Keyboard->isKeyDown(OIS::KC_4))
		{
			kc = OIS::KC_4;
			if(pressKey(kc) == false) return false;
			addColorValue(4);
		}
		else if(m_Keyboard->isKeyDown(OIS::KC_5))
		{
			kc = OIS::KC_5;
			if(pressKey(kc) == false) return false;
			addColorValue(5);
		}
		else if(m_Keyboard->isKeyDown(OIS::KC_6))
		{
			kc = OIS::KC_6;
			if(pressKey(kc) == false) return false;
			addColorValue(6);
		}
		else if(m_Keyboard->isKeyDown(OIS::KC_7))
		{
			kc = OIS::KC_7;
			if(pressKey(kc) == false) return false;
			addColorValue(7);
		}
		else if(m_Keyboard->isKeyDown(OIS::KC_8))
		{
			kc = OIS::KC_8;
			if(pressKey(kc) == false) return false;
			addColorValue(8);
		}
		else if(m_Keyboard->isKeyDown(OIS::KC_9))
		{
			kc = OIS::KC_9;
			if(pressKey(kc) == false) return false;
			addColorValue(9);
		}
		else if(m_Keyboard->isKeyDown(OIS::KC_LSHIFT))
		{
			kc = OIS::KC_LSHIFT;
			if(pressKey(kc) == false) return false;
			if(checkColorValue() == false) return false;
			if(gc.r == -1) gc.r = 0;
			if(gc.g == -1) gc.g = 0;
			if(gc.b == -1) gc.b = 0;
			return true;
		}
		
		checkColorValue();
		return false;
	}
}

void GameKeyboard::resetColor()
{
	gc.r = -1;
	gc.g = -1;
	gc.b = -1;
}

bool GameKeyboard::checkColorValue()
{
	if(cBool == 0)
	{
		if(gc.r > 255 || gc.r == -1)
		{
			gc.r = -1;
			return false;
		}
	}
	else if(cBool == 1)
	{
		if(gc.g > 255 || gc.g == -1)
		{
			gc.g = -1;
			return false;
		}
	}
	else if(cBool == 2)
	{
		if(gc.b > 255 || gc.b == -1)
		{
			gc.b = -1;
			return false;
		}
	}
	return true;
}

bool GameKeyboard::checkIfZero()
{
	if(cBool == 0)
	{
		if(gc.r <= 0)
		{
			return true;
		}
	}
	else if(cBool == 1)
	{
		if(gc.g <= 0)
		{
			return true;
		}
	}
	else if(cBool == 2)
	{
		if(gc.b <= 0)
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
		if(gc.r != 0)
		{
			if(gc.r == -1) gc.r = 0;
			gc.r *= 10;
			gc.r += newInput;
		}
	}
	else if(cBool == 1)
	{
		if(gc.g != 0)
		{
			if(gc.g == -1) gc.g = 0;
			gc.g *= 10;
			gc.g += newInput;
		}
	}
	else if(cBool == 2)
	{
		if(gc.b != 0)
		{
			if(gc.b == -1) gc.b = 0;
			gc.b *= 10;
			gc.b += newInput;
		}
	}
}

void GameKeyboard::checkReleaseKeys()
{
	KeyBoolMap::iterator iter;
	for(iter = KBM.begin(); iter != KBM.end(); ++iter)
	{
		releaseKey(iter->first);
	}
}