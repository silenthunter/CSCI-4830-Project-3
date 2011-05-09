#include "GraphicsManager.h"

GraphicsManager::GraphicsManager(void)
{
}

GraphicsManager::~GraphicsManager(void)
{
}

void GraphicsManager::init()
{
#ifdef _DEBUG
	string pluginsFile = "plugins.cfg";
#else
	string pluginsFile = "plugins_release.cfg";
#endif
	string configFile = "ogre.cfg";
	string logFile = "./ogre.log";
	string resourcesFile = "resources.cfg";

	root = new Ogre::Root(pluginsFile, configFile, logFile);
	root->setRenderSystem(root->getRenderSystemByName("OpenGL Rendering Subsystem"));
	root->initialise(false);

	ConfigFile cf;
	cf.load(resourcesFile);

	ConfigFile::SectionIterator seci = cf.getSectionIterator();
	string secName, typeName, archName;
	while(seci.hasMoreElements())
	{
		secName = seci.peekNextKey();
		ConfigFile::SettingsMultiMap * settings = seci.getNext();
		ConfigFile::SettingsMultiMap::iterator i;
		for(i = settings->begin(); i!=settings->end(); i++)
		{
			typeName = i->first;
			archName = i->second;
			ResourceGroupManager::getSingleton().addResourceLocation(archName, typeName, secName);
		}
	}

	GetWindow("Project 3");
	SetUpCamera();
	initBackground();
	//return root;
}

void GraphicsManager::initBackground()
{
	//http://www.ogre3d.org/tikiwiki/Displaying+2D+Backgrounds&structure=Cookbook

	// Create background material
	MaterialPtr material = MaterialManager::getSingleton().create("Background", "General");
	material->getTechnique(0)->getPass(0)->createTextureUnitState("Meshes/spacesky.jpg");
	material->getTechnique(0)->getPass(0)->setDepthCheckEnabled(false);
	material->getTechnique(0)->getPass(0)->setDepthWriteEnabled(false);
	material->getTechnique(0)->getPass(0)->setLightingEnabled(false);

	// Create background rectangle covering the whole screen
	int bgCubeBoundary = 5000;
	createCube(bgCubeBoundary);

	// Attach background to the scene
	SceneNode* node = manager->getRootSceneNode()->createChildSceneNode("Background");
	node->attachObject(bgCube);
}

void GraphicsManager::createCube(int bound)
{
    bgCube = manager->createManualObject("BGCube");
	bgCube->begin("Background");

	//Vertices
    bgCube->position(-bound, -bound, -bound);   //0
    bgCube->position(bound, -bound, -bound);    //1
    bgCube->position(bound, -bound, bound);    //2
    bgCube->position(-bound, -bound, bound);    //3
    bgCube->position(-bound, bound, -bound);    //4
    bgCube->position(bound, bound, -bound);    //5
    bgCube->position(bound, bound, bound);    //6
    bgCube->position(-bound, bound, bound);    //7

	//Normal
    bgCube->triangle(0, 2, 1);
    bgCube->triangle(0, 2, 3);
    bgCube->triangle(3, 4, 0);
    bgCube->triangle(3, 7, 4);
    bgCube->triangle(4, 7, 6);
    bgCube->triangle(4, 6, 5);
    bgCube->triangle(5, 2, 1);
    bgCube->triangle(5, 6, 2);
    bgCube->triangle(0, 4, 1);
    bgCube->triangle(5, 1, 4);
    bgCube->triangle(3, 6, 7);
    bgCube->triangle(3, 2, 6);
}

void GraphicsManager::loadCanvasObject(string fileName, float scale, Vector3 position)
{

	ConvexDecomposition::WavefrontObj wo;
	int loadedWO = wo.loadObj(("Meshes/" + fileName).c_str());

	if(!loadedWO) return;

	numIndices = wo.mTriCount;

	float *normals = new float[wo.mVertexCount * 3];
	indices = new int[wo.mTriCount * 3];
	for(int i = 0; i < wo.mTriCount * 3; i++)
		indices[i] = wo.mIndices[i];

	for(int i = 0; i < wo.mTriCount * 3; i++)
	{
		normals[wo.mIndices[i] * 3] = wo.mNormals[wo.mIndicesToNormals[i] * 3];
		normals[wo.mIndices[i] * 3 + 1] = wo.mNormals[wo.mIndicesToNormals[i] * 3 + 1];
		normals[wo.mIndices[i] * 3 + 2] = wo.mNormals[wo.mIndicesToNormals[i] * 3 + 2];
	}

	 /// Create the mesh via the MeshManager
    Ogre::MeshPtr msh = MeshManager::getSingleton().createManual("Canvas", "General");

	/// Create one submesh
    SubMesh* sub = msh->createSubMesh();

	float *vertices = new float[wo.mVertexCount * 6];//3 floats for each vertices and 3 for its normal

	//Get all vertices and their normals
	for(int i = 0; i < wo.mVertexCount; i++)
	{
		vertices[i * 6] = wo.mVertices[i * 3] * scale;
		vertices[i * 6 + 1] = wo.mVertices[i * 3 + 1] * scale;
		vertices[i * 6 + 2] = wo.mVertices[i * 3 + 2] * scale;
		vertices[i * 6 + 3] = normals[i * 3];
		vertices[i * 6 + 4] = normals[i * 3 + 1];
		vertices[i * 6 + 5] = normals[i * 3 + 2];
	}

	rsCanvas = Root::getSingleton().getRenderSystem();
	colours = new RGBA[wo.mVertexCount];
	for(int i = 0; i < wo.mVertexCount; i++)
		rsCanvas->convertColourValue(ColourValue(.1, .9, .1), colours + i);

	//assign faces based on node indexes
	unsigned short *faces = new unsigned short[wo.mTriCount * 3];
	for(int i = 0; i < wo.mTriCount * 3; i++)
		faces[i] = wo.mIndices[i];

	 msh->sharedVertexData = new VertexData();
	 msh->sharedVertexData->vertexCount = wo.mVertexCount;

	 /// Create declaration (memory format) of vertex data
    VertexDeclaration* decl = msh->sharedVertexData->vertexDeclaration;
    size_t offset = 0;
    // 1st buffer
    decl->addElement(0, offset, VET_FLOAT3, VES_POSITION);
    offset += VertexElement::getTypeSize(VET_FLOAT3);
    decl->addElement(0, offset, VET_FLOAT3, VES_NORMAL);
    offset += VertexElement::getTypeSize(VET_FLOAT3);
    /// Allocate vertex buffer of the requested number of vertices (vertexCount) 
    /// and bytes per vertex (offset)
    HardwareVertexBufferSharedPtr vbuf = 
        HardwareBufferManager::getSingleton().createVertexBuffer(
        offset, msh->sharedVertexData->vertexCount, HardwareBuffer::HBU_STATIC_WRITE_ONLY);
    /// Upload the vertex data to the card
    vbuf->writeData(0, vbuf->getSizeInBytes(), vertices, true);

    /// Set vertex buffer binding so buffer 0 is bound to our vertex buffer
    VertexBufferBinding* bind = msh->sharedVertexData->vertexBufferBinding; 
    bind->setBinding(0, vbuf);

    // 2nd buffer
    offset = 0;
    decl->addElement(1, offset, VET_COLOUR, VES_DIFFUSE);
    offset += VertexElement::getTypeSize(VET_COLOUR);
    /// Allocate vertex buffer of the requested number of vertices (vertexCount) 
    /// and bytes per vertex (offset)
    vbufColor = HardwareBufferManager::getSingleton().createVertexBuffer(
		offset, msh->sharedVertexData->vertexCount, HardwareBuffer::HBU_WRITE_ONLY);
    /// Upload the vertex data to the card
    vbufColor->writeData(0, vbufColor->getSizeInBytes(), colours, true);

    /// Set vertex buffer binding so buffer 1 is bound to our colour buffer
    bind->setBinding(1, vbufColor);

    /// Allocate index buffer of the requested number of vertices (ibufCount) 
    HardwareIndexBufferSharedPtr ibuf = HardwareBufferManager::getSingleton().
        createIndexBuffer(
        HardwareIndexBuffer::IT_16BIT, 
		wo.mTriCount * 3, 
        HardwareBuffer::HBU_STATIC_WRITE_ONLY);

    /// Upload the index data to the card
    ibuf->writeData(0, ibuf->getSizeInBytes(), faces, true);

    /// Set parameters of the submesh
    sub->useSharedVertices = true;
    sub->indexData->indexBuffer = ibuf;
	sub->indexData->indexCount = wo.mTriCount * 3;
    sub->indexData->indexStart = 0;

    /// Set bounding information (for culling)
    msh->_setBounds(AxisAlignedBox(-100,-100,-100,100,100,100));
    msh->_setBoundingSphereRadius(Math::Sqrt(3*100*100));

    /// Notify -Mesh object that it has been loaded
    msh->load();

	//Add to the scene
	MaterialPtr material = MaterialManager::getSingleton().create(
      "Test/ColourTest", ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
	material->getTechnique(0)->getPass(0)->setVertexColourTracking(TVC_AMBIENT);

	brush = manager->createEntity("canvas", "Canvas");
	brush->setMaterialName("Test/ColourTest");
	SceneNode* thisSceneNode = manager->getRootSceneNode()->createChildSceneNode("ObjectScene");
	thisSceneNode->setPosition(0, 0, -20);
	thisSceneNode->translate(position);
	thisSceneNode->attachObject(brush);
}

Ogre::RenderWindow* GraphicsManager::GetWindow(string name)
{
	Ogre::NameValuePairList nvpl;
	nvpl["parentWindowHandle"] = Ogre::StringConverter::toString((size_t)NULL);
	nvpl["externalWindowHandle"] = Ogre::StringConverter::toString((size_t)NULL);

	window = root->createRenderWindow(name, 0, 0, false, &nvpl);
	window->setVisible(true);
	if(render_windows.size() == 0)
		ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
		
	render_windows[name] = window;
	window->setDeactivateOnFocusChange(false);

	window->resize(800,600);

	return window;
}

void GraphicsManager::SetUpCamera()
{
	manager = root->createSceneManager(Ogre::ST_GENERIC, "main");
	root_sn = manager->getRootSceneNode();
	manager->setShadowTechnique(ShadowTechnique::SHADOWTYPE_STENCIL_ADDITIVE);
	manager->setAmbientLight(Ogre::ColourValue(.5, .5, .5)); //Abient light set here

	//set up camera
	Ogre::SceneNode *player = manager->createSceneNode("Player");
	c = manager->createCamera("main_camera");
	c->setFOVy(Ogre::Degree(45));
	c->setNearClipDistance(.01);
	c->setFarClipDistance(100);
	c->setAutoAspectRatio(true);

	//TODO: Remove temporary settings
	c->setPosition(Ogre::Vector3(0, 0, 0));
	//c->lookAt(8,3,0);
	/*Ogre::Light *l = manager->createLight("light1");
	root_sn->attachObject(l);
	l->setPosition(0, 6, 0);*/

	//attach the camera
	vp = window->addViewport(c);
	c_sn = manager->createSceneNode("main_camera");
	c_sn->setPosition(0, 3, 0);
	//c_sn->attachObject(c);

	//c->lookAt(0, 0, 0);
	//c->yaw(Degree(30));

	root_sn->addChild(player);
	player->addChild(c_sn);
	Ogre::SceneNode *cPitch_sn = c_sn->createChildSceneNode("cameraPitch");
	cPitch_sn->attachObject(c);
}

void GraphicsManager::RenderFrame(Ogre::Real timeSinceLastFrame)
{
	root->renderOneFrame(timeSinceLastFrame);
}

Ogre::SceneNode* GraphicsManager::GetRootSceneNode()
{
	return root_sn;
}

Ogre::SceneManager* GraphicsManager::GetManager()
{
	return manager;
}

Ogre::RenderWindow* GraphicsManager::GetWindow()
{
	return window;
}

int findLinearNode(btSoftBody::Node *node, btSoftBody *body)
{
	int i;
	for(i = 0; i < body->m_nodes.size(); i++)
	{
		if(node == &(body->m_nodes[i])) return i;
	}

	return i;
}

//Copied most of this from Ogre's wiki
void GraphicsManager::InitBrushFromPainter(Painter &paint)
{
	 /// Create the mesh via the MeshManager
    Ogre::MeshPtr msh = MeshManager::getSingleton().createManual("ColourCube", "General");

	/// Create one submesh
    SubMesh* sub = msh->createSubMesh();

	btSoftBody* bulletBody = paint.brush;
	float *vertices = new float[(bulletBody->m_nodes.size() - 1) * 6];//3 floats for each vertices and 3 for its normal

	//Get all vertices and their normals
	for(int i = 0; i < bulletBody->m_nodes.size() - 1; i++)
	{
		vertices[i * 6] = bulletBody->m_nodes[i].m_x.x();
		vertices[i * 6 + 1] = bulletBody->m_nodes[i].m_x.y();
		vertices[i * 6 + 2] = bulletBody->m_nodes[i].m_x.z();
		vertices[i * 6 + 3] = bulletBody->m_nodes[i].m_n.x();
		vertices[i * 6 + 4] = bulletBody->m_nodes[i].m_n.y();
		vertices[i * 6 + 5] = bulletBody->m_nodes[i].m_n.z();
	}

	RenderSystem* rs = Root::getSingleton().getRenderSystem();
	RGBA *colours = new RGBA[bulletBody->m_nodes.size() - 1];
	for(int i = 0; i < bulletBody->m_nodes.size() - 1; i++)
		rs->convertColourValue(ColourValue(.1, .9, .1), colours + i);

	//assign faces based on node indexes
	unsigned short *faces = new unsigned short[bulletBody->m_faces.size() * 3];
	for(int i = 0; i < bulletBody->m_faces.size(); i++)
	{
		//TODO: Implement binary search if the initialization is too slow
		faces[i * 3] = findLinearNode(bulletBody->m_faces[i].m_n[0], bulletBody);
		faces[i * 3 + 1] = findLinearNode(bulletBody->m_faces[i].m_n[1], bulletBody);
		faces[i * 3 + 2] = findLinearNode(bulletBody->m_faces[i].m_n[2], bulletBody);
		cout << faces [i * 3] << " "  << faces [i * 3 + 1] << " "  << faces [i * 3 + 2] << endl;
		/*faces[i * 3] = bulletBody->m_nodes.findBinarySearch(*bulletBody->m_faces[i].m_n[0]);
		faces[i * 3 + 1] = bulletBody->m_nodes.findBinarySearch(*bulletBody->m_faces[i].m_n[1]);
		faces[i * 3 + 2] = bulletBody->m_nodes.findBinarySearch(*bulletBody->m_faces[i].m_n[2]);*/
	}

	 msh->sharedVertexData = new VertexData();
	 msh->sharedVertexData->vertexCount = bulletBody->m_nodes.size() - 1;

	 /// Create declaration (memory format) of vertex data
    VertexDeclaration* decl = msh->sharedVertexData->vertexDeclaration;
    size_t offset = 0;
    // 1st buffer
    decl->addElement(0, offset, VET_FLOAT3, VES_POSITION);
    offset += VertexElement::getTypeSize(VET_FLOAT3);
    decl->addElement(0, offset, VET_FLOAT3, VES_NORMAL);
    offset += VertexElement::getTypeSize(VET_FLOAT3);
    /// Allocate vertex buffer of the requested number of vertices (vertexCount) 
    /// and bytes per vertex (offset)
    vbuf = 
        HardwareBufferManager::getSingleton().createVertexBuffer(
        offset, msh->sharedVertexData->vertexCount, HardwareBuffer::HBU_STATIC_WRITE_ONLY);
    /// Upload the vertex data to the card
    vbuf->writeData(0, vbuf->getSizeInBytes(), vertices, true);

    /// Set vertex buffer binding so buffer 0 is bound to our vertex buffer
    VertexBufferBinding* bind = msh->sharedVertexData->vertexBufferBinding; 
    bind->setBinding(0, vbuf);

    // 2nd buffer
    offset = 0;
    decl->addElement(1, offset, VET_COLOUR, VES_DIFFUSE);
    offset += VertexElement::getTypeSize(VET_COLOUR);
    /// Allocate vertex buffer of the requested number of vertices (vertexCount) 
    /// and bytes per vertex (offset)
    vbuf = HardwareBufferManager::getSingleton().createVertexBuffer(
        offset, msh->sharedVertexData->vertexCount, HardwareBuffer::HBU_STATIC_WRITE_ONLY);
    /// Upload the vertex data to the card
    vbuf->writeData(0, vbuf->getSizeInBytes(), colours, true);

    /// Set vertex buffer binding so buffer 1 is bound to our colour buffer
    bind->setBinding(1, vbuf);

    /// Allocate index buffer of the requested number of vertices (ibufCount) 
    HardwareIndexBufferSharedPtr ibuf = HardwareBufferManager::getSingleton().
        createIndexBuffer(
        HardwareIndexBuffer::IT_16BIT, 
		bulletBody->m_faces.size() * 3, 
        HardwareBuffer::HBU_STATIC_WRITE_ONLY);

    /// Upload the index data to the card
    ibuf->writeData(0, ibuf->getSizeInBytes(), faces, true);

    /// Set parameters of the submesh
    sub->useSharedVertices = true;
    sub->indexData->indexBuffer = ibuf;
	sub->indexData->indexCount = bulletBody->m_faces.size() * 3;
    sub->indexData->indexStart = 0;

    /// Set bounding information (for culling)
    msh->_setBounds(AxisAlignedBox(-100,-100,-100,100,100,100));
    msh->_setBoundingSphereRadius(Math::Sqrt(3*100*100));

    /// Notify -Mesh object that it has been loaded
    msh->load();

	//Add to the scene
	MaterialPtr material = MaterialManager::getSingleton().create(
      "Test/ColourTest", ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
	material->getTechnique(0)->getPass(0)->setVertexColourTracking(TVC_AMBIENT);

	brush = manager->createEntity("cc", "ColourCube");
	brush->setMaterialName("Test/ColourTest");
	SceneNode* thisSceneNode = manager->getRootSceneNode()->createChildSceneNode("brushSN");
	thisSceneNode->setPosition(0, 0, -20);
	thisSceneNode->attachObject(brush);
}

void GraphicsManager::updateOgreMeshFromBulletMesh(Painter &paint)
{
    //retrieve the vertex data from the softbody.
    //SoftBody *ogreSoftBody= static_cast<SoftBody*>(softBody->getUserPointer());
    //MeshPtr mesh= ogreSoftBody->getEntity()->getMesh();
	btSoftBody* softBody = paint.brush;
	MeshPtr mesh = brush->getMesh();
    Ogre::SubMesh* submesh = mesh->getSubMesh(0);
    Ogre::VertexData* vertex_data = submesh->useSharedVertices ? mesh->sharedVertexData : submesh->vertexData;

	//Not sure what this conditional statement was for...but commenting it out didn't seem to hurt
    //if (!submesh->useSharedVertices)
    //{
        const Ogre::VertexElement* posElem =vertex_data->vertexDeclaration->findElementBySemantic(Ogre::VES_POSITION);
        Ogre::HardwareVertexBufferSharedPtr vbuf= vertex_data->vertexBufferBinding->getBuffer(posElem->getSource());
        const unsigned int vSize = (unsigned int)vbuf->getVertexSize();
        unsigned char* vertex =static_cast<unsigned char*>(vbuf->lock(Ogre::HardwareBuffer::HBL_DISCARD));

        float* pReal;

        //get the vertex data from bullet.
        btSoftBody::tNodeArray&   nodes(softBody->m_nodes);

        //now we simply update the coords of the vertices. Of course this only works
        //properly when the Ogre data and the Bullet data have the same number of vertices
        //and when they are in the same order. It seems that bullet doesn't shuffle the
        //vertices, but this is only an observation!
        for (int j=0;j<nodes.size();++j)
        {
            posElem->baseVertexPointerToElement(vertex, &pReal);
            vertex += vSize;
            *pReal++= nodes[j].m_x.x();
            *pReal++= nodes[j].m_x.y();
            *pReal++= nodes[j].m_x.z();
        }
        vbuf->unlock();
    //}
}

void GraphicsManager::applyPaint(Painter &paint)
{
	std::list<ContactResult> cResults = paint.getCollisions();
	std::list<ContactResult>::iterator itr = cResults.begin();

	for(int i = 0; i < cResults.size(); i++, itr++)
	{
		if(itr->triangleIndex < 0 || itr->triangleIndex > numIndices * 3) continue; //Error Check
		int idx = itr->triangleIndex;
		rsCanvas->convertColourValue(ColourValue(1, 0, 0), colours + indices[idx * 3]);
	}

	vbufColor->writeData(0, vbufColor->getSizeInBytes(), colours, true);
}