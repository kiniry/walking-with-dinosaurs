#include "Physics.h"

void Physics::pointCamera(){
	btVector3 aPoint = calcPosition();
	//setCameraPosition(btVector3(aPoint.x(),aPoint.y()-100,aPoint.z()-100));
	setCameraTargetPosition(aPoint);
	//setCameraDistance(30);
}

float Physics::getBoxHalfHeight(btCollisionObject* object){
	btRigidBody* body = (btRigidBody*) object;
	btBoxShape* box = (btBoxShape*) body->getCollisionShape();;

	btQuaternion rot = body->getWorldTransform().getRotation().inverse();
	btVector3 sides = box->getHalfExtentsWithMargin();

	btVector3 x = btVector3(sides.getX(),0,0);
	btVector3 y = btVector3(0,sides.getY(),0);
	btVector3 z = btVector3(0,0,sides.getZ());

	return abs(rotate(&x, &rot).getY())+abs(rotate(&y, &rot).getY())+abs(rotate(&z, &rot).getY());
}


void Physics::calcSize(){
	btCollisionObjectArray objs = m_dynamicsWorld->getCollisionObjectArray();
	heighstPoint = -99999;
	lowestPoint = 99999;
	height = 0;
	for (int i = 1; i < objs.size(); i++){
		float halfHeight = getBoxHalfHeight(objs.at(i));
		float y = objs.at(i)->getWorldTransform().getOrigin().getY();
		if(y-halfHeight < lowestPoint){
			lowestPoint=y-halfHeight;
		}
		if(y+halfHeight > heighstPoint){
			heighstPoint=y+halfHeight;
		}
	}
	height= heighstPoint-lowestPoint;
	if(height<=0){
		heighstPoint = 99999;
		lowestPoint = -99999;
		height= heighstPoint-lowestPoint;
	}
}

btVector3 Physics::calcPosition(){
	float mass=0;
	for(int i =1; i < m_dynamicsWorld->getCollisionObjectArray().size(); i++){

		mass+=1/ ((btRigidBody*)m_dynamicsWorld->getCollisionObjectArray().at(i))->getInvMass();

	}
	btVector3 point =btVector3(0,0,0);
	for(int i =1; i < m_dynamicsWorld->getCollisionObjectArray().size(); i++){

		btVector3 origin = m_dynamicsWorld->getCollisionObjectArray().at(i)->getWorldTransform().getOrigin();
		float percentage =(1/(((btRigidBody*)m_dynamicsWorld->getCollisionObjectArray().at(i))->getInvMass()))/mass;
		
		point.setX(point.getX()+ origin.getX()*percentage);
		point.setY(point.getY()+ origin.getY()*percentage);
		point.setZ(point.getZ()+ origin.getZ()*percentage);
	}

	return point;
}

bool Physics::checkInternCollissions(){

	//check for intern collisions
	btCollisionObjectArray objects = m_dynamicsWorld->getCollisionObjectArray();

	MyContactResultCallback result;
	result.m_connected=false;

	for (int i = 1; i < objects.size(); i++){
		for (int j = i+1; j < objects.size(); j++){
			m_dynamicsWorld->contactPairTest(objects.at(i),objects.at(j),result);

			if(result.m_connected == true){
				printf("dohhhh");

				return false;
			}
		}
	}
	return true;
}

bool Physics::checkHeight(){
	//check for height
	calcSize();
	if(height>=15){
		printf("height faild");
		totaltime=0;
		return false;
	}
	totaltime=0;
	return true;
}

void Physics::checkForDismemberment(){
	bool enabled = true;
	for(int i=0;i<m_dynamicsWorld->getNumConstraints();i++){
		if(!m_dynamicsWorld->getConstraint(i)->isEnabled()){
			enabled = false;
			printf("broken\n");
			break;
		}
	}
	if(!enabled){
		fitness = -999999;
	}
}

/**
* moves ground down to avoid collisions from the beging and things stuck in ground
*/
void Physics::solveGroundConflicts(){
	calcSize();

	//startPoint=calcPosition();


	btCollisionObjectArray objects = m_dynamicsWorld->getCollisionObjectArray();

	btVector3 origin = objects.at(0)->getWorldTransform().getOrigin();
	origin.setY(lowestPoint-((btBoxShape*)objects.at(0)->getCollisionShape())->getHalfExtentsWithMargin().getY());
	objects.at(0)->getWorldTransform().setOrigin(origin);
	btDefaultMotionState* myMotionState = (btDefaultMotionState*)(((btRigidBody*)objects.at(0))->getMotionState());
	myMotionState->setWorldTransform(objects.at(0)->getWorldTransform());

	groundY =  m_dynamicsWorld->getCollisionObjectArray().at(0)->getWorldTransform().getOrigin().getY()+((btBoxShape*)objects.at(0)->getCollisionShape())->getHalfExtentsWithMargin().getY();
	/*
	bool conflicts = true;
	btCollisionObjectArray objects = m_dynamicsWorld->getCollisionObjectArray();
	MyContactResultCallback result;

	while(conflicts){
	result.m_connected=false;
	conflicts=false;

	for (int j = 1; j < objects.size(); j++){
	m_dynamicsWorld->contactPairTest(objects.at(0),objects.at(j),result);

	if(result.m_connected == true){
	btVector3 oldOrigin = objects.at(0)->getWorldTransform().getOrigin();
	oldOrigin.setY(oldOrigin.y()-1);
	objects.at(0)->getWorldTransform().setOrigin(oldOrigin);
	btDefaultMotionState* myMotionState = (btDefaultMotionState*)(((btRigidBody*)objects.at(0))->getMotionState());
	myMotionState->setWorldTransform(objects.at(0)->getWorldTransform());

	conflicts=true;
	break;
	}
	}
	setCameraTargetPosition(objects.at(1)->getWorldTransform().getOrigin()); //drunk compiler
	btVector3 cam = getCameraPosition();
	cam.setY(getCameraTargetPosition().y());
	setCameraPosition(cam);
	}
	*/
	pointCamera();
}

void Physics::simulationLoopStep(float stepSize){
	totaltime += stepSize*1000;
	if(m_dynamicsWorld->getNumCollisionObjects()>1){
		if(theNet!=NULL){
			theNet->computeNetwork();
			for(int i=0;i< (int) subnets.size();i++){
				subnets.at(i)->computeNetwork();
			}
		}
		if(enableEffectors){
			for(int i=0;i< (int) effectorNNindex.size();i=i+3){
			#ifdef NNMAINONLY
				setEffect(i/3,
					theNet->getOutput(effectorNNindex.at(i)),
					theNet->getOutput(effectorNNindex.at(i+1)),
					theNet->getOutput(effectorNNindex.at(i+2))
					);
			#else
				setEffect(i/3,
					subnets.at(i/3)->getOutput(effectorNNindex.at(i)),
					subnets.at(i/3)->getOutput(effectorNNindex.at(i+1)),
					subnets.at(i/3)->getOutput(effectorNNindex.at(i+2))
					);
			
			#endif
			}
		}

		//fitness test
		calcFitness(testType);

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
		btGeneric6DofConstraint* constraint1;
		float x,y,z;

		//pointer == -1 if its not a sensor
		if(((int)(m_dynamicsWorld->getConstraint(i)->getUserConstraintPtr()))>=0)
			switch((m_dynamicsWorld->getConstraint(i))->getConstraintType()){
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

	/*if(m_dynamicsWorld->getNumConstraints()>0){
	for(int i=m_dynamicsWorld->getNumConstraints()-1;i>=0;i--){
	btTypedConstraint* con = m_dynamicsWorld->getConstraint(i);
	con->enableFeedback(true);
	if(con->getAppliedImpulse()>m_dynamicsWorld->getConstraint(i)->getBreakingImpulseThreshold()){
	m_dynamicsWorld->removeConstraint(m_dynamicsWorld->getConstraint(i));
	}
	else{
	con->enableFeedback(false);
	}
	}
	}*/
}

bool Physics::relaxCreature(){
	enableEffectors=false;
	btCollisionObject* ground = m_dynamicsWorld->getCollisionObjectArray().at(0);
	btScalar friction = ground->getFriction();
	ground->setFriction(0);
	float last = 0.f;
	int count = 0;int totalCount=0;
	while(count<20){
		simulationLoopStep(1/1000.f);
		float center = calcPosition().y();
		if(center<(last+0.0000001f)&&center>(last-0.0000001f)){count++;}
		else{count=0;}
		last = center;
		if(totalCount>20000){
			ground->setFriction(friction);
			startPoint=calcPosition();
			pastPoint=startPoint;
			enableEffectors=true;
			fitness=0;
			printf("relax failed");
			return false;
		}
		totalCount++;
	}
	ground->setFriction(friction);
	startPoint=calcPosition();
	pastPoint=startPoint;
	enableEffectors=true;
	fitness=0;
	return true;
}

void Physics::runSimulation(){

	solveGroundConflicts();

	if(checkInternCollissions() && relaxCreature() && checkHeight()){
			while(totaltime<simulationTime){
				simulationLoopStep(1/1000.f);
			}
	}else{
			fitness = -999999;
	}

	checkForDismemberment();
}

void Physics::clientMoveAndDisplay(boolean fixed)
{
	//solveGroundConflicts();
	float ms = getDeltaTimeMicroseconds();
	if(fixed){
		simulationLoopStep(1 / 1000.f);
	}else{
	simulationLoopStep(ms / 1000000.f); //normal speed
	//simulationLoopStep(ms / 100000000.f); //slow-mode
	}
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	m_dynamicsWorld->debugDrawWorld();

	renderme();

	glFlush();

#ifdef _CONSOLE
	swapBuffers();
#endif

	pointCamera();
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

void Physics::showInfo(int& xOffset,int& yStart, int yIncr){
	char blockTime[128];

	sprintf(blockTime,"--- Total running time: %lu ms ---", totaltime );
	displayProfileString(xOffset,yStart,blockTime);
	yStart += yIncr;

	sprintf(blockTime,"--- Current fitness value: %f ---", fitness );
	displayProfileString(xOffset,yStart,blockTime);
	yStart += yIncr;

	sprintf(blockTime,"-------------------------------------------------");
	displayProfileString(xOffset,yStart,blockTime);
	yStart += yIncr;
}

void	Physics::initPhysics(){
	testType = move;
	totaltime=0;
	currentBoxIndex=0;
	currentJointIndex=0;
	noBoxes =0;
	fitness=0;
	enableEffectors=true;

	setTexturing(true);
	setShadows(true);

	setCameraDistance(btScalar(20.));

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
	int* userP = new int();
	*userP = 0;
	groundShape->setUserPointer2((void*)userP);

	m_collisionShapes.push_back(groundShape);

	btTransform groundTransform;
	groundTransform.setIdentity();
	groundTransform.setOrigin(btVector3(0,0,0));

	btRigidBody* ground = localCreateRigidBody(0.,groundTransform,groundShape,COL_GROUND,COL_BOX); //ground collides with boxes only
	ground->setUserPointer((void*)(-1));;

	groundY =  ground->getWorldTransform().getOrigin().getY()+groundShape->getHalfExtentsWithMargin().getY();


	currentBoxIndex++;

	theNet=NULL;
}

//creates a box with side lengths x,y,z
int Physics::createBox(int x1, int y1, int z1){
	if(noBoxes==0){scews.push_back(new btQuaternion(0,0,0));}
	noBoxes++;
	if(noBoxes>maxBoxes){
		fitness= -999999;
		totaltime=simulationTime;
	}

	// max 10 & min 0.05
	if(x1==0){x1=95;}
	if(y1==0){y1=95;}
	if(z1==0){z1=95;}
	float x=(x1%995+5)/100.f;
	float y=(y1%995+5)/100.f;
	float z=(z1%995+5)/100.f;

	btAssert(x>=0.05 && x<=10);
	btAssert(y>=0.05 && y<=10);
	btAssert(z>=0.05 && z<=10);

	btBoxShape* boxShape = new btBoxShape(btVector3(x/2.f,y/2.f,z/2.f));
	int* userP = new int();
	*userP = 1;
	boxShape->setUserPointer2((void*)userP);
	m_collisionShapes.push_back(boxShape);

	btTransform startTransform;
	startTransform.setIdentity();
	btRigidBody* box;
	btScalar mass = btScalar(x*y*z*DensityHuman);
	//btScalar mass = btScalar(0);
	startTransform.setOrigin(btVector3(0,0,0));

	box= localCreateRigidBody(mass,startTransform,boxShape,COL_BOX,COL_GROUND);

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
	float userPointer = ((UserPointerStruct*)(m_dynamicsWorld->getConstraint(jointIndex)->getUserConstraintPtr()))->CrossSectionalStrength;

	int theSign;
	float absX;
	float absY;
	float absZ;
	float absolute;
	float J;

	switch(m_dynamicsWorld->getConstraint(jointIndex)->getConstraintType()){
	case D6_CONSTRAINT_TYPE:
		absX = abs(valueX);
		absY = abs(valueY);
		absZ = abs(valueZ);
		absolute = absX+absY+absZ;
		J=1;
		if(absolute>userPointer){
			J=userPointer/absolute;
		}
		btRotationalLimitMotor* m0 = ((btGeneric6DofConstraint*)m_dynamicsWorld->getConstraint(jointIndex))->getRotationalLimitMotor(0);
		btRotationalLimitMotor* m1 = ((btGeneric6DofConstraint*)m_dynamicsWorld->getConstraint(jointIndex))->getRotationalLimitMotor(1);
		btRotationalLimitMotor* m2 = ((btGeneric6DofConstraint*)m_dynamicsWorld->getConstraint(jointIndex))->getRotationalLimitMotor(2);

		//beware! m_maxMotorForce actually sets the min and max impulse to -m_maxMotorForce and +m_maxMotorForce respectively
		//thus this must not be negative. the direction of the force can instead be set using the sign of m_targetVelocity
		m0->m_maxMotorForce=absX*J;
		m1->m_maxMotorForce=absY*J;
		m2->m_maxMotorForce=absZ*J;
		m0->m_targetVelocity=sign(valueX)*MAXDWORD;
		m1->m_targetVelocity=sign(valueY)*MAXDWORD;
		m2->m_targetVelocity=sign(valueZ)*MAXDWORD;
		m0->m_enableMotor=true;
		m1->m_enableMotor=true;
		m2->m_enableMotor=true;
		return GENERIC6DOF;
		break;
	}
	return -1;
}
inline float Physics::sign(float input){
	if(input<0){return -1;}
	return 1;
}

int Physics::createJoint(	int box1, int box2,	int scewIn,
						 int preX, int preY, int preS,
						 int postX, int postY, int postS,
						 int dofX, int dofY, int dofZ){
							 //default value
							 if(preX==0){preX=50;}
							 if(preY==0){preY=50;}
							 if(postX==0){postX=50;}
							 if(postY==0){postY=50;}

							 preX=preX%99+1;
							 preY=preY%99+1;
							 preS=preS%6;
							 postX=postX%99+1;
							 postY=postY%99+1;
							 postS=postS%6;
							 scewIn=scewIn%360;
							 btScalar scew = ((float)scewIn*2.f*PI)/360.f;

							 //tjek input in debug mode
							 btAssert(preX>0 && preX<100);
							 btAssert(preY>0 && preY<100);
							 btAssert(preS>-1 && preS<6);

							 btAssert(postX>0 && postX<100);
							 btAssert(postY>0 && postY<100);
							 btAssert(postS>-1 && postS<6);

							 //Get box pointers
							 btRigidBody* Box1 = (btRigidBody*) m_dynamicsWorld->getCollisionObjectArray().at(box1);
							 btRigidBody* Box2 = (btRigidBody*) m_dynamicsWorld->getCollisionObjectArray().at(box2);

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
							 localBox1.setRotation(rotation1.inverse());

							 //box2
							 btVector3 halfside2 = ((btBoxShape*)Box2->getCollisionShape())->getHalfExtentsWithMargin();
							 btVector3 connection2 = getLocalJointPosition(postX,postY,postS,&halfside2);

							 btQuaternion rotation2 = getLocalRotation(preS, postS,scew,box1);
							 rotation2*=rotation1;

							 btVector3 center2 = center1+rotate(&connection1,&rotation1.inverse())-rotate(&connection2,&rotation2);

							 //rotate and translate box
							 btTransform trans2;
							 trans2.setIdentity();
							 //trans2.setRotation(rotation2);
							
							 trans2.setRotation(rotation2.inverse());
							 trans2.setOrigin(center2);
							 Box2->setCenterOfMassTransform(trans2);

							 btDefaultMotionState* myMotionState = (btDefaultMotionState*)((Box2)->getMotionState());
							 myMotionState->setWorldTransform(Box2->getWorldTransform());

							 //rotate and translate joint
							 localBox2.setRotation(rotation2);

							 localBox2.setOrigin(connection2);

							 //setup contraint/joint
							 btGeneric6DofConstraint* gen6C;
							 //int DOFx = dofX %1;	int DOFy = dofY %1;	int DOFz = dofZ %1;
							 int DOFx = dofX %170;	int DOFy = dofY %170;	int DOFz = dofZ %170;
							 // int DOFx = dofX %170+10;	int DOFy = dofY%170+10;	int DOFz = dofZ %170+10;
							 float DOFxR = ((float)DOFx*2.f*PI)/360.f; float DOFyR = ((float)DOFy*2.f*PI)/360.f; float DOFzR = ((float)DOFz*2.f*PI)/360.f;
							 // printf("%f %f %f\n", DOFxR,DOFyR,DOFzR);
							 UserPointerStruct* theStruct = new UserPointerStruct();
							 btScalar mass1=1/Box1->getInvMass();
							 btScalar mass2=1/Box2->getInvMass();


									 gen6C = new btGeneric6DofConstraint(*Box1,*Box2,localBox1,localBox2,true);
									 gen6C->setLimit(0,0,0);//dist to other box can be set as (0,dist,dist)
									 gen6C->setLimit(1,0,0);
									 gen6C->setLimit(2,0,0);
									 gen6C->setAngularLowerLimit(btVector3(-DOFxR/2,-DOFyR/2,-DOFzR/2));
									 gen6C->setAngularUpperLimit(btVector3(DOFxR/2,DOFyR/2,DOFzR/2));

									 sensors.push_back(0);
									 sensors.push_back(0);
									 sensors.push_back(0);

									 theStruct->sensorIndex=sensors.size()-3;
									 float crossSection =getCrossSectionGen6d(preS, &halfside1,preX,preY,postS,&halfside2,postX,postY);
									 theStruct->CrossSectionalStrength=crossSection*muscleStregnth;
									 gen6C->setUserConstraintPtr(theStruct);

									 gen6C->setBreakingImpulseThreshold(tensileStrength*crossSection);
									 m_dynamicsWorld->addConstraint(gen6C,true);

							 int returnVal = currentJointIndex;
							 currentJointIndex++;

							 return returnVal;
}

//calculates the contact area between the boxes and scals the max force accordingly
//contact area is calculated as the minimum area where thes contac if the box is rotated around y axis
float Physics::getCrossSectionGen6d(int preS,btVector3* halfside1, int preX, int preY, int postS, btVector3* halfside2, int postX, int postY){
	//CSA
	float x,y, spaceLeftX, spaceLeftY;

	switch(preS){
	case 0:
	case 5:
		x=halfside1->getY();
		y=halfside1->getX();
		spaceLeftX=x-(x/50.*((float)abs(preY-50)));
		spaceLeftY=y-(y/50.*((float)abs(preX-50)));
		//printf("space left1 %f\n", spaceLeftY);
		//printf("space left2 %f\n", spaceLeftX);
		break;
	case 1:
	case 4:
		x=halfside1->getZ();
		y=halfside1->getY();
		spaceLeftX=x-(x/50.*((float)abs(preX-50)));
		spaceLeftY=y-(y/50.*((float)abs(preY-50)));
		//printf("space left1 %f\n", spaceLeftY);
		//printf("space left2 %f\n", spaceLeftX);
		break;
	case 2:
	case 3:
		x=halfside1->getZ();
		y=halfside1->getX();
		spaceLeftX=x-(x/50.*((float)abs(preY-50)));
		spaceLeftY=y-(y/50.*((float)abs(preX-50)));
		//printf("space left1 %f\n", spaceLeftY);
		//printf("space left2 %f\n", spaceLeftX);
		break;
	}

	//CSA
	float x2,y2, spaceLeftX2, spaceLeftY2;

	switch(postS){
	case 0:
	case 5:
		x2=halfside2->getY();
		y2=halfside2->getX();
		spaceLeftX2=x2-(x2/50.*((float)abs(postY-50)));
		spaceLeftY2=y2-(y2/50.*((float)abs(postX-50)));
		//printf("space left1 %f\n", spaceLeftY2);
		//printf("space left2 %f\n", spaceLeftX2);
		break;
	case 1:
	case 4:
		x2=halfside2->getZ();
		y2=halfside2->getY();
		spaceLeftX2=x2-(x2/50.*((float)abs(postX-50)));
		spaceLeftY2=y2-(y2/50.*((float)abs(postY-50)));
		//printf("space left1 %f\n", spaceLeftY2);
		//printf("space left2 %f\n", spaceLeftX2);
		break;
	case 2:
	case 3:
		x2=halfside2->getZ();
		y2=halfside2->getX();
		spaceLeftX2=x2-(x2/50.*((float)abs(postY-50)));
		spaceLeftY2=y2-(y2/50.*((float)abs(postX-50)));
		//printf("space left1 %f\n", spaceLeftY2);
		//printf("space left2 %f\n", spaceLeftX2);
		break;
	}

	//float forceOffsetPercent=1.- (max(abs(postY-50)+abs(preY-50),max(abs(preX-50)+abs(postX-50),max(abs(postX-50)+abs(preY-50),max(abs(preX-50)+abs(postY-50),max(abs(preX-50)+abs(preY-50),abs(postX-50)+abs(postY-50)))))))/100.;
	//float areal =min(x*x2,min(y*y2,min(x2*y,min(x*y2,min(x*y,x2*y2)))))*4;
	float areal =min(spaceLeftX*spaceLeftX2,min(spaceLeftY*spaceLeftY2,min(spaceLeftX2*spaceLeftY,min(spaceLeftX*spaceLeftY2,min(spaceLeftX*spaceLeftY,spaceLeftX2*spaceLeftY2)))))*4;
	//printf("areal %f\n", areal);
	return areal;
}

//rotates vector3 by a Quaternion
btVector3 Physics::rotate(btVector3* vec, btQuaternion* quant){
	btVector3 result = *vec;

	btMatrix3x3 rot = btMatrix3x3(*quant);

	return result*rot;
}

btQuaternion Physics::getLocalRotation(int pre, int post, btScalar scew,int prevBoxIndex){
	
	btQuaternion* scewer;
	//btScalar scew = PI/8.;
	switch(pre){
	case 0:
		scewer = new btQuaternion(0,0,scew);break; //arms
	case 1:
		scewer = new btQuaternion(0,scew,0);break; //noeffect...side?? untested
	case 2://x+
		scewer = new btQuaternion(scew,0,0);break; //top (upperbody & head)
	case 3://z+
		scewer = new btQuaternion(scew,0,0);break;	//bot (dino legs)
	case 4://x-
		scewer = new btQuaternion(0,scew,0);break; //noeffect...side?? untested
	case 5://z-
		scewer = new btQuaternion(0,0,scew);break; //tail
	default:
		perror("not a legal s value");
		break;
	}
	*scewer*=(*scews.at(prevBoxIndex-1))*(*scews.at(prevBoxIndex-1));
	scews.push_back(scewer);
	printf("s: %d %d\n",pre,post);
	
	
	btQuaternion rot;
	if((pre==0 && post==4) || (pre==1 && post==0) || (pre==4 && post==5) || (pre==5 && post==1)){
		//04 10 45 51
		rot=btQuaternion(PI/2.,0 ,0);
	}else if((pre==4 && post==0) || (pre==0 && post==1) || (pre==5 && post==4) || (pre==1 && post==5)){
		//01 15 40 54
		rot=btQuaternion(-PI/2.,0 ,0);
	}else if((pre==0 && post==2) || (pre==2 && post==5) || (pre==3 && post==0) || (pre==5 && post==3)){
		//02 25 30 53
		rot=btQuaternion(0,PI/2.,0);
	}else if((pre==2 && post==0) || (pre==5 && post==2) || (pre==0 && post==3) || (pre==3 && post==5)){
		//03 20 35 52
		rot=btQuaternion(0,-PI/2.,0);
	}else if((pre==1 && post==3) || (pre==2 && post==1) || (pre==3 && post==4) || (pre==4 && post==2)){
		//13 21 34 42
		rot=btQuaternion(0, 0, PI/2);
	}else if((pre==3 && post==1) || (pre==1 && post==2) || (pre==4 && post==3) || (pre==2 && post==4)){
		//12 24 31 43
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
		}else if(pre==2){
			//22
			rot=btQuaternion(0,PI,0);
		}else if(pre==3){
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
	
	return rot*(*scewer);
}
//calculates the poistion of where the joint connects to the box in regards to the local box center
btVector3 Physics::getLocalJointPosition(int x, int y, int s, btVector3* halfSizes){
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
	/*	*/

	while(scews.size()>0){
		delete scews.at(scews.size()-1);
		scews.pop_back();
	}

	//delete NNs
	while(subnets.size()>0){
		NeuralNetwork* aSubnet = subnets.at(subnets.size()-1);
		if(aSubnet->hasOwnFirstLayer()){
			aSubnet->killFirstLayer();
		}
		delete aSubnet;
		subnets.pop_back();
	}
	if(theNet!=NULL){
		theNet->killFirstLayer();
		delete theNet;
	}

	//delete contraints
	while(m_dynamicsWorld->getNumConstraints()>0){
		btTypedConstraint* deathPointer = m_dynamicsWorld->getConstraint(0);
		if((int)deathPointer->getUserConstraintPtr()!=-1){
		delete deathPointer->getUserConstraintPtr();
		}
		m_dynamicsWorld->removeConstraint(deathPointer);
		delete deathPointer;
	}

	//remove the rigidbodies from the dynamics world and delete them
	for (int i=m_dynamicsWorld->getNumCollisionObjects()-1; i>=0 ;i--){
		btCollisionObject* obj = m_dynamicsWorld->getCollisionObjectArray().at(i);
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
		delete shape->getUserPointer2();
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
	int box3 = createBox(895,95,395);


	int box4 = createBox(195,195,195);
	createJoint(box3, box4,0, 50, 50, 2, 50, 50,50, 30,30,30);

	/*
	int box = createBox(85,385,185);
	createJoint(box3, box, 0,50, 50, 3, 50, 50, 3, 0,0,0);

	int box5 = createBox(95,95,395);
	createJoint(box, box5, 0,50, 50,5, 50, 50, 0, 0,0,0);
	*/


	//	createSensor(box2, pressure);

	//NN test
	std::vector<NeuralNode*> inputs;

	for(int i=0;i< (int) sensors.size();i++){
		inputs.push_back(new NeuralNode(&sensors.at(i)));
	}

	inputs.push_back(new NeuralNode(1)); //index 3
	inputs.push_back(new NeuralNode(-2)); //index 4
	float* testPoint = new float;
	*testPoint = 5;
	inputs.push_back(new NeuralNode(testPoint)); //index 5
	theNet = new NeuralNetwork(inputs);
	theNet->insertNode(SUM,5,3,3,1);
	theNet->insertNode(SIN,1,1);
	theNet->changeLayer();
	theNet->insertNode(PRODUCT,0,1,2,2);
	theNet->stopBuilding();

	NeuralNetwork* aNet = new NeuralNetwork(theNet->getLastLayer());
	aNet->insertNode(PRODUCT,0,0,10000,10000);
	aNet->stopBuilding();
	subnets.push_back(aNet);

	effectorNNindex.push_back(0);
	effectorNNindex.push_back(1);
	effectorNNindex.push_back(2);

	solveGroundConflicts();
}



void Physics::calcFitness(fitnessTest test){
	
	//er det her relevant
	switch(test){
	case move:
		{

			fitness= fitMove();

		}
		break;
	case oldMove:
		{
			btVector3 tmpPos = calcPosition();
			btVector3 pastVector = pastPoint-startPoint;
			btVector3 vector = tmpPos-startPoint;
			pastPoint = tmpPos;

			float pastLength = sqrt(pastVector.getX()*pastVector.getX()+pastVector.getZ()*pastVector.getZ());
			float length = sqrt(vector.getX()*vector.getX()+vector.getZ()*vector.getZ());

			float weight = totaltime/10000.f;
			fitness += (length-pastLength)*weight;
		}
		break;
	case jump:
		{
			fitness=fitJump();
		}
		break;
	case combi:
		{
			//dosnt work

			fitness=(fitJump()+1)*fitMove();
		}
		break;
	case none:
		fitness=0;

		break;

	default:

		printf("unkown fitness test\n");
	}
}

float Physics::fitJump(){
	
			//er der kontakt emd jorden
			// hvis der ikke er så er det afstanden fra jord til nederste punkt
			calcSize();
			
			//fix ground collision
			//collision detection
			//one per btPersistentManifold for each collision
			int numManifolds = m_dynamicsWorld->getDispatcher()->getNumManifolds();

			for (int i=0;i<numManifolds;i++){
				btPersistentManifold* contactManifold =  m_dynamicsWorld->getDispatcher()->getManifoldByIndexInternal(i);
				int box1 = (int)contactManifold->getBody0()->getUserPointer();
				int box2 = (int)contactManifold->getBody1()->getUserPointer();

				if(box1 == ground || box1 == ground){
					return fitness;
				}
			}

			return max(fitness,  lowestPoint-groundY);
}


float Physics::fitMove(){


	btVector3 tmpPos= calcPosition()-startPoint;

	return sqrt(tmpPos.getX()*tmpPos.getX()+tmpPos.getZ()*tmpPos.getZ());

}