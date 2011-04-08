#include <Ogre.h>
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

	Ogre::Entity* ObjectEntity = graphicsManager.GetManager()->createEntity("ObjectEntity", "cube.mesh");
	ObjectEntity->setMaterialName("Box");
	Ogre::SceneNode* ObjectScene = graphicsManager.GetRootSceneNode()->createChildSceneNode("ObjectScene");
	//ObjectScene->attachObject(ObjectEntity);
	ObjectScene->setPosition(0, 0, -20);
	ObjectScene->setScale(.05, .05, .05);

	Painter paint;

	//Debug Drawer
	SceneNode* debugSN = graphicsManager.GetRootSceneNode()->createChildSceneNode("debugSN");
	BtOgre::DebugDrawer* dbgdraw = new BtOgre::DebugDrawer(debugSN, paint.getDynamicsWorld());
	debugSN->setPosition(0, 0, -20);
	paint.getDynamicsWorld()->setDebugDrawer(dbgdraw);

#pragma region Main Loop
	//Main Loop
	GameTimer timer;
	while(1)
	{
		double elapsed = timer.getElapsedTimeSec();
		graphicsManager.RenderFrame(elapsed);
		Ogre::WindowEventUtilities::messagePump();
		dbgdraw->step();

		debugSN->yaw(Degree(10 * elapsed));

		paint.update(elapsed);
	}
#pragma endregion

}