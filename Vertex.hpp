////////////////////////////////////////
// Vertex.hpp
////////////////////////////////////////

#pragma once

#include "Core.h"

////////////////////////////////////////////////////////////////////////////////

// Vertex class contains all the information for each vertex in the skin

class Vertex {
public:
	Vertex(glm::vec3 pos, glm::vec3 norm)
		: Pos(pos)
		, Norm(norm)
		, BlendedPos(glm::vec3(0))
		, BlendedNorm(glm::vec3(0))
	{};

	// Setters /////////////////////////////////////////////////////////////////
	void SetPos(glm::vec3 newPos) { Pos = newPos; }
	void SetNorm(glm::vec3 newNorm) { Norm = newNorm; }
	void SetBindingIndices(std::vector<int> newBI) { BindingIndices = newBI; }
	void SetBindingWeights(std::vector<float> newBW) { BindingWeights = newBW; }
	void SetBlendedPos(glm::vec3 newPos) { BlendedPos = newPos; }
	void SetBlendedNorm(glm::vec3 newNorm) { BlendedNorm = newNorm; }

	// Getters /////////////////////////////////////////////////////////////////
	glm::vec3 GetPos() { return Pos; }
	glm::vec3 GetNorm() { return Norm; }
	const std::vector<int>& GetBindingIndices() { return BindingIndices; }
	const std::vector<float>& GetBindingWeights() { return BindingWeights; }
	glm::vec3 GetBlendedPos() { return BlendedPos; }
	glm::vec3 GetBlendedNorm() { return BlendedNorm; }

private:
	glm::vec3 Pos;                      // Coordinates of position
	glm::vec3 Norm;                     // Coordinates of normal
	glm::vec3 BlendedPos;               // Weighted pos
	glm::vec3 BlendedNorm;              // Weight norm
	std::vector<int> BindingIndices;    // Indices of binded matrices
	std::vector<float> BindingWeights;  // Weights for each binded matrix
};

////////////////////////////////////////////////////////////////////////////////