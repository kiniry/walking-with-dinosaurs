
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
#include "btBulletDynamicsCommon.h"

#include <stdio.h> //printf debugging
#include "GLDebugDrawer.h"


static GLDebugDrawer gDebugDraw;


void Physics::clientMoveAndDisplay()
{
	//TODO: insert fitness test here
	



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

	//collision detection
	//one per btPersistentManifold for each collision 
	int numManifolds = m_dynamicsWorld->getDispatcher()->getNumManifolds();
	for (int i=0;i<numManifolds;i++)
	{
		btPersistentManifold* contactManifold =  m_dynamicsWorld->getDispatcher()->getManifoldByIndexInternal(i);

		//get all contact points
		/*
		btCollisionObject* obA = static_cast<btCollisionObject*>(contactManifold->getBody0());
		btCollisionObject* obB = static_cast<btCollisionObject*>(contactManifold->getBody1());
	
		int numContacts = contactManifold->getNumContacts();
		for (int j=0;j<numContacts;j++)
		{
			btManifoldPoint& pt = contactManifold->getContactPoint(j);
			if (pt.getDistance()<0.f)
			{
				const btVector3& ptA = pt.getPositionWorldOnA();
				const btVector3& ptB = pt.getPositionWorldOnB();
				const btVector3& normalOnB = pt.m_normalWorldOnB;
			}
		}
		
	*/
		//printf("collision %d %d\n", *((int*)contactManifold->getBody0()->getUserPointer()), *((int*)contactManifold->getBody1()->getUserPointer()));
		
		
		//you can un-comment out this line, and then all points are removed
		//contactManifold->clearManifold();	
	}

	
	//angel sensor
	for(int i = 0; i < m_dynamicsWorld->getNumConstraints(); i++){
		btHingeConstraint* constraint;
		btGeneric6DofConstraint* constraint1;
		float x,y,z; 
		switch((m_dynamicsWorld->getConstraint(i))->getConstraintType()){

			case HINGE_CONSTRAINT_TYPE:
				constraint = (btHingeConstraint*) m_dynamicsWorld->getConstraint(i);
				x = constraint->getHingeAngle();
				printf("%f\n",x);
				break;
			case D6_CONSTRAINT_TYPE:
				constraint1 = (btGeneric6DofConstraint*) m_dynamicsWorld->getConstraint(i);
				x = constraint1->getRotationalLimitMotor(0)->m_currentPosition;
				y = constraint1->getRotationalLimitMotor(1)->m_currentPosition;
				z = constraint1->getRotationalLimitMotor(2)->m_currentPosition;
				printf("%f %f %f\n", x, y, z);
				break;
		}
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

	///create ground body
	btBoxShape* groundShape = new btBoxShape(btVector3(btScalar(1000.),btScalar(1.),btScalar(1000.)));

	m_collisionShapes.push_back(groundShape);

	btTransform groundTransform;
	groundTransform.setIdentity();
	groundTransform.setOrigin(btVector3(0,0,0));

	btRigidBody* ground = localCreateRigidBody(0.,groundTransform,groundShape);
	malloc(sizeof(int));
	int* thePoint = (int*) malloc(sizeof(int));
	*thePoint=-1;
	ground->setUserPointer(thePoint);
	
	currentBoxIndex++;


}

void Physics::testPhysics(){
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
}

int Physics::createBox(int x, int y, int z)
{
	btBoxShape* boxShape = new btBoxShape(btVector3(x,y,z));
	m_collisionShapes.push_back(boxShape);

	btTransform startTransform;
	startTransform.setIdentity();

	btScalar mass = btScalar(x*y*z*KG);

	btRigidBody* box = localCreateRigidBody(mass,startTransform,boxShape);
	int* thePoint = (int*) malloc(sizeof(int));
	*thePoint=-1;
	box->setUserPointer(thePoint);

	int returnVal = currentBoxIndex;
	currentBoxIndex++;
	return returnVal;
}

//skal udvides
int Physics::createSensor(int boxIndex, int type){
	btRigidBody* box = (btRigidBody*) m_dynamicsWorld->getCollisionObjectArray().at(boxIndex);
	int* thePoint = (int*) malloc(sizeof(int));
	*thePoint=boxIndex;
	box->setUserPointer(thePoint);
	switch (type){
		case pressure:
			box->setCollisionFlags(box->getCollisionFlags() | btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK);
			break;
		case light:
			break;

		case angle:

			break;
	}



	return 0;
}

int Physics::setEffect(int jointIndex, int valueX,int valueY,int valueZ){
	switch(m_dynamicsWorld->getConstraint(jointIndex)->getConstraintType()){
	case HINGE_CONSTRAINT_TYPE:
		((btHingeConstraint*)m_dynamicsWorld->getConstraint(jointIndex))->enableAngularMotor(true,btScalar(MAXDWORD),btScalar(valueX));
		return HINGE;
		break;
	/*case CONETWIST_CONSTRAINT_TYPE:
		((btConeTwistConstraint*)m_dynamicsWorld->getConstraint(jointIndex))->setMaxMotorImpulse(valueX*valueX+valueY*valueY+valueZ*valueZ);
		((btConeTwistConstraint*)m_dynamicsWorld->getConstraint(jointIndex))->setMotorTarget(btQuaternion(valueX,valueY,valueZ,100));
		return CONETWIST;
		break;*/
	case D6_CONSTRAINT_TYPE:
		((btGeneric6DofConstraint*)m_dynamicsWorld->getConstraint(jointIndex))->getRotationalLimitMotor(0)->m_maxMotorForce=valueX;
		((btGeneric6DofConstraint*)m_dynamicsWorld->getConstraint(jointIndex))->getRotationalLimitMotor(1)->m_maxMotorForce=valueY;
		((btGeneric6DofConstraint*)m_dynamicsWorld->getConstraint(jointIndex))->getRotationalLimitMotor(2)->m_maxMotorForce=valueZ;
		((btGeneric6DofConstraint*)m_dynamicsWorld->getConstraint(jointIndex))->getRotationalLimitMotor(0)->m_targetVelocity=MAXDWORD;
		((btGeneric6DofConstraint*)m_dynamicsWorld->getConstraint(jointIndex))->getRotationalLimitMotor(1)->m_targetVelocity=MAXDWORD;
		((btGeneric6DofConstraint*)m_dynamicsWorld->getConstraint(jointIndex))->getRotationalLimitMotor(2)->m_targetVelocity=MAXDWORD;
		((btGeneric6DofConstraint*)m_dynamicsWorld->getConstraint(jointIndex))->getRotationalLimitMotor(0)->m_enableMotor=true;
		((btGeneric6DofConstraint*)m_dynamicsWorld->getConstraint(jointIndex))->getRotationalLimitMotor(1)->m_enableMotor=true;
		((btGeneric6DofConstraint*)m_dynamicsWorld->getConstraint(jointIndex))->getRotationalLimitMotor(2)->m_enableMotor=true;
		return GENERIC6DOF;
		break;
	}
	return -1;
}

int Physics::createJoint(	int box1,
							int box2,
							int type,
							int preX, int preY, int preS,
							int postX, int postY, int postS,
							int dofX, int dofY, int dofZ)
{	
	//Get box pointers
	btRigidBody* Box1 = (btRigidBody*) m_dynamicsWorld->getCollisionObjectArray().at(box1);
	btRigidBody* Box2 = (btRigidBody*) m_dynamicsWorld->getCollisionObjectArray().at(box2);

	//Define the local transform on the shapes regarding to the joints. (prolly from the center of the shape)
	btTransform localA, localB;
	localA.setIdentity();localB.setIdentity();
	//Rotation
	setLocalRotation(preS, postS, &localA, &localB);
	//Translation in regards to the boxes.
	btVector3 box1HalfSize = ((btBoxShape*)Box1->getCollisionShape())->getHalfExtentsWithoutMargin();
	btVector3 box2HalfSize = ((btBoxShape*)Box2->getCollisionShape())->getHalfExtentsWithoutMargin();
	localA.setOrigin(getLocalTransform(preX,preY,preS,&box1HalfSize));
	localB.setOrigin(getLocalTransform(postX,postY,postS,&box2HalfSize));

	//setup contraint/joint
	btHingeConstraint* hingeC;
	btConeTwistConstraint* coneC;
	btGeneric6DofConstraint* gen6C;
	int DOFx = dofX %180;	int DOFy = dofY %180;	int DOFz = dofZ %180;
	float DOFxR = ((float)DOFx*2*PI)/360; float DOFyR = ((float)DOFy*2*PI)/360; float DOFzR = ((float)DOFz*2*PI)/360;
	switch(type){
	case HINGE:
		hingeC = new btHingeConstraint(*Box1,*Box2,localA,localB);
		hingeC->setLimit(btScalar(-DOFxR/2),btScalar(DOFxR/2));
		m_dynamicsWorld->addConstraint(hingeC,true);
		break;
	/*case CONETWIST: //use GENERIC6DOF instead
		coneC = new btConeTwistConstraint(*Box1,*Box2,localA,localB);
		coneC->setLimit(btScalar(DOFxR),btScalar(DOFyR),btScalar(DOFzR));
		m_dynamicsWorld->addConstraint(coneC,true);
		break;*/
	case GENERIC6DOF:
		gen6C = new btGeneric6DofConstraint(*Box1,*Box2,localA,localB,true);
		gen6C->setLimit(0,0,0);//dist to other box can be set as (0,dist,dist) if wanted
		gen6C->setLimit(1,0,0);
		gen6C->setLimit(2,0,0);
		gen6C->setLimit(3,-DOFxR/2,DOFxR/2);
		gen6C->setLimit(4,-DOFyR/2,DOFyR/2);
		gen6C->setLimit(5,-DOFzR/2,DOFzR/2);
		m_dynamicsWorld->addConstraint(gen6C,true);
		break;
	}
	
	int returnVal = currentJointIndex;
	currentJointIndex++;
	return returnVal;
}

void Physics::setLocalRotation(int myS, int opS, btTransform* myTrans, btTransform* opTrans)
{
	myTrans->getBasis().setEulerZYX(0,0,0);
	//if equal
	if(myS==opS&&myS!=0&&myS!=1){

		opTrans->getBasis().setEulerZYX(0,0,PI);
	}
	else if(myS==opS){
		opTrans->getBasis().setEulerZYX(0,PI,0);
	}
	//02 04 12 14
	else if(myS==4&&opS==0 || myS==0&&opS==2 || myS==2&&opS==1 || myS==1&&opS==4){
		opTrans->getBasis().setEulerZYX(0,PI/2,0);
	}
	else if(myS==0&&opS==4 || myS==2&&opS==0 || myS==1&&opS==2 || myS==4&&opS==1){
		opTrans->getBasis().setEulerZYX(0,-PI/2,0);
	}
	//03 05 13 15
	else if(myS==3&&opS==1 || myS==1&&opS==5 || myS==0&&opS==3 || myS==5&&opS==0){
		opTrans->getBasis().setEulerZYX(-PI/2,0,0);
	}
	else if(myS==1&&opS==3 || myS==5&&opS==1 || myS==3&&opS==0 || myS==0&&opS==5){
		opTrans->getBasis().setEulerZYX(PI/2,0,0);
	}
	//23 25 43 45
	else if(myS==2&&opS==3 || myS==5&&opS==2 || myS==3&&opS==4 || myS==4&&opS==5){
		opTrans->getBasis().setEulerZYX(0,0,-PI/2);
	}
	else if(myS==3&&opS==2 || myS==2&&opS==5 || myS==4&&opS==3 || myS==5&&opS==4){
		opTrans->getBasis().setEulerZYX(0,0,PI/2);
	}
	//if opposite
	else{
		opTrans->getBasis().setEulerZYX(0,0,0);
	}
}


btVector3 Physics::getLocalTransform(float x, float y, int s, btVector3* halfSizes)
{
	switch(s){
	case 0://bottom (y-)
		return btVector3(x,y,-halfSizes->z());
		break;
	case 1://top (y+)
		return btVector3(x,y,halfSizes->z());
		break;
	case 2://x+
		return btVector3(halfSizes->x(),x,y);
		break;
	case 3://z+
		return btVector3(x,halfSizes->y(),y);
		break;
	case 4://x-
		return btVector3(-halfSizes->x(),x,y);
		break;
	case 5://z-
		return btVector3(x,-halfSizes->y(),y);
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




