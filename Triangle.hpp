////////////////////////////////////////
// Triangle.hpp
////////////////////////////////////////

#pragma once

#include "Vertex.hpp"

////////////////////////////////////////////////////////////////////////////////

// Triangle class is composed of the vertices for the skin

class Triangle {
public:
	Triangle(Vertex* v0, Vertex* v1, Vertex* v2)
		: V0(v0)
		, V1(v1)
		, V2(v2)
	{};

	// Setters /////////////////////////////////////////////////////////////////
	void SetTriangles(Vertex* newV0, Vertex* newV1, Vertex* newV2)
	{
		V0 = newV0;
		V1 = newV1;
		V2 = newV2;
	}
	void SetIndices(std::vector<int> newIndices) { Indices = newIndices; }

	// Getters /////////////////////////////////////////////////////////////////
	const std::vector<Vertex*>& GetTriangles() { return std::vector<Vertex*>{V0, V1, V2}; }
	const std::vector<int>& GetIndices() { return Indices; }

private:
	Vertex* V0;
	Vertex* V1;
	Vertex* V2;
	std::vector<int> Indices;
};

////////////////////////////////////////////////////////////////////////////////