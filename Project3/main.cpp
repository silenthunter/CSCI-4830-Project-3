#include <Ogre.h>
#include <OIS.h>
#include "GraphicsManager.h"
#include "GameTimer.h"
#include "Painter.h"

#include "BtOgrePG.h"
#include "BtOgreGP.h"
#include "BtOgreExtras.h"

GraphicsManager graphicsManager;

bool colorInput(OIS::Keyboard *m_Keyboard, int *cValue, int cBool)
{
	if(m_Keyboard->isKeyDown(OIS::KC_R))
	{
		cBool = 0;
		(*cValue) = 0;
	}
	else if(m_Keyboard->isKeyDown(OIS::KC_G))
	{
		cBool = 1;
		(*cValue) = 0;
	}
	else if(m_Keyboard->isKeyDown(OIS::KC_B))
	{
		cBool = 2;
		(*cValue) = 0;
	}
	else
	{
		(*cValue) *= 10;
		if(m_Keyboard->isKeyDown(OIS::KC_1))
		{
			(*cValue) += 1;
		}
		else if(m_Keyboard->isKeyDown(OIS::KC_2))
		{
			(*cValue) += 2;
		}
		else if(m_Keyboard->isKeyDown(OIS::KC_3))
		{
			(*cValue) += 3;
		}
		else if(m_Keyboard->isKeyDown(OIS::KC_4))
		{
			(*cValue) += 4;
		}
		else if(m_Keyboard->isKeyDown(OIS::KC_5))
		{
			(*cValue) += 5;
		}
		else if(m_Keyboard->isKeyDown(OIS::KC_6))
		{
			(*cValue) += 6;	
		}
		else if(m_Keyboard->isKeyDown(OIS::KC_7))
		{
			(*cValue) += 7;
		}
		else if(m_Keyboard->isKeyDown(OIS::KC_8))
		{
			(*cValue) += 8;
		}
		else if(m_Keyboard->isKeyDown(OIS::KC_9))
		{
			(*cValue) += 9;
		}
		else if(m_Keyboard->isKeyDown(OIS::KC_RETURN))
		{
			if((*cValue) > 255)
			{
				(*cValue) = 0;
				return false;
			}
			return true;
		}
		else
		{
			(*cValue) = 0;
		}
	}
	if((*cValue) > 255) (*cValue) = 0;
	return false;
}

void main(int argc, char *argv[])
{
	graphicsManager.init();

	graphicsManager.loadCanvasObject("cube.mesh", "cube.obj", 2.5f);

	Painter paint;

	//Debug Drawer
	SceneNode* debugSN = graphicsManager.GetRootSceneNode()->createChildSceneNode("debugSN");
	BtOgre::DebugDrawer* dbgdraw = new BtOgre::DebugDrawer(debugSN, paint.getDynamicsWorld());
	debugSN->setPosition(0, 0, -20);
	paint.getDynamicsWorld()->setDebugDrawer(dbgdraw);

	graphicsManager.InitBrushFromPainter(paint);

	//Allow for keyboard control
	RenderWindow* ogreWindow = graphicsManager.GetWindow();
	size_t hWnd = 0;
	ogreWindow->getCustomAttribute("WINDOW", &hWnd);
	OIS::InputManager *m_InputManager = OIS::InputManager::createInputSystem(hWnd);
	OIS::Keyboard *m_Keyboard = static_cast<OIS::Keyboard*>(m_InputManager->createInputObject(OIS::OISKeyboard, false));
	char *keyStates = new char[512];

	graphicsManager.GetRootSceneNode()->getChild("ObjectScene")->yaw(Degree(-90));

#pragma region Main Loop
	//Main Loop
	GameTimer timer;
	const float speed = 5.f;
	btVector3 pos(0, 0, 0);

	int cValue = 0;
	int cBool = -1;

	while(1)
	{
		//graphicsManager.GetRootSceneNode()->getChild("ObjectScene")->yaw(Degree(3));
		//graphicsManager.GetRootSceneNode()->getChild("ObjectScene")->pitch(Degree(3));

		double elapsed = timer.getElapsedTimeSec();
		graphicsManager.RenderFrame(elapsed);
		Ogre::WindowEventUtilities::messagePump();
		dbgdraw->step();
		graphicsManager.applyPaint(paint);

		m_Keyboard->capture();

		if(m_Keyboard->isKeyDown(OIS::KC_W))
			pos.setY(pos.y() + speed * elapsed);
		if(m_Keyboard->isKeyDown(OIS::KC_S))
			pos.setY(pos.y() - speed * elapsed);
		if(m_Keyboard->isKeyDown(OIS::KC_A))
			pos.setX(pos.x() - speed * elapsed);
		if(m_Keyboard->isKeyDown(OIS::KC_D))
			pos.setX(pos.x() + speed * elapsed);
		if(m_Keyboard->isKeyDown(OIS::KC_Q))
			graphicsManager.GetRootSceneNode()->getChild("ObjectScene")->yaw(Degree(-3));
		if(m_Keyboard->isKeyDown(OIS::KC_E))
			graphicsManager.GetRootSceneNode()->getChild("ObjectScene")->yaw(Degree(3));
		if(m_Keyboard->isKeyDown(OIS::KC_Z))
			graphicsManager.GetRootSceneNode()->getChild("ObjectScene")->pitch(Degree(3));
		if(m_Keyboard->isKeyDown(OIS::KC_X))
			graphicsManager.GetRootSceneNode()->getChild("ObjectScene")->pitch(Degree(-3));
		if(m_Keyboard->isKeyDown(OIS::KC_R))
			graphicsManager.GetRootSceneNode()->getChild("ObjectScene")->roll(Degree(3));
		if(m_Keyboard->isKeyDown(OIS::KC_F))
			graphicsManager.GetRootSceneNode()->getChild("ObjectScene")->roll(Degree(-3));
		if(m_Keyboard->isKeyDown(OIS::KC_SPACE))
			paint.resetBrush();
		if(m_Keyboard->isKeyDown(OIS::KC_ESCAPE))
			return;
		if(colorInput(m_Keyboard, &cValue, cBool) == true)
		{
			//Feed (*cValue) in here to a color according to cBool
		}

		//update brush and sync
		paint.setAnchorPosition(pos);
		paint.update(elapsed);
		graphicsManager.updateOgreMeshFromBulletMesh(paint);

		//Save last key states
		m_Keyboard->copyKeyStates(keyStates);
	}
#pragma endregion

}