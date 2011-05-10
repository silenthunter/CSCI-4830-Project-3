#include <Ogre.h>
#include <OIS.h>
#include "GraphicsManager.h"
#include "GameTimer.h"
#include "haptics.h"
#include "Painter.h"
#include "OpenALSoundSystem.h"
#include "GameKeyboard.h"

#include "BtOgrePG.h"
#include "BtOgreGP.h"
#include "BtOgreExtras.h"

//#define NOVINT

GraphicsManager graphicsManager;
OpenALSoundSystem sound;
GameKeyboard gkb;

double soundPos[3] = {0, 0, 0};
double zeroVel[3] = {0, 0, 0};
const string ssStr = "SoundSourceString";
string SoundString = "music.wav";

#ifdef NOVINT
HapticsClass hap;
#endif

//Insert 0 - 9, R, G, B, and Enter into the unordered_map
void GKBColorInputMap()
{
	gkb.KBM.insert(KeyBoolMap::value_type(OIS::KC_0, false));
	gkb.KBM.insert(KeyBoolMap::value_type(OIS::KC_1, false));
	gkb.KBM.insert(KeyBoolMap::value_type(OIS::KC_2, false));
	gkb.KBM.insert(KeyBoolMap::value_type(OIS::KC_3, false));
	gkb.KBM.insert(KeyBoolMap::value_type(OIS::KC_4, false));
	gkb.KBM.insert(KeyBoolMap::value_type(OIS::KC_5, false));
	gkb.KBM.insert(KeyBoolMap::value_type(OIS::KC_6, false));
	gkb.KBM.insert(KeyBoolMap::value_type(OIS::KC_7, false));
	gkb.KBM.insert(KeyBoolMap::value_type(OIS::KC_8, false));
	gkb.KBM.insert(KeyBoolMap::value_type(OIS::KC_9, false));
	gkb.KBM.insert(KeyBoolMap::value_type(OIS::KC_LSHIFT, false));
	gkb.KBM.insert(KeyBoolMap::value_type(OIS::KC_R, false));
	gkb.KBM.insert(KeyBoolMap::value_type(OIS::KC_G, false));
	gkb.KBM.insert(KeyBoolMap::value_type(OIS::KC_B, false));
}

void main(int argc, char *argv[])
{

	string obj= "cube.obj";
	graphicsManager.init();

	graphicsManager.loadCanvasObject(obj, 2.5f, Vector3(0, 5, 0));

	#ifdef NOVINT
	//Haptics stuff
	hap.init(24, 10);
	#endif
	Painter paint(obj);

	//Debug Drawer
	SceneNode* debugSN = graphicsManager.GetRootSceneNode()->createChildSceneNode("debugSN");
	BtOgre::DebugDrawer* dbgdraw = new BtOgre::DebugDrawer(debugSN, paint.getDynamicsWorld());
	debugSN->setPosition(0, 0, -20);
	paint.getDynamicsWorld()->setDebugDrawer(dbgdraw);

	graphicsManager.InitBrushFromPainter(paint);

	#pragma region Set up OpenAL
	sound.init();
	sound.createSound(SoundString, SoundString);
	sound.createSource(ssStr, zeroVel, zeroVel);
	sound.assignSourceSound(ssStr, SoundString, 1, 1, 1);
	sound.playSound(ssStr);
	#pragma endregion

	//Allow for keyboard control
	RenderWindow* ogreWindow = graphicsManager.GetWindow();
	size_t hWnd = 0;
	ogreWindow->getCustomAttribute("WINDOW", &hWnd);
	OIS::InputManager *m_InputManager = OIS::InputManager::createInputSystem(hWnd);
	OIS::Keyboard *m_Keyboard = static_cast<OIS::Keyboard*>(m_InputManager->createInputObject(OIS::OISKeyboard, false));
	char *keyStates = new char[512];

	#pragma region Set up GameKeyboard
	GKBColorInputMap(); //Insert color input keys
	gkb.setKeyboard(m_Keyboard);
	#pragma endregion

	#ifdef NOVINT
	hap.synchFromServo();
	double startPos[3];
	hap.getPosition(startPos);
	#endif

	//graphicsManager.GetRootSceneNode()->getChild("ObjectScene")->yaw(Degree(-90));

#pragma region Main Loop
	//Main Loop
	GameTimer timer;
	const float speed = 5.f;
	float rotation = 0.f;
	float rotationY = 0.f;
	const float NovintScale = 4.f;
#ifndef NOVINT
	btVector3 pos(0, 0, 0);
#endif
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

		#ifdef NOVINT
		hap.synchFromServo();
		#endif

		graphicsManager.applyPaint(paint);
		//dbgdraw->step();

		m_Keyboard->capture();

#ifndef NOVINT
		if(m_Keyboard->isKeyDown(OIS::KC_W))
			pos.setY(pos.y() + speed * elapsed);
		if(m_Keyboard->isKeyDown(OIS::KC_S))
			pos.setY(pos.y() - speed * elapsed);
		if(m_Keyboard->isKeyDown(OIS::KC_A))
			pos.setX(pos.x() - speed * elapsed);
		if(m_Keyboard->isKeyDown(OIS::KC_D))
			pos.setX(pos.x() + speed * elapsed);
#endif
		if(gkb.handleColorInput() == true)
		{
			//Convert gkb.gc from a 0 - 255 scale to a 0 - 1 scale by dividing all member variables (r, g, and b) by 255.f
			//Send gkb.gc to the paint brush
			graphicsManager.updateBrushColor(gkb.gc.r / 255.f, gkb.gc.g / 255.f, gkb.gc.b / 255.f);
			gkb.resetColor();
		}
		if(m_Keyboard->isKeyDown(OIS::KC_SPACE))
			paint.resetBrush();
		if(m_Keyboard->isKeyDown(OIS::KC_ESCAPE))
			return;
		if(m_Keyboard->isKeyDown(OIS::KC_E))
			rotation += 60 * elapsed;
		if(m_Keyboard->isKeyDown(OIS::KC_Q))
			rotation -= 60 * elapsed;

		#ifdef NOVINT
		//update brush and sync
		double pos[3];
		hap.getPosition(pos);

		//pos[1] -= 5;//Compensate for starting position

		Vector3 ogPos(pos[0], pos[1], pos[2]);
		Quaternion q(Degree(rotation), Vector3::UNIT_Y);
		Quaternion qY(Degree(rotationY), Vector3::UNIT_X);
		ogPos *= NovintScale;
		ogPos = q.Inverse() * ogPos;
		ogPos = qY.Inverse() * ogPos;
		pos[0] = ogPos.x;
		pos[1] = ogPos.y;
		pos[2] = ogPos.z;
		paint.setAnchorPosition(btVector3(pos[0] - startPos[0] * NovintScale, pos[1] - startPos[1] * NovintScale, pos[2] - startPos[2] * NovintScale));
		#else
		
		Vector3 ogPos(pos.x(), pos.y(), pos.z());
		ogPos.y -= 5;//Compensate for starting position
		Quaternion q(Degree(rotation), Vector3::UNIT_Y);
		Quaternion qY(Degree(rotationY), Vector3::UNIT_X);
		ogPos = q.Inverse() * ogPos;
		ogPos = qY.Inverse() * ogPos;
		ogPos.y += 5;//Compensate for starting position
		btVector3 newPos(0,0,0);
		newPos.setX(ogPos.x);
		newPos.setY(ogPos.y);
		newPos.setZ(ogPos.z);

		paint.setAnchorPosition(newPos);
		#endif

		paint.update(elapsed);
		graphicsManager.updateOgreMeshFromBulletMesh(paint);
		graphicsManager.GetRootSceneNode()->getChild("ObjectScene")->setOrientation(q * qY);
		graphicsManager.GetRootSceneNode()->getChild("brushSN")->setOrientation(q * qY);

		//Haptic forces from Bullet
		btVector3 force = -paint.getForceDirection();
		Vector3 forceOgre(force.x(), force.y(), force.z());
		double forceMag = forceOgre.length() / 10000 * (paint.isContacting() ? 5.5 : 3);//log(forceOgre.length()) * 100;

		/*double diff = forceMag - lastForceMag;
		forceMag = lastForceMag + diff * .01;
		lastForceMag = forceMag;*/

		#ifdef NOVINT
		if(cnt > 25)
			hap.forceDirection(forceOgre.normalisedCopy(), forceMag);
		#endif

		//Save last key states
		m_Keyboard->copyKeyStates(keyStates);
	}
#pragma endregion

}