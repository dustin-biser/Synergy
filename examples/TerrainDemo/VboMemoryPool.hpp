/// VboMemoryPool

#pragma once

#include <Synergy/Synergy.hpp>

#include <stack>
#include <unordered_map>

enum class VboAvailability {
	InUse,
	Available
};

class VboMemoryPool {
public:
	VboMemoryPool();

	~VboMemoryPool();

	/// Creates storage for \c numVbos vetex buffers, where each buffer
	/// is of size \c bytesPerVbo.
	void allocateStorage(GLsizei numVbos, GLsizei bytesPerVbo);

	/// Sets vbo equal to a buffer object name that is associated with
	/// pre-allocated vertex buffer storage.
	void requestVertexBuffer(GLuint & vbo);

	/// Restores vertex buffer storage associated with \c vbo back
	/// to memory pool, and sets \c vbo to 0.
	void reclaimVertexBuffer(GLuint & vbo);

private:
	void allocateVertexBufferStorage(GLuint vbo, GLsizei numBytes);


	std::unordered_map<GLuint, VboAvailability> vboAvailabilityMap;
	std::stack<GLuint> availableVbos;
};