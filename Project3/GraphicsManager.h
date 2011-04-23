#pragma once

#include <Ogre.h>
#include <string>
#include "Painter.h"

#include <btBulletDynamicsCommon.h>
#include <btBulletCollisionCommon.h>
#include <BulletSoftBody\btSoftRigidDynamicsWorld.h>
#include <BulletCollision\CollisionDispatch\btSphereSphereCollisionAlgorithm.h>
#include <BulletCollision\NarrowPhaseCollision\btGjkEpa2.h>
#include <BulletSoftBody/btSoftBodyRigidBodyCollisionConfiguration.h>
#include <BulletSoftBody/btSoftBodyHelpers.h>
#include <OgreHardwarePixelBuffer.h>

using namespace Ogre;
using namespace std;

class GraphicsManager
{
private:
	Ogre::Root *root;
	std::map<std::string,Ogre::RenderWindow*> render_windows;
	Ogre::SceneNode *root_sn;
	Ogre::Camera *c;
	Ogre::SceneNode *c_sn;
	Ogre::RenderWindow *window;
	Ogre::SceneManager *manager;
	Ogre::Viewport *vp;
	Ogre::Entity *brush;
	Ogre::Entity *canvas;
	Ogre::TexturePtr texture;
	HardwarePixelBufferSharedPtr hardwarePtr;

	void SetUpCamera();
	Ogre::RenderWindow* GetWindow(string name);

public:
	GraphicsManager(void);
	~GraphicsManager(void);

	Ogre::RenderWindow* GetWindow();
	void RenderFrame(Ogre::Real timeSinceLastFrame);
	void init();
	Ogre::SceneNode *GetRootSceneNode();
	Ogre::SceneManager *GetManager();
	void InitBrushFromPainter(Painter &paint);
	void updateOgreMeshFromBulletMesh(Painter &paint);
	void applyPaint();
	void loadCanvasObject(string fileName);
};

