////////////////////////////////////////
// Skin.h
////////////////////////////////////////

#pragma once

#include "Model.h"
#include "Tokenizer.h"
#include "Triangle.hpp"
#include "Skeleton.hpp"

////////////////////////////////////////////////////////////////////////////////

// Skin

class Skin {
public:
	Skin(const char *file, Skeleton* skell);
	~Skin();

	bool Load(const char *file);
	void SetBuffers();
	void Update();
	void Draw(const glm::mat4x4 &viewProjMtx, uint shader);

private:
	// Constants
	std::vector<Vertex*> Vertices;
	std::vector<Triangle*> Triangles;
	std::vector<glm::mat4x4> Bindings; // Binding matrices, in order parsed
	std::vector<glm::mat4x4> BInverses; // Inverse of all binding matrices

	std::vector<glm::vec3> BlendedPos;
	std::vector<glm::vec3> BlendedNorm;
	std::vector<unsigned int> Indices;

	uint VAO;
	uint VertexBuffer;
	uint NormalBuffer;
	uint IndexBuffer;

	Skeleton* Skell;

	// Variables
};

////////////////////////////////////////////////////////////////////////////////
