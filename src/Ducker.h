#pragma once
#include "ts3_functions.h"
#include <QtCore/QMap>
#include <QtCore/QList>

class Ducker {
public:
	static Ducker& getInstance();

	Ducker();
	~Ducker() = default;

	void Init(const struct TS3Functions funcs, char* pluginID);

	void SetEnabled(bool enabled);
	bool IsEnabled();

	void OnTalkStatusChangeEvent(uint64 serverConnectionHandlerID, int status, int isReceivedWhisper, anyID clientID, int clientFrequence);
	void OnEditPlaybackVoiceDataEvent(uint64 serverConnectionHandlerID, anyID clientID, short* samples, int sampleCount, int channels);
	void OnClientMoveEvent(uint64 serverConnectionHandlerID, anyID clientID, uint64 oldChannelID, uint64 newChannelID, int visibility, const char* moveMessage);

private:
	char* _pluginID;
	struct TS3Functions _ts3;
	

	QMap<uint64, QList<anyID>> _currentChannelClientIDs;

	bool _enabled = false;
	float _ducking = 0.2f;
};

