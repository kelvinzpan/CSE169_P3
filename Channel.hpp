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
			newKey.SetTangentIn(tanInVal);
			newKey.SetTangentOut(tanOutVal);
			this->Keys.push_back(newKey);

			//std::cout << "Keyframe with time " << time << " and value " << value <<
			//	" and ruleIn " << tanIn << " and ruleOut " << tanOut << 
			//	" and tanIn " << tanInVal << " and tanOut " << tanOutVal << std::endl;
		}

		// Finish
		return true;
	}

	////////////////////////////////////////////////////////////////////////////

	float Evaluate(float time)
	{


		return 0.0f;
	}

	// Setters /////////////////////////////////////////////////////////////////

	// Getters /////////////////////////////////////////////////////////////////

private:
	std::vector<Keyframe> Keys;
	std::string ExtrapIn, ExtrapOut;
};

////////////////////////////////////////////////////////////////////////////////