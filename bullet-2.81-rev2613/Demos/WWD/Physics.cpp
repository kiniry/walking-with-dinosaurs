
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

#include "Physics.h"
#include "GlutStuff.h"
#include "btBulletDynamicsCommon.h"

#include <stdio.h>
#include "GLDebugDrawer.h"






static GLDebugDrawer gDebugDraw;

				struct   MyContactResultCallback : public btCollisionWorld::ContactResultCallback
				{
					bool m_connected;
					btScalar m_margin;
					MyContactResultCallback() :m_connected(false),m_margin(0.05)
					{
					}
					virtual   btScalar   addSingleResult(btManifoldPoint& cp,   const btCollisionObjectWrapper* colObj0Wrap,int partId0,int index0,const btCollisionObjectWrapper* colObj1Wrap,int partId1,int index1)
					{
						//if (cp.getDistance()<=m_margin)
							m_connected = true;
						return 1.f;
					}
			   };


bool Physics::isLegal(){
		btCollisionObjectArray objects = m_dynamicsWorld->getCollisionObjectArray();


		MyContactResultCallback result;
		result.m_connected=false;

	for (int i = 1; i < objects.size()-1; i++){
		for (int j = i+1; j < objects.size()-1; j++){
			m_dynamicsWorld->contactPairTest(objects.at(i),objects.at(j),result);

			if(result.m_connected == true){
				//fitness = (-1)*(std::numeric_limits<float>::max());
				printf("dohhhh");

				return false;
			}
		}

	}
	return true;
}

void Physics::solveGroundConflicts(){
	bool conflicts = true;
	btCollisionObjectArray objects = m_dynamicsWorld->getCollisionObjectArray();
	MyContactResultCallback result;
		
	while(conflicts){
		result.m_connected=false;
		conflicts=false;
		
		for (int j = 1; j < objects.size(); j++){
			m_dynamicsWorld->contactPairTest(objects.at(0),objects.at(j),result);

			if(result.m_connected == true){
				
				//objects.at(2)->getWorldTransform().setOrigin(objects.at(2)->getWorldTransform().getOrigin());
				//btDefaultMotionState* myMotionState2 = (btDefaultMotionState*)(((btRigidBody*)objects.at(2))->getMotionState());
				//myMotionState2->setWorldTransform(objects.at(2)->getWorldTransform());
				

				//fitness = (-1)*(std::numeric_limits<float>::max());
				//printf("ground conflict\n");
				//printf("box1 x:%f y:%f z:%f\n",objects.at(1)->getWorldTransform().getOrigin().x(),objects.at(1)->getWorldTransform().getOrigin().y(),objects.at(1)->getWorldTransform().getOrigin().z());
				//printf("box2 x:%f y:%f z:%f\n",objects.at(2)->getWorldTransform().getOrigin().x(),objects.at(2)->getWorldTransform().getOrigin().y(),objects.at(2)->getWorldTransform().getOrigin().z());
				btVector3 oldOrigin = objects.at(0)->getWorldTransform().getOrigin();
				oldOrigin.setY(oldOrigin.y()-1);
				objects.at(0)->getWorldTransform().setOrigin(oldOrigin);
				btDefaultMotionState* myMotionState = (btDefaultMotionState*)(((btRigidBody*)objects.at(0))->getMotionState());
				myMotionState->setWorldTransform(objects.at(0)->getWorldTransform());
				
				btTransform ob0 = objects.at(0)->getWorldTransform();

				btTransform ob1 = objects.at(1)->getWorldTransform();

				btTransform ob2 = objects.at(2)->getWorldTransform();

				conflicts=true;
				break;
			}
		}
		setCameraTargetPosition(objects.at(1)->getWorldTransform().getOrigin()); //drunk compiler
		btVector3 cam = getCameraPosition();
		cam.setY(getCameraTargetPosition().y());
		setCameraPosition(cam);
	}
}


void Physics::simulationLoopStep(float stepSize){
	if(theNet!=NULL){
			theNet->computeNetwork();
			for(int i=0;i< (int) subnets.size();i++){
				subnets.at(i)->computeNetwork();
			}
		}

		//fitness test
		calcFitness();

		for(int i=0;i< (int) effectorNNindex.size();i=i+3){
			setEffect(i/3,
				subnets.at(i/3)->getOutput(effectorNNindex.at(i)),
				subnets.at(i/3)->getOutput(effectorNNindex.at(i+1)),
				subnets.at(i/3)->getOutput(effectorNNindex.at(i+2))
				);
		}

		//fixed step... 1ms
		m_dynamicsWorld->stepSimulation(stepSize); 


		for(int i=0; i < (int) sensors.size();i++){
			sensors.at(i)=0;

		}

		//collision detection
		//one per btPersistentManifold for each collision 
		int numManifolds = m_dynamicsWorld->getDispatcher()->getNumManifolds();

		for (int i=0;i<numManifolds;i++)
		{

			btPersistentManifold* contactManifold =  m_dynamicsWorld->getDispatcher()->getManifoldByIndexInternal(i);
			int box1 = (int)contactManifold->getBody0()->getUserPointer();
			int box2 = (int)contactManifold->getBody1()->getUserPointer();

			if(box1 >= 0){
				sensors.at(box1)=1;

			}
			if(box2 >= 0){
				sensors.at(box2)=1;

			}


		}


		//angel sensor
		for(int i = 0; i < m_dynamicsWorld->getNumConstraints(); i++){
			btHingeConstraint* constraint;
			btGeneric6DofConstraint* constraint1;
			float x,y,z; 

			//pointer == -1 if its not a sensor
			if(((int)(m_dynamicsWorld->getConstraint(i)->getUserConstraintPtr()))>=0)
				switch((m_dynamicsWorld->getConstraint(i))->getConstraintType()){

				case HINGE_CONSTRAINT_TYPE:
					constraint = (btHingeConstraint*) m_dynamicsWorld->getConstraint(i);
					sensors.at( ((UserPointerStruct*)(constraint->getUserConstraintPtr()))->sensorIndex );
					x = constraint->getHingeAngle();

					sensors.at(((UserPointerStruct*)constraint->getUserConstraintPtr())->sensorIndex)=x;


					break;
				case D6_CONSTRAINT_TYPE:
					constraint1 = (btGeneric6DofConstraint*) m_dynamicsWorld->getConstraint(i);

					x = constraint1->getRotationalLimitMotor(0)->m_currentPosition;
					y = constraint1->getRotationalLimitMotor(1)->m_currentPosition;
					z = constraint1->getRotationalLimitMotor(2)->m_currentPosition;

					sensors.at(((UserPointerStruct*)constraint1->getUserConstraintPtr())->sensorIndex)=x;
					sensors.at(((UserPointerStruct*)constraint1->getUserConstraintPtr())->sensorIndex+1)=y;
					sensors.at(((UserPointerStruct*)constraint1->getUserConstraintPtr())->sensorIndex+2)=z;


					break;
			}
		}
}

void Physics::runSimulation(){

	if(!isLegal()){
				fitness = -999999;
				timeUsed = 10000;

	}

	solveGroundConflicts();

	while(timeUsed<10000){ //10 s = 10000 ms
	
		simulationLoopStep(1/1000.f);

		timeUsed++;

	}
}

void Physics::clientMoveAndDisplay()
{
	//solveGroundConflicts();
	float ms = getDeltaTimeMicroseconds();
	timeUsed += ms;
	simulationLoopStep(ms / 1000000.f); //normal speed
	//simulationLoopStep(ms / 100000000.f); //slow-mode
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 

	m_dynamicsWorld->debugDrawWorld();

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


	///use the default collision dispatcher. For parallel processing you can use a diffent dispatcher (see Extras/BulletMultiThreaded)
	m_dispatcher = new	btCollisionDispatcher(m_collisionConfiguration);

	m_broadphase = new btDbvtBroadphase();

	///the default constraint solver. For parallel processing you can use a different solver (see Extras/BulletMultiThreaded)
	btSequentialImpulseConstraintSolver* sol = new btSequentialImpulseConstraintSolver;
	m_solver = sol;

	m_dynamicsWorld = new btDiscreteDynamicsWorld(m_dispatcher,m_broadphase,m_solver,m_collisionConfiguration);
	m_dynamicsWorld->setDebugDrawer(&gDebugDraw);

	m_dynamicsWorld->setGravity(btVector3(0,-10,0));

	//create ground body
	btBoxShape* groundShape = new btBoxShape(btVector3(btScalar(1000.),btScalar(10.),btScalar(1000.)));

	m_collisionShapes.push_back(groundShape);

	btTransform groundTransform;
	groundTransform.setIdentity();
	groundTransform.setOrigin(btVector3(0,0,0));

	btRigidBody* ground = localCreateRigidBody(0.,groundTransform,groundShape);
	ground->setUserPointer((void*)(-1));;

	currentBoxIndex++;

	theNet=NULL;
}

//creates a box with side lengths x,y,z
int Physics::createBox(int x1, int y1, int z1){

	// max 10 & min 0.05
	float x=(x1%995+5)/100.f;
	float y=(y1%995+5)/100.f;
	float z=(z1%995+5)/100.f;

	btAssert(x>=0.05 && x<=10);
	btAssert(y>=0.05 && y<=10);
	btAssert(z>=0.05 && z<=10);


	btBoxShape* boxShape = new btBoxShape(btVector3(x/2.f,y/2.f,z/2.f));
	m_collisionShapes.push_back(boxShape);

	btTransform startTransform;
	startTransform.setIdentity();
	btRigidBody* box;
	btScalar mass = btScalar(x*y*z*DensityHuman);
	if(currentBoxIndex == 1){
		startTransform.setOrigin(btVector3(0,0,0));

	}

	box= localCreateRigidBody(mass,startTransform,boxShape);


	box->setUserPointer((void*)-1);

	int returnVal = currentBoxIndex;
	currentBoxIndex++;
	return returnVal;
}


int Physics::createSensor(int boxIndex, int type){
	btRigidBody* box = (btRigidBody*) m_dynamicsWorld->getCollisionObjectArray().at(boxIndex);

	switch (type){
	case pressure:
		box->setCollisionFlags(box->getCollisionFlags() | btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK);			
		sensors.push_back(0);
		box->setUserPointer((void*)(sensors.size()-1));
		break;
	case light:
		break;

	}

	return 0;
}

int Physics::setEffect(int jointIndex, float valueX,float valueY,float valueZ){
	//btBoxShape* hej =(btBoxShape*)((m_dynamicsWorld->getConstraint(jointIndex))->getRigidBodyB().getCollisionShape());
	float userPointer = ((UserPointerStruct*)(m_dynamicsWorld->getConstraint(jointIndex)->getUserConstraintPtr()))->CrossSectionalStrength;
	switch(m_dynamicsWorld->getConstraint(jointIndex)->getConstraintType()){
	case HINGE_CONSTRAINT_TYPE:
		if(valueX>userPointer){
			valueX = userPointer;
		}else if(valueX<-userPointer){
			valueX = -userPointer;
		}
		((btHingeConstraint*)m_dynamicsWorld->getConstraint(jointIndex))->enableAngularMotor(true,btScalar(MAXDWORD),btScalar(valueX));
		return HINGE;
		break;
	case D6_CONSTRAINT_TYPE:
		float absolute = abs(valueX)+abs(valueY)+abs(valueZ);
		float J=1;
		if(absolute>userPointer){
			J=userPointer/absolute;
		}
		((btGeneric6DofConstraint*)m_dynamicsWorld->getConstraint(jointIndex))->getRotationalLimitMotor(0)->m_maxMotorForce=valueX*J;
		((btGeneric6DofConstraint*)m_dynamicsWorld->getConstraint(jointIndex))->getRotationalLimitMotor(1)->m_maxMotorForce=valueY*J;
		((btGeneric6DofConstraint*)m_dynamicsWorld->getConstraint(jointIndex))->getRotationalLimitMotor(2)->m_maxMotorForce=valueZ*J;
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

int Physics::createJoint(	int box1, int box2,	int type,
							int preX, int preY, int preS,
							int postX, int postY, int postS,
							int dofX, int dofY, int dofZ){

							 preX=preX%101;
							 preY=preY%101;
							 preS=preS%6;
							 postX=postX%101;
							 postY=postY%101;
							 postS=postS%6;

							 //tjek input in debug mode
							 btAssert(preX>=0 && preX<=100);							
							 btAssert(preY>=0 && preY<=100);
							 btAssert(preS>-1 && preS<6);

							 btAssert(postX>=0 && postX<=100);							
							 btAssert(postY>=0 && postY<=100);
							 btAssert(postS>-1 && postS<6);


							 //Get box pointers
							 btRigidBody* Box1 = (btRigidBody*) m_dynamicsWorld->getCollisionObjectArray().at(box1);
							 btRigidBody* Box2 = (btRigidBody*) m_dynamicsWorld->getCollisionObjectArray().at(box2);

							 //btCollisionObject* Box1 = m_dynamicsWorld->getCollisionObjectArray().at(box1);
							 //btCollisionObject* Box2 = m_dynamicsWorld->getCollisionObjectArray().at(box2);


							 //Define the local transform on the shapes regarding to the joint.
							 btTransform localBox1, localBox2;
							 localBox1.setIdentity();
							 localBox2.setIdentity();

							 
							 //box1
							 btVector3 halfside1 = ((btBoxShape*)Box1->getCollisionShape())->getHalfExtentsWithMargin();
							 btVector3 center1 =Box1->getWorldTransform().getOrigin();
							 btQuaternion rotation1 =Box1->getWorldTransform().getRotation();

							 btVector3 connection1 = getLocalJointPosition(preX,preY,preS,&halfside1);

							 //translate joint
							 localBox1.setOrigin(connection1);


							 //box2
							 btVector3 halfside2 = ((btBoxShape*)Box2->getCollisionShape())->getHalfExtentsWithMargin();
							 btVector3 connection2 = getLocalJointPosition(postX,postY,postS,&halfside2);
							// btQuaternion rotation2 = getLocalRotation(preS, postS)+rotation1;
							/* 
							 btMatrix3x3 rot1 = btMatrix3x3(rotation1);
							
							 btMatrix3x3 rot2 = btMatrix3x3( getLocalRotation(preS, postS));
							 
								rot1=rot1*rot2;	
								btQuaternion rotation2 = btQuaternion();
								CalculateRotation(&rotation2, &rot1);
							
							*/
								btQuaternion rotation2 = getLocalRotation(preS, postS);
								rotation2*=rotation1;

							 btVector3 center2 = center1+rotate(&connection1,&rotation1)-rotate(&connection2,&rotation2);


							 //rotate and translate box
							 btTransform trans2;
							 trans2.setIdentity();
							 trans2.setRotation(rotation2.inverse());
							 trans2.setOrigin(center2);
							 Box2->setCenterOfMassTransform(trans2);
							 //btDefaultMotionState* myMotionState = (btDefaultMotionState*)(((btRigidBody*)Box2)->getMotionState());
							 //myMotionState->setWorldTransform(Box2->getWorldTransform());
							 //printf("rotate1 x:%f y:%f z:%f\n",rotate(&connection1,&rotation1).x(),rotate(&connection1,&rotation1).y(),rotate(&connection1,&rotation1).z());
							 //printf("cent2 x:%f y:%f z:%f\n",center2.x(),center2.y(),center2.z());
							 //printf("trans2 x:%f y:%f z:%f\n",trans2.getOrigin().x(),trans2.getOrigin().y(),trans2.getOrigin().z());
							 //printf("ibox2 x:%f y:%f z:%f\n",Box2->getWorldTransform().getOrigin().x(),Box2->getWorldTransform().getOrigin().y(),Box2->getWorldTransform().getOrigin().z());
							 //m_dynamicsWorld->updateAabbs();

							 //rotate and translate joint
							 localBox2.setRotation(rotation2);
							
							 localBox2.setOrigin(connection2);




							 //setup contraint/joint
							 btHingeConstraint* hingeC;
							 btGeneric6DofConstraint* gen6C;
							int DOFx = dofX %180;	int DOFy = dofY %180;	int DOFz = dofZ %180;
							// int DOFx = dofX %170+10;	int DOFy = dofY%170+10;	int DOFz = dofZ %170+10;
							 float DOFxR = ((float)DOFx*2*PI)/360; float DOFyR = ((float)DOFy*2*PI)/360; float DOFzR = ((float)DOFz*2*PI)/360;
							// printf("%f %f %f\n", DOFxR,DOFyR,DOFzR);
							 UserPointerStruct* theStruct = new UserPointerStruct();
							 switch(type){
							 case HINGE:
								 hingeC = new btHingeConstraint(*Box1,*Box2,localBox1,localBox2);
								 //hingeC = new btHingeConstraint(*((btRigidBody*) Box1),*((btRigidBody*)Box2),localBox1,localBox2);
								 hingeC->setLimit(btScalar(-DOFxR/2),btScalar(DOFxR/2));
								 m_dynamicsWorld->addConstraint(hingeC,true);

								 sensors.push_back(0);
								 theStruct->sensorIndex=sensors.size()-1;
								 theStruct->CrossSectionalStrength=getCrossSection(postS,&halfside2);
								 hingeC->setUserConstraintPtr(theStruct);
								 //hingeC->setUserConstraintPtr((void*)(sensors.size()-1));
								 //hingeC->setUserConstraintId((int)getCrossSection(postS,&halfside2));//TODO: create struct for userpointer and use float instead
								 
								 break;
							 case GENERIC6DOF:
								 gen6C = new btGeneric6DofConstraint(*Box1,*Box2,localBox1,localBox2,true);
								 //gen6C = new btGeneric6DofConstraint(*((btRigidBody*)Box1),*((btRigidBody*)Box2),localBox1,localBox2,true);
								 gen6C->setLimit(0,0,0);//dist to other box can be set as (0,dist,dist) 
								 gen6C->setLimit(1,0,0);
								 gen6C->setLimit(2,0,0);
								 gen6C->setLimit(3,-DOFxR/2,DOFxR/2);
								 gen6C->setLimit(4,-DOFyR/2,DOFyR/2);
								 gen6C->setLimit(5,-DOFzR/2,DOFzR/2);
								 m_dynamicsWorld->addConstraint(gen6C,true);

								 sensors.push_back(0);
								 sensors.push_back(0);
								 sensors.push_back(0);	

								 theStruct->sensorIndex=sensors.size()-3;
								 theStruct->CrossSectionalStrength=getCrossSection(postS,&halfside2);
								 gen6C->setUserConstraintPtr(theStruct);
								 //gen6C->setUserConstraintPtr((void*)(sensors.size()-3));
								 //gen6C->setUserConstraintId((int)getCrossSection(postS,&halfside2));//TODO: create struct for userpointer and use float instead
								 break;
							 }

							 int returnVal = currentJointIndex;
							 currentJointIndex++;

							 

							 return returnVal;
}

inline float Physics::getCrossSection(int s,btVector3* Halfsize){
	switch(s){
	case 0:
	case 5:
		return Halfsize->x()*2.f*Halfsize->y()*2.f*csa;
		break;
	case 1:
	case 4:
		return Halfsize->z()*2.f*Halfsize->y()*2.f*csa;
		break;
	case 2:
	case 3:
		return Halfsize->x()*2.f*Halfsize->z()*2.f*csa;
		break;
	}
	return 0;
}

//rotates vector3 by a Quaternion
btVector3 Physics::rotate(btVector3* vec, btQuaternion* quant){
	btVector3 result = *vec;

	btMatrix3x3 rot = btMatrix3x3(*quant);

	return result*rot;
}

//ikke vores
void Physics::CalculateRotation( btQuaternion *q, btMatrix3x3  *m ) {
	m->getRow(0);
	m->getRow(1);
	m->getRow(2);
	const int a[3][3] = { (int)m->getRow(0).getX(),(int) m->getRow(0).getY(), (int)m->getRow(0).getZ(), (int)m->getRow(1).getX(),(int) m->getRow(1).getY(), (int)m->getRow(1).getZ(),(int) m->getRow(2).getX(), (int)m->getRow(2).getY(), (int)m->getRow(2).getZ() }; 
  float trace = a[0][0] + a[1][1] + a[2][2]; // I removed + 1.0f; see discussion with Ethan
  if( trace > 0 ) {// I changed M_EPSILON to 0
    float s = 0.5f / sqrtf(trace+ 1.0f);
	q->setW(0.25f / s);
    q->setX(( a[2][1] - a[1][2] ) * s);
    q->setY( ( a[0][2] - a[2][0] ) * s);
    q->setZ( ( a[1][0] - a[0][1] ) * s);
  } else {
    if ( a[0][0] > a[1][1] && a[0][0] > a[2][2] ) {
      float s = 2.0f * sqrtf( 1.0f + a[0][0] - a[1][1] - a[2][2]);
     q->setW((a[2][1] - a[1][2] ) / s);
      q->setX( 0.25f * s);
      q->setY(  (a[0][1] + a[1][0] ) / s);
      q->setZ( (a[0][2] + a[2][0] ) / s);
    } else if (a[1][1] > a[2][2]) {
      float s = 2.0f * sqrtf( 1.0f + a[1][1] - a[0][0] - a[2][2]);
      q->setW( (a[0][2] - a[2][0] ) / s);
     q->setX((a[0][1] + a[1][0] ) / s);
      q->setY(  0.25f * s);
     q->setZ( (a[1][2] + a[2][1] ) / s);
    } else {
      float s = 2.0f * sqrtf( 1.0f + a[2][2] - a[0][0] - a[1][1] );
     q->setW((a[1][0] - a[0][1] ) / s);
     q->setX((a[0][2] + a[2][0] ) / s);
     q->setY(  (a[1][2] + a[2][1] ) / s);
      q->setZ( 0.25f * s);
    }
  }
}

btQuaternion Physics::getLocalRotation(int pre, int post){

	btQuaternion rot;
	if((pre==0 && post==4) || (pre==1 && post==0) || (pre==4 && post==5) || (pre==5 && post==1)){
		//04 10 45 51
		//rot=btQuaternion(PI,0 ,0);
		rot=btQuaternion(PI/2.,0 ,0);
	}else if((pre==4 && post==0) || (pre==0 && post==1) || (pre==5 && post==4) || (pre==1 && post==5)){
		//01 15 40 54
		//rot=btQuaternion(-PI,0 ,0);
		rot=btQuaternion(-PI/2.,0 ,0);
	}else if((pre==0 && post==2) || (pre==2 && post==5) || (pre==3 && post==0) || (pre==5 && post==3)){
		//02 25 30 53
		//rot=btQuaternion(0,PI,0);

		rot=btQuaternion(0,PI/2.,0);
	}else if((pre==2 && post==0) || (pre==5 && post==2) || (pre==0 && post==3) || (pre==3 && post==5)){
		//03 20 35 52
		//rot=btQuaternion(0,-PI,0);

		rot=btQuaternion(0,-PI/2.,0);
	}else if((pre==1 && post==3) || (pre==2 && post==1) || (pre==3 && post==4) || (pre==4 && post==2)){
		//13 21 34 42
		//rot=btQuaternion(0, 0, PI);
		rot=btQuaternion(0, 0, PI/2);
	}else if((pre==3 && post==1) || (pre==1 && post==2) || (pre==4 && post==3) || (pre==2 && post==4)){
		//12 24 31 43
		//rot=btQuaternion(0, 0, -PI);

		rot=btQuaternion(0, 0, -PI/2.);
	}else if(pre+post==5){
		//opposite
		//05 14 23 32 41 50
		rot=btQuaternion(0,0,0);
	}else if(pre==post){
		//equals
		if(pre<2){
			//00 11
			rot=btQuaternion(PI,0,0);
		}else if(pre<4){
			//22 33
			rot=btQuaternion(0,PI,0);
		}else if(pre==4){
			//44
			rot=btQuaternion(0,0,PI);
		}else if (pre==5){
			//55
			rot=btQuaternion(0,PI,0);
		}
	}

	return rot;
}
//calculates the poistion of where the joint connects to the box in regards to the local box center
btVector3 Physics::getLocalJointPosition(int x, int y, int s, btVector3* halfSizes){
	//tjek input in debug mode
	btAssert(x>=0 && x<=100);							
	btAssert(y>=0 && y<=100);
	btAssert(s>-1 && s<6);
	btAssert(halfSizes);



	btVector3 result;

	//calculate offset from center of the side
	double h=(x-50)/50.f;
	double w=(y-50)/50.f;


	switch(s){
	case 0:
		result = btVector3(h*halfSizes->x(),w*halfSizes->y(),-halfSizes->z());
		break;
	case 1:
		result = btVector3(-halfSizes->x(),w*halfSizes->y(),h*halfSizes->z());
		break;
	case 2://x+
		result = btVector3(h*halfSizes->x(),-halfSizes->y(),w*halfSizes->z());
		break;
	case 3://z+
		result = btVector3(h*halfSizes->x(),halfSizes->y(),w*halfSizes->z());
		break;
	case 4://x-
		result = btVector3(halfSizes->x(),w*halfSizes->y(),h*halfSizes->z());
		break;
	case 5://z-
		result = btVector3(h*halfSizes->x(),w*halfSizes->y(),halfSizes->z());
		break;
	default:
		perror("not a legal s value");
		break;
	}

	return result;
}


void	Physics::clientResetScene(){
	exitPhysics();
	initPhysics();
}


void	Physics::exitPhysics(){

	//cleanup in the reverse order of creation/initialization

	//delete NNs
	while(subnets.size()>0){
		delete subnets.at(subnets.size()-1);
		subnets.pop_back();
	}
	theNet->killFirstLayer();
	delete theNet;


	//delete contraints
	while(m_dynamicsWorld->getNumConstraints()>0){
		btTypedConstraint* deathPointer = m_dynamicsWorld->getConstraint(0);
		m_dynamicsWorld->removeConstraint(deathPointer);
		delete deathPointer;
	}

	//remove the rigidbodies from the dynamics world and delete them
	for (int i=m_dynamicsWorld->getNumCollisionObjects()-1; i>=0 ;i--){
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
	for (int j=0;j<m_collisionShapes.size();j++){
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

void Physics::testPhysics(){


	int box2 = createBox(95,895,95);


	int box3 = createBox(195,495,95);


	
	createJoint(box2, box3, GENERIC6DOF,50, 50, 2, 50, 50, 3, 45,45,0);
	
	/*int box4 = createBox(195,195,95);
	createJoint(box3, box4, GENERIC6DOF,50, 50, 5, 50, 50,0, 45,45,0);
		
	int box = createBox(195,195,195);
	createJoint(box3, box, GENERIC6DOF,50, 50, 4, 50, 50, 4, 45,45,0);
	

	int box5 = createBox(95,295,95);
	createJoint(box, box5, GENERIC6DOF,50, 50, 4, 50, 50, 4, 45,45,0);
	
	if(!isLegal()){
		printf("fail!");
	}else{
		printf("legal");
	}

	createSensor(box2, pressure);
 /*
	//NN test
	std::vector<NeuralNode*> inputs;
	
	for(int i=0;i< (int) sensors.size();i++){
		inputs.push_back(new NeuralNode(&sensors.at(i)));
	}

	inputs.push_back(new NeuralNode(1));
	inputs.push_back(new NeuralNode(-2));
	testPoint = new float;
	*testPoint = 5;
	inc = 0;
inputs.push_back(new NeuralNode(testPoint));
	theNet = new NeuralNetwork(inputs);
	theNet->insertNode(SUM,7,1,3,1);
	theNet->insertNode(SIN,0,1);
	theNet->changeLayer();
	theNet->insertNode(PRODUCT,0,1,2,1);
	theNet->stopBuilding();
	*/
}



void Physics::calcFitness(){

	btVector3 origin = m_dynamicsWorld->getCollisionObjectArray().at(1)->getWorldTransform().getOrigin();
	fitness = sqrt((origin.x()*origin.x())+(origin.z()*origin.z()));

}

