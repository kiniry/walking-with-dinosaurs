
#ifndef PHYSICS_H
#define PHYSICS_H

#ifdef _WINDOWS
#include "Win32DemoApplication.h"
#define PlatformDemoApplication Win32DemoApplication
#else
#include "GlutDemoApplication.h"
#define PlatformDemoApplication GlutDemoApplication
#endif

#include "LinearMath/btAlignedObjectArray.h"
#include "NeuralNetwork.h"
#include "DinoTreeNode.h"
#include <math.h>
#include <vector>

#define PI 3.1415926
enum{pressure, angle, light};

enum{HINGE,GENERIC6DOF};
class btBroadphaseInterface;
class btCollisionShape;
class btOverlappingPairCache;
class btCollisionDispatcher;
class btConstraintSolver;
struct btCollisionAlgorithmCreateFunc;
class btDefaultCollisionConfiguration;

///Physics is good starting point for learning the code base and porting.
class Physics : public PlatformDemoApplication
{

	int inc;
	float* testPoint;
	std::vector<int> ancestor;

	float fitness;
	
	int currentBoxIndex,currentJointIndex;
	unsigned long timeUsed;
	//keep the collision shapes, for deletion/cleanup
	btAlignedObjectArray<btCollisionShape*>	m_collisionShapes;

	btBroadphaseInterface*	m_broadphase;

	btCollisionDispatcher*	m_dispatcher;

	btConstraintSolver*	m_solver;

	btDefaultCollisionConfiguration* m_collisionConfiguration;
	
	void	initPhysics();

	void	exitPhysics();

	public:

	float getFitness(){
		return fitness;
	}

	void calcFitness();

	NeuralNetwork* theNet;

	std::vector<NeuralNetwork*> subnets;

	DinoTreeNode* DinoStructure;

	Physics(std::vector<int> Ancestor){
		ancestor=Ancestor;
		initPhysics();
	}
	Physics(){
		initPhysics();
	}
	std::vector<int> getAncestor(){
		return ancestor;
	}

	void runSimulation();

	virtual ~Physics(){
		exitPhysics();
	}


	virtual void clientMoveAndDisplay();

	virtual void displayCallback();
	virtual void clientResetScene();

	std::vector<float> sensors;
	std::vector<int> effectorNNindex;

	static DemoApplication* Create()
	{
		Physics* demo = new Physics;
		demo->myinit();
		demo->initPhysics();
		return demo;
	}

	int createBox(int x,int y,int z);
	int createSensor(int box, int type);
	int createJoint(		int box1, int box2, int type,
							int preX, int preY, int preS,
							int postX, int postY, int postS,
							int dofX, int dofY, int dofZ);
	btVector3 getLocalJointPosition(int x, int y, int s, btVector3* halfSizes);
	btQuaternion getLocalRotation(int myS, int opS);
	btVector3 Physics::rotate(btVector3* vec, btQuaternion* quant);
	void testPhysics();

	int setEffect(int jointIndex, int valueX,int valueY,int valueZ);
};

#endif

