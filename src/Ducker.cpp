#include "Ducker.h"
#include "teamspeak/public_rare_definitions.h"
#include "teamspeak/public_errors.h"
#include "ts3_functions.h"
#include "KRTComms.h"

#include <array>

#define MAX_CHANNELS 8

Ducker::Ducker() {
	for (int i = 0; i < TYPE_COUNT; i++) {
		_enabled[i] = false;
		_ducking[i] = 1.0f;
	}
}

Ducker& Ducker::getInstance() {
	static Ducker instance;
	return instance;
}

void Ducker::Init(const struct TS3Functions funcs, char* pluginID) {
	_ts3 = funcs;
	_pluginID = pluginID;
	//_ts3.getClientID(_serverConnectionHandlerID, &_me);
}

void Ducker::SetEnabled(int type, bool enabled) {
	_enabled[type] = enabled;
}

bool Ducker::IsEnabled() {
	for (int type = 0; type < TYPE_COUNT; type++) {
		if (_enabled[type]) return true;
	}
	return false;
}

bool Ducker::IsEnabled(int type) {
	return _enabled[type];
}

void Ducker::SetGain(int type, float value) {
	_ducking[type] = value;
}

void Ducker::OnTalkStatusChangeEvent(uint64 serverConnectionHandlerID, int status, int isReceivedWhisper, anyID clientID, int clientFrequence) {

	anyID me;
	_ts3.getClientID(serverConnectionHandlerID, &me);
	if (me == clientID) {
		return;
	}
}

void Ducker::OnEditPlaybackVoiceDataEvent(uint64 serverConnectionHandlerID, anyID clientID, short* samples, int sampleCount, int channels, int type) {

	if (!IsEnabled(type)) return;

	static thread_local size_t allocatedFloatsSample = 0;
	static thread_local std::array<std::vector<float>, MAX_CHANNELS> floatsSample;
	if (allocatedFloatsSample != floatsSample[0].size())
		allocatedFloatsSample = 0; //It happened that allocatedFloatsSample==960 and floatsSample[0] was of size 0...


	if (allocatedFloatsSample < (size_t)sampleCount)  //Not enough buffer, create new one
	{
		for (int j = 0; j < MAX_CHANNELS; j++) {
			floatsSample[j].resize(sampleCount);
		}
		allocatedFloatsSample = sampleCount;
	}

	//Split channels into separate arrays
	for (int i = 0; i < sampleCount * channels; i += channels) {
		for (int j = 0; j < channels; j++) {
			floatsSample[j][i / channels] = static_cast<float>(samples[i + j]) / static_cast<float>(SHRT_MAX);
		}
	}

	for (int i = 0; i < sampleCount * channels; i += channels) {
		for (auto j = 0; j < channels; j++) {
			auto sample = floatsSample[j][i / channels] * _ducking[type];			
			short newValue;
			if (sample > 1.0) newValue = SHRT_MAX;
			else if (sample < -1.0) newValue = SHRT_MIN;
			else newValue = static_cast<short>(sample * (SHRT_MAX - 1));
			samples[i + j] = newValue;
		}
	}

}

void Ducker::OnClientMoveEvent(uint64 serverConnectionHandlerID, anyID clientID, uint64 oldChannelID, uint64 newChannelID, int visibility, const char* moveMessage) {

	unsigned int error;

	anyID me;
	_ts3.getClientID(serverConnectionHandlerID, &me);
	if (me == clientID) {

		_currentChannelClientIDs[serverConnectionHandlerID].clear();

		anyID* clients;
		if ((error = _ts3.getChannelClientList(serverConnectionHandlerID, newChannelID, &clients)) == ERROR_ok) {
			for (int i = 0; i < clients[i]; i++) {
				if (clients[i] == me)
					continue;

				_currentChannelClientIDs[serverConnectionHandlerID].push_back(clients[i]);
			}
		}
	}


}