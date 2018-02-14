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
		std::vector<float> pose(3 + skell->GetJoints().size() * 3);
		this->Pose = pose;

		this->IsLooping = true;
		this->IsPaused = false;
		this->Speed = 0.05f;
	};

	////////////////////////////////////////////////////////////////////////////

	void Update()
	{
		// Increment time, playback options
		if (!this->IsPaused) this->CurrTime += this->Speed;
		if (this->IsLooping && 
			(this->CurrTime > this->Anim->GetEndTime() ||
			this->CurrTime < this->Anim->GetStartTime()) ) this->CurrTime = this->Anim->GetStartTime();

		// Evaluate animation
		this->Anim->Evaluate(this->CurrTime, this->Pose);

		// First 3 DOFs are position
		Joint* root = this->Skell->GetRoot();
		root->SetPosition(glm::vec3(this->Pose[0], this->Pose[1], this->Pose[2]));

		// Rest of the DOFs are rotations
		for (unsigned int i = 3; i < this->Pose.size(); i++)
		{
			int jointNum = (i - 3) / 3;
			int DOFNum = (i - 3) % 3;
			this->Skell->GetJoint(jointNum)->GetDOFs()[DOFNum]->SetValue(this->Pose[i]);
		}
	}

	// Adjust Playback /////////////////////////////////////////////////////////
	void ResetAnim()     { SetCurrTime(this->Anim->GetStartTime()); }
	void SpeedUp()       { this->Speed += 0.05f; }
	void SlowDown()      { this->Speed -= 0.05f; }
	void ToggleLooping() { this->IsLooping = !this->IsLooping; }
	void TogglePause()   { this->IsPaused = !this->IsPaused; }

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
	float GetSpeed()                    { return Speed; }

private:
	Animation* Anim;
	Skeleton* Skell;

	float CurrTime;
	std::vector<float> Pose;
	
	bool IsLooping;
	bool IsPaused;
	float Speed;
};

////////////////////////////////////////////////////////////////////////////////