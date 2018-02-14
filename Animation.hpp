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
		this->StartTime = token.GetFloat();
		this->EndTime = token.GetFloat();
		token.FindToken("numchannels");
		int numChannels = token.GetInt();
		std::cout << "Animation with starting at " << this->StartTime <<
			" and ending at " << this->EndTime << " with " << numChannels << " channels." << std::endl;

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

	void Evaluate(float time, std::vector<float>& pose)
	{
		for (unsigned int i = 0; i < pose.size(); i++)
		{
			pose[i] = this->Channels[i].Evaluate(time);
		}
	}

	// Setters /////////////////////////////////////////////////////////////////
	void SetStartTime(float time) { StartTime = time; }
	void SetEndTime(float time) { EndTime = time; }

	// Getters /////////////////////////////////////////////////////////////////
	const std::vector<Channel>& GetChannels() { return Channels; }
	float GetStartTime() { return StartTime; }
	float GetEndTime() { return EndTime; }

private:
	std::vector<Channel> Channels;
	float StartTime, EndTime;
};

////////////////////////////////////////////////////////////////////////////////