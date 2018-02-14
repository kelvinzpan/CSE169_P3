////////////////////////////////////////
// Joint.h
////////////////////////////////////////

#pragma once

#include "Model.h"
#include "Tokenizer.h"
#include "DOF.hpp"

////////////////////////////////////////////////////////////////////////////////

// Joints are each of the nodes in a skeleton.
// Specific joint classes extend this one.

class Joint {
public:
	Joint();
	~Joint();

	bool Load(Tokenizer &t);
	void MakeLocalMatrix();
	void Update(const glm::mat4x4 &parentMtx);
	void Draw(const glm::mat4x4 &viewProjMtx, uint shader);

	void AddParent(Joint * p) { Parent = p; }
	void AddChild(Joint * c) { Children.push_back(c); }

	void SetPosition(glm::vec3 pos) { Position = pos; }

	const std::vector<DOF*>& GetDOFs() { return DOFs; }
	Joint* GetParent() { return Parent; }
	const std::vector<Joint*>& GetChildren() { return Children; }
	glm::mat4x4 GetWorldMtx() { return WorldMtx; }


private:
	// Constants
	Model JointModel;

	std::vector<DOF *> DOFs;
	glm::vec3 Offset;
	glm::vec3 BoxMin;
	glm::vec3 BoxMax;
	glm::vec2 RotXLimit; // (min, max)
	glm::vec2 RotYLimit;
	glm::vec2 RotZLimit;
	glm::vec3 Pose;

	Joint* Parent;
	std::vector<Joint *> Children;

	// Variables
	glm::mat4x4 LocalMtx;
	glm::mat4x4 WorldMtx;
	glm::vec3 Position;
};

////////////////////////////////////////////////////////////////////////////////
