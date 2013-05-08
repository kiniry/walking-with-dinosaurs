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

#ifdef _WIN32 //needed for glut.h
#include <windows.h>
#endif
#include "GLDebugFont.h"



#include "GlutStuff.h"
#include "GL_ShapeDrawer.h"
#include "BulletCollision/CollisionShapes/btPolyhedralConvexShape.h"
#include "BulletCollision/CollisionShapes/btTriangleMeshShape.h"
#include "BulletCollision/CollisionShapes/btBoxShape.h"
#include "BulletCollision/CollisionShapes/btSphereShape.h"
#include "BulletCollision/CollisionShapes/btConeShape.h"
#include "BulletCollision/CollisionShapes/btCylinderShape.h"
#include "BulletCollision/CollisionShapes/btTetrahedronShape.h"
#include "BulletCollision/CollisionShapes/btCompoundShape.h"
#include "BulletCollision/CollisionShapes/btCapsuleShape.h"
#include "BulletCollision/CollisionShapes/btConvexTriangleMeshShape.h"
#include "BulletCollision/CollisionShapes/btUniformScalingShape.h"
#include "BulletCollision/CollisionShapes/btStaticPlaneShape.h"
#include "BulletCollision/CollisionShapes/btMultiSphereShape.h"
#include "BulletCollision/CollisionShapes/btConvexPolyhedron.h"

///
#include "BulletCollision/CollisionShapes/btShapeHull.h"

#include "LinearMath/btTransformUtil.h"


#include "LinearMath/btIDebugDraw.h"
//for debugmodes

#include <stdio.h> //printf debugging

//#define USE_DISPLAY_LISTS 1
#ifdef USE_DISPLAY_LISTS

#include <map>

using namespace std;

//Set for storing Display list per trimesh
struct TRIMESH_KEY
{
	btCollisionShape* m_shape;
	GLuint m_dlist;//OpenGL display list	
};

typedef map<unsigned long,TRIMESH_KEY> TRIMESH_KEY_MAP;

typedef pair<unsigned long,TRIMESH_KEY> TRIMESH_KEY_PAIR;

TRIMESH_KEY_MAP g_display_lists;

class GlDisplaylistDrawcallback : public btTriangleCallback
{
public:

	virtual void processTriangle(btVector3* triangle,int partId, int triangleIndex)
	{

		btVector3 diff1 = triangle[1] - triangle[0];
		btVector3 diff2 = triangle[2] - triangle[0];
		btVector3 normal = diff1.cross(diff2);

		normal.normalize();

		glBegin(GL_TRIANGLES);
		glColor3f(1, 1, 1);
		glNormal3d(normal.getX(),normal.getY(),normal.getZ());
		glVertex3d(triangle[0].getX(), triangle[0].getY(), triangle[0].getZ());

		//glColor3f(0, 1, 0);
		glNormal3d(normal.getX(),normal.getY(),normal.getZ());
		glVertex3d(triangle[1].getX(), triangle[1].getY(), triangle[1].getZ());

		//glColor3f(0, 1, 0);
		glNormal3d(normal.getX(),normal.getY(),normal.getZ());
		glVertex3d(triangle[2].getX(), triangle[2].getY(), triangle[2].getZ());
		glEnd();

		/*glBegin(GL_LINES);
		glColor3f(1, 1, 0);
		glNormal3d(normal.getX(),normal.getY(),normal.getZ());
		glVertex3d(triangle[0].getX(), triangle[0].getY(), triangle[0].getZ());
		glNormal3d(normal.getX(),normal.getY(),normal.getZ());
		glVertex3d(triangle[1].getX(), triangle[1].getY(), triangle[1].getZ());
		glColor3f(1, 1, 0);
		glNormal3d(normal.getX(),normal.getY(),normal.getZ());
		glVertex3d(triangle[2].getX(), triangle[2].getY(), triangle[2].getZ());
		glNormal3d(normal.getX(),normal.getY(),normal.getZ());
		glVertex3d(triangle[1].getX(), triangle[1].getY(), triangle[1].getZ());
		glColor3f(1, 1, 0);
		glNormal3d(normal.getX(),normal.getY(),normal.getZ());
		glVertex3d(triangle[2].getX(), triangle[2].getY(), triangle[2].getZ());
		glNormal3d(normal.getX(),normal.getY(),normal.getZ());
		glVertex3d(triangle[0].getX(), triangle[0].getY(), triangle[0].getZ());
		glEnd();*/


	}
};

GLuint  OGL_get_displaylist_for_shape(btCollisionShape * shape)
{
	TRIMESH_KEY_MAP::iterator map_iter;

	unsigned long key = (unsigned long)shape;
	map_iter = g_display_lists.find(key);
	if(map_iter!=g_display_lists.end())
	{
		return map_iter->second.m_dlist;
	}

	return 0;
}

void OGL_displaylist_clean()
{
	TRIMESH_KEY_MAP::iterator map_iter,map_itend;

	map_iter = g_display_lists.begin();

	while(map_iter!=map_itend)
	{
		glDeleteLists(map_iter->second.m_dlist,1);		
		map_iter++;
	}

	g_display_lists.clear();
}


void OGL_displaylist_register_shape(btCollisionShape * shape)
{
	btVector3 aabbMax(btScalar(BT_LARGE_FLOAT),btScalar(BT_LARGE_FLOAT),btScalar(BT_LARGE_FLOAT));
	btVector3 aabbMin(-btScalar(BT_LARGE_FLOAT),-btScalar(BT_LARGE_FLOAT),-btScalar(BT_LARGE_FLOAT));
	GlDisplaylistDrawcallback drawCallback;
	TRIMESH_KEY dlist;

	dlist.m_dlist = glGenLists(1);
	dlist.m_shape = shape;

	unsigned long key = (unsigned long)shape;

	g_display_lists.insert(TRIMESH_KEY_PAIR(key,dlist));

	glNewList(dlist.m_dlist,GL_COMPILE);

//	glEnable(GL_CULL_FACE);

	glCullFace(GL_BACK);

	if (shape->isConcave())
	{
		btConcaveShape* concaveMesh = (btConcaveShape*) shape;			
		//todo pass camera, for some culling		
		concaveMesh->processAllTriangles(&drawCallback,aabbMin,aabbMax);
	}

//	glDisable(GL_CULL_FACE);	

	glEndList();
}
#endif //USE_DISPLAY_LISTS

void GL_ShapeDrawer::drawCoordSystem()  {
	glBegin(GL_LINES);
	glColor3f(1, 0, 0);
	glVertex3d(0, 0, 0);
	glVertex3d(1, 0, 0);
	glColor3f(0, 1, 0);
	glVertex3d(0, 0, 0);
	glVertex3d(0, 1, 0);
	glColor3f(0, 0, 1);
	glVertex3d(0, 0, 0);
	glVertex3d(0, 0, 1);
	glEnd();

}





class GlDrawcallback : public btTriangleCallback
{

public:

	bool	m_wireframe;

	GlDrawcallback()
		:m_wireframe(false)
	{
	}

	virtual void processTriangle(btVector3* triangle,int partId, int triangleIndex)
	{

		(void)triangleIndex;
		(void)partId;


		if (m_wireframe)
		{
			glBegin(GL_LINES);
			glColor3f(1, 0, 0);
			glVertex3d(triangle[0].getX(), triangle[0].getY(), triangle[0].getZ());
			glVertex3d(triangle[1].getX(), triangle[1].getY(), triangle[1].getZ());
			glColor3f(0, 1, 0);
			glVertex3d(triangle[2].getX(), triangle[2].getY(), triangle[2].getZ());
			glVertex3d(triangle[1].getX(), triangle[1].getY(), triangle[1].getZ());
			glColor3f(0, 0, 1);
			glVertex3d(triangle[2].getX(), triangle[2].getY(), triangle[2].getZ());
			glVertex3d(triangle[0].getX(), triangle[0].getY(), triangle[0].getZ());
			glEnd();
		} else
		{
			glBegin(GL_TRIANGLES);
			//glColor3f(1, 1, 1);
			
			
			glVertex3d(triangle[0].getX(), triangle[0].getY(), triangle[0].getZ());
			glVertex3d(triangle[1].getX(), triangle[1].getY(), triangle[1].getZ());
			glVertex3d(triangle[2].getX(), triangle[2].getY(), triangle[2].getZ());

			glVertex3d(triangle[2].getX(), triangle[2].getY(), triangle[2].getZ());
			glVertex3d(triangle[1].getX(), triangle[1].getY(), triangle[1].getZ());
			glVertex3d(triangle[0].getX(), triangle[0].getY(), triangle[0].getZ());
			glEnd();
		}
	}
};

class TriangleGlDrawcallback : public btInternalTriangleIndexCallback
{
public:
	virtual void internalProcessTriangleIndex(btVector3* triangle,int partId,int  triangleIndex)
	{
		(void)triangleIndex;
		(void)partId;


		glBegin(GL_TRIANGLES);//LINES);
		glColor3f(1, 0, 0);
		glVertex3d(triangle[0].getX(), triangle[0].getY(), triangle[0].getZ());
		glVertex3d(triangle[1].getX(), triangle[1].getY(), triangle[1].getZ());
		glColor3f(0, 1, 0);
		glVertex3d(triangle[2].getX(), triangle[2].getY(), triangle[2].getZ());
		glVertex3d(triangle[1].getX(), triangle[1].getY(), triangle[1].getZ());
		glColor3f(0, 0, 1);
		glVertex3d(triangle[2].getX(), triangle[2].getY(), triangle[2].getZ());
		glVertex3d(triangle[0].getX(), triangle[0].getY(), triangle[0].getZ());
		glEnd();
	}
};


void GL_ShapeDrawer::drawSphere(btScalar radius, int lats, int longs) 
{
	int i, j;
	for(i = 0; i <= lats; i++) {
		btScalar lat0 = SIMD_PI * (-btScalar(0.5) + (btScalar) (i - 1) / lats);
		btScalar z0  = radius*sin(lat0);
		btScalar zr0 =  radius*cos(lat0);

		btScalar lat1 = SIMD_PI * (-btScalar(0.5) + (btScalar) i / lats);
		btScalar z1 = radius*sin(lat1);
		btScalar zr1 = radius*cos(lat1);

		glBegin(GL_QUAD_STRIP);
		for(j = 0; j <= longs; j++) {
			btScalar lng = 2 * SIMD_PI * (btScalar) (j - 1) / longs;
			btScalar x = cos(lng);
			btScalar y = sin(lng);
			glNormal3f(x * zr1, y * zr1, z1);
			glVertex3f(x * zr1, y * zr1, z1);
			glNormal3f(x * zr0, y * zr0, z0);
			glVertex3f(x * zr0, y * zr0, z0);
		}
		glEnd();
	}
}

void GL_ShapeDrawer::drawCylinder(float radius,float halfHeight, int upAxis)
{


	glPushMatrix();
	switch (upAxis)
	{
	case 0:
		glRotatef(-90.0, 0.0, 1.0, 0.0);
		glTranslatef(0.0, 0.0, -halfHeight);
		break;
	case 1:
		glRotatef(-90.0, 1.0, 0.0, 0.0);
		glTranslatef(0.0, 0.0, -halfHeight);
		break;
	case 2:

		glTranslatef(0.0, 0.0, -halfHeight);
		break;
	default:
		{
			btAssert(0);
		}

	}

	GLUquadricObj *quadObj = gluNewQuadric();

	//The gluCylinder subroutine draws a cylinder that is oriented along the z axis. 
	//The base of the cylinder is placed at z = 0; the top of the cylinder is placed at z=height. 
	//Like a sphere, the cylinder is subdivided around the z axis into slices and along the z axis into stacks.

	gluQuadricDrawStyle(quadObj, (GLenum)GLU_FILL);
	gluQuadricNormals(quadObj, (GLenum)GLU_SMOOTH);

	gluDisk(quadObj,0,radius,15, 10);

	gluCylinder(quadObj, radius, radius, 2.f*halfHeight, 15, 10);
	glTranslatef(0.0f, 0.0f, 2.f*halfHeight);
	glRotatef(-180.0f, 0.0f, 1.0f, 0.0f);
	gluDisk(quadObj,0.f,radius,15, 10);

	glPopMatrix();
	gluDeleteQuadric(quadObj);
}

GL_ShapeDrawer::ShapeCache*		GL_ShapeDrawer::cache(btConvexShape* shape)
{
	ShapeCache*		sc=(ShapeCache*)shape->getUserPointer();
	if(!sc)
	{
		sc=new(btAlignedAlloc(sizeof(ShapeCache),16)) ShapeCache(shape);
		sc->m_shapehull.buildHull(shape->getMargin());
		m_shapecaches.push_back(sc);
		shape->setUserPointer(sc);
		/* Build edges	*/ 
		const int			ni=sc->m_shapehull.numIndices();
		const int			nv=sc->m_shapehull.numVertices();
		const unsigned int*	pi=sc->m_shapehull.getIndexPointer();
		const btVector3*	pv=sc->m_shapehull.getVertexPointer();
		btAlignedObjectArray<ShapeCache::Edge*>	edges;
		sc->m_edges.reserve(ni);
		edges.resize(nv*nv,0);
		for(int i=0;i<ni;i+=3)
		{
			const unsigned int* ti=pi+i;
			const btVector3		nrm=btCross(pv[ti[1]]-pv[ti[0]],pv[ti[2]]-pv[ti[0]]).normalized();
			for(int j=2,k=0;k<3;j=k++)
			{
				const unsigned int	a=ti[j];
				const unsigned int	b=ti[k];
				ShapeCache::Edge*&	e=edges[btMin(a,b)*nv+btMax(a,b)];
				if(!e)
				{
					sc->m_edges.push_back(ShapeCache::Edge());
					e=&sc->m_edges[sc->m_edges.size()-1];
					e->n[0]=nrm;e->n[1]=-nrm;
					e->v[0]=a;e->v[1]=b;
				}
				else
				{
					e->n[1]=nrm;
				}
			}
		}
	}
	return(sc);
}

void renderSquareA(float x, float y, float z)
{
	glBegin(GL_LINE_LOOP);
	glVertex3f(x, y, z);
	glVertex3f(x + 10.f, y, z);
	glVertex3f(x + 10.f, y + 10.f, z);
	glVertex3f(x, y + 10.f, z);
	glEnd();
}

inline void glDrawVector(const btVector3& v) { glVertex3d(v[0], v[1], v[2]); }

void GL_ShapeDrawer::drawSkybox(){
	//glFrontFace(GL_CW);
	glDisable(GL_LIGHTING);
	//glPushMatrix(); 
	//btglMultMatrix(m);
	if(m_textureenabled&&(!m_skyboxinitialized))
		{
			std::vector<char*> names = std::vector<char*>();
			m_skyboxhandle = new std::vector<unsigned int>();
			//for(int i=0;i<6;i++){names.push_back("Textures\\scale.png");}
			names.push_back("Textures\\front1h.jpg");	//defined front place
			names.push_back("Textures\\right1h.jpg");	//defined right place
			names.push_back("Textures\\top1h.jpg");		//defined top place
			names.push_back("Textures\\left1h.jpg");	//defined left place
			names.push_back("Textures\\back1h.jpg");	//defined back place
			names.push_back("Textures\\bot1h.png");		//defined bottom place

			for(int i=0;i<6;i++){
				int x=0,y=0,n=0;
				char* path = getTexturePath(names.at(i));
				//char* path = getTexturePath("Textures\\back1h.jpg");
				stbi_uc* theTex = stbi_load(path,&x,&y,&n,0);
				delete path;

				GLenum type = GL_RGB;
				if(n==4){type=GL_RGBA;}
				unsigned int texVal=0;
				m_skyboxhandle->push_back(texVal);
				glGenTextures(1,(GLuint*)&(m_skyboxhandle->at(i)));
				glBindTexture(GL_TEXTURE_2D,m_skyboxhandle->at(i));
				glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);
				glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR_MIPMAP_LINEAR);
				glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
				glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
				gluBuild2DMipmaps(GL_TEXTURE_2D,n,x,y,type,GL_UNSIGNED_BYTE,theTex);
				delete theTex;
			}
		}

		glMatrixMode(GL_TEXTURE);
		glLoadIdentity();
		glScalef(1.f,1.f,1.f);	//scale tex 2 down
		glMatrixMode(GL_MODELVIEW);

		m_skyboxinitialized=true;

		glEnable(GL_COLOR_MATERIAL);
		if(m_textureenabled) 
		{
			glEnable(GL_TEXTURE_2D);
				//glBindTexture(GL_TEXTURE_2D,m_texturehandle);	//tex 1
				//glBindTexture(GL_TEXTURE_2D,m_skyboxhandle->at(0));
		} else
		{
			glDisable(GL_TEXTURE_2D);
		}

		glColor3f(1,1, 1);

		bool useWireframeFallback = true;
					//const btBoxShape* boxShape = static_cast<const btBoxShape*>(shape);
					//btVector3 halfExtent = boxShape->getHalfExtentsWithMargin();
					btVector3 halfExtent = btVector3(5000,5000,5000);
					static int indices[36] = {
						2,3,0,//3,1,2,
						1,0,3,//0,2,1,
						6,2,4,//2,0,6,
						0,4,2,//4,6,0,
						0,1,4,//1,5,0,
						5,4,1,//4,0,5,
						3,7,1,//7,5,3,
						5,1,7,//1,3,5,
						7,6,5,//6,4,7,
						4,5,6,//5,7,4,
						3,2,7,//
						6,7,2};//

					 btVector3 vertices[8]={	
						btVector3(halfExtent[0],halfExtent[1],halfExtent[2]),
						btVector3(-halfExtent[0],halfExtent[1],halfExtent[2]),
						btVector3(halfExtent[0],-halfExtent[1],halfExtent[2]),	
						btVector3(-halfExtent[0],-halfExtent[1],halfExtent[2]),	
						btVector3(halfExtent[0],halfExtent[1],-halfExtent[2]),
						btVector3(-halfExtent[0],halfExtent[1],-halfExtent[2]),	
						btVector3(halfExtent[0],-halfExtent[1],-halfExtent[2]),	
						btVector3(-halfExtent[0],-halfExtent[1],-halfExtent[2])};

					//int si=36;
					int toggle = 1;
					int i=0;
					for(int j=1;j<7;j++){
						glBindTexture(GL_TEXTURE_2D,m_skyboxhandle->at(j-1));
						glBegin (GL_TRIANGLES);
						for(i;i<6*j;i+=3)
						{
							const btVector3& v1 = vertices[indices[i]];;
							const btVector3& v2 = vertices[indices[i+1]];
							const btVector3& v3 = vertices[indices[i+2]];

							btVector3 normal = (v3-v1).cross(v2-v1);
							normal.normalize ();
							glNormal3f(-normal.getX(),-normal.getY(),-normal.getZ());
							glTexCoord2f(toggle,toggle);
							glVertex3f (v1.x(), v1.y(), v1.z());
							glTexCoord2f((1-toggle),toggle);
							glVertex3f (v2.x(), v2.y(), v2.z());
							glTexCoord2f(toggle,(1-toggle));
							glVertex3f (v3.x(), v3.y(), v3.z());
							toggle = 1-toggle;
						}
						glEnd();
					}


					useWireframeFallback = false;
					
	
	glEnable(GL_LIGHTING);
	//glFrontFace(GL_CCW);
	//glDisable(GL_CULL_FACE);
	//glClear(GL_STENCIL_BUFFER_BIT);
	//glEnable(GL_CULL_FACE);
	

}

void GL_ShapeDrawer::drawOpenGL(btScalar* m, const btCollisionShape* shape, const btVector3& color,int	debugMode,const btVector3& worldBoundsMin,const btVector3& worldBoundsMax, btMatrix3x3* basis)
{
	
	if (shape->getShapeType() == CUSTOM_CONVEX_SHAPE_TYPE)
	{
		btVector3 org(m[12], m[13], m[14]);
		btVector3 dx(m[0], m[1], m[2]);
		btVector3 dy(m[4], m[5], m[6]);
//		btVector3 dz(m[8], m[9], m[10]);
		const btBoxShape* boxShape = static_cast<const btBoxShape*>(shape);
		btVector3 halfExtent = boxShape->getHalfExtentsWithMargin();
		dx *= halfExtent[0];
		dy *= halfExtent[1];
//		dz *= halfExtent[2];
		glColor3f(1,1,1);
		glDisable(GL_LIGHTING);
		glLineWidth(2);

		glBegin(GL_LINE_LOOP);
		glDrawVector(org - dx - dy);
		glDrawVector(org - dx + dy);
		glDrawVector(org + dx + dy);
		glDrawVector(org + dx - dy);
		glEnd();
		return;
	} 
	else if((shape->getShapeType() == BOX_SHAPE_PROXYTYPE) && (debugMode & btIDebugDraw::DBG_FastWireframe))
	{
		btVector3 org(m[12], m[13], m[14]);
		btVector3 dx(m[0], m[1], m[2]);
		btVector3 dy(m[4], m[5], m[6]);
		btVector3 dz(m[8], m[9], m[10]);
		const btBoxShape* boxShape = static_cast<const btBoxShape*>(shape);
		btVector3 halfExtent = boxShape->getHalfExtentsWithMargin();
		dx *= halfExtent[0];
		dy *= halfExtent[1];
		dz *= halfExtent[2];
		glBegin(GL_LINE_LOOP);
		glDrawVector(org - dx - dy - dz);
		glDrawVector(org + dx - dy - dz);
		glDrawVector(org + dx + dy - dz);
		glDrawVector(org - dx + dy - dz);
		glDrawVector(org - dx + dy + dz);
		glDrawVector(org + dx + dy + dz);
		glDrawVector(org + dx - dy + dz);
		glDrawVector(org - dx - dy + dz);
		glEnd();
		glBegin(GL_LINES);
		glDrawVector(org + dx - dy - dz);
		glDrawVector(org + dx - dy + dz);
		glDrawVector(org + dx + dy - dz);
		glDrawVector(org + dx + dy + dz);
		glDrawVector(org - dx - dy - dz);
		glDrawVector(org - dx + dy - dz);
		glDrawVector(org - dx - dy + dz);
		glDrawVector(org - dx + dy + dz);
		glEnd();
		return;
	}

	glPushMatrix(); 
	btglMultMatrix(m);


	if (shape->getShapeType() == UNIFORM_SCALING_SHAPE_PROXYTYPE)
	{
		const btUniformScalingShape* scalingShape = static_cast<const btUniformScalingShape*>(shape);
		const btConvexShape* convexShape = scalingShape->getChildShape();
		float	scalingFactor = (float)scalingShape->getUniformScalingFactor();
		{
			btScalar tmpScaling[4][4]={{scalingFactor,0,0,0},
			{0,scalingFactor,0,0},
			{0,0,scalingFactor,0},
			{0,0,0,1}};

			drawOpenGL( (btScalar*)tmpScaling,convexShape,color,debugMode,worldBoundsMin,worldBoundsMax,basis);
		}
		glPopMatrix();
		return;
	}

	if (shape->getShapeType() == COMPOUND_SHAPE_PROXYTYPE)
	{
		const btCompoundShape* compoundShape = static_cast<const btCompoundShape*>(shape);
		for (int i=compoundShape->getNumChildShapes()-1;i>=0;i--)
		{
			btTransform childTrans = compoundShape->getChildTransform(i);
			const btCollisionShape* colShape = compoundShape->getChildShape(i);
			ATTRIBUTE_ALIGNED16(btScalar) childMat[16];
			childTrans.getOpenGLMatrix(childMat);
			drawOpenGL(childMat,colShape,color,debugMode,worldBoundsMin,worldBoundsMax,basis);
		}

	} else
	{
		if(m_textureenabled&&(!m_textureinitialized))
		{
			int x=0,y=0,n=0;
			char* path = getTexturePath("Textures\\Grass2.png");
			stbi_uc* theTex = stbi_load(path,&x,&y,&n,0);
			delete path;

			GLenum type = GL_RGB;
			if(n==4){type=GL_RGBA;}
			glGenTextures(1,(GLuint*)&m_texturehandle);
			glBindTexture(GL_TEXTURE_2D,m_texturehandle);
			//glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
			glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);
			glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR_MIPMAP_LINEAR);
			glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_MIRRORED_REPEAT);
			glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_MIRRORED_REPEAT);
			gluBuild2DMipmaps(GL_TEXTURE_2D,n,x,y,type,GL_UNSIGNED_BYTE,theTex);
			
			
			int x2=0,y2=0,n2=0;
			char* path2 = getTexturePath("Textures\\scale3.png");
			stbi_uc* theTex2 = stbi_load(path2,&x2,&y2,&n2,0);
			delete path2;

			GLenum type2 = GL_RGB;
			if(n2==4){type2=GL_RGBA;}

			glGenTextures(1,(GLuint*)&m_texturehandle2);
			glBindTexture(GL_TEXTURE_2D,m_texturehandle2);
			glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
			glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);
			glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR_MIPMAP_LINEAR);
			glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
			glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
			gluBuild2DMipmaps(GL_TEXTURE_2D,n2,x2,y2,type2,GL_UNSIGNED_BYTE,theTex2);
			delete[] theTex;
			delete[] theTex2;


		}

		glMatrixMode(GL_TEXTURE);
		glLoadIdentity();
		if(*(int*)(shape->getUserPointer2())==0){
				glScalef(0.025f,0.025f,0.025f);	//scale tex 1 up (the size of the place to put the texture down relative to the texture)
			}else if(*(int*)(shape->getUserPointer2())==1){
				glScalef(2.f,2.f,2.f);	//scale tex 2 down
			}
		glMatrixMode(GL_MODELVIEW);

		//btScalar* openGLMat = new btScalar[12];
		//basis->getOpenGLSubMatrix(openGLMat);
		//static const GLfloat	planex[]={1,0,0,0};
		//static const GLfloat	planex[]={openGLMat[0],openGLMat[1],openGLMat[2],openGLMat[3]};
		//static const GLfloat	planez[]={openGLMat[8],openGLMat[9],openGLMat[10],openGLMat[11]};
		//	static const GLfloat	planey[]={0,1,0,0};
			//static const GLfloat	planez[]={0,0,1,0};
			//glTexGenfv(GL_S,GL_OBJECT_PLANE,planex);
			//glTexGenfv(GL_T,GL_OBJECT_PLANE,planez);
			//glTexGeni(GL_S,GL_TEXTURE_GEN_MODE,GL_OBJECT_LINEAR);
			//glTexGeni(GL_T,GL_TEXTURE_GEN_MODE,GL_OBJECT_LINEAR);
			//glEnable(GL_TEXTURE_GEN_S);
			//glEnable(GL_TEXTURE_GEN_T);
			//glEnable(GL_TEXTURE_GEN_R);
			m_textureinitialized=true;

		
			

		//drawCoordSystem();

		//glPushMatrix();
		glEnable(GL_COLOR_MATERIAL);
		if(m_textureenabled) 
		{
			glEnable(GL_TEXTURE_2D);
			//int x,y,n;
			//m_texturehandle = (unsigned int)stbi_load("Grass2.png", &x, &y, &n, 0);
			//m_texturehandle = 0;
			if(*(int*)(shape->getUserPointer2())==0){
				glBindTexture(GL_TEXTURE_2D,m_texturehandle);	//tex 1
			}else if(*(int*)(shape->getUserPointer2())==1){
				glBindTexture(GL_TEXTURE_2D,m_texturehandle2);	//tex 2
			}
		} else
		{
			glDisable(GL_TEXTURE_2D);
		}

		glColor3f(color.x(),color.y(), color.z());

		bool useWireframeFallback = true;

		if (!(debugMode & btIDebugDraw::DBG_DrawWireframe))
		{
			///you can comment out any of the specific cases, and use the default

			///the benefit of 'default' is that it approximates the actual collision shape including collision margin
			//int shapetype=m_textureenabled?MAX_BROADPHASE_COLLISION_TYPES:shape->getShapeType();
			int shapetype=shape->getShapeType();
			switch (shapetype)
			{

				case SPHERE_SHAPE_PROXYTYPE:
				{
					const btSphereShape* sphereShape = static_cast<const btSphereShape*>(shape);
					float radius = sphereShape->getMargin();//radius doesn't include the margin, so draw with margin
					drawSphere(radius,10,10);
					useWireframeFallback = false;
					break;
				}

				case BOX_SHAPE_PROXYTYPE:
				{
					const btBoxShape* boxShape = static_cast<const btBoxShape*>(shape);
					btVector3 halfExtent = boxShape->getHalfExtentsWithMargin();
					
					static int indices[36] = {
						0,1,2,
						3,2,1,
						4,0,6,
						2,6,0,//6,0,2,
						5,1,4,
						0,4,1,//4,1,0,
						3,1,7,//7,3,1,
						5,7,1,//7,1,5,
						5,4,7,
						6,7,4,//7,4,6,
						3,7,2,//7,2,3,
						6,2,7};//7,6,2};

					 btVector3 vertices[8]={	
						btVector3(halfExtent[0],halfExtent[1],halfExtent[2]),
						btVector3(-halfExtent[0],halfExtent[1],halfExtent[2]),
						btVector3(halfExtent[0],-halfExtent[1],halfExtent[2]),	
						btVector3(-halfExtent[0],-halfExtent[1],halfExtent[2]),	
						btVector3(halfExtent[0],halfExtent[1],-halfExtent[2]),
						btVector3(-halfExtent[0],halfExtent[1],-halfExtent[2]),	
						btVector3(halfExtent[0],-halfExtent[1],-halfExtent[2]),	
						btVector3(-halfExtent[0],-halfExtent[1],-halfExtent[2])};
#if 1
					glBegin (GL_TRIANGLES);
					int si=36;
					/*int val = halfExtent[0];
					if(val>halfExtent[1]){val=halfExtent[1];}
					if(val>halfExtent[2]){val=halfExtent[2];}
					if(val<1){val=1;}
					val=val*6;*/
					int toggle = 1;
					for (int i=0;i<si;i+=3)
					{
						const btVector3& v1 = vertices[indices[i]];;
						const btVector3& v2 = vertices[indices[i+1]];
						const btVector3& v3 = vertices[indices[i+2]];

						btScalar xSize = max((v1).distance(v2),1)*3;
						btScalar ySize = max((v1).distance(v3),1)*3;

						btVector3 normal = (v3-v1).cross(v2-v1);
						normal.normalize ();
						glNormal3f(normal.getX(),normal.getY(),normal.getZ());
						glTexCoord2f(toggle*xSize,toggle*ySize);
						glVertex3f (v1.x(), v1.y(), v1.z());
						glTexCoord2f((1-toggle)*xSize,toggle*ySize);
						glVertex3f (v2.x(), v2.y(), v2.z());
						glTexCoord2f(toggle*xSize,(1-toggle)*ySize);
						glVertex3f (v3.x(), v3.y(), v3.z());
						toggle = 1-toggle;
					}
					glEnd();
#endif

					useWireframeFallback = false;
					break;
				}



#if 0
			
			case CONE_SHAPE_PROXYTYPE:
				{
					const btConeShape* coneShape = static_cast<const btConeShape*>(shape);
					int upIndex = coneShape->getConeUpIndex();
					float radius = coneShape->getRadius();//+coneShape->getMargin();
					float height = coneShape->getHeight();//+coneShape->getMargin();
					switch (upIndex)
					{
					case 0:
						glRotatef(90.0, 0.0, 1.0, 0.0);
						break;
					case 1:
						glRotatef(-90.0, 1.0, 0.0, 0.0);
						break;
					case 2:
						break;
					default:
						{
						}
					};

					glTranslatef(0.0, 0.0, -0.5*height);
					glutSolidCone(radius,height,10,10);
					useWireframeFallback = false;
					break;

				}
#endif

			case STATIC_PLANE_PROXYTYPE:
				{
					const btStaticPlaneShape* staticPlaneShape = static_cast<const btStaticPlaneShape*>(shape);
					btScalar planeConst = staticPlaneShape->getPlaneConstant();
					const btVector3& planeNormal = staticPlaneShape->getPlaneNormal();
					btVector3 planeOrigin = planeNormal * planeConst;
					btVector3 vec0,vec1;
					btPlaneSpace1(planeNormal,vec0,vec1);
					btScalar vecLen = 100.f;
					btVector3 pt0 = planeOrigin + vec0*vecLen;
					btVector3 pt1 = planeOrigin - vec0*vecLen;
					btVector3 pt2 = planeOrigin + vec1*vecLen;
					btVector3 pt3 = planeOrigin - vec1*vecLen;
					glBegin(GL_LINES);
					glVertex3f(pt0.getX(),pt0.getY(),pt0.getZ());
					glVertex3f(pt1.getX(),pt1.getY(),pt1.getZ());
					glVertex3f(pt2.getX(),pt2.getY(),pt2.getZ());
					glVertex3f(pt3.getX(),pt3.getY(),pt3.getZ());
					glEnd();


					break;

				}

/*
			case CYLINDER_SHAPE_PROXYTYPE:
				{
					const btCylinderShape* cylinder = static_cast<const btCylinderShape*>(shape);
					int upAxis = cylinder->getUpAxis();


					float radius = cylinder->getRadius();
					float halfHeight = cylinder->getHalfExtentsWithMargin()[upAxis];

					drawCylinder(radius,halfHeight,upAxis);

					break;
				}
*/

			case MULTI_SPHERE_SHAPE_PROXYTYPE:
			{
				const btMultiSphereShape* multiSphereShape = static_cast<const btMultiSphereShape*>(shape);

				btTransform childTransform;
				childTransform.setIdentity();

				
				for (int i = multiSphereShape->getSphereCount()-1; i>=0;i--)
				{
					btSphereShape sc(multiSphereShape->getSphereRadius(i));
					childTransform.setOrigin(multiSphereShape->getSpherePosition(i));
					ATTRIBUTE_ALIGNED16(btScalar) childMat[16];
					childTransform.getOpenGLMatrix(childMat);
					drawOpenGL(childMat,&sc,color,debugMode,worldBoundsMin,worldBoundsMax,basis);
				}

				break;
			}

			default:
				{
					if (shape->isConvex())
					{
						const btConvexPolyhedron* poly = shape->isPolyhedral() ? ((btPolyhedralConvexShape*) shape)->getConvexPolyhedron() : 0;
						if (poly)
						{
							int i;
							glBegin (GL_TRIANGLES);
							for (i=0;i<poly->m_faces.size();i++)
							{
								btVector3 centroid(0,0,0);
								int numVerts = poly->m_faces[i].m_indices.size();
								if (numVerts>2)
								{
									btVector3 v1 = poly->m_vertices[poly->m_faces[i].m_indices[0]];
									for (int v=0;v<poly->m_faces[i].m_indices.size()-2;v++)
									{
										
										btVector3 v2 = poly->m_vertices[poly->m_faces[i].m_indices[v+1]];
										btVector3 v3 = poly->m_vertices[poly->m_faces[i].m_indices[v+2]];
										btVector3 normal = (v3-v1).cross(v2-v1);
										normal.normalize ();
										glNormal3f(normal.getX(),normal.getY(),normal.getZ());
										glVertex3f (v1.x(), v1.y(), v1.z());
										glVertex3f (v2.x(), v2.y(), v2.z());
										glVertex3f (v3.x(), v3.y(), v3.z());
									}
								}
							}
							glEnd ();
						} else
						{
							ShapeCache*	sc=cache((btConvexShape*)shape);
							//glutSolidCube(1.0);
							btShapeHull* hull = &sc->m_shapehull/*(btShapeHull*)shape->getUserPointer()*/;

							if (hull->numTriangles () > 0)
							{
								int index = 0;
								const unsigned int* idx = hull->getIndexPointer();
								const btVector3* vtx = hull->getVertexPointer();

								glBegin (GL_TRIANGLES);

								for (int i = 0; i < hull->numTriangles (); i++)
								{
									int i1 = index++;
									int i2 = index++;
									int i3 = index++;
									btAssert(i1 < hull->numIndices () &&
										i2 < hull->numIndices () &&
										i3 < hull->numIndices ());

									int index1 = idx[i1];
									int index2 = idx[i2];
									int index3 = idx[i3];
									btAssert(index1 < hull->numVertices () &&
										index2 < hull->numVertices () &&
										index3 < hull->numVertices ());

									btVector3 v1 = vtx[index1];
									btVector3 v2 = vtx[index2];
									btVector3 v3 = vtx[index3];
									btVector3 normal = (v3-v1).cross(v2-v1);
									normal.normalize ();
									glNormal3f(normal.getX(),normal.getY(),normal.getZ());
									glVertex3f (v1.x(), v1.y(), v1.z());
									glVertex3f (v2.x(), v2.y(), v2.z());
									glVertex3f (v3.x(), v3.y(), v3.z());

								}
								glEnd ();

							}
						}
					}
				}
			}

		}


		glNormal3f(0,1,0);
	

		/// for polyhedral shapes
		if (debugMode==btIDebugDraw::DBG_DrawFeaturesText && (shape->isPolyhedral()))
		{
			btPolyhedralConvexShape* polyshape = (btPolyhedralConvexShape*) shape;

			{

				glColor3f(1.f, 1.f, 1.f);
				int i;
				for (i=0;i<polyshape->getNumVertices();i++)
				{
					btVector3 vtx;
					polyshape->getVertex(i,vtx);
					char buf[12];
					sprintf(buf," %d",i);
					//btDrawString(BMF_GetFont(BMF_kHelvetica10),buf);
				}

				for (i=0;i<polyshape->getNumPlanes();i++)
				{
					btVector3 normal;
					btVector3 vtx;
					polyshape->getPlane(normal,vtx,i);
					//btScalar d = vtx.dot(normal);

					//char buf[12];
					//sprintf(buf," plane %d",i);
					//btDrawString(BMF_GetFont(BMF_kHelvetica10),buf);

				}
			}

		}


#ifdef USE_DISPLAY_LISTS

		if (shape->getShapeType() == TRIANGLE_MESH_SHAPE_PROXYTYPE||shape->getShapeType() == GIMPACT_SHAPE_PROXYTYPE)
		{
			GLuint dlist =   OGL_get_displaylist_for_shape((btCollisionShape * )shape);
			if (dlist)
			{
				glCallList(dlist);
			}
			else
			{
#else		
		if (shape->isConcave() && !shape->isInfinite())
		{
			btConcaveShape* concaveMesh = (btConcaveShape*) shape;

			GlDrawcallback drawCallback;
			drawCallback.m_wireframe = (debugMode & btIDebugDraw::DBG_DrawWireframe)!=0;

			concaveMesh->processAllTriangles(&drawCallback,worldBoundsMin,worldBoundsMax);

		}
#endif

#ifdef USE_DISPLAY_LISTS
	}
}
#endif





	}
	glPopMatrix();

}

//
void		GL_ShapeDrawer::drawShadow(btScalar* m,const btVector3& extrusion,const btCollisionShape* shape,const btVector3& worldBoundsMin,const btVector3& worldBoundsMax)
{
	glPushMatrix(); 
	btglMultMatrix(m);
	if(shape->getShapeType() == UNIFORM_SCALING_SHAPE_PROXYTYPE)
	{
		const btUniformScalingShape* scalingShape = static_cast<const btUniformScalingShape*>(shape);
		const btConvexShape* convexShape = scalingShape->getChildShape();
		float	scalingFactor = (float)scalingShape->getUniformScalingFactor();
		btScalar tmpScaling[4][4]={	{scalingFactor,0,0,0},
		{0,scalingFactor,0,0},
		{0,0,scalingFactor,0},
		{0,0,0,1}};
		drawShadow((btScalar*)tmpScaling,extrusion,convexShape,worldBoundsMin,worldBoundsMax);
		glPopMatrix();
		return;
	}
	else if(shape->getShapeType()==COMPOUND_SHAPE_PROXYTYPE)
	{
		const btCompoundShape* compoundShape = static_cast<const btCompoundShape*>(shape);
		for (int i=compoundShape->getNumChildShapes()-1;i>=0;i--)
		{
			btTransform childTrans = compoundShape->getChildTransform(i);
			const btCollisionShape* colShape = compoundShape->getChildShape(i);
			ATTRIBUTE_ALIGNED16(btScalar) childMat[16];
			childTrans.getOpenGLMatrix(childMat);
			drawShadow(childMat,extrusion*childTrans.getBasis(),colShape,worldBoundsMin,worldBoundsMax);
		}
	}
	else
	{
	//	bool useWireframeFallback = true;
	//	glColor3f(0.f,0.f,0.f);
		if (shape->isConvex())
		{
			ShapeCache*	sc=cache((btConvexShape*)shape);
			btShapeHull* hull =&sc->m_shapehull;
			glBegin(GL_QUADS);
			for(int i=0;i<sc->m_edges.size();++i)
			{			
				const btScalar		d=btDot(sc->m_edges[i].n[0],extrusion);
				if((d*btDot(sc->m_edges[i].n[1],extrusion))<0)
				{
					const int			q=	d<0?1:0;
					const btVector3&	a=	hull->getVertexPointer()[sc->m_edges[i].v[q]];
					const btVector3&	b=	hull->getVertexPointer()[sc->m_edges[i].v[1-q]];
					glVertex3f(a[0],a[1],a[2]);
					glVertex3f(b[0],b[1],b[2]);
					glVertex3f(b[0]+extrusion[0],b[1]+extrusion[1],b[2]+extrusion[2]);
					glVertex3f(a[0]+extrusion[0],a[1]+extrusion[1],a[2]+extrusion[2]);
				}
			}
			glEnd();
		}

	}




	if (shape->isConcave())//>getShapeType() == TRIANGLE_MESH_SHAPE_PROXYTYPE||shape->getShapeType() == GIMPACT_SHAPE_PROXYTYPE)
		//		if (shape->getShapeType() == TRIANGLE_MESH_SHAPE_PROXYTYPE)
	{
		btConcaveShape* concaveMesh = (btConcaveShape*) shape;

		GlDrawcallback drawCallback;
		drawCallback.m_wireframe = false;

		concaveMesh->processAllTriangles(&drawCallback,worldBoundsMin,worldBoundsMax);

	}
	glPopMatrix();

}

//
GL_ShapeDrawer::GL_ShapeDrawer()
{
	m_texturehandle			=	0;
	m_textureenabled		=	false;
	m_textureinitialized	=	false;
	m_skyboxinitialized	=	false;
	//glEnable(GL_SEA)
}

GL_ShapeDrawer::~GL_ShapeDrawer()
{
	int i;
	for (i=0;i<m_shapecaches.size();i++)
	{
		m_shapecaches[i]->~ShapeCache();
		btAlignedFree(m_shapecaches[i]);
	}
	m_shapecaches.clear();
	if(m_textureinitialized)
	{
		glDeleteTextures(1,(const GLuint*) &m_texturehandle);
		glDeleteTextures(1,(const GLuint*) &m_texturehandle2);
	}
	if(m_skyboxinitialized){
		for(int i=0;i<m_skyboxhandle->size();i++){
			glDeleteTextures(1,(const GLuint*) &m_skyboxhandle->at(i));
		}
		delete m_skyboxhandle;
	}
}


