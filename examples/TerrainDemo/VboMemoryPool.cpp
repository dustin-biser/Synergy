#include "VboMemoryPool.hpp"

#include <sstream>
#include <Synergy/Core/Exception.hpp>

using namespace Synergy;
using namespace std;
using VboAvailability::InUse;
using VboAvailability::Available;


//---------------------------------------------------------------------------------------
VboMemoryPool::VboMemoryPool() {

}

//---------------------------------------------------------------------------------------
VboMemoryPool::~VboMemoryPool() {
	GLuint vbo;
	for(auto key_value : vboAvailabilityMap) {
		vbo = key_value.first;
		glDeleteBuffers(1, &vbo);
		CHECK_GL_ERRORS;
	}
}

//---------------------------------------------------------------------------------------
void VboMemoryPool::allocateStorage(GLsizei numVbos, GLsizei bytesPerVbo) {
	GLuint vbo;
	for(GLsizei i(0); i < numVbos; ++i) {
		glGenBuffers(1, &vbo);
		pair<GLuint, VboAvailability> key_value(vbo, Available);
		vboAvailabilityMap.insert(key_value);
		availableVbos.push(vbo);

		allocateVertexBufferStorage(vbo, bytesPerVbo);
	}
}

//---------------------------------------------------------------------------------------
void VboMemoryPool::requestVertexBuffer(GLuint & vbo) {
	if (availableVbos.size() > 0) {
		vbo = availableVbos.top();
		vboAvailabilityMap[vbo] = InUse;
		availableVbos.pop();
	}
}

//---------------------------------------------------------------------------------------
void VboMemoryPool::reclaimVertexBuffer(GLuint & vbo) {
	if (vboAvailabilityMap.count(vbo) > 0) {
		vboAvailabilityMap[vbo]	= Available;
		availableVbos.push(vbo);
		vbo = 0;
	} else {
		stringstream stream;
		stream << "Invalid vertex buffer object (val = " << vbo << ") passed to "
				"VboMemoryPool::reclaimVertexBuffer(...)" << endl;
		stream << "No such vertex buffer object exists in VboMemoryPool." << endl;
		throw Exception(stream.str());
	}
}


//---------------------------------------------------------------------------------------
void VboMemoryPool::allocateVertexBufferStorage(GLuint vbo, GLsizei numBytes) {
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, numBytes, nullptr, GL_DYNAMIC_COPY);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	CHECK_GL_ERRORS;
}
