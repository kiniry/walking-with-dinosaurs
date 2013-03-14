/*
Bullet Continuous Collision Detection and Physics Library
Copyright (c) 2003-2006 Erwin Coumans  http://continuousphysics.com/Bullet/

This software is provided 'as-is', without any express or implied warranty.
In no event will the authors be held liable for any damages arising from the use of this software.
Permission is granted to anyone to use this software for any purpose, 
including commercial applications, and to alter it and redistribute it freely, 
subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
*/
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
#include <math.h>

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
	int currentBoxIndex,currentJointIndex;
	unsigned long timeUsed;
	//keep the collision shapes, for deletion/cleanup
	btAlignedObjectArray<btCollisionShape*>	m_collisionShapes;

	btBroadphaseInterface*	m_broadphase;

	btCollisionDispatcher*	m_dispatcher;

	btConstraintSolver*	m_solver;

	btDefaultCollisionConfiguration* m_collisionConfiguration;

	public:

	Physics()
	{
	}
	virtual ~Physics()
	{
		exitPhysics();
	}
	void	initPhysics();

	void	exitPhysics();

	virtual void clientMoveAndDisplay();

	virtual void displayCallback();
	virtual void clientResetScene();
	btScalar startBoxMass;
	static DemoApplication* Create()
	{
		Physics* demo = new Physics;
		demo->myinit();
		demo->initPhysics();
		return demo;
	}

	int createBox(int x,int y,int z);
	int createSensor(int box, int type);
	int createJoint(		int box1,
							int box2,
							int type,
							int preX, int preY, int preS,
							int postX, int postY, int postS,
							int dofX, int dofY, int dofZ);
	btVector3 getLocalJointPosition(float x, float y, int s, btVector3* halfSizes);
	btQuaternion getLocalRotation(int myS, int opS);
	btVector3 Physics::rotate(btVector3* vec, btQuaternion* quant);
	void testPhysics();

	int setEffect(int jointIndex, int valueX,int valueY,int valueZ);
};

#endif

