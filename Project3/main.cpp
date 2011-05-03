#include <Ogre.h>
#include <OIS.h>
#include "GraphicsManager.h"
#include "GameTimer.h"
#include "Painter.h"

#include "BtOgrePG.h"
#include "BtOgreGP.h"
#include "BtOgreExtras.h"

GraphicsManager graphicsManager;

void main(int argc, char *argv[])
{
	graphicsManager.init();

	graphicsManager.loadCanvasObject("cube.mesh", "cube.obj");

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

#pragma region Main Loop
	//Main Loop
	GameTimer timer;
	const float speed = 5.f;
	btVector3 pos(0, 0, 0);
	while(1)
	{
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

		//update brush and sync
		paint.setAnchorPosition(pos);
		paint.update(elapsed);
		graphicsManager.updateOgreMeshFromBulletMesh(paint);

		//Save last key states
		m_Keyboard->copyKeyStates(keyStates);
	}
#pragma endregion

}