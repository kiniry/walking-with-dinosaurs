
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
	

	//Sleep(200);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 

	//simple dynamics world doesn't handle fixed-time-stepping
	float ms = getDeltaTimeMicroseconds();
	timeUsed += ms;


	///step the simulation
	m_dynamicsWorld->stepSimulation(ms / 1000000.f);
	//optional but useful: debug drawing
	m_dynamicsWorld->debugDrawWorld();
	


	//collision detection
	//one per btPersistentManifold for each collision 
	int numManifolds = m_dynamicsWorld->getDispatcher()->getNumManifolds();
	for (int i=0;i<numManifolds;i++)
	{
		btPersistentManifold* contactManifold =  m_dynamicsWorld->getDispatcher()->getManifoldByIndexInternal(i);

		//printf("collision between box %d and %d\n", *((int*)contactManifold->getBody0()->getUserPointer()), *((int*)contactManifold->getBody1()->getUserPointer()));
			
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
				//printf("%f %f %f\n", x, y, z);
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
	timeUsed=0;
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


int Physics::createBox(int x, int y, int z)
{
	btBoxShape* boxShape = new btBoxShape(btVector3(x,y,z));
	m_collisionShapes.push_back(boxShape);

	btTransform startTransform;
	startTransform.setIdentity();
	btRigidBody* box;
	btScalar mass = btScalar(x*y*z*KG);
	if(currentBoxIndex == 1){
		startTransform.setOrigin(btVector3(0,10,0));

	}
	
	//box= localCreateRigidBody(0,startTransform,boxShape);
	box= localCreateRigidBody(mass,startTransform,boxShape);

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

	//Define the local transform on the shapes regarding to the joint. (properbly from the center of the shape)
	btTransform localBox1, localBox2;
	localBox1.setIdentity();
	localBox2.setIdentity();


	//box1
	btVector3 halfside1 = ((btBoxShape*)Box1->getCollisionShape())->getHalfExtentsWithoutMargin();
	btVector3 center1 =	Box1->getCenterOfMassPosition();
	btQuaternion rotation1 = Box1->getCenterOfMassTransform().getRotation();
	btVector3 connection1 = getLocalJointPosition(preX,preY,preS,&halfside1);
	
	//translate joint
	localBox1.setOrigin(connection1);


	//box2
	btVector3 halfside2 = ((btBoxShape*)Box2->getCollisionShape())->getHalfExtentsWithoutMargin();
	btVector3 connection2 = getLocalJointPosition(postX,postY,postS,&halfside2);
	btQuaternion rotation2 = getLocalRotation(preS, postS)+rotation1;
	btVector3 center2 = center1+rotate(&connection1,&rotation1)+rotate(&connection2,&rotation2);
	


	//rotate and translate box
	btTransform trans2;
	trans2.setIdentity();
	trans2.setRotation(rotation2);
	trans2.setOrigin(center2);
	Box2->setCenterOfMassTransform(trans2);

	//rotate and translate joint
	localBox2.setRotation(rotation2);
	localBox2.setOrigin(connection2);




		
	//setup contraint/joint
	btHingeConstraint* hingeC;
	btGeneric6DofConstraint* gen6C;
	int DOFx = dofX %180;	int DOFy = dofY %180;	int DOFz = dofZ %180;
	float DOFxR = ((float)DOFx*2*PI)/360; float DOFyR = ((float)DOFy*2*PI)/360; float DOFzR = ((float)DOFz*2*PI)/360;
	switch(type){
	case HINGE:
		hingeC = new btHingeConstraint(*Box1,*Box2,localBox1,localBox2);
		hingeC->setLimit(btScalar(-DOFxR/2),btScalar(DOFxR/2));
		m_dynamicsWorld->addConstraint(hingeC,true);
		break;
	case GENERIC6DOF:
		gen6C = new btGeneric6DofConstraint(*Box1,*Box2,localBox1,localBox2,true);
		gen6C->setLimit(0,0,0);//dist to other box can be set as (0,dist,dist) 
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

//rotates vec by quant
btVector3 Physics::rotate(btVector3* vec, btQuaternion* quant){
	btVector3 result = *vec;
	
	
	return result;
}

btQuaternion Physics::getLocalRotation(int pre, int post){

	btQuaternion rot;


	if((pre==0 && post==4) || (pre==1 && post==0) || (pre==4 && post==5) || (pre==5 && post==1)){
	//04 10 45 51
	rot=btQuaternion(PI,0 ,0);
	}else if((pre==4 && post==0) || (pre==0 && post==1) || (pre==5 && post==4) || (pre==1 && post==5)){
	//01 15 40 54
	rot=btQuaternion(-PI,0 ,0);
	}else if((pre==0 && post==2) || (pre==2 && post==5) || (pre==3 && post==0) || (pre==5 && post==3)){
	//02 25 30 53
	rot=btQuaternion(0,PI,0);
	}else if((pre==2 && post==0) || (pre==5 && post==2) || (pre==0 && post==3) || (pre==3 && post==5)){
	//03 20 35 52
	rot=btQuaternion(0,-PI,0);
	}else if((pre==1 && post==3) || (pre==2 && post==1) || (pre==3 && post==4) || (pre==4 && post==2)){
	//13 21 34 42
	rot=btQuaternion(0, 0, PI);
	}else if((pre==3 && post==1) || (pre==1 && post==2) || (pre==4 && post==3) || (pre==2 && post==4)){
	//12 24 31 43
	rot=btQuaternion(0, 0, -PI);
	}else if(pre+post==5){
	//opposite
	//05 14 23 32 41 50
	rot=btQuaternion(0,0,0);
	}else if(pre==post){
		//equals
		if(pre<2){
		//00 11
		rot=btQuaternion(2*PI,0,0);
		}else if(pre<4){
		//22 33
		rot=btQuaternion(0,2*PI,0);
		}else if(pre<6){
		//44 55
		rot=btQuaternion(0,0,2*PI);
		}
	}
	
	return rot;
}
//calculates the poistion of where the joint connects to the box in regards to the local box origo
//TODO: �ndre v�rdier s� de fungere som procent af sidderne i stedet for absolutte
btVector3 Physics::getLocalJointPosition(float x, float y, int s, btVector3* halfSizes)
{
	switch(s){
	case 0://bottom (y-)
		return btVector3(x,y,-halfSizes->z());
		break;
	case 1://top (y+)
		return btVector3(-halfSizes->x(),x,y);
		break;
	case 2://x+
		return btVector3(x,-halfSizes->y(),y);
		break;
	case 3://z+
		return btVector3(x,halfSizes->y(),y);
		break;
	case 4://x-
		return btVector3(halfSizes->x(),x,y);
		break;
	case 5://z-
		return btVector3(x,y,halfSizes->z());
		
		break;
	default:
		perror("not a legal s value");
		break;
	}
}


void	Physics::clientResetScene()
{
	exitPhysics();
	initPhysics();
	testPhysics();
}
	
void Physics::testPhysics(){
	int box = createBox(3,3,1);
	int box2 = createBox(3,3,1);
	//int box3 = WWDPhysics.createBox(3,3,1);
	createJoint(box, box2, GENERIC6DOF,0.5, 0.5, 1, 0.5, 0.5, 4, 45,45,0);
	//WWDPhysics.createJoint(box, box3, GENERIC6DOF,0.5, 0.5, 5, 0.5, 0.5, 1, 45,45,0);

	//WWDPhysics.createSensor(box, pressure);
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




