
///create 125 (5x5x5) dynamic object
#define ARRAY_SIZE_X 5
#define ARRAY_SIZE_Y 5
#define ARRAY_SIZE_Z 5

//maximum number of objects (and allow user to shoot additional boxes)
#define MAX_PROXIES (ARRAY_SIZE_X*ARRAY_SIZE_Y*ARRAY_SIZE_Z + 1024)

///scaling of the objects (0.1 = 20 centimeter boxes )
#define SCALING 1.
#define START_POS_X -5
#define START_POS_Y -5
#define START_POS_Z -3
#define KG 1

#include "Physics.h"
#include "GlutStuff.h"
///btBulletDynamicsCommon.h is the main Bullet include file, contains most common include files.
#include "btBulletDynamicsCommon.h"

#include <stdio.h> //printf debugging
#include "GLDebugDrawer.h"

static GLDebugDrawer gDebugDraw;


void Physics::clientMoveAndDisplay()
{
	//TODO: insert fitness test here
	int nr = 1;
	btVector3 pos = m_dynamicsWorld->getCollisionObjectArray().at(nr)->getWorldTransform().operator*(btVector3(1,1,1));
	printf("%f",pos.y());
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 

	//simple dynamics world doesn't handle fixed-time-stepping
	float ms = getDeltaTimeMicroseconds();
	
	///step the simulation
	if (m_dynamicsWorld)
	{
		m_dynamicsWorld->stepSimulation(ms / 1000000.f);
		//optional but useful: debug drawing
		m_dynamicsWorld->debugDrawWorld();
	}
		
	renderme(); 

	glFlush();

	swapBuffers();

}



void Physics::displayCallback(void) {

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 
	
	renderme();

	//optional but useful: debug drawing to detect problems
	if (m_dynamicsWorld)
		m_dynamicsWorld->debugDrawWorld();

	glFlush();
	swapBuffers();
}





void	Physics::initPhysics()
{
	currentBoxIndex=0;
	currentJointIndex=0;
	setTexturing(true);
	setShadows(true);

	setCameraDistance(btScalar(SCALING*20.));

	///collision configuration contains default setup for memory, collision setup
	m_collisionConfiguration = new btDefaultCollisionConfiguration();
	//m_collisionConfiguration->setConvexConvexMultipointIterations();

	///use the default collision dispatcher. For parallel processing you can use a diffent dispatcher (see Extras/BulletMultiThreaded)
	m_dispatcher = new	btCollisionDispatcher(m_collisionConfiguration);

	m_broadphase = new btDbvtBroadphase();

	///the default constraint solver. For parallel processing you can use a different solver (see Extras/BulletMultiThreaded)
	btSequentialImpulseConstraintSolver* sol = new btSequentialImpulseConstraintSolver;
	m_solver = sol;

	m_dynamicsWorld = new btDiscreteDynamicsWorld(m_dispatcher,m_broadphase,m_solver,m_collisionConfiguration);
	m_dynamicsWorld->setDebugDrawer(&gDebugDraw);
	
	m_dynamicsWorld->setGravity(btVector3(0,-10,0));

	///create ground bodies
	btBoxShape* groundShape = new btBoxShape(btVector3(btScalar(1000.),btScalar(1.),btScalar(1000.)));
	m_collisionShapes.push_back(groundShape);

	btTransform groundTransform;
	groundTransform.setIdentity();
	groundTransform.setOrigin(btVector3(0,0,0));

	localCreateRigidBody(0.,groundTransform,groundShape);
	currentBoxIndex++;

	///try
	//btBoxShape* colShape = new btBoxShape(btVector3(SCALING*1,SCALING*2,SCALING*1));
	btCapsuleShape* colShape = new btCapsuleShape(btScalar(1),btScalar(2));
	m_collisionShapes.push_back(colShape);

	btTransform startTransform;
	startTransform.setIdentity();
	startTransform.setOrigin(btVector3(0,5,0));

	btScalar	mass(1.f);
	
	btRigidBody* box1 = localCreateRigidBody(mass,startTransform,colShape);
	currentBoxIndex++;

	//startTransform.setOrigin(btVector3(5,5,0));
	btRigidBody* box2 = localCreateRigidBody(mass,startTransform,colShape);
	currentBoxIndex++;
	//box2->applyCentralForce(btVector3(-1001,0,0));
	btRigidBody* box3 = localCreateRigidBody(mass,startTransform,colShape);
	currentBoxIndex++;
	//Define the two types of constraints/joints
	btHingeConstraint* hingeC;
	btConeTwistConstraint* coneC;

	//Define the local transform on the shapes regarding to the joints. (prolly from the center of the shape)
	btTransform localA, localB;
	localA.setIdentity();localB.setIdentity();
	//Rotation - SUBJECT TO CHANGE!
	localA.getBasis().setEulerZYX(0,0,0);
	localB.getBasis().setEulerZYX(0,0,0);
	//Translation in regards to the boxes.
	localA.setOrigin(btVector3(SCALING*0,SCALING*2,SCALING*0));
	localB.setOrigin(btVector3(SCALING*0,SCALING*-2,SCALING*0));

	//setup contraint/joint
	hingeC = new btHingeConstraint(*box1,*box2,localA,localB);
	hingeC->setLimit(btScalar(-PI/2),btScalar(PI/2));
	m_dynamicsWorld->addConstraint(hingeC,true);
	currentJointIndex++;

	hingeC = new btHingeConstraint(*box3,*box1,localA,localB);
	hingeC->setLimit(btScalar(-PI/2),btScalar(PI/2));
	m_dynamicsWorld->addConstraint(hingeC,true);
	currentJointIndex++;

	//test of createBox
	int b1 = createBox(2,1,4);
	int b2 = createBox(2,4,2);

	createJoint(b1,	b2,	HINGE,
				1, 1, 0,
				3, 1, 3,
				30, 0, 500);

	//how to get a constraint at a specific index, and set its breaking threshold
	//m_dynamicsWorld->getConstraint(0)->setBreakingImpulseThreshold(btScalar(10));

	/*{
		//create a few dynamic rigidbodies
		// Re-using the same collision is better for memory usage and performance

		btBoxShape* colShape = new btBoxShape(btVector3(SCALING*1,SCALING*1,SCALING*1));
		//btCollisionShape* colShape = new btSphereShape(btScalar(1.));
		m_collisionShapes.push_back(colShape);

		/// Create Dynamic Objects
		btTransform startTransform;
		startTransform.setIdentity();

		btScalar	mass(1.f);

		float start_x = START_POS_X - ARRAY_SIZE_X/2;
		float start_y = START_POS_Y;
		float start_z = START_POS_Z - ARRAY_SIZE_Z/2;

		for (int k=0;k<ARRAY_SIZE_Y;k++)
		{
			for (int i=0;i<ARRAY_SIZE_X;i++)
			{
				for(int j = 0;j<ARRAY_SIZE_Z;j++)
				{
					startTransform.setOrigin(SCALING*btVector3(
										btScalar(2.0*i + start_x),
										btScalar(20+2.0*k + start_y),
										btScalar(2.0*j + start_z)));

					localCreateRigidBody(mass,startTransform,colShape);
				}
			}
		}
	}*/


}
int Physics::createBox(float x, float y, float z)
{
	btBoxShape* boxShape = new btBoxShape(btVector3(x,y,z));
	m_collisionShapes.push_back(boxShape);

	btTransform startTransform;
	startTransform.setIdentity();

	btScalar mass = btScalar(x*y*z*KG);

	btRigidBody* box1 = localCreateRigidBody(mass,startTransform,boxShape);
	
	int returnVal = currentBoxIndex;
	currentBoxIndex++;
	return returnVal;
}

int Physics::createJoint(	int box1,
							int box2,
							int type,
							float preX, float preY, float preS,
							float postX, float postY, float postS,
							float dofX, float dofY, float dofZ)
{	
	//Get box pointers
	btRigidBody* Box1 = (btRigidBody*) m_dynamicsWorld->getCollisionObjectArray().at(box1);
	btRigidBody* Box2 = (btRigidBody*) m_dynamicsWorld->getCollisionObjectArray().at(box2);

	//Define the local transform on the shapes regarding to the joints. (prolly from the center of the shape)
	btTransform localA, localB;
	localA.setIdentity();localB.setIdentity();
	//Rotation - SUBJECT TO CHANGE!
	localA.getBasis().setEulerZYX(0,0,0);
	localB.getBasis().setEulerZYX(0,0,0);
	//Translation in regards to the boxes.
	btVector3 box1HalfSize = ((btBoxShape*)Box1->getCollisionShape())->getHalfExtentsWithoutMargin();
	btVector3 box2HalfSize = ((btBoxShape*)Box2->getCollisionShape())->getHalfExtentsWithoutMargin();
	localA.setOrigin(getLocalTransform(preX,preY,preS,&box1HalfSize));
	localB.setOrigin(getLocalTransform(postX,postY,postS,&box2HalfSize));

	//setup contraint/joint
	btHingeConstraint* hingeC;
	btConeTwistConstraint* coneC;
	int DOFx = ((int)dofX) %180;	int DOFy = ((int)dofY) %180;	int DOFz = ((int)dofZ) %180;
	float DOFxR = ((float)DOFx*2*PI)/360; float DOFyR = ((float)DOFy*2*PI)/360; float DOFzR = ((float)DOFz*2*PI)/360;
	switch(type){
	case HINGE:
		hingeC = new btHingeConstraint(*Box1,*Box2,localA,localB);
		hingeC->setLimit(btScalar(-DOFxR/2),btScalar(DOFxR/2));
		m_dynamicsWorld->addConstraint(hingeC,true);
		break;
	case CONETWIST:
		coneC = new btConeTwistConstraint(*Box1,*Box2,localA,localB);
		coneC->setLimit(btScalar(DOFxR),btScalar(DOFyR),btScalar(DOFzR));
		m_dynamicsWorld->addConstraint(coneC,true);
		break; 
	}
	
	currentJointIndex++;

	int returnVal = currentJointIndex;
	currentJointIndex++;
	return returnVal;
}

btVector3 Physics::getLocalTransform(float x, float y, int s, btVector3* halfSizes)
{
	switch(s){
	case 0://bottom (y-)
		return btVector3(x,-halfSizes->y(),y);
		break;
	case 1://top (y+)
		return btVector3(x,halfSizes->y(),y);
		break;
	case 2://x+
		return btVector3(halfSizes->x(),y,y);
		break;
	case 3://z+
		return btVector3(x,y,halfSizes->z());
		break;
	case 4://x-
		return btVector3(-halfSizes->x(),x,y);
		break;
	case 5://z-
		return btVector3(x,y,-halfSizes->z());
		break;
	}
}

void	Physics::clientResetScene()
{
	exitPhysics();
	initPhysics();
}
	

void	Physics::exitPhysics()
{

	//cleanup in the reverse order of creation/initialization

	//remove the rigidbodies from the dynamics world and delete them
	int i;
	for (i=m_dynamicsWorld->getNumCollisionObjects()-1; i>=0 ;i--)
	{
		btCollisionObject* obj = m_dynamicsWorld->getCollisionObjectArray()[i];
		btRigidBody* body = btRigidBody::upcast(obj);
		if (body && body->getMotionState())
		{
			delete body->getMotionState();
		}
		m_dynamicsWorld->removeCollisionObject( obj );
		delete obj;
	}

	//delete collision shapes
	for (int j=0;j<m_collisionShapes.size();j++)
	{
		btCollisionShape* shape = m_collisionShapes[j];
		delete shape;
	}
	m_collisionShapes.clear();

	delete m_dynamicsWorld;
	
	delete m_solver;
	
	delete m_broadphase;
	
	delete m_dispatcher;

	delete m_collisionConfiguration;

	
}




