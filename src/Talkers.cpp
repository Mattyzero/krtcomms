#include "Talkers.h"
#include "Ducker.h"
#include "KRTComms.h"

#include <math.h>

Talkers::Talkers() {

}

Talkers::~Talkers() {
	foreach(uint64 serverConnectionHandlerID, _talkers.keys()) {
		foreach(QString key, _talkers[serverConnectionHandlerID].keys()) {
			delete _talkers[serverConnectionHandlerID][key];
			_talkers[serverConnectionHandlerID][key] = NULL;
		}
	}
}

Talkers& Talkers::getInstance() {
	static Talkers instance;
	return instance;
}

void Talkers::Add(uint64 serverConnectionHandlerID, anyID clientID, int isReceivedWhisper, int frequence) {
	struct ClientInfo* clientInfo = NULL;
	QString key = QString::number(frequence) + "_" + QString::number(clientID);
	if (!_talkers[serverConnectionHandlerID].contains(key)) {
		clientInfo = new ClientInfo;
		clientInfo->clientID = clientID;
		clientInfo->isWhispering = isReceivedWhisper;
		clientInfo->frequence = frequence;

		_talkers[serverConnectionHandlerID][key] = clientInfo;
	}
	else {
		clientInfo = _talkers[serverConnectionHandlerID][key];
		clientInfo->isWhispering = isReceivedWhisper;
		clientInfo->frequence = frequence;
	}
}

void Talkers::Remove(uint64 serverConnectionHandlerID, anyID clientID, int isReceivedWhisper, int frequence) {
	QString key = QString::number(frequence) + "_" + QString::number(clientID);
	if (_talkers[serverConnectionHandlerID].contains(key)) {
		delete _talkers[serverConnectionHandlerID][key];
		_talkers[serverConnectionHandlerID][key] = NULL;
		_talkers[serverConnectionHandlerID].remove(key);
	}
}

void Talkers::Clear(uint64 serverConnectionHandlerID, int frequence) {
	foreach(QString key, _talkers[serverConnectionHandlerID].keys()) {
		if (key.startsWith(QString::number(frequence))) {
			delete _talkers[serverConnectionHandlerID][key];
			_talkers[serverConnectionHandlerID][key] = NULL;
			_talkers[serverConnectionHandlerID].remove(key);
		}
	}
}

void Talkers::SetFrequenceIfNotSet(uint64 serverConnectionHandlerID, anyID clientID, int frequence) {
	QString key = QString::number(frequence) + "_" + QString::number(clientID);
	if (_talkers[serverConnectionHandlerID].contains(key)) {
		struct ClientInfo* clientInfo = _talkers[serverConnectionHandlerID][key];
		if (clientInfo->frequence == -1) {
			clientInfo->frequence = frequence;
		}
	}
}

bool Talkers::IsWhispering(uint64 serverConnectionHandlerID, anyID clientID) {
	QString clientIDString = QString::number(clientID);
	foreach(QString key, _talkers[serverConnectionHandlerID].keys()) {
		if (key.endsWith(clientIDString)) {
			return _talkers[serverConnectionHandlerID][key]->isWhispering;
		}
	}
	return false;
}

bool Talkers::IsWhispering(uint64 serverConnectionHandlerID, anyID clientID, int frequence) {
	QString key = QString::number(frequence) + "_" + QString::number(clientID);
	if (_talkers[serverConnectionHandlerID].contains(key)) {
		return _talkers[serverConnectionHandlerID][key]->isWhispering;
	}
	return false;
}



bool Talkers::IsAnyWhispering(uint64 serverConnectionHandlerID) {
	foreach(ClientInfo* info, _talkers[serverConnectionHandlerID].values()) {
		if (info->isWhispering) {
			return true;
		}
	}
	return false;
}

bool Talkers::IsAnyWhisperingInFrequence(uint64 serverConnectionHandlerID, int frequence) {
	foreach(ClientInfo* info, _talkers[serverConnectionHandlerID].values()) {
		if (info->isWhispering && info->frequence == frequence) {
			return true;
		}
	}
	return false;
}

Ducker::Type Talkers::PrioritizedFrequence(uint64 serverConnectionHandlerID, anyID clientID) {
	QString clientIDString = QString::number(clientID);
	Ducker::Type duckerType = Ducker::Type::NONE;
	foreach(QString key, _talkers[serverConnectionHandlerID].keys()) {
		if (key.endsWith(clientIDString)) {			
			ClientInfo* current = _talkers[serverConnectionHandlerID][key];
			foreach(ClientInfo* info, _talkers[serverConnectionHandlerID].values()) {
				if (info->isWhispering) {
					/*
					if (info->frequence % 99999 == 0 && current->frequence % 99999 != 0) {
						return Ducker::Type::BROADCAST;
					}*/
					if (info->frequence % 10000 == 0 && current->frequence % 10000 != 0) {
						return Ducker::Type::FREQ_1YZ_AB;
					}
					/*
					if (info->frequence % 1000 == 0 && current->frequence % 1000 != 0) {
						if(duckerType > Ducker::Type::FREQ_X1Z_AB)
							duckerType = Ducker::Type::FREQ_X1Z_AB;
					}*/
					if (info->frequence % 100 == 0 && current->frequence % 100 != 0) {
						if (duckerType == Ducker::Type::NONE || duckerType > Ducker::Type::FREQ_XY1_AB)
							duckerType = Ducker::Type::FREQ_XY1_AB;
					}
					if (info->frequence % 10 == 0 && current->frequence % 10 != 0) {
						if (duckerType == Ducker::Type::NONE || duckerType > Ducker::Type::FREQ_XYZ_1B)
							duckerType = Ducker::Type::FREQ_XYZ_1B;
					}
				}
			}
		}
	}

	return duckerType;
}

