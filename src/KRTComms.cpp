#include "KRTComms.h"
#include "teamspeak/public_rare_definitions.h"
#include "teamspeak/public_errors.h"
#include "ts3_functions.h"
#include "Helpers.h"
#include "Talkers.h"
#include "Ducker.h"
#include "Encrypter.h"
#include "channels.h"

#include <chrono>
#include <thread>
#include <array>
#include <QtCore/QString>
#include <QtWidgets/QDialog>
#include <QtCore/QFileInfo>
#include <math.h>

#ifndef M_PI
#define M_PI    3.14159265358979323846f
#endif

#define MAX_CHANNELS 8


char* KRTComms::version = "0.1.2";

KRTComms::KRTComms() {
	for (int i = 0; i < RADIO_COUNT; i++) {
		_pans[i] = 0.0f;
		_gains[i] = 1.0f;
		_isWhispering[i] = false;
		_doubleClickCount[i] = 0;
		_muted[i] = false;
		_toggleMuted[i] = false;
		_doubleClickTimer[i].setSingleShot(true);
		_doubleClickConnection[i] = NULL;
	}

	_allMuted = false;
	_channelMuted = false;
}

KRTComms& KRTComms::getInstance() {
	static KRTComms instance;
	return instance;
}

void KRTComms::Log(QString message) {
	if(KRTComms::getInstance()._debug)
		KRTComms::getInstance()._ts3.printMessageToCurrentTab(message.toStdString().c_str());
}

void KRTComms::Init(const struct TS3Functions funcs, char* pluginID, channels* channels) {
	_ts3 = funcs;
	_pluginID = pluginID;
	_channels = channels;
	//_ts3.getClientID(_serverConnectionHandlerID, &_me);

	char soundsPath[512];
	_ts3.getPluginPath(soundsPath, 512, pluginID);

	for (int radio_id = 0; radio_id < RADIO_COUNT; radio_id++) {
		QString startPath = QString(soundsPath) + "\\krt_comms\\start" + QString::number(radio_id+1) + ".wav";
		if (QFileInfo(startPath).exists()) {
			_soundsPath[radio_id][0] = startPath;
		}
		else {
			_soundsPath[radio_id][0] = QString(soundsPath) + "\\krt_comms\\start.wav";
		}

		QString endPath = QString(soundsPath) + "\\krt_comms\\end" + QString::number(radio_id+1) + ".wav";
		if (QFileInfo(endPath).exists()) {
			_soundsPath[radio_id][1] = endPath;
		}
		else {
			_soundsPath[radio_id][1] = QString(soundsPath) + "\\krt_comms\\end.wav";
		}
	}
}

void KRTComms::SetDebug(bool debug) {
	_debug = debug;
}

void KRTComms::InfoData(uint64 serverConnectionHandlerID, uint64 id, enum PluginItemType type, char** data) {
	if (!_debug) return;

	QString message;

	switch (type) {	
	case PLUGIN_CLIENT:
		message = "ClientID: " + QString::number(id);
		*data = (char*)malloc(128 * sizeof(char));  /* Must be allocated in the plugin! */
		snprintf(*data, 128, "%s", message.toStdString().c_str());  /* bbCode is supported. HTML is not supported */
		break;
	default:
		data = NULL;
	}
}

void KRTComms::SendPluginCommand(uint64 serverConnectionHandlerID, const char* pluginID, QString command, int targetMode, const anyID* targetIDs, const char* returnCode) {
	anyID me;
	_ts3.getClientID(serverConnectionHandlerID, &me);

	command = QString::number(me) + "\t" + command;
	_ts3.sendPluginCommand(serverConnectionHandlerID, _pluginID, command.toStdString().c_str(), targetMode, targetIDs, returnCode);
}

void KRTComms::ProcessPluginCommand(uint64 serverConnectionHandlerID, const char* pluginCommand, anyID invokerClientID, const char* invokerName) {

	QString command = QString(pluginCommand);	
	QStringList tokens = command.split('\t');

	if(invokerClientID == 0)
		invokerClientID = tokens[0].toUInt();

	if (_debug) {
		char displayName[256];
		int error;
		if ((error = _ts3.getClientDisplayName(serverConnectionHandlerID, invokerClientID, displayName, 256)) != ERROR_ok) {
			
		}

		QString logmessage = "ProcessPluginCommand: " + QString(pluginCommand) + " | " + QString::number(invokerClientID) + " : " + QString(displayName);
		_ts3.logMessage(logmessage.toStdString().c_str(), LogLevel_DEBUG, "KRTC ProcessPluginCommand", serverConnectionHandlerID);
		_ts3.printMessageToCurrentTab(logmessage.toStdString().c_str());
	}
	//Hier Commands überprüfen die auch an sich selbst gehen

	anyID me;
	_ts3.getClientID(serverConnectionHandlerID, &me);
	if (me == invokerClientID) {
		return;
	}

	if (tokens[1] == "SENDON") {
		bool ok;
		int frequence = Encrypter::Decrypt(serverConnectionHandlerID, tokens[2]).toInt(&ok, 10);
		if (ok && ActiveInFrequence(serverConnectionHandlerID, frequence)) {
			int radio_id = GetRadioId(serverConnectionHandlerID, frequence);
			_channels->EnableReceiveLamp(radio_id);
			Talkers::getInstance().Add(serverConnectionHandlerID, invokerClientID, true, frequence);

			if(_soundsEnabled)
				_ts3.playWaveFile(serverConnectionHandlerID, _soundsPath[radio_id][0].toStdString().c_str());
		}
		return;
	}

	if (tokens[1] == "SENDOFF" || tokens[1] == "LEFT") {
		bool ok;
		int frequence = Encrypter::Decrypt(serverConnectionHandlerID, tokens[2]).toInt(&ok, 10);
		if (ok && ActiveInFrequence(serverConnectionHandlerID, frequence)) {			
			Talkers::getInstance().Remove(serverConnectionHandlerID, invokerClientID, false, frequence);
			if (!Talkers::getInstance().IsAnyWhisperingInFrequence(serverConnectionHandlerID, frequence)) {
				int radio_id = GetRadioId(serverConnectionHandlerID, frequence);
				_channels->DisableReceiveLamp(radio_id);

				if(_soundsEnabled && tokens[1] == "SENDOFF")
					_ts3.playWaveFile(serverConnectionHandlerID, _soundsPath[radio_id][1].toStdString().c_str());
			}
		}
		if(tokens[1] != "LEFT") return;
	}

	if (tokens[1] == "JOINED") {
		bool ok;
		int frequence = Encrypter::Decrypt(serverConnectionHandlerID, tokens[2]).toInt(&ok, 10);
		if (ok && ActiveInFrequence(serverConnectionHandlerID, frequence)) {
			if (AddToFrequence(serverConnectionHandlerID, frequence, invokerClientID, invokerName, true)) { //AddToFrequence returns false wenn selbst
				AnswerTheCall(serverConnectionHandlerID, frequence, invokerClientID);
			}
		}
		return;
	}

	if (tokens[1] == "LEFT") {
		bool ok;
		int frequence = Encrypter::Decrypt(serverConnectionHandlerID, tokens[2]).toInt(&ok, 10);
		if (ok) {			
			RemoveFromFrequence(serverConnectionHandlerID, frequence, invokerClientID);
		}	
		return;
	}

	if (tokens[1] == "METOO") {
		bool ok;
		int frequence = Encrypter::Decrypt(serverConnectionHandlerID, tokens[2]).toInt(&ok, 10);
		if (ok && ActiveInFrequence(serverConnectionHandlerID, frequence)) {
			AddToFrequence(serverConnectionHandlerID, frequence, invokerClientID, invokerName, false); //Bei METOO braucht er keinen UpdateWhisperTo
		}
		return;
	}
}

/*
void KRTComms::WhisperDirect(std::vector<uint64> targetChannelIDArray, std::vector<anyID> targetClientIDArray) {
	KRTComms::getInstance().WhisperTo(targetChannelIDArray, targetClientIDArray);
}*/

/*
char* KRTComms::GetWhisperList() {
	return KRTComms::getInstance().WhisperList();
}*/

void KRTComms::SetActiveRadio(uint64 serverConnectionHandlerID, int radio_id, bool state, int frequence) {
	
	try {
		
		int old_frequence = GetFrequence(serverConnectionHandlerID, radio_id);
		bool isActive = _activeRadios[serverConnectionHandlerID].contains(radio_id);

		QString logmessage = "SetActiveRadio : Radio " + QString::number(radio_id+1) + " : " + (state ? "true" : "false") + " : Old F.:" + QString::number(old_frequence) + " : New F.:" + QString::number(frequence);

		if (state) {
			if (isActive && old_frequence != frequence && _activeRadios[serverConnectionHandlerID].values().count(old_frequence) <= 1) {
				QString command = "LEFT\t" + Encrypter::Encrypt(radio_id, QString::number(old_frequence));
				SendPluginCommand(serverConnectionHandlerID, _pluginID, command, PluginCommandTarget_SERVER, NULL, NULL); //TODO Überlegung ob LEFT nur an die Clients in Frequenz gesendet werden soll?
				logmessage += " LEFT";

				RemoveAllFromFrequence(serverConnectionHandlerID, old_frequence);
				_isWhispering[radio_id] = false;
				Talkers::getInstance().Clear(serverConnectionHandlerID, old_frequence);
			}

			if (!isActive || old_frequence != frequence) {
				_activeRadios[serverConnectionHandlerID][radio_id] = frequence;
				QString command = "JOINED\t" + Encrypter::Encrypt(radio_id, QString::number(frequence));
				SendPluginCommand(serverConnectionHandlerID, _pluginID, command, PluginCommandTarget_SERVER, NULL, NULL);
				logmessage += " JOINED";
			}
		}
		else {
			if (isActive) {
				if (_activeRadios[serverConnectionHandlerID].values().count(old_frequence) <= 1) {
					
					QString command = "LEFT\t" + Encrypter::Encrypt(radio_id, QString::number(old_frequence));
					SendPluginCommand(serverConnectionHandlerID, _pluginID, command, PluginCommandTarget_SERVER, NULL, NULL);
					logmessage += " LEFT";

					RemoveAllFromFrequence(serverConnectionHandlerID, old_frequence);
					_isWhispering[radio_id] = false;
					Talkers::getInstance().Clear(serverConnectionHandlerID, old_frequence);
				}

				_activeRadios[serverConnectionHandlerID].remove(radio_id);
			}
		}

		if (_debug) {
			_ts3.logMessage(logmessage.toStdString().c_str(), LogLevel_DEBUG, "KRTC SetActiveRadio", serverConnectionHandlerID);
			_ts3.printMessageToCurrentTab(logmessage.toStdString().c_str());
		}
	}
	catch (const std::exception& e) {
		_ts3.logMessage(e.what(), LogLevel_ERROR, "KRTC SetActiveRadio", serverConnectionHandlerID);
	}
	catch (...) {		
		_ts3.logMessage("Unkown exception", LogLevel_ERROR, "KRTC SetActiveRadio", serverConnectionHandlerID);
	}
}

bool KRTComms::ActiveInRadio(uint64 serverConnectionHandlerID, int radio_id) {
	bool active = _activeRadios[serverConnectionHandlerID].contains(radio_id);
	/*
	if (_debug) {
		QString logmessage = "ActiveInRadio: " + QString::number(serverConnectionHandlerID) + " : " + QString::number(radio_id) + " : " + (active ? "true" : "false");
		_ts3.printMessageToCurrentTab(logmessage.toStdString().c_str());
	}*/
	return active;
}

bool KRTComms::ActiveInFrequence(uint64 serverConnectionHandlerID, int frequence) {
	bool isActive = _activeRadios[serverConnectionHandlerID].values().contains(frequence);
	if (isActive) return true;

	foreach(int freq, _activeRadios[serverConnectionHandlerID].values()) {
		if (freq == 99999 || freq % 100 == 99 && freq / 100 == frequence / 100) {
			//_ts3.printMessageToCurrentTab("Broadcast");
			return true;
		}

		if (frequence == 99999 || frequence % 100 == 99 && freq / 100 == frequence / 100) {
			//_ts3.printMessageToCurrentTab("Broadcast2");
			return true;
		}
	}
	

	return isActive;
}

bool KRTComms::AddToFrequence(uint64 serverConnectionHandlerID, int frequence, anyID clientID, const char* nickname, bool shouldUpdate) {

	//Falls Broadcast auch Empfangen soll dann das hier löschen
	bool broadcast = false;
	foreach(int freq, _activeRadios[serverConnectionHandlerID].values()) {
		if (freq == 99999 || freq % 100 == 99 && freq / 100 == frequence / 100) {
			frequence = freq;
			broadcast = true;
			//_ts3.printMessageToCurrentTab("Broadcast3");
			break;
		}
		if (frequence == 99999 || frequence % 100 == 99 && freq / 100 == frequence / 100) {
			//_ts3.printMessageToCurrentTab("Broadcast4");
			return true;
		}
	}
	//---

	if (_targetClientIDs[serverConnectionHandlerID][frequence].contains(clientID)) {
		if (_debug) {
			_ts3.printMessageToCurrentTab("Already In");
		}
		return false;
	}

	_targetClientIDs[serverConnectionHandlerID][frequence].push_back(clientID);
	if (_debug) {
		_ts3.printMessageToCurrentTab("Added");
	}
	
	if (!_nicknames[serverConnectionHandlerID].contains(clientID)) {
		_nicknames[serverConnectionHandlerID][clientID] = nickname;
	}

	if(shouldUpdate) UpdateWhisperTo(serverConnectionHandlerID, frequence);

	if (broadcast) return false;

	return true;
}

bool KRTComms::RemoveFromFrequence(uint64 serverConnectionHandlerID, int frequence, anyID clientID) {
	//_ts3.printMessageToCurrentTab("REMOVED");

	foreach(int freq, _activeRadios[serverConnectionHandlerID].values()) {
		if (freq == 99999 || freq % 100 == 99 && freq / 100 == frequence / 100) {
			frequence = freq;
			//_ts3.printMessageToCurrentTab("Broadcast5");
			break;
		}
		if (frequence == 99999 || frequence % 100 == 99 && freq / 100 == frequence / 100) {
			//_ts3.printMessageToCurrentTab("Broadcast6");
			return true;
		}
	}

	bool shouldUpdate = _targetClientIDs[serverConnectionHandlerID][frequence].removeOne(clientID);

	//QString logmessage = QString::number(serverConnectionHandlerID)  + "   |   " + QString::number(frequence) + "   |   " + QString::number(clientID) + "   |   _targetClientIDs Count: " + QString::number(_targetClientIDs[serverConnectionHandlerID][frequence].size());
	//_ts3.printMessageToCurrentTab(logmessage.toStdString().c_str());
	
	//UpdateWhisperTo nur wenn wirklich jemand von der Liste entfernt wurde
	if(shouldUpdate) UpdateWhisperTo(serverConnectionHandlerID, frequence);

	return shouldUpdate;
}

void KRTComms::RemoveAllFromFrequence(uint64 serverConnectionHandlerID, int frequence) {
	//_ts3.printMessageToCurrentTab("REMOVED ALL");
	_targetClientIDs[serverConnectionHandlerID][frequence].clear();
}

bool KRTComms::AnswerTheCall(uint64 serverConnectionHandlerID, int frequence, anyID clientID) {

	QList<anyID> tmp;
	tmp.append(clientID);
	tmp.append(0);

	QString command = "METOO\t" + Encrypter::Encrypt(GetRadioId(serverConnectionHandlerID, frequence), QString::number(frequence));
	SendPluginCommand(serverConnectionHandlerID, _pluginID, command, PluginCommandTarget_CLIENT, tmp.toVector().constData(), NULL);
//	if (_debug) {
//		_ts3.printMessageToCurrentTab(("AnswerTheCall: " + QString::number(clientID)).toStdString().c_str());
//	}
	return true;
}

void KRTComms::WhisperToRadio(uint64 serverConnectionHandlerID, int radio_id) {

	bool isActive = _activeRadios[serverConnectionHandlerID].contains(radio_id);
	if (!isActive) return;

	int frequence = GetFrequence(serverConnectionHandlerID, radio_id);

	QList<anyID> tmp = QList(_targetClientIDs[serverConnectionHandlerID][frequence]);
	tmp.append(0);

	if (_isWhispering[radio_id]) {
		_channels->DisableSendLamp(radio_id);
		if (tmp.size() > 1) {
			QString command = "SENDOFF\t" + Encrypter::Encrypt(radio_id, QString::number(frequence));
			SendPluginCommand(serverConnectionHandlerID, _pluginID, command, PluginCommandTarget_CLIENT, tmp.toVector().constData(), NULL);
		}
	}
	else {
		_channels->EnableSendLamp(radio_id);
		if (tmp.size() > 1) {
			QString command = "SENDON\t" + Encrypter::Encrypt(radio_id, QString::number(frequence));
			SendPluginCommand(serverConnectionHandlerID, _pluginID, command, PluginCommandTarget_CLIENT, tmp.toVector().constData(), NULL);
		}
	}

	std::vector<anyID> empty;
	_isWhispering[radio_id] = !_isWhispering[radio_id];

	
	
	if (_debug) {
		QString logmessage = "WhisperToRadio: " + QString::number(radio_id) + " | " + QString::number(frequence);
		_ts3.logMessage(logmessage.toStdString().c_str(), LogLevel_DEBUG, "KRTC WhisperToRadio", serverConnectionHandlerID);
		//_ts3.printMessageToCurrentTab(logmessage.toStdString().c_str());
	}

	UpdateWhisperTo(serverConnectionHandlerID, frequence);
	/*
	QList<uint64> targetChannelIDs = _targetChannelIDs[serverConnectionHandlerID][frequence];
	QList<anyID> targetClientIDs;

	for (int i = 0; i < _isWhispering.size(); i++) {
		if (_isWhispering[i]) {
			int freq = GetFrequence(serverConnectionHandlerID, i);
			targetClientIDs.append(_targetClientIDs[serverConnectionHandlerID][freq]);			
		}
	}

	WhisperTo(serverConnectionHandlerID, targetChannelIDs, targetClientIDs);*/
}

void KRTComms::UpdateWhisperTo(uint64 serverConnectionHandlerID, int frequence) {
	QList<uint64> targetChannelIDs = _targetChannelIDs[serverConnectionHandlerID][frequence];
	QList<anyID> targetClientIDs;

	for (int i = 0; i < _isWhispering.size(); i++) {
		if (_isWhispering[i]) {
			int freq = GetFrequence(serverConnectionHandlerID, i);
			targetClientIDs.append(_targetClientIDs[serverConnectionHandlerID][freq]);
		}
	}

	WhisperTo(serverConnectionHandlerID, targetChannelIDs, targetClientIDs);
}

void KRTComms::WhisperTo(uint64 serverConnectionHandlerID, QList<uint64> targetChannelIDArray, QList<anyID> targetClientIDArray) {
	char* returnCode = new char[256];

	//int* result = NULL;

	//_ts3.isWhispering(serverConnectionHandlerID, NULL, result);

	//Liste / Array muss mit einer 0 enden (0 terminiert)
	targetChannelIDArray.append(0);
	targetClientIDArray.append(0);

	if (targetChannelIDArray.size() <= 1 && targetClientIDArray.size() <= 1) {
		_ts3.requestClientSetWhisperList(serverConnectionHandlerID, NULL, NULL, NULL, returnCode);
		SetPushToTalk(serverConnectionHandlerID, false);
	}
	else {
		SetPushToTalk(serverConnectionHandlerID, true);
		_ts3.requestClientSetWhisperList(serverConnectionHandlerID, NULL, NULL, targetClientIDArray.toVector().constData(), returnCode);
	}

	if (_debug) {
		char message[256];
		sprintf_s(message, 256, "Whisper Clients: %d | isWispering: %s", targetClientIDArray.size() - 1, _isWhispering.values().contains(true) ? "true" : "false");
		_ts3.printMessageToCurrentTab(message);

		for (int i = 0; i < targetClientIDArray.size() - 1; i++) {
			char displayName[256];
			int error;
			if ((error = _ts3.getClientDisplayName(serverConnectionHandlerID, targetClientIDArray[i], displayName, 256)) != ERROR_ok) {				
				strncpy_s(displayName, _nicknames[serverConnectionHandlerID][targetClientIDArray[i]].toStdString().c_str(), 256);
			}

			QString logmessage = "DisplayNames: " + QString::number(targetClientIDArray[i]) + " : " + QString(displayName);
			_ts3.logMessage(logmessage.toStdString().c_str(), LogLevel_DEBUG, "KRTC WhisperTo", serverConnectionHandlerID);
			_ts3.printMessageToCurrentTab(logmessage.toStdString().c_str());
		}
	}

	//if (result)
	//	free(result);

	free(returnCode);
}

void KRTComms::SetPushToTalk(uint64 serverConnectionHandlerID, bool shouldTalk) {
	unsigned int error;

	if (!_pttActive) {
		// Is VAD (Voice Activation Detection) aktiv?
		char* vad;
		if ((error = _ts3.getPreProcessorConfigValue(serverConnectionHandlerID, "vad", &vad)) != ERROR_ok) {
			return;
		}
		_vadActive = !strcmp(vad, "true");
		_ts3.freeMemory(vad);

		// Schaun ob PTT (Push-To-Talk) + VAD (Voice Activation Detection) aktiv ist
		int input;
		if ((error = _ts3.getClientSelfVariableAsInt(serverConnectionHandlerID, CLIENT_INPUT_DEACTIVATED, &input)) != ERROR_ok) {
			return;
		}
		_inputActive = !input; // We want to know when it is active, not when it is inactive
	}

	// Wenn VAD (Voice Activation Detection) an ist schalten wir dennoch auf Dauersenden um und aktivieren es später wieder
	if ((error = _ts3.setPreProcessorConfigValue(serverConnectionHandlerID, "vad",
		(shouldTalk && (_vadActive && _inputActive)) ? "false" : (_vadActive) ? "true" : "false")) != ERROR_ok) {
		return;
	}
	
	// Activate the input, restore the input setting afterwards
	if ((error = _ts3.setClientSelfVariableAsInt(serverConnectionHandlerID, CLIENT_INPUT_DEACTIVATED,
		(shouldTalk || _inputActive) ? INPUT_ACTIVE : INPUT_DEACTIVATED)) != ERROR_ok) {
		return;
	}

	//QString logmessage = "Test: " + QString(_pttActive ? "pttActive true " : "pttActive false ") + QString(_vadActive ? "_vadActive true " : "_vadActive false ") + QString(_inputActive ? "_inputActive true " : "_inputActive false ") + " " + QString(shouldTalk ? "shouldTalk true" : "shouldTalk false");
	//_ts3.printMessageToCurrentTab(logmessage.toStdString().c_str());


	_ts3.flushClientSelfUpdates(serverConnectionHandlerID, NULL);

	_pttActive = shouldTalk;
}

void KRTComms::Reset(uint64 serverConnectionHandlerID) {
	//_ts3.printMessageToCurrentTab("RESET");
	_ts3.requestClientSetWhisperList(serverConnectionHandlerID, NULL, NULL, NULL, NULL);
	SetPushToTalk(serverConnectionHandlerID, false);
	for (int radio_id = 0; radio_id < _isWhispering.size(); radio_id++) {
		_isWhispering[radio_id] = false;
		if (!_activeRadios[serverConnectionHandlerID].contains(radio_id)) continue;
		_channels->DisableSendLamp(radio_id);

		int frequence = GetFrequence(serverConnectionHandlerID, radio_id);

		QList<anyID> tmp = QList(_targetClientIDs[serverConnectionHandlerID][frequence]);
		if (tmp.size() == 0) continue;
		tmp.append(0);

		
		QString command = "SENDOFF\t" + Encrypter::Encrypt(radio_id, QString::number(frequence));
		SendPluginCommand(serverConnectionHandlerID, _pluginID, command, PluginCommandTarget_CLIENT, tmp.toVector().constData(), NULL);
	}
	_channelMuted = false;
}

void KRTComms::SetPan(int radio_id, float pan) {
	_pans[radio_id] = pan;

	//QString logmessage = "Radio: " + QString::number(radio_id) + " Pan: " + QString::number(_pans[radio_id]);
	//_ts3.printMessageToCurrentTab(logmessage.toStdString().c_str());
}

void KRTComms::SetVolumeGain(int radio_id, float gain) {
	_gains[radio_id] = gain;
}

int KRTComms::GetFrequence(uint64 serverConnectionHandlerID, int radio_id) {
	return _activeRadios[serverConnectionHandlerID].value(radio_id, -1);
}

int KRTComms::GetRadioId(uint64 serverConnectionHandlerID, int frequence) {
	foreach(int radio_id, _activeRadios[serverConnectionHandlerID].keys()) {
		if (_activeRadios[serverConnectionHandlerID][radio_id] == frequence) {
			return radio_id;
		}
	}
	return -1;
}

void KRTComms::Disconnect() {
	foreach(uint64 serverConnectionHandlerID, _activeRadios.keys()) {
		Disconnect(serverConnectionHandlerID);
	}
}

void KRTComms::Disconnect(uint64 serverConnectionHandlerID) {
	if (_debug) {
		_ts3.printMessageToCurrentTab(("Disconnect " + QString::number(serverConnectionHandlerID)).toStdString().c_str());
	}
	foreach(int radio_id, _activeRadios[serverConnectionHandlerID].keys()) {
		SetActiveRadio(serverConnectionHandlerID, radio_id, false, -1);
	}
}

void KRTComms::Disconnected(uint64 serverConnectionHandlerID, anyID clientID) {
	if(_debug)
		_ts3.printMessageToCurrentTab((QString::number(clientID) + " Disconnected").toStdString().c_str());

	foreach(int frequence, _activeRadios[serverConnectionHandlerID].values()) {
		RemoveFromFrequence(serverConnectionHandlerID, frequence, clientID);
	}
}

void KRTComms::RequestHotkeyInputDialog(int radio_id, QWidget *parent) {
	RequestHotkeyInputDialog("send_ch_" + QString::number(radio_id), parent);
}

void KRTComms::RequestHotkeyInputDialog(QString keyword, QWidget *parent) {
	_keyword = keyword;
	_parent = parent;
	_ts3.requestHotkeyInputDialog(_pluginID, _keyword.toStdString().c_str(), 1, _parent);
}

void KRTComms::OnHotkeyRecordedEvent(QString keyword, QString key) {
	if (!_keyword.isEmpty()) {
		if(_key.isEmpty())
			_key = key;

		std::this_thread::sleep_for(std::chrono::milliseconds(500));
		_ts3.requestHotkeyInputDialog(_pluginID, (_keyword + "_").toStdString().c_str(), 0, _parent);
		_keyword = "";
	}
	else {		
		if (key != _key) {
			_keyword = keyword.remove(keyword.length() - 1, 1);
			if (_channels->onDifferentKey(_keyword, _key, _parent)) {
				_key = ""; //siehe channels.cpp Kommentar
			}
		}
		else {
			_key = "";
		}
	}
}

void KRTComms::OnTalkStatusChangeEvent(uint64 serverConnectionHandlerID, int status, int isReceivedWhisper, anyID clientID) {
	
}

void KRTComms::OnEditPlaybackVoiceDataEvent(uint64 serverConnectionHandlerID, anyID clientID, short* samples, int sampleCount, int channels) {
		
	//Hier alles was den Ducker nicht braucht coden

	//Wenn alles gemuted ist braucht er nichts ducken
	bool all_muted = true;
	for (int radio_id = 0; radio_id < RADIO_COUNT; radio_id++) {
		if (!_muted[radio_id]) {
			all_muted = false;
			break;
		}
	}
	if (all_muted) return;

	bool notWhispering = !Talkers::getInstance().IsWhispering(serverConnectionHandlerID, clientID);
	if (_channelMuted && notWhispering) {
		Ducker::getInstance().EditPlaybackVoiceData(samples, sampleCount, channels, 0.0f);
		return;
	}
	
	Ducker::Type shouldDuck = Ducker::Type::NONE;
	
	if (Talkers::getInstance().IsAnyWhisperingAndNotMuted(serverConnectionHandlerID)) {
		if (notWhispering) {
			shouldDuck = Ducker::Type::CHANNEL;
		}
		else {
			shouldDuck = Talkers::getInstance().PrioritizedFrequence(serverConnectionHandlerID, clientID);
		}
	}

	if (shouldDuck != Ducker::Type::NONE) {
		Ducker::getInstance().OnEditPlaybackVoiceDataEvent(serverConnectionHandlerID, clientID, samples, sampleCount, channels, shouldDuck);		
	}
}

void KRTComms::OnEditPostProcessVoiceDataEvent(uint64 serverConnectionHandlerID, anyID clientID, short* samples, int sampleCount, int channels, const unsigned int* channelSpeakerArray, unsigned int* channelFillMask) {
	
	if (channels == 1)
		return;

	// keine daten in den channels
	if (*channelFillMask == 0)
		return;

	try {
		foreach(int radio_id, _activeRadios[serverConnectionHandlerID].keys()) {
			
			int frequence = GetFrequence(serverConnectionHandlerID, radio_id);
			if (_targetClientIDs[serverConnectionHandlerID][frequence].contains(clientID) && Talkers::getInstance().IsWhispering(serverConnectionHandlerID, clientID, frequence)) {
				
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
				
				float p, gl, gr;

				p = M_PI * (_pans[radio_id] + 1) / 4;
				
				gl = ::cos(p);
				gr = ::sin(p);

				//QString logmessage = "HMM: Channels: " + QString::number(channels) + " | " + QString::number(gl) + " " + QString::number(gr);
				//_ts3.printMessageToCurrentTab(logmessage.toStdString().c_str());

				QMap<unsigned int, int> speaker2Channel;
				for (int i = 0; i < channels; ++i)
				{
					if (*channelFillMask & (1 << i))
						speaker2Channel.insert(channelSpeakerArray[i], i);
				}

				
				for (int i = 0; i < sampleCount * channels; i += channels) {
					for (auto j = 0; j < channels; j++) {
						auto sample = floatsSample[j][i / channels] * _gains[radio_id] * (_muted[radio_id] ? 0.0f : 1.0f);
						if (_pans[radio_id] != 0.0f) {
							if (speaker2Channel.value(SPEAKER_HEADPHONES_LEFT, -1) == j || speaker2Channel.value(SPEAKER_FRONT_LEFT, -1) == j) {
								sample *= gl;
							}
							if (speaker2Channel.value(SPEAKER_HEADPHONES_RIGHT, -1) == j || speaker2Channel.value(SPEAKER_FRONT_RIGHT, -1) == j) {
								sample *= gr;
							}
						}
						short newValue;
						if (sample > 1.0) newValue = SHRT_MAX;
						else if (sample < -1.0) newValue = SHRT_MIN;
						else newValue = static_cast<short>(sample * (SHRT_MAX - 1));
						samples[i + j] = newValue;
					}
				}

				break;
			}
		}
	}
	catch (const std::exception& e) {
		_ts3.logMessage(e.what(), LogLevel_ERROR, "KRTC OnEditPostProcessVoiceDataEvent", serverConnectionHandlerID);
	}
	catch (...) {
		_ts3.logMessage("Unkown exception", LogLevel_ERROR, "KRTC OnEditPostProcessVoiceDataEvent", serverConnectionHandlerID);
	}
}

void KRTComms::OnEditCapturedVoiceDataEvent(uint64 serverConnectionHandlerID, short* samples, int sampleCount, int channels, int* edited) {
	
}

void KRTComms::OnHotkeyEvent(uint64 serverConnectionHandlerID, int radio_id) {

	if (_doubleClickConnection[radio_id] != NULL) {
		QObject::disconnect(*_doubleClickConnection[radio_id]);
		delete _doubleClickConnection[radio_id];
		_doubleClickConnection[radio_id] = NULL;
	}

	if (_doubleClickTimer[radio_id].isActive()) {
		_doubleClickTimer[radio_id].stop();
		//_ts3.printMessageToCurrentTab("stopped");
	}

	_doubleClickCount[radio_id]++;
	//_ts3.printMessageToCurrentTab(("doubleClickCount: " + QString::number(_doubleClickCount[radio_id]) + " | " + QString::number(radio_id)).toStdString().c_str());
	if (_toggleMute) {
		if (_doubleClickCount[radio_id] % 2 != 0) {
			ToggleMute(serverConnectionHandlerID, radio_id);
		} else {
			_doubleClickCount[radio_id] = 0;
		}
		return;
	}
	
	_doubleClickConnection[radio_id] = new QMetaObject::Connection;	
	*_doubleClickConnection[radio_id] = QObject::connect(&_doubleClickTimer[radio_id], &QTimer::timeout, [this, serverConnectionHandlerID, radio_id]() {
		//_ts3.printMessageToCurrentTab("Timedout");

		bool oriIsWhispering = _isWhispering[radio_id];
		//Mein Brain ist gef***
		if (_doubleClickCount[radio_id] == 3 && !oriIsWhispering && !_allMuted || _doubleClickCount[radio_id] == 4 && oriIsWhispering && !_allMuted || (_allMuted || _toggleMuted[radio_id]) && oriIsWhispering) {
			if (!_toggleMuted[radio_id]) {
				_muted[radio_id] = true;
				_channels->MuteReceiveLamp(radio_id);
			}
			//_ts3.printMessageToCurrentTab("Mute");
			if (oriIsWhispering) { //Wenn gerade gewhispert wird und dann ein doppelklick kommt whispern ausschalten
				this->WhisperToRadio(serverConnectionHandlerID, radio_id);				
				_toggleMuted[radio_id] = false;
			}
		}
		else if (_muted[radio_id] && !oriIsWhispering) {
			_muted[radio_id] = false;
			_channels->UnMuteReceiveLamp(radio_id);
			//_ts3.printMessageToCurrentTab("UnMute");

			if ((_allMuted || _toggleMuted[radio_id]) && _doubleClickCount[radio_id] % 2 != 0) {
				this->WhisperToRadio(serverConnectionHandlerID, radio_id);				
			}
		}
		else if(_doubleClickCount[radio_id] % 2 != 0) {
			this->WhisperToRadio(serverConnectionHandlerID, radio_id);
			//_ts3.printMessageToCurrentTab("Modulo");
		}
		
		_doubleClickCount[radio_id] = 0;

		QObject::disconnect(*_doubleClickConnection[radio_id]);
		delete _doubleClickConnection[radio_id];
		_doubleClickConnection[radio_id] = NULL;
	});
	_doubleClickTimer[radio_id].start(200);
}

void KRTComms::PushToMute(uint64 serverConnectionHandlerID, QString type) {
	if (type == "all") {
		PushToMuteAll(serverConnectionHandlerID);
	}
	else if (type == "channel") {
		PushToMuteChannel(serverConnectionHandlerID);
	}
}

void KRTComms::PushToMuteAll(uint64 serverConnectionHandlerID) {
	_allMuted = !_allMuted;

	for (int radio_id = 0; radio_id < RADIO_COUNT; radio_id++) {
		_muted[radio_id] = _allMuted || _toggleMuted[radio_id];
		if (_muted[radio_id]) {
			_channels->MuteReceiveLamp(radio_id);
		}
		else {
			_channels->UnMuteReceiveLamp(radio_id);
		}
	}
}

void KRTComms::PushToMuteChannel(uint64 serverConnectionHandlerID) {
	_channelMuted = !_channelMuted;
	_channels->ChangeChannelMuted(_channelMuted);
}

void KRTComms::ToggleMute(uint64 serverConnectionhandlerID) {
	_toggleMute = !_toggleMute;
}

void KRTComms::ToggleMute(uint64 serverConnectionHandlerID, int radio_id) {
	_toggleMuted[radio_id] = !_toggleMuted[radio_id];
	_muted[radio_id] = !_muted[radio_id];
	if (_muted[radio_id]) {
		_channels->MuteReceiveLamp(radio_id);
	}
	else {
		_channels->UnMuteReceiveLamp(radio_id);
	}
}

bool KRTComms::IsFrequenceMuted(uint64 serverConnectionHandlerID, int frequence) {

	for (int radio_id = 0; radio_id < RADIO_COUNT; radio_id++) {
		if (GetFrequence(serverConnectionHandlerID, radio_id) == frequence && _muted[radio_id]) {
			return true;
		}
	}

	return false;
}

QList<int> KRTComms::GetMutedFrequences(uint64 serverConnectionHandlerID) {
	QList<int> list;
	for (int radio_id = 0; radio_id < RADIO_COUNT; radio_id++) {
		if (_muted[radio_id]) {
			list.append(GetFrequence(serverConnectionHandlerID, radio_id));
		}
	}
	return list;
}

int KRTComms::OnServerErrorEvent(uint64 serverConnectionHandlerID, const char* errorMessage, unsigned int error, const char* returnCode, const char* extraMessage) {
	if (_debug) {
		_ts3.printMessageToCurrentTab(("PLUGIN: onServerErrorEvent " + QString::number(serverConnectionHandlerID) + " " + QString(errorMessage) + " " + QString::number(error) + " " + (returnCode ? QString(returnCode) : "") + " " + QString(extraMessage)).toStdString().c_str());
	}
	if (returnCode) {
		/* A plugin could now check the returnCode with previously (when calling a function) remembered returnCodes and react accordingly */
		/* In case of using a a plugin return code, the plugin can return:
		 * 0: Client will continue handling this error (print to chat tab)
		 * 1: Client will ignore this error, the plugin announces it has handled it */
		return 1;
	}
	return 0;  /* If no plugin return code was used, the return value of this function is ignored */
}

void KRTComms::OnClientMoveTimeoutEvent(uint64 serverConnectionHandlerID, anyID clientID, uint64 oldChannelID, uint64 newChannelID, int visibility, const char* timeoutMessage) {
	if (_debug) {
		QString logmessage = "OnClientMoveTimeoutEvent " + QString::number(clientID) + " oldChannelID: " + QString::number(oldChannelID) + " newChannleID: " + QString::number(newChannelID) + " vis: " + QString::number(visibility) + " timeoutMessage: " + QString(timeoutMessage);
		_ts3.printMessageToCurrentTab(logmessage.toStdString().c_str());
	}

	if (newChannelID == 0) {
		Disconnected(serverConnectionHandlerID, clientID);
	}
}

void KRTComms::SetSoundsEnabled(bool enabled) {
	_soundsEnabled = enabled;
}

