/********************************************************************************
* ReactPhysics3D physics library, http://www.reactphysics3d.com                 *
* Copyright (c) 2010-2016 Daniel Chappuis                                       *
*********************************************************************************
*                                                                               *
* This software is provided 'as-is', without any express or implied warranty.   *
* In no event will the authors be held liable for any damages arising from the  *
* use of this software.                                                         *
*                                                                               *
* Permission is granted to anyone to use this software for any purpose,         *
* including commercial applications, and to alter it and redistribute it        *
* freely, subject to the following restrictions:                                *
*                                                                               *
* 1. The origin of this software must not be misrepresented; you must not claim *
*    that you wrote the original software. If you use this software in a        *
*    product, an acknowledgment in the product documentation would be           *
*    appreciated but is not required.                                           *
*                                                                               *
* 2. Altered source versions must be plainly marked as such, and must not be    *
*    misrepresented as being the original software.                             *
*                                                                               *
* 3. This notice may not be removed or altered from any source distribution.    *
*                                                                               *
********************************************************************************/

// Libraries
#include "ConcaveMesh.h"

// Constructor
ConcaveMesh::ConcaveMesh(rp3d::CollisionWorld* world, const std::string& meshPath)
	: PhysicsObject(meshPath) {

	// Compute the scaling matrix
	mScalingMatrix = openglframework::Matrix4::identity();

	// For each subpart of the mesh
	for (unsigned int i = 0; i < getNbParts(); i++) {

		// Vertex and Indices array for the triangle mesh (data in shared and not copied)
		rp3d::TriangleVertexArray* vertexArray =
			new rp3d::TriangleVertexArray(getNbVertices(), &(mVertices[0]), sizeof(openglframework::Vector3),
				&(mNormals[0]), 3 * sizeof(float),
				getNbFaces(i), &(mIndices[i][0]), 3 * sizeof(int),
				rp3d::TriangleVertexArray::VertexDataType::VERTEX_FLOAT_TYPE,
				rp3d::TriangleVertexArray::NormalDataType::NORMAL_FLOAT_TYPE,
				rp3d::TriangleVertexArray::IndexDataType::INDEX_INTEGER_TYPE);

		// Add the triangle vertex array of the subpart to the triangle mesh
		mPhysicsTriangleMesh.addSubpart(vertexArray);
	}

	// Create the collision shape for the rigid body (convex mesh shape) and
	// do not forget to delete it at the end
	mConcaveShape = new rp3d::ConcaveMeshShape(&mPhysicsTriangleMesh);

	mPreviousTransform = rp3d::Transform::identity();

	// Create a rigid body corresponding to the sphere in the dynamics world
	mBody = world->createCollisionBody(mPreviousTransform);

	// Add a collision shape to the body and specify the mass of the collision shape
	mProxyShape = mBody->addCollisionShape(mConcaveShape, rp3d::Transform::identity());

	mTransformMatrix = mTransformMatrix * mScalingMatrix;
}

// Constructor
ConcaveMesh::ConcaveMesh(float mass, rp3d::DynamicsWorld* dynamicsWorld, const std::string& meshPath)
	: PhysicsObject(meshPath) {

	// Compute the scaling matrix
	mScalingMatrix = openglframework::Matrix4::identity();

	// For each subpart of the mesh
	for (unsigned int i = 0; i < getNbParts(); i++) {

		// Vertex and Indices array for the triangle mesh (data in shared and not copied)
		rp3d::TriangleVertexArray* vertexArray =
			new rp3d::TriangleVertexArray(getNbVertices(), &(mVertices[0]), sizeof(openglframework::Vector3),
				getNbFaces(i), &(mIndices[i][0]), 3 * sizeof(int),
				rp3d::TriangleVertexArray::VertexDataType::VERTEX_FLOAT_TYPE,
				rp3d::TriangleVertexArray::IndexDataType::INDEX_INTEGER_TYPE);

		// Add the triangle vertex array of the subpart to the triangle mesh
		mPhysicsTriangleMesh.addSubpart(vertexArray);
	}

	// Create the collision shape for the rigid body (convex mesh shape) and
	// do not forget to delete it at the end
	mConcaveShape = new rp3d::ConcaveMeshShape(&mPhysicsTriangleMesh);

	mPreviousTransform = rp3d::Transform::identity();

	// Create a rigid body corresponding to the sphere in the dynamics world
	rp3d::RigidBody* body = dynamicsWorld->createRigidBody(mPreviousTransform);

	// Add a collision shape to the body and specify the mass of the collision shape
	mProxyShape = body->addCollisionShape(mConcaveShape, rp3d::Transform::identity(), mass);

	mBody = body;

	mTransformMatrix = mTransformMatrix * mScalingMatrix;
}

// Destructor
ConcaveMesh::~ConcaveMesh() {

	// Destroy the triangle mesh data for the physics engine
	for (unsigned int i = 0; i < mPhysicsTriangleMesh.getNbSubparts(); i++) {
		delete mPhysicsTriangleMesh.getSubpart(i);
	}

	// Destroy the mesh
	destroy();

	delete mConcaveShape;
}
