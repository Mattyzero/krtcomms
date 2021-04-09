#pragma once
#include "teamspeak/public_rare_definitions.h"
#include "teamspeak/public_errors.h"
#include <QtCore/QMap>
#include <QtCore/QList>
#include "Ducker.h"
#include <cmath>

class Talkers
{
public:
	static Talkers& getInstance();

	struct ClientInfo {
		anyID clientID;
		bool isWhispering;
		int frequence;
	};

	Talkers();
	~Talkers();


	void Add(uint64 serverConnectionHandlerID, anyID clientID, int isReceivedWhisper, int frequence);
	void Remove(uint64 serverConnectionHandlerID, anyID clientID, int isReceivedWhisper, int frequence);
	void Clear(uint64 serverConnectionHandlerID, int frequence);

	bool IsWhispering(uint64 serverConnectionHandlerID, anyID clientID);
	bool IsWhispering(uint64 serverConnectionHandlerID, anyID clientID, int frequence);
	bool IsAnyWhispering(uint64 serverConnectionHandlerID);
	bool IsAnyWhisperingInFrequence(uint64 serverConnectionHandlerID, int frequence);
	bool IsAnyWhisperingAndNotMuted(uint64 serverConnectionHandlerID);
	bool IsBroadcasting(uint64 serverConnectionHandlerID, anyID clientID, int frequence);
	QList<int> GetFrequences(uint64 serverConnectionHandlerID, anyID clientID);
	Ducker::Type PrioritizedFrequence(uint64 serverConnectionHandlerID, anyID clientID);

	int toInt(double frequence);
private:
	QMap<uint64, QMap<QString, ClientInfo*>> _talkers;
};

