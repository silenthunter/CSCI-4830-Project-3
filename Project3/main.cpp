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

void main(int argc, char *argv[])
{
	graphicsManager.init();

	Ogre::Entity* ObjectEntity = graphicsManager.GetManager()->createEntity("ObjectEntity", "cube.mesh");
	ObjectEntity->setMaterialName("Box");
	Ogre::SceneNode* ObjectScene = graphicsManager.GetRootSceneNode()->createChildSceneNode("ObjectScene");
	ObjectScene->attachObject(ObjectEntity);
	ObjectScene->setPosition(0, 5, -20);
	ObjectScene->setScale(.05, .05, .05);

	//Haptics stuff
	hap.init(24, 10);

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
	hap.synchFromServo();
	double startPos[3];
	hap.getPosition(startPos);

#pragma region Main Loop
	//Main Loop
	GameTimer timer;
	const float speed = 5.f;
	//btVector3 pos(0, 0, 0);
	while(1)
	{
		double elapsed = timer.getElapsedTimeSec();
		graphicsManager.RenderFrame(elapsed);
		Ogre::WindowEventUtilities::messagePump();
		hap.synchFromServo();
		//dbgdraw->step();

		m_Keyboard->capture();

		/*if(m_Keyboard->isKeyDown(OIS::KC_W))
			pos.setY(pos.y() + speed * elapsed);
		if(m_Keyboard->isKeyDown(OIS::KC_S))
			pos.setY(pos.y() - speed * elapsed);
		if(m_Keyboard->isKeyDown(OIS::KC_A))
			pos.setX(pos.x() - speed * elapsed);
		if(m_Keyboard->isKeyDown(OIS::KC_D))
			pos.setX(pos.x() + speed * elapsed);*/

		//update brush and sync
		double pos[3];
		hap.getPosition(pos);
		paint.setAnchorPosition(btVector3(pos[0] - startPos[0], pos[1] - startPos[1], pos[2] - startPos[2]));
		//paint.setAnchorPosition(pos);
		paint.update(elapsed);
		graphicsManager.updateOgreMeshFromBulletMesh(paint);

		//Haptic forces from Bullet
		btVector3 force = -paint.getForceDirection();
		Vector3 forceOgre(force.x(), force.y(), force.z());
		double forceMag = forceOgre.length() / 1000;//log(forceOgre.length()) * 100;
		hap.forceDirection(forceOgre.normalisedCopy(), forceMag);

		//Save last key states
		m_Keyboard->copyKeyStates(keyStates);
	}
#pragma endregion

}