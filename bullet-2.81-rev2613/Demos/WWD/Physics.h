/**
*
*	Physics engine
*
*	also contains parts of visualisation at the monent
*
**/

#pragma once
#ifdef _WINDOWS
#include "Win32DemoApplication.h"
#define PlatformDemoApplication Win32DemoApplication
#else
#include "GlutDemoApplication.h"
#define PlatformDemoApplication GlutDemoApplication
#endif

#include "LinearMath/btAlignedObjectArray.h"
#include "NeuralNetwork.h"
#include "MTree.h"
#include <math.h>
#include <vector>
#include <limits>

#include "GlutStuff.h"
#include "btBulletDynamicsCommon.h"

#include <stdio.h>
#include "GLDebugDrawer.h"

static GLDebugDrawer gDebugDraw;

struct   MyContactResultCallback : public btCollisionWorld::ContactResultCallback{
	bool m_connected;
	btScalar m_margin;
	MyContactResultCallback() :m_connected(false),m_margin(0.05)
	{
	}
	virtual   btScalar   addSingleResult(btManifoldPoint& cp,   const btCollisionObjectWrapper* colObj0Wrap,int partId0,int index0,const btCollisionObjectWrapper* colObj1Wrap,int partId1,int index1)
	{
		m_connected = true;
		return 1.f;
	}
};

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
#define musclePercentage 0.15 //the ca percentage of muscle mass in a joint
#define muscleStregnth 6.3*100.f*100.f * musclePercentage

//footprint size of the shoulder tendon is 25 x 11-22 mm http://www.shoulderdoc.co.uk/article.asp?article=672
//mean load failure strength of the shoulder tendon is 273 N http://www.ncbi.nlm.nih.gov/pubmed/18182197
#define tensilePercentage 10.
#define tensileStrength 661818 * tensilePercentage

#define PI 3.1415926

#define maxBoxes 10
#define simulationTime 10000 //10 s = 10000 ms

#define BIT(x) (1<<(x))		//snippet from wikipedia http://www.bulletphysics.org/mediawiki-1.5.8/index.php?title=Collision_Filtering

enum collisiontypes {		//definition of the collision groups
	COL_NOTHING = 0,		//Collide with nothing
	COL_GROUND = BIT(0),	//Ground collision group
	COL_BOX = BIT(1),		//Boxes collision group
};

enum{pressure, angle, light};

enum{HINGE,GENERIC6DOF};
class btBroadphaseInterface;
class btCollisionShape;
class btOverlappingPairCache;
class btCollisionDispatcher;
class btConstraintSolver;
struct btCollisionAlgorithmCreateFunc;
class btDefaultCollisionConfiguration;

enum fitnessTest{none,jump, move, oldMove, combi};

class Physics : public PlatformDemoApplication{
private:
	int noBoxes;
	float fitness;
	float heighstPoint;
	float lowestPoint;
	float height;
	float groundY;
	btVector3 startPoint;

	int currentBoxIndex,currentJointIndex;

	//keep the collision shapes, for deletion/cleanup
	btAlignedObjectArray<btCollisionShape*>	m_collisionShapes;

	btBroadphaseInterface*	m_broadphase;

	btCollisionDispatcher*	m_dispatcher;

	btConstraintSolver*	m_solver;

	btDefaultCollisionConfiguration* m_collisionConfiguration;

	void	initPhysics();

	void	exitPhysics();

	struct UserPointerStruct{
		int sensorIndex;
		float CrossSectionalStrength;
	};

	bool isLegal();

	void checkForDismemberment();
	btVector3 calcPosition();
	int setEffect(int jointIndex, float valueX,float valueY,float valueZ);
	void simulationLoopStep(float stepSize);
	void Physics::calcButtom();
	btVector3 getLocalJointPosition(int x, int y, int s, btVector3* halfSizes);
	btQuaternion getLocalRotation(int myS, int opS);
	btVector3 Physics::rotate(btVector3* vec, btQuaternion* quant);
	inline float getCrossSectionHinge(int s,btVector3* Halfsize);
	float getCrossSectionGen6d(int preS,btVector3* halfside1, int preX, int preY, int postS, btVector3* halfside2, int postX, int postY);
	inline float sign(float input);
	float getBoxHalfHeight(btCollisionObject* object);
	float Physics::fitJump();
	float Physics::fitMove();
public:

	enum boxes{ground=-2};

	virtual void displayCallback();
	virtual void clientResetScene();
	virtual void clientMoveAndDisplay();
	virtual void showInfo(int& xOffset,int& yStart, int yIncr);
	NeuralNetwork* theNet;

	std::vector<NeuralNetwork*> subnets;
	std::vector<float> sensors;
	std::vector<int> effectorNNindex;

	float getFitness(){
		return fitness;
	}

	void calcFitness(fitnessTest test);

	Physics(){
		initPhysics();
	}

	void setFitnesFunction(fitnessTest type){
		fitness=0;
		testType=type;
	}

	void runSimulation();

	virtual ~Physics(){
		exitPhysics();
	}

	int createBox(int x,int y,int z);
	int createSensor(int box, int type);
	int createJoint(int box1, int box2, int type,
		int preX, int preY, int preS,
		int postX, int postY, int postS,
		int dofX, int dofY, int dofZ);

	void testPhysics();
	void calcSize();
	void solveGroundConflicts();
	fitnessTest testType;
};
