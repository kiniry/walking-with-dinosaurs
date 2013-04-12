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


class Physics : public PlatformDemoApplication{
	private:
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

	

	

		struct UserPointerStruct{
			int sensorIndex;
			float CrossSectionalStrength;
		};

		virtual void clientMoveAndDisplay();

		virtual void displayCallback();
		virtual void clientResetScene();

		btVector3 getLocalJointPosition(int x, int y, int s, btVector3* halfSizes);
		btQuaternion getLocalRotation(int myS, int opS);
		btVector3 Physics::rotate(btVector3* vec, btQuaternion* quant);
		bool isLegal();

		void CalculateRotation( btQuaternion *q, btMatrix3x3  *a );
		inline float getCrossSection(int s,btVector3* Halfsize);
		int setEffect(int jointIndex, float valueX,float valueY,float valueZ);
		void simulationLoopStep(float stepSize);


	public:

		NeuralNetwork* theNet;

		std::vector<NeuralNetwork*> subnets;
		std::vector<float> sensors;
		std::vector<int> effectorNNindex;
		
		
		
		DinoTreeNode* DinoStructure;
		
		
		float getFitness(){
			return fitness;
		}

		void calcFitness();

		

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


		int createBox(int x,int y,int z);
		int createSensor(int box, int type);
		int createJoint(		int box1, int box2, int type,
								int preX, int preY, int preS,
								int postX, int postY, int postS,
								int dofX, int dofY, int dofZ);
		
		void testPhysics();
		
		void solveGroundConflicts();
};

#endif

