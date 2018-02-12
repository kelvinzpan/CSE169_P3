////////////////////////////////////////
// Animation.hpp
////////////////////////////////////////

#pragma once

#include "Tokenizer.h"
#include "Channel.hpp"

////////////////////////////////////////////////////////////////////////////////

// The Animation class plays an animation by playing all Channels

class Animation {
public:
	Animation(const char *file)
	{
		Load(file);
	};

	////////////////////////////////////////////////////////////////////////////

	bool Load(const char *file)
	{
		Tokenizer token;
		token.Open(file);
		token.FindToken("animation");
		token.FindToken("{");

		// Get info
		token.FindToken("range");
		this->TimeStart = token.GetFloat();
		this->TimeEnd = token.GetFloat();
		token.FindToken("numchannels");
		float numChannels = token.GetInt();
		std::cout << "Animation with starting at " << this->TimeStart <<
			" and ending at " << this->TimeEnd << " with " << numChannels << " channels." << std::endl;

		// Parse channels
		for (int i = 0; i < numChannels; i++)
		{
			token.FindToken("channel");
			token.FindToken("{");

			Channel newChannel = Channel();
			newChannel.Load(token);
			this->Channels.push_back(newChannel);
		}

		// Finish
		token.Close();
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
	std::vector<Channel> Channels;
	float TimeStart, TimeEnd;
};

////////////////////////////////////////////////////////////////////////////////