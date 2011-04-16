#pragma once

#include <btBulletDynamicsCommon.h>
#include <btBulletCollisionCommon.h>
#include <BulletSoftBody\btSoftRigidDynamicsWorld.h>
#include <BulletCollision\CollisionDispatch\btSphereSphereCollisionAlgorithm.h>
#include <BulletCollision\NarrowPhaseCollision\btGjkEpa2.h>
#include <BulletSoftBody/btSoftBodyRigidBodyCollisionConfiguration.h>
#include <BulletSoftBody/btSoftBodyHelpers.h>
#include "ConvexDecomposition\ConvexDecomposition.h"
#include "ConvexDecomposition\cd_wavefront.h"
#include <iostream>

class Painter
{
	friend class GraphicsManager;
private:
	btSoftRigidDynamicsWorld* dynamicsWorld;
	btSoftBodyWorldInfo worldInfo;
	btSoftBody* brush;
	btRigidBody* target;
	void loadObj(const char* fileName, btVector3 &position, btScalar scaling = 1.f);
	void loadTarget(const char* fileName, btVector3 &position, btScalar scaling = 1.f);
	int updateCounter;

public:
	Painter(void);
	~Painter(void);

	void update(double elapsed);
	void setAnchorPosition(btVector3 &pos);

	btSoftRigidDynamicsWorld* getDynamicsWorld();
};

