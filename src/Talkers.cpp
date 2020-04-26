#include "Talkers.h"

#include <math.h>

Talkers::Talkers() {

}

Talkers::~Talkers() {
	foreach(uint64 serverConnectionHandlerID, _talkers.keys()) {
		foreach(anyID clientID, _talkers[serverConnectionHandlerID].keys()) {
			delete _talkers[serverConnectionHandlerID][clientID];
			_talkers[serverConnectionHandlerID][clientID] = NULL;
		}
	}
}

Talkers& Talkers::getInstance() {
	static Talkers instance;
	return instance;
}

void Talkers::Add(uint64 serverConnectionHandlerID, anyID clientID, int isReceivedWhisper, int frequence) {
	struct ClientInfo* clientInfo = NULL;
	if (!_talkers[serverConnectionHandlerID].contains(clientID)) {
		clientInfo = new ClientInfo;
		clientInfo->clientID = clientID;
		clientInfo->isWhispering = isReceivedWhisper;
		clientInfo->frequence = frequence;

		_talkers[serverConnectionHandlerID][clientID] = clientInfo;
	}
	else {
		clientInfo = _talkers[serverConnectionHandlerID][clientID];
		clientInfo->isWhispering = isReceivedWhisper;
		clientInfo->frequence = frequence;
	}
}

void Talkers::Remove(uint64 serverConnectionHandlerID, anyID clientID, int isReceivedWhisper) {
	if (_talkers[serverConnectionHandlerID].contains(clientID)) {
		delete _talkers[serverConnectionHandlerID][clientID];
		_talkers[serverConnectionHandlerID][clientID] = NULL;
		_talkers[serverConnectionHandlerID].remove(clientID);
	}
}

bool Talkers::IsWhispering(uint64 serverConnectionHandlerID, anyID clientID) {
	if (_talkers[serverConnectionHandlerID].contains(clientID)) {
		return _talkers[serverConnectionHandlerID][clientID]->isWhispering;
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

bool Talkers::PrioritizedFrequence(uint64 serverConnectionHandlerID, anyID clientID) {
	ClientInfo* current = _talkers[serverConnectionHandlerID][clientID];
		
	foreach(ClientInfo* info, _talkers[serverConnectionHandlerID].values()) {		
		if (info->isWhispering) {
			if (info->frequence % 10000 == 0 && current->frequence % 10000 != 0) {
				return true;
			}
			if (info->frequence % 1000 == 0 && current->frequence % 1000 != 0) {
				return true;
			}
			if (info->frequence % 100 == 0 && current->frequence % 100 != 0) {
				return true;
			}
		}
	}

	return false;
}

