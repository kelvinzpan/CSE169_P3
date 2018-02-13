////////////////////////////////////////
// AnimationPlayer.hpp
////////////////////////////////////////

#pragma once

#include "Tokenizer.h"
#include "Animation.hpp"
#include "Skeleton.hpp"

////////////////////////////////////////////////////////////////////////////////

// The AnimationPlayer class plays a given Animation by accessing Animation,
// Skeleton data while keeping track of time

class AnimationPlayer {
public:
	AnimationPlayer(Animation* anim, Skeleton* skell)
	{
		this->Anim = anim;
		this->Skell = skell;
		this->CurrTime = anim->GetStartTime();
		std::vector<float> pose(skell->GetJoints().size() * 3);
		this->Pose = pose;
		this->Speed = 0.05f;
	};

	////////////////////////////////////////////////////////////////////////////

	void Update()
	{
		// Increment time
		this->CurrTime += this->Speed;
		if (this->CurrTime > this->Anim->GetEndTime()) this->CurrTime = this->Anim->GetStartTime();

		// Evaluate animation
		this->Anim->Evaluate(this->CurrTime, this->Pose);

		// Pose rig
		for (unsigned int i = 0; i < this->Pose.size(); i++)
		{
			int jointNum = i / 3;
			int DOFNum = i % 3;
			this->Skell->GetJoint(jointNum)->GetDOFs()[DOFNum]->SetValue(this->Pose[i]);
		}
	}

	// Setters /////////////////////////////////////////////////////////////////
	void SetAnim(Animation* anim)         { Anim = anim; }
	void SetSkell(Skeleton* skell)        { Skell = skell; }
	void SetCurrTime(float time)          { CurrTime = time; }
	void SetPose(std::vector<float> pose) { Pose = pose; }

	// Getters /////////////////////////////////////////////////////////////////
	Animation* GetAnim()                { return Anim; }
	Skeleton* GetSkell()                { return Skell; }
	float GetCurrTime()                 { return CurrTime; }
	const std::vector<float>& GetPose() { return Pose; }

private:
	Animation* Anim;
	Skeleton* Skell;

	float CurrTime;
	std::vector<float> Pose;
	
	float Speed;
};

////////////////////////////////////////////////////////////////////////////////