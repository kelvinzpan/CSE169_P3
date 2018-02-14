////////////////////////////////////////
// Channel.hpp
////////////////////////////////////////

#pragma once

#include "Tokenizer.h"
#include "Keyframe.hpp"

////////////////////////////////////////////////////////////////////////////////

// The Channel class animates a single DOF by interpolating between keyframes

class Channel {
public:
	Channel()
	{
	};

	////////////////////////////////////////////////////////////////////////////

	bool Load(Tokenizer &token)
	{
		// Token currently at "channel {
		// Get extrapolations
		token.FindToken("extrapolate");
		char extrapInTemp[256];
		token.GetToken(extrapInTemp);
		if (strcmp(extrapInTemp, "constant") == 0) this->ExtrapIn = "constant";
		else if (strcmp(extrapInTemp, "linear") == 0) this->ExtrapIn = "linear";
		else if (strcmp(extrapInTemp, "cycle") == 0) this->ExtrapIn = "cycle";
		else if (strcmp(extrapInTemp, "cycle_offset") == 0) this->ExtrapIn = "cycle_offset";
		else if (strcmp(extrapInTemp, "bounce") == 0) this->ExtrapIn = "bounce";
		char extrapOutTemp[256];
		token.GetToken(extrapOutTemp);
		if (strcmp(extrapOutTemp, "constant") == 0) this->ExtrapOut = "constant";
		else if (strcmp(extrapOutTemp, "linear") == 0) this->ExtrapOut = "linear";
		else if (strcmp(extrapOutTemp, "cycle") == 0) this->ExtrapOut = "cycle";
		else if (strcmp(extrapOutTemp, "cycle_offset") == 0) this->ExtrapOut = "cycle_offset";
		else if (strcmp(extrapOutTemp, "bounce") == 0) this->ExtrapOut = "bounce";

		token.FindToken("keys");
		int numKeys = token.GetInt();
		//std::cout << "Channel with " << numKeys << " keys and " <<
		//	this->ExtrapIn << " in and " << this->ExtrapOut << " out." << std::endl;

		// Parse keys
		token.FindToken("{");
		for (int i = 0; i < numKeys; i++)
		{
			float time = token.GetFloat();
			float value = token.GetFloat();
			char tanIn, tanOut;
			float tanInVal = 0.0f;
			float tanOutVal = 0.0f;
			
			// Get tangents
			char tanInTemp[256];
			token.GetToken(tanInTemp);
			if (strcmp(tanInTemp, "flat") == 0) tanIn = 'f';
			else if (strcmp(tanInTemp, "linear") == 0) tanIn = 'l';
			else if (strcmp(tanInTemp, "smooth") == 0) tanIn = 's';
			else
			{
				tanIn = 'x';
				tanInVal = (float) std::atof(tanInTemp);
			}
			char tanOutTemp[256];
			token.GetToken(tanOutTemp);
			if (strcmp(tanOutTemp, "flat") == 0) tanOut = 'f';
			else if (strcmp(tanOutTemp, "linear") == 0) tanOut = 'l';
			else if (strcmp(tanOutTemp, "smooth") == 0) tanOut = 's';
			else
			{
				tanOut = 'x';
				tanOutVal = (float) std::atof(tanOutTemp);
			}

			// Make keyframe
			Keyframe newKey = Keyframe(time, value, tanIn, tanOut);
			newKey.SetRuleIn(tanIn);
			newKey.SetRuleOut(tanOut);
			newKey.SetTangentIn(tanInVal);
			newKey.SetTangentOut(tanOutVal);
			this->Keys.push_back(newKey);

			//std::cout << "Keyframe with time " << time << " and value " << value <<
			//	" and ruleIn " << tanIn << " and ruleOut " << tanOut << 
			//	" and tanIn " << tanInVal << " and tanOut " << tanOutVal << std::endl;
		}

		// Finish
		this->StartTime = this->Keys[0].GetTime();
		this->EndTime = this->Keys[this->Keys.size() - 1].GetTime();
		this->StartValue = this->Keys[0].GetValue();
		this->EndValue = this->Keys[this->Keys.size() - 1].GetValue();
		this->Precompute();
		return true;
	}

	////////////////////////////////////////////////////////////////////////////

	void Precompute()
	{
		// Compute tangents from given rules
		for (unsigned int i = 0; i < this->Keys.size(); i++)
		{
			Keyframe key = this->Keys[i];

			// Calculate rule in ///////////////////////////////////////////////
			if (key.GetRuleIn() == 'f')
			{
				// Fixed tangent
				key.SetTangentIn(0.0f);
			}
			else if (key.GetRuleIn() == 'l' && i != 0)
			{
				// Linear tangent, invalid for first key
				Keyframe prevKey = this->Keys[i - 1];
				key.SetTangentIn((key.GetValue() - prevKey.GetValue()) / (key.GetTime() - prevKey.GetTime()));

			}
			else if (key.GetRuleIn() == 's' && i != 0 && i != this->Keys.size() - 1)
			{
				// Smooth tangent, invalid for first and last keys
				Keyframe prevKey = this->Keys[i - 1];
				Keyframe nextKey = this->Keys[i + 1];
				key.SetTangentIn((nextKey.GetValue() - prevKey.GetValue()) / (nextKey.GetTime() - prevKey.GetTime()));
			}

			// Calculate rule out //////////////////////////////////////////////
			if (key.GetRuleOut() == 'f')
			{
				// Fixed tangent
				key.SetTangentOut(0.0f);
			}
			else if (key.GetRuleOut() == 'l' && i != this->Keys.size() - 1)
			{
				// Linear tangent, invalid for last key
				Keyframe nextKey = this->Keys[i + 1];
				key.SetTangentOut((nextKey.GetValue() - key.GetValue()) / (nextKey.GetTime() - key.GetTime()));

			}
			else if (key.GetRuleIn() == 's' && i != 0 && i != this->Keys.size() - 1)
			{
				// Smooth tangent, invalid for first and last keys
				Keyframe prevKey = this->Keys[i - 1];
				Keyframe nextKey = this->Keys[i + 1];
				key.SetTangentOut((nextKey.GetValue() - prevKey.GetValue()) / (nextKey.GetTime() - prevKey.GetTime()));
			}

			// Account for linears and smooths at the end (default to linear)
			if (true)
				0;
			else if (i == 0 && this->Keys.size() > 1)
			{
				if (key.GetRuleOut() == 's')
				{
					Keyframe nextKey = this->Keys[i + 1];
					key.SetTangentIn((nextKey.GetValue() - key.GetValue()) / (nextKey.GetTime() - key.GetTime()));
				}
				if (key.GetRuleIn() == 'l' || key.GetRuleIn() == 's') key.SetTangentIn(key.GetTangentOut());
			}
			else if (i == this->Keys.size() - 1 && this->Keys.size() > 1)
			{
				if (key.GetRuleIn() == 's')
				{
					Keyframe prevKey = this->Keys[i - 1];
					key.SetTangentIn((key.GetValue() - prevKey.GetValue()) / (key.GetTime() - prevKey.GetTime()));
				}
				if (key.GetRuleOut() == 'l' || key.GetRuleOut() == 's') key.SetTangentOut(key.GetTangentIn());
			}
		}

		// Compute cubic coefficients from tangents and other data
		glm::mat4x4 hermiteMat;
		hermiteMat[0] = { 2, -3, 0, 1 };
		hermiteMat[1] = { -2, 3, 0, 0 };
		hermiteMat[2] = { 1, -2, 1, 0 };
		hermiteMat[3] = { 1, -1, 0, 0 };

		if (this->Keys.size() == 1) std::cout << "ONE KEYFRAME ==============================" << std::endl;
		else std::cout << "MULTIPLE KEYFRAMES ==============================" << std::endl;

		// First keyframe in each span holds the coefficients
		for (unsigned int i = 0; i < this->Keys.size() - 1; i++)
		{
			Keyframe key = this->Keys[i];
			Keyframe nextKey = this->Keys[i + 1];

			float timeDiff = nextKey.GetTime() - key.GetTime();
			glm::vec4 hermiteVec = glm::vec4(
				key.GetValue(),
				nextKey.GetValue(),
				timeDiff * key.GetTangentOut(),
				timeDiff * nextKey.GetTangentIn());
			glm::vec4 coeffs = hermiteMat * hermiteVec;

			//glm::vec4 coeffs;
			//float p0 = key.GetValue();
			//float p1 = nextKey.GetValue();
			//float tv0 = (nextKey.GetTime() - key.GetTime()) * key.GetTangentOut();
			//float tv1 = (nextKey.GetTime() - key.GetTime()) * nextKey.GetTangentIn();
			//coeffs.x = 2*p0 - 2*p1 + tv0 + tv1;
			//coeffs.y = -3*p0 + 3*p1 - 2*tv0 - tv1;
			//coeffs.z = tv0;
			//coeffs.w = p0;
			//key.SetCubicCoeff(coeffs.x, coeffs.y, coeffs.z, coeffs.w);

			std::cout << "KEYFRAME " << i << ": Rule in: " << key.GetRuleIn() << ", Tan in: " << key.GetTangentIn() <<
				", Rule out: " << key.GetRuleOut() << ", Tan out: " << key.GetTangentOut() << std::endl;
			printVec4(coeffs);
			std::cout << std::endl;
		}
	}

	////////////////////////////////////////////////////////////////////////////

	float Evaluate(float time)
	{
		// Case 1: before the first key
		if (time < this->StartTime)
		{
			// Extrapolate
			if (this->ExtrapIn == "constant")
			{
				return this->StartValue; // Backwards, get first
			}
			else if (this->ExtrapIn == "linear")
			{
				float slope = this->Keys[0].GetTangentIn(); // Backwards, get first
				float t = this->StartTime;
				return this->StartValue - slope * (t - time); // Backwards, subtract
			}
			else if (this->ExtrapIn == "cycle")
			{
				float timeDiff = this->EndTime - this->StartTime;
				while (time < this->StartTime) time += timeDiff; // Backwards, add
				return this->Evaluate(time);
			}
			else if (this->ExtrapIn == "cycle_offset")
			{
				float timeDiff = this->EndTime - this->StartTime;
				float valDiff = this->EndValue - this->StartValue;
				float offset = 0;
				while (time < this->StartTime) // Backwards, add
				{
					time += timeDiff;
					offset -= valDiff; // Backwards, subtract
				}
				return offset + this->Evaluate(time);
			}
			else if (this->ExtrapIn == "bounce")
			{
				float timeDiff = this->EndTime - this->StartTime;
				int count = 0;
				while (time < this->StartTime) { // Backwards, add
					time += timeDiff;
					count++;
				}
				if (count % 2 == 1) { // Odd, reflect
					time = this->StartTime + (timeDiff - time);
					return this->Evaluate(time);
				}
				else // Even, keep
				{
					return this->Evaluate(time);
				}
			}
			else
			{
				std::cerr << "No ExtrapIn rule detected." << std::endl;
				return 0.0f;
			}
		}
		// Case 2: after the last key
		else if (time > this->EndTime)
		{
			// Extrapolate
			if (this->ExtrapOut == "constant")
			{
				return this->EndValue; // Forwards, get last
			}
			else if (this->ExtrapOut == "linear")
			{
				float slope = this->Keys[this->Keys.size() - 1].GetTangentOut(); // Forwards, get last
				float t = this->EndTime;
				return this->EndValue + slope * (time - t); // Forwards, add
			}
			else if (this->ExtrapOut == "cycle")
			{
				float timeDiff = this->EndTime - this->StartTime;
				while (time > this->EndTime) time -= timeDiff; // Forwards, subtract
				return this->Evaluate(time);
			}
			else if (this->ExtrapOut == "cycle_offset")
			{
				float timeDiff = this->EndTime - this->StartTime;
				float valDiff = this->EndValue - this->StartValue;
				float offset = 0;
				while (time > this->EndTime) // Forwards, subtract
				{
					time -= timeDiff;
					offset += valDiff; // Forwards, add
				}
				return offset + this->Evaluate(time);
			}
			else if (this->ExtrapOut == "bounce")
			{
				float timeDiff = this->EndTime - this->StartTime;
				int count = 0;
				while (time > this->EndTime) { // Forwards, subtract
					time -= timeDiff;
					count++;
				}
				if (count % 2 == 1) { // Odd, reflect
					time = this->StartTime + (timeDiff - time);
					return this->Evaluate(time);
				}
				else // Even, keep
				{
					return this->Evaluate(time);
				}
			}
			else
			{
				std::cerr << "No ExtrapOut rule detected." << std::endl;
				return 0.0f;
			}
		}
		// Case 3: within a key
		else
		{
			// Binary search for the span
			int l = 0;
			int r = this->Keys.size() - 1;

			while (l <= r)
			{
				int m = (l + r) / 2;
				Keyframe mkey = this->Keys[m];

				//std::cout << "Left is " << l << " and right is " << r << " and m is " << m << std::endl;
				//std::cout << "Time is " << time << " and mkey time is " << mkey.GetTime() << std::endl;

				// Exact match
				if (mkey.GetTime() == time)
				{
					//if (m!= 0) std::cout << "Found Keyframe " << m << "!" << std::endl;
					return mkey.GetValue();
				}

				// Keep searching
				if (time < mkey.GetTime())
				{
					r = m - 1;
				}
				else
				{
					l = m + 1;
				}
			}

			// Exact match not found, get span (r and l reversed)
			Keyframe lkey = this->Keys[r];
			Keyframe rkey = this->Keys[l];

			float u = (time - lkey.GetTime()) / (rkey.GetTime() - lkey.GetTime());
			float a = lkey.GetA();
			float b = lkey.GetB();
			float c = lkey.getC();
			float d = lkey.getD();

			float x = d + u*(c + u*(b + u*(a)));
			return x;
		}
	}

	// Setters /////////////////////////////////////////////////////////////////
	void SetExtrapIn(std::string extrapIn)   { ExtrapIn = extrapIn; }
	void SetExtrapOut(std::string extrapOut) { ExtrapOut = extrapOut; }

	// Getters /////////////////////////////////////////////////////////////////
	const std::vector<Keyframe>& GetKeys() { return Keys; }
	std::string GetExtrapIn()              { return ExtrapIn; }
	std::string GetExtrapOut()             { return ExtrapOut; }

private:
	std::vector<Keyframe> Keys;
	std::string ExtrapIn, ExtrapOut;
	float StartTime, EndTime;
	float StartValue, EndValue;
};

////////////////////////////////////////////////////////////////////////////////