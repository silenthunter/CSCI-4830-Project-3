#include "Painter.h"


Painter::Painter(void)
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
	dynamicsWorld->setGravity(btVector3(0,0,0));

	updateCounter = 0;

	loadObj("Meshes/Sphere.obj", btVector3(0,0,0), 1.2f);
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
	for (i = 0; i < btNodes.size(); ++i)
	{
	   //printf("Soft body node %d: (%f, %f, %f)\n", i, btNodes[i].m_x.x(), btNodes[i].m_x.y(), btNodes[i].m_x.z());
		xAvg += btNodes[i].m_x.x();
		yAvg += btNodes[i].m_x.y();
		zAvg += btNodes[i].m_x.z();
	}

	if(updateCounter++ % 100 == 0)
	{
		printf("Average position (%f, %f, %f)\n", xAvg / i, yAvg / i, zAvg / i);
		printf("FPS : %f\n", 1000 * elapsed);
	}
}

void Painter::loadObj(const char* fileName, btVector3 &position, btScalar scaling)
{
	ConvexDecomposition::WavefrontObj wo;
	int loadedWO = wo.loadObj(fileName);

	if(loadedWO)
	{
		brush = btSoftBodyHelpers::CreateFromTriMesh(worldInfo, wo.mVertices, wo.mIndices, wo.mTriCount);

		brush->generateBendingConstraints(2);
		brush->setTotalMass(30,true);
		brush->generateClusters(64);
		brush->setDeactivationTime(DISABLE_DEACTIVATION);

		//Create and attach a center node
		brush->appendNode(btVector3(0, 0, 0), 10);
		btSoftBody::Material  *mt = brush->appendMaterial();
		mt->m_kAST = 1;
		mt->m_kLST = .3;
		mt->m_kVST = .3;
		for(int i = 0; i < brush->m_nodes.size() - 1; i++)
			brush->appendLink(i, brush->m_nodes.size() - 1, mt);

		dynamicsWorld->addSoftBody(brush);

	}
}


void Painter::setAnchorPosition(btVector3 &pos)
{
	brush->m_nodes[brush->m_nodes.size() - 1].m_x = pos;
}
