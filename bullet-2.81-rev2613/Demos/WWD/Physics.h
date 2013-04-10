/**
*
*	Physics engine
*
*	also contains parts of visualisation at the monent
*
**/


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
#include <limits>


//bullet units
/**
*	Bullet assumes units to be in meters and time in seconds.
*	Moving objects are assumed to be in the range of 0.05 units, about the size of a pebble, to 10, the size of a truck. 
*	The simulation steps in fraction of seconds (1/60 sec or 60 hertz), and gravity in meters per square second (9.8 m/s^2).
*/

//real world units
//average density of the human body 1062 kg/m3 acording to http://www.dtic.mil/dtic/tr/fulltext/u2/639241.pdf
#define DensityHuman 1062 

//average strength human cross-sectional area 6.3 kg/cm2 http://link.springer.com/article/10.1007%2FBF00696087
#define csa 6.3*100.f*100.f

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
	bool isLegal();
	void testPhysics();
	void CalculateRotation( btQuaternion *q, btMatrix3x3  *a );
	inline float getCrossSection(int s,btVector3* Halfsize);
	int setEffect(int jointIndex, int valueX,int valueY,int valueZ);
};

#endif

