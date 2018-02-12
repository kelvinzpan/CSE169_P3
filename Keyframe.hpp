////////////////////////////////////////
// Keyframe.hpp
////////////////////////////////////////

#pragma once

#include "Core.h"

////////////////////////////////////////////////////////////////////////////////

// The Keyframe class represents a time in the animation

class Keyframe {
public:
	Keyframe(float time, float value, char ruleIn, char ruleOut)
		: Time(time)
		, Value(value)
		, RuleIn(ruleIn)
		, RuleOut(ruleOut)
	{
	};

	// Setters /////////////////////////////////////////////////////////////////
	void SetTime(float time)         { Time = time; }
	void SetValue(float value)       { Value = value; }
	void SetTangentIn(float tanIn)   { TangentIn = tanIn; }
	void SetTangentOut(float tanOut) { TangentOut = tanOut; }
	void SetRuleIn(char ruleIn)      { RuleIn = ruleIn; }
	void SetRuleOut(char ruleOut)    { RuleOut = ruleOut; }
	void SetCubicCoeff(float a, float b, float c, float d)
	{
		A = a;
		B = b;
		C = c;
		D = d;
	}

	// Getters /////////////////////////////////////////////////////////////////
	float GetTime()       { return Time; }
	float GetValue()      { return Value; }
	float GetTangentIn()  { return TangentIn; }
	float GetTangentOut() { return TangentOut; }
	char GetRuleIn()      { return RuleIn; }
	char GetRuleOut()     { return RuleOut; }
	float GetA()          { return A; }
	float GetB()          { return B; }
	float getC()          { return C; }
	float getD()          { return D; }

private:
	float Time;
	float Value;
	float TangentIn, TangentOut;
	char RuleIn, RuleOut; // Tangent rules
	float A, B, C, D; // Cubic coefficients
};

////////////////////////////////////////////////////////////////////////////////