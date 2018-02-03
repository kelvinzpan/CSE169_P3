////////////////////////////////////////
// Skeleton.hpp
////////////////////////////////////////

#pragma once

#include "Joint.h"

////////////////////////////////////////////////////////////////////////////////

// Skeleton holds the root of a Joint tree

class Skeleton {
public:
	Skeleton(const char *file)
	{
		World = glm::mat4x4(1);
		JointPtr = 0;
		DOFPtr = 0;
		Load(file);
		CreateJointList();
	}

	~Skeleton()
	{
		delete(Root);
	}

	////////////////////////////////////////////////////////////////////////////

	bool Load(const char *file)
	{
		Tokenizer token;
		token.Open(file);
		token.FindToken("balljoint");

		// Parse tree
		Root = new Joint();
		Root->Load(token);

		// Finish
		token.Close();
		return true;
	}

	////////////////////////////////////////////////////////////////////////////

	void CreateJointList()
	{
		// Preorder traversal using recursion
		CJLHelper(Root);
	}

	void CJLHelper(Joint* currJoint)
	{
		Joints.push_back(currJoint);
		for (Joint* child : currJoint->GetChildren()) CJLHelper(child);
	}

	////////////////////////////////////////////////////////////////////////////

	glm::mat4x4 GetJointWorldMtx(unsigned int jointNum)
	{
		if (jointNum >= Joints.size())
		{
			std::cerr << "Invalid joint number " << jointNum << ", there are " << Joints.size() << " joints." << std::endl;
			return glm::mat4x4(1);
		}
		return Joints[jointNum]->GetWorldMtx();
	}

	////////////////////////////////////////////////////////////////////////////
	
	void PrevDOF()
	{
		DOFPtr--;

		if (DOFPtr < 0)
		{
			JointPtr--;
			if (JointPtr < 0) JointPtr = Joints.size() - 1;
			DOFPtr = Joints[JointPtr]->GetDOFs().size() - 1;
		}

		std::cout << "Moved to Joint " << JointPtr << ", DOF " << DOFPtr << std::endl;
	}

	void NextDOF()
	{
		DOFPtr++;

		if ((unsigned int) DOFPtr >= Joints[JointPtr]->GetDOFs().size())
		{
			DOFPtr = 0;
			JointPtr++;
			if (JointPtr == Joints.size()) JointPtr = 0;
		}

		std::cout << "Moved to Joint " << JointPtr << ", DOF " << DOFPtr << std::endl;
	}

	void PrintDOF() { std::cout << "Currently on Joint " << JointPtr << ", DOF " << DOFPtr << std::endl; }

	////////////////////////////////////////////////////////////////////////////

	void IncreaseDOF()
	{
		DOF* currDOF = Joints[JointPtr]->GetDOFs()[DOFPtr];
		currDOF->IncreaseValue();
		std::cout << "Increased Joint " << JointPtr << ", DOF " << DOFPtr << " value to " << currDOF->GetValue() << std::endl;
	}

	void DecreaseDOF()
	{
		DOF* currDOF = Joints[JointPtr]->GetDOFs()[DOFPtr];
		currDOF->DecreaseValue();
		std::cout << "Increased Joint " << JointPtr << ", DOF " << DOFPtr << " value to " << currDOF->GetValue() << std::endl;
	}

	////////////////////////////////////////////////////////////////////////////

	void Update()
	{
		Root->Update(World);
	}

	////////////////////////////////////////////////////////////////////////////

	void Draw(const glm::mat4x4 &viewProjMtx, uint shader)
	{
		Root->Draw(viewProjMtx, shader);
	}

	// Setters /////////////////////////////////////////////////////////////////
	void SetWorldMtx(glm::mat4x4 newWorld) { World = newWorld; }

	// Getters /////////////////////////////////////////////////////////////////
	Joint* GetRoot() { return Root; }
	glm::mat4x4 GetWorldMtx() { return World; }

private:
	// Constants
	Joint * Root;
	std::vector<Joint*> Joints;

	// Variables
	glm::mat4x4 World;
	int JointPtr; // Current index of Joints
	int DOFPtr; // Current index of DOFs for current Joint
};

////////////////////////////////////////////////////////////////////////////////