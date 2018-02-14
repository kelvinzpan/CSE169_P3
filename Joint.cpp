////////////////////////////////////////
// Joint.cpp
////////////////////////////////////////

#include "Joint.h"

////////////////////////////////////////////////////////////////////////////////

Joint::Joint()
{
	// Default values, overwritten by Load if available
	this->Offset = glm::vec3(0, 0, 0);
	this->BoxMin = glm::vec3(-0.1f, -0.1f, -0.1f);
	this->BoxMax = glm::vec3(0.1f, 0.1f, 0.1f);
	this->RotXLimit = glm::vec2(-100000, 100000);
	this->RotYLimit = glm::vec2(-100000, 100000);
	this->RotZLimit = glm::vec2(-100000, 100000);
	this->Pose = glm::vec3(0, 0, 0);

	this->Position = glm::vec3(0);
}

////////////////////////////////////////////////////////////////////////////////

Joint::~Joint()
{
	for (DOF * d : this->DOFs) delete(d);
	for (Joint * c : this->Children) delete(c);
}

////////////////////////////////////////////////////////////////////////////////

bool Joint::Load(Tokenizer &token)
{
	token.FindToken("{");

	while(true) {
		char temp[256];
		token.GetToken(temp);

		// Parse values into variables
		if(strcmp(temp,"offset")==0)
		{
			this->Offset.x=token.GetFloat();
			this->Offset.y=token.GetFloat();
			this->Offset.z=token.GetFloat();
		}
		else if(strcmp(temp, "boxmin") == 0)
		{
			this->BoxMin.x = token.GetFloat();
			this->BoxMin.y = token.GetFloat();
			this->BoxMin.z = token.GetFloat();
		}
		else if (strcmp(temp, "boxmax") == 0)
		{
			this->BoxMax.x = token.GetFloat();
			this->BoxMax.y = token.GetFloat();
			this->BoxMax.z = token.GetFloat();
		}
		else if (strcmp(temp, "rotxlimit") == 0)
		{
			this->RotXLimit.x = token.GetFloat();
			this->RotXLimit.y = token.GetFloat();
		}
		else if (strcmp(temp, "rotylimit") == 0)
		{
			this->RotYLimit.x = token.GetFloat();
			this->RotYLimit.y = token.GetFloat();
		}
		else if (strcmp(temp, "rotzlimit") == 0)
		{
			this->RotZLimit.x = token.GetFloat();
			this->RotZLimit.y = token.GetFloat();
		}
		else if (strcmp(temp, "pose") == 0)
		{
			this->Pose.x = token.GetFloat();
			this->Pose.y = token.GetFloat();
			this->Pose.z = token.GetFloat();
		}
		else if(strcmp(temp,"balljoint")==0)
		{
			Joint * jnt = new Joint();
			jnt->Load(token);
			this->AddChild(jnt);
			jnt->AddParent(this);
		}
		else if(strcmp(temp,"}")==0) break;
		else token.SkipLine(); // Unrecognized token
	}

	// Load variables
	this->JointModel.MakeBox(this->BoxMin, this->BoxMax);
	
	DOF * RotX = new DOF(this->Pose.x, this->RotXLimit.x, this->RotXLimit.y);
	this->DOFs.push_back(RotX);
	DOF * RotY = new DOF(this->Pose.y, this->RotYLimit.x, this->RotYLimit.y);
	this->DOFs.push_back(RotY);
	DOF * RotZ = new DOF(this->Pose.z, this->RotZLimit.x, this->RotZLimit.y);
	this->DOFs.push_back(RotZ);

	return true;
}

////////////////////////////////////////////////////////////////////////////////

void Joint::MakeLocalMatrix()
{
	// Start with identity
	this->LocalMtx = glm::mat4x4(1);

	// Apply translation
	glm::mat4x4 toTrans = glm::translate(glm::mat4x4(1), this->Offset + this->Position);

	// Apply rotations
	glm::mat4x4 xRot = glm::rotate(glm::mat4x4(1), this->DOFs[0]->GetValue(), glm::vec3(1, 0, 0));
	glm::mat4x4 yRot = glm::rotate(glm::mat4x4(1), this->DOFs[1]->GetValue(), glm::vec3(0, 1, 0));
	glm::mat4x4 zRot = glm::rotate(glm::mat4x4(1), this->DOFs[2]->GetValue(), glm::vec3(0, 0, 1));

	// Multiply
	this->LocalMtx = toTrans * this->LocalMtx * zRot * yRot * xRot;
}

////////////////////////////////////////////////////////////////////////////////

void Joint::Update(const glm::mat4x4 &parentMtx)
{
	// Compute LocalMatrix
	MakeLocalMatrix();

	// Compute WorldMatrix
	this->WorldMtx = parentMtx * this->LocalMtx;

	// Recursively call Update() on children
	for (Joint * c : this->Children)
	{
		c->Update(this->WorldMtx);
	}
}

////////////////////////////////////////////////////////////////////////////////

void Joint::Draw(const glm::mat4x4 &viewProjMtx, uint shader)
{
	// OpenGL stuff
	JointModel.Draw(this->WorldMtx, viewProjMtx, shader);

	// Recursively call Draw() on children
	for (Joint * c : this->Children)
	{
		c->Draw(viewProjMtx, shader);
	}
}

////////////////////////////////////////////////////////////////////////////////
