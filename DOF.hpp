////////////////////////////////////////
// DOF.hpp
////////////////////////////////////////

#pragma once

#include "Core.h"

////////////////////////////////////////////////////////////////////////////////

// The DOF class is used by Joint and other classes to indicate degrees of
// freedom in a skeleton.

class DOF {
public:
	DOF(float v, float min, float max)
		: value(v)
		, min(min)
		, max(max)
	{
		if (min > max) std::cerr << "Invalid DOF defined." << std::endl;
		if (value < min || value > max)
		{
			std::cerr << "Invalid DOF value (" << value << "), clamping.";
			SetValue(value);
			std::cerr << " Value is now " << value << "." << std::endl;
		}
	};

	void IncreaseValue()
	{
		SetValue(this->value + 0.1f);
	}

	void DecreaseValue()
	{
		SetValue(this->value - 0.1f);
	}

	// Setters /////////////////////////////////////////////////////////////////
	void SetValue(float v)
	{
		value = v;
		if (value > max) value = max;
		else if (value < min) value = min;
	}
	void SetMin(float m)
	{
		if (m > max) std::cerr << "Invalid min for DOF." << std::endl;
		else
		{
			min = m;
			SetValue(GetValue());
		}
	}
	void SetMax(float m)
	{
		if (m < min) std::cerr << "Invalid max for DOF." << std::endl;
		else
		{
			max = m;
			SetValue(GetValue());
		}
	}
	void SetMinMax(float min, float max)
	{
		if (min > max) std::cerr << "Invalid min/max for DOF." << std::endl;
		else
		{
			this->min = min;
			this->max = max;
			SetValue(GetValue());
		}
	}

	// Getters /////////////////////////////////////////////////////////////////
	float GetValue() { return value; }
	float GetMin() { return min; }
	float GetMax() { return max; }

private:
	float value;		// Current value of DOF
	float min;			// Min value of DOF
	float max;			// Max value of DOF
};

////////////////////////////////////////////////////////////////////////////////