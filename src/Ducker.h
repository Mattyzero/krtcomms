#pragma once
#include "ts3_functions.h"
#include <QtCore/QMap>
#include <QtCore/QList>

constexpr auto TYPE_COUNT = 4;

class Ducker {
public:
	static Ducker& getInstance();

	enum Type {
		NONE = -1,
		CHANNEL,
		FREQ1XX,
		FREQX1X,
		FREQXX1
	};

	Ducker();
	~Ducker() = default;

	void Init(const struct TS3Functions funcs, char* pluginID);

	void SetEnabled(int type, bool enabled);
	bool IsEnabled();
	bool IsEnabled(int type);

	void SetGain(int type, float value);

	void OnTalkStatusChangeEvent(uint64 serverConnectionHandlerID, int status, int isReceivedWhisper, anyID clientID, int clientFrequence);
	void OnEditPlaybackVoiceDataEvent(uint64 serverConnectionHandlerID, anyID clientID, short* samples, int sampleCount, int channels, int type);
	void OnClientMoveEvent(uint64 serverConnectionHandlerID, anyID clientID, uint64 oldChannelID, uint64 newChannelID, int visibility, const char* moveMessage);

private:
	char* _pluginID;
	struct TS3Functions _ts3;
	

	QMap<uint64, QList<anyID>> _currentChannelClientIDs;

	bool _enabled[TYPE_COUNT];
	float _ducking[TYPE_COUNT];
};

