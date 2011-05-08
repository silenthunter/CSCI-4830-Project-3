#include "Painter.h"
#include <fstream>

Painter::Painter(string obj)
{
	btCollisionConfiguration* m_collisionConfiguration = new btSoftBodyRigidBodyCollisionConfiguration();
	btDispatcher* m_dispatcher = new btCollisionDispatcher(m_collisionConfiguration);

	btScalar maxProxies = 32766;
	btVector3 worldAabbMin(-1000,-1000,-1000);
	btVector3 worldAabbMax(1000,1000,1000);
	btBroadphaseInterface* m_broadphase = new btAxisSweep3(worldAabbMin,worldAabbMax,maxProxies);

	btSequentialImpulseConstraintSolver* solver = new btSequentialImpulseConstraintSolver();
	btConstraintSolver* m_solver = solver;

	dynamicsWorld = new btSoftRigidDynamicsWorld(m_dispatcher,m_broadphase,m_solver,m_collisionConfiguration);
	worldInfo.m_broadphase = m_broadphase;
	worldInfo.m_dispatcher = m_dispatcher;
	worldInfo.m_gravity.setValue(0, 0, 0);
	worldInfo.m_sparsesdf.Initialize();
	dynamicsWorld->setGravity(btVector3(0,0,0));

	updateCounter = 0;

	string location = "Meshes/";
	location.append(obj);
	loadObj("Meshes/Sphere.obj", btVector3(0,0,0), 1.2f);
	loadTarget(location.c_str(), btVector3(0,5,0), 2.5f);
}

Painter::~Painter(void)
{
}

btSoftRigidDynamicsWorld* Painter::getDynamicsWorld()
{
	return dynamicsWorld;
}

void Painter::update(double elapsed)
{
	dynamicsWorld->stepSimulation(elapsed);
	//brush->setVelocity(btVector3(0, 0, 0));
	btSoftBody::tNodeArray& btNodes = brush->m_nodes;
	double xAvg = 0, yAvg = 0, zAvg = 0;

	int i;
	btVector3 vec(0, 0, 0);
	for (i = 0; i < btNodes.size(); ++i)
	{
	   //printf("Soft body node %d: (%f, %f, %f)\n", i, btNodes[i].m_x.x(), btNodes[i].m_x.y(), btNodes[i].m_x.z());
		xAvg += btNodes[i].m_x.x();
		yAvg += btNodes[i].m_x.y();
		zAvg += btNodes[i].m_x.z();
	}

	for(i = 0; i < brush->m_links.size(); i++)
		if(brush->m_links[i].m_n[1] == &(brush->m_nodes[brush->m_nodes.size() - 1]))
			vec += brush->m_links[i].m_c3;

	//lastForceDirection = forceDirection;
	//forceDirection = lastForceDirection.lerp(vec, .5);
	forceDirection = vec;

	if(updateCounter++ % 25 == 0)
	{
		printf("Average position (%f, %f, %f)\n", vec.x(),vec.y(), vec.z());
		//printf("Average position (%f, %f, %f)\n", xAvg / i, yAvg / i, zAvg / i);
		printf("FPS : %f\n", 1 / elapsed);

		if(getCollisions().size() > 0)
		{
		//int vec = getCollisions().front();
		//printf("index (%d)\n", vec);
		}
	}
}

//http://bulletphysics.org/Bullet/phpBB3/viewtopic.php?f=9&t=6665
//From my bullet physics post
void Painter::loadObj(const char* fileName, btVector3 &position, btScalar scaling)
{
	ConvexDecomposition::WavefrontObj wo;
	int loadedWO = wo.loadObj(fileName);

	if(loadedWO)
	{
		brush = btSoftBodyHelpers::CreateFromTriMesh(worldInfo, wo.mVertices, wo.mIndices, wo.mTriCount);

		brush->generateBendingConstraints(2);
		brush->generateClusters(64);
		brush->getCollisionShape()->setMargin(.2);
		brush->setDeactivationTime(DISABLE_DEACTIVATION);
		brush->m_cfg.collisions = btSoftBody::fCollision::SDF_RS;

		brush->m_cfg.kDP = .1;
		brush->m_cfg.kCHR = 0;
		brush->m_cfg.piterations = 5;

		//Create and attach a center node
		brush->appendNode(btVector3(0, 0, 0), 10);
		btSoftBody::Material  *mt = brush->appendMaterial();
		mt->m_kAST = 1;
		mt->m_kLST = .3;
		mt->m_kVST = .03;
		for(int i = 0; i < brush->m_nodes.size() - 1; i++)
		{
			brush->appendLink(i, brush->m_nodes.size() - 1, mt);
		}

		dynamicsWorld->addSoftBody(brush);

	}
}

void Painter::loadTarget(const char* fileName, btVector3 &position, btScalar scaling)
{
	ConvexDecomposition::WavefrontObj wo;
	int loadedWO = wo.loadObj(fileName);

	if(loadedWO)
	{
		btTriangleMesh* trimesh = new btTriangleMesh();

		btVector3 localScaling(scaling, scaling, scaling);
		
		int i;
		for ( i=0;i<wo.mTriCount;i++)
		{
			int index0 = wo.mIndices[i*3];
			int index1 = wo.mIndices[i*3+1];
			int index2 = wo.mIndices[i*3+2];

			btVector3 vertex0(wo.mVertices[index0*3], wo.mVertices[index0*3+1],wo.mVertices[index0*3+2]);
			btVector3 vertex1(wo.mVertices[index1*3], wo.mVertices[index1*3+1],wo.mVertices[index1*3+2]);
			btVector3 vertex2(wo.mVertices[index2*3], wo.mVertices[index2*3+1],wo.mVertices[index2*3+2]);
			
			vertex0 *= localScaling;
			vertex1 *= localScaling;
			vertex2 *= localScaling;

			trimesh->addTriangle(vertex0,vertex1,vertex2);
		}

		
		btBvhTriangleMeshShape * tmpConvexShape = new btBvhTriangleMeshShape(trimesh, true, true);
		btDefaultMotionState *motionState = new btDefaultMotionState(btTransform(btQuaternion(0,0,0,1), position));
		btRigidBody::btRigidBodyConstructionInfo rigidBodyCI(0, motionState, tmpConvexShape, btVector3(0,0,0));
		target = new btRigidBody(rigidBodyCI);
		target->setCollisionFlags(target->getCollisionFlags() | btCollisionObject::CF_STATIC_OBJECT);
		target->setFriction(btScalar(.1f));
		target->setRestitution(btScalar(.1f));
		dynamicsWorld->addRigidBody(target);
	}
}

void Painter::setAnchorPosition(btVector3 &pos)
{
	brush->m_nodes[brush->m_nodes.size() - 1].m_x = pos;
}

btVector3 Painter::getForceDirection()
{
	return forceDirection;
}

std::list<ContactResult> Painter::getCollisions()
{
	std::list<ContactResult> retn;
	//triIndex.clear();

	std::ofstream myfile;
	myfile.open("somefile.txt");

	for(int i = 0; i < brush->m_rcontacts.size(); i++)
	{
		btSoftBody::Node *node = brush->m_rcontacts[i].m_node;
		btVector3 pos = brush->m_nodes[brush->m_nodes.size() - 1].m_x;
		//retn.push_back(pos);
		btVector3 secondPos = node->m_x - pos;
		MyRayResultCallback rayCallback(pos, secondPos, this, brush);
		dynamicsWorld->rayTest(pos, secondPos, rayCallback);

		ContactResult NewContact;
		NewContact.collisionPt = rayCallback.contactPt;
		NewContact.triangleIndex = rayCallback.triIndex;
		retn.push_back(NewContact);

		//myfile << NewContact.collisionPt.getX() << " " << NewContact.collisionPt.getY() << " " << NewContact.collisionPt.getZ() << "\n";
	}
	myfile.close();

	return retn;
}

btScalar MyRayResultCallback::addSingleResult(btCollisionWorld::LocalRayResult& rayResult,bool normalInWorldSpace)
{
	ClosestRayResultCallback::addSingleResult(rayResult, normalInWorldSpace);
	if(rayResult.m_collisionObject == self || rayResult.m_localShapeInfo == NULL) return rayResult.m_hitFraction;
	triIndex =rayResult.m_localShapeInfo->m_triangleIndex;
	contactPt = this->m_hitPointWorld - this->m_collisionObject->getWorldTransform().getOrigin();
    return rayResult.m_hitFraction;
}

bool MyRayResultCallback::needsCollision(btBroadphaseProxy *proxy0) const
{
	return true;
};

bool Painter::isContacting()
{
	bool retn = brush->m_rcontacts.size() > 0;
	return retn;
}
