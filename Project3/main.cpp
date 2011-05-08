#include <Ogre.h>
#include <OIS.h>
#include "GraphicsManager.h"
#include "GameTimer.h"
#include "haptics.h"
#include "Painter.h"

#include "BtOgrePG.h"
#include "BtOgreGP.h"
#include "BtOgreExtras.h"
#include <math.h>

GraphicsManager graphicsManager;
HapticsClass hap;

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

	graphicsManager.loadCanvasObject("cube.obj", 2.5f, Vector3(0, 5, 0));

	//Haptics stuff
	//hap.init(24, 10);

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
	//hap.synchFromServo();
	double startPos[3];
	//hap.getPosition(startPos);

	graphicsManager.GetRootSceneNode()->getChild("ObjectScene")->yaw(Degree(-90));

#pragma region Main Loop
	//Main Loop
	GameTimer timer;
	const float speed = 5.f;
	float rotation = 0.f;
	const float NovintScale = 4.f;
	btVector3 pos(0, 0, 0);
	int cValue = 0;
	int cBool = -1;
	double lastForceMag = 0;
	int cnt = 0;

	while(1)
	{
		cnt++;
		double elapsed = timer.getElapsedTimeSec();
		graphicsManager.RenderFrame(elapsed);
		Ogre::WindowEventUtilities::messagePump();
		//hap.synchFromServo();
		graphicsManager.applyPaint(paint);
		//dbgdraw->step();

		m_Keyboard->capture();

		if(m_Keyboard->isKeyDown(OIS::KC_W))
			pos.setY(pos.y() + speed * elapsed);
		if(m_Keyboard->isKeyDown(OIS::KC_S))
			pos.setY(pos.y() - speed * elapsed);
		if(m_Keyboard->isKeyDown(OIS::KC_A))
			pos.setX(pos.x() - speed * elapsed);
		if(m_Keyboard->isKeyDown(OIS::KC_D))
			pos.setX(pos.x() + speed * elapsed);
		if(colorInput(m_Keyboard, &cValue, cBool) == true)
		{
			//Feed (*cValue) in here to a color according to cBool
		}
		if(m_Keyboard->isKeyDown(OIS::KC_D))
			rotation += 60 * elapsed;
		if(m_Keyboard->isKeyDown(OIS::KC_A))
			rotation -= 60 * elapsed;

		//update brush and sync
		//double pos[3];
		//hap.getPosition(pos);
		
		/*Vector3 ogPos(pos[0], pos[1], pos[2]);
		Quaternion q(Degree(rotation), Vector3::UNIT_Y);
		ogPos *= NovintScale;
		ogPos = q * ogPos;
		pos[0] = ogPos.x;
		pos[1] = ogPos.y;
		pos[2] = ogPos.z;*/

		//paint.setAnchorPosition(btVector3(pos[0] - startPos[0] * NovintScale, pos[1] - startPos[1] * NovintScale, pos[2] - startPos[2] * NovintScale));
		paint.setAnchorPosition(pos);
		paint.update(elapsed);
		graphicsManager.updateOgreMeshFromBulletMesh(paint);
		//graphicsManager.GetRootSceneNode()->getChild("ObjectScene")->setOrientation(q);

		//Haptic forces from Bullet
		btVector3 force = -paint.getForceDirection();
		Vector3 forceOgre(force.x(), force.y(), force.z());
		double forceMag = forceOgre.length() / 10000 * (paint.isContacting() ? 5.5 : 3);//log(forceOgre.length()) * 100;

		/*double diff = forceMag - lastForceMag;
		forceMag = lastForceMag + diff * .01;
		lastForceMag = forceMag;*/

		//if(cnt > 25)
		//	hap.forceDirection(forceOgre.normalisedCopy(), forceMag);

		//Save last key states
		m_Keyboard->copyKeyStates(keyStates);
	}
#pragma endregion

}