#pragma once

#include <btBulletDynamicsCommon.h>
#include <btBulletCollisionCommon.h>
#include <BulletSoftBody\btSoftRigidDynamicsWorld.h>
#include <BulletCollision\CollisionDispatch\btSphereSphereCollisionAlgorithm.h>
#include <BulletCollision\NarrowPhaseCollision\btGjkEpa2.h>
#include <BulletSoftBody/btSoftBodyRigidBodyCollisionConfiguration.h>
#include <BulletSoftBody/btSoftBodyHelpers.h>
#include <BulletSoftBody/btSoftRigidCollisionAlgorithm.h>
#include "ConvexDecomposition\ConvexDecomposition.h"
#include "ConvexDecomposition\cd_wavefront.h"
#include <iostream>
#include <list>
#include <string.h>

using namespace std;

struct MyRayResultCallback;

struct ContactResult
{
	int triangleIndex;
	btVector3 collisionPt;
};

class Painter
{
	friend class GraphicsManager;
	friend struct MyRayResultCallback;
private:

	btSoftRigidDynamicsWorld* dynamicsWorld;
	btSoftBodyWorldInfo worldInfo;
	btSoftBody* brush;
	btRigidBody* target;
	btVector3 *origOff;
	void loadObj(const char* fileName, btVector3 &position, btScalar scaling = 1.f);
	void loadTarget(const char* fileName, btVector3 &position, btScalar scaling = 1.f);
	int updateCounter;

public:
	Painter(string obj);
	~Painter(void);

	void update(double elapsed);
	void setAnchorPosition(btVector3 &pos);
	void resetBrush();
	std::list<ContactResult> getCollisions();
	btSoftRigidDynamicsWorld* getDynamicsWorld();
};

struct MyRayResultCallback : public btCollisionWorld::ClosestRayResultCallback
{
	MyRayResultCallback(const btVector3& rayFromWorld,const btVector3& rayToWorld, Painter* painter, btCollisionObject *me)
		: m_rayFromWorld(rayFromWorld), m_rayToWorld(rayToWorld), paint(painter), self(me), ClosestRayResultCallback(rayFromWorld, rayToWorld){}
	btVector3   m_rayFromWorld;//used to calculate hitPointWorld from hitFraction
	btVector3   m_rayToWorld;
	Painter *paint;
	btCollisionObject *self;
	
	btVector3 contactPt;
	int triIndex;

	virtual btScalar addSingleResult(btCollisionWorld::LocalRayResult& rayResult, bool normalInWorldSpace);
	virtual bool needsCollision (btBroadphaseProxy *proxy0) const;
};
