#include "KRTComms.h"
#include "teamspeak/public_rare_definitions.h"
#include "teamspeak/public_errors.h"
#include "ts3_functions.h"
#include "Helpers.h"
#include "Talkers.h"
#include "Ducker.h"
#include "Encrypter.h"

#include <chrono>
#include <thread>
#include <array>
#include <QtCore/QString>
#include <QtWidgets/QDialog>
#include <math.h>

#ifndef M_PI
#define M_PI    3.14159265358979323846f
#endif

#define MAX_CHANNELS 8
#define RADIO_COUNT 4

char* KRTComms::version = "0.0.6";

KRTComms::KRTComms() {
	for (int i = 0; i < RADIO_COUNT; i++) {
		_pans[i] = 0.0f;
		_gains[i] = 1.0f;
		_isWhispering[i] = false;
	}
}

KRTComms& KRTComms::getInstance() {
	static KRTComms instance;
	return instance;
}

void KRTComms::Log(QString message) {
	if(KRTComms::getInstance()._debug)
		KRTComms::getInstance()._ts3.printMessageToCurrentTab(message.toStdString().c_str());
}

void KRTComms::Init(const struct TS3Functions funcs, char* pluginID) {
	_ts3 = funcs;
	_pluginID = pluginID;
	//_ts3.getClientID(_serverConnectionHandlerID, &_me);
}

void KRTComms::SetDebug(bool debug) {
	_debug = debug;
}

void KRTComms::SendPluginCommand(uint64 serverConnectionHandlerID, const char* pluginID, QString command, int targetMode, const anyID* targetIDs, const char* returnCode) {
	anyID me;
	_ts3.getClientID(serverConnectionHandlerID, &me);

	command = QString::number(me) + "\t" + command;
	_ts3.sendPluginCommand(serverConnectionHandlerID, _pluginID, command.toStdString().c_str(), PluginCommandTarget_SERVER, NULL, NULL);
}

void KRTComms::ProcessPluginCommand(uint64 serverConnectionHandlerID, const char* pluginCommand, anyID invokerClientID, const char* invokerName) {

	QString command = QString(pluginCommand);	
	QStringList tokens = command.split('\t');

	if(invokerClientID == 0)
		invokerClientID = tokens[0].toUInt();

	if (_debug) {
		QString logmessage = "ProcessPluginCommand: " + QString(pluginCommand) + " | " + QString::number(invokerClientID);
		_ts3.logMessage(logmessage.toStdString().c_str(), LogLevel_DEBUG, "KRTC ProcessPluginCommand", serverConnectionHandlerID);
		_ts3.printMessageToCurrentTab(logmessage.toStdString().c_str());
	}
	//Hier Commands überprüfen die auch an sich selbst gehen

	anyID me;
	_ts3.getClientID(serverConnectionHandlerID, &me);
	if (me == invokerClientID) {
		return;
	}

	if (tokens[1] == "JOINED") {
		bool ok;
		int frequence = Encrypter::Decrypt(serverConnectionHandlerID, tokens[2]).toInt(&ok, 10);
		if (ok && ActiveInFrequence(serverConnectionHandlerID, frequence)) {
			if (AddToFrequence(serverConnectionHandlerID, frequence, invokerClientID, invokerName)) { //false wenn selbst
				AnswerTheCall(serverConnectionHandlerID, frequence, invokerClientID);
			}
		}
	}

	if (tokens[1] == "LEFT") {
		bool ok;
		int frequence = Encrypter::Decrypt(serverConnectionHandlerID, tokens[2]).toInt(&ok, 10);
		if (ok) {			
			RemoveFromFrequence(serverConnectionHandlerID, frequence, invokerClientID);			
		}		
	}

	if (tokens[1] == "METOO") {
		bool ok;
		int frequence = Encrypter::Decrypt(serverConnectionHandlerID, tokens[2]).toInt(&ok, 10);
		if (ok && ActiveInFrequence(serverConnectionHandlerID, frequence)) {
			AddToFrequence(serverConnectionHandlerID, frequence, invokerClientID, invokerName);
		}
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
				SendPluginCommand(serverConnectionHandlerID, _pluginID, command, PluginCommandTarget_SERVER, NULL, NULL);
				logmessage += " LEFT";

				RemoveAllFromFrequence(serverConnectionHandlerID, old_frequence);
			}

			if (!isActive || old_frequence != frequence) {
				_activeRadios[serverConnectionHandlerID][radio_id] = frequence;
				QString command = "JOINED\t" + Encrypter::Encrypt(radio_id, QString::number(frequence));
				SendPluginCommand(serverConnectionHandlerID, _pluginID, command, PluginCommandTarget_SERVER, NULL, NULL);
				logmessage += " JOINED";
			}
		}
		else {
			if (_activeRadios[serverConnectionHandlerID].contains(radio_id)) {
				if (_activeRadios[serverConnectionHandlerID].values().count(old_frequence) <= 1) {
					
					QString command = "LEFT\t" + Encrypter::Encrypt(radio_id, QString::number(old_frequence));
					SendPluginCommand(serverConnectionHandlerID, _pluginID, command, PluginCommandTarget_SERVER, NULL, NULL);
					logmessage += " LEFT";

					RemoveAllFromFrequence(serverConnectionHandlerID, old_frequence);
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
	return _activeRadios[serverConnectionHandlerID].values().contains(frequence);
}

bool KRTComms::AddToFrequence(uint64 serverConnectionHandlerID, int frequence, anyID clientID, const char* nickname) {
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

	return true;
}

bool KRTComms::RemoveFromFrequence(uint64 serverConnectionHandlerID, int frequence, anyID clientID) {
	//_ts3.printMessageToCurrentTab("REMOVED");
	return _targetClientIDs[serverConnectionHandlerID][frequence].removeOne(clientID);
}

void KRTComms::RemoveAllFromFrequence(uint64 serverConnectionHandlerID, int frequence) {
	//_ts3.printMessageToCurrentTab("REMOVED ALL");
	_targetClientIDs[serverConnectionHandlerID][frequence].clear();
}

bool KRTComms::AnswerTheCall(uint64 serverConnectionHandlerID, int frequence, anyID clientID) {
	QString command = "METOO\t" + Encrypter::Encrypt(GetRadioId(serverConnectionHandlerID, frequence), QString::number(frequence));
	SendPluginCommand(serverConnectionHandlerID, _pluginID, command, PluginCommandTarget_CLIENT, &clientID, NULL);
	return true;
}

void KRTComms::WhisperToRadio(uint64 serverConnectionHandlerID, int radio_id) {
	std::vector<anyID> empty;
	_isWhispering[radio_id] = !_isWhispering[radio_id];

	int frequence = GetFrequence(serverConnectionHandlerID, radio_id);
	if (_debug) {
		QString logmessage = "WhisperToRadio: " + QString::number(radio_id) + " | " + QString::number(frequence);
		_ts3.logMessage(logmessage.toStdString().c_str(), LogLevel_DEBUG, "KRTC WhisperToRadio", serverConnectionHandlerID);
		//_ts3.printMessageToCurrentTab(logmessage.toStdString().c_str());
	}

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
	const char* returnCode = new char[256];

	//int* result = NULL;

	//_ts3.isWhispering(serverConnectionHandlerID, NULL, result);

	if (targetChannelIDArray.size() == 0 && targetClientIDArray.size() == 0) {
		_ts3.requestClientSetWhisperList(serverConnectionHandlerID, NULL, NULL, NULL, returnCode);
		SetPushToTalk(serverConnectionHandlerID, false);
	}
	else {
		SetPushToTalk(serverConnectionHandlerID, true);
		_ts3.requestClientSetWhisperList(serverConnectionHandlerID, NULL, targetChannelIDArray.toVector().data(), targetClientIDArray.toVector().data(), returnCode);
	}

	if (_debug) {
		char message[256];
		sprintf_s(message, 256, "Whisper Clients: %d | isWispering: %s", targetClientIDArray.size(), _isWhispering.values().contains(true) ? "true" : "false");
		_ts3.printMessageToCurrentTab(message);
	}

	//if (result)
	//	free(result);
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
	}
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
	foreach(int radio_id, _activeRadios[serverConnectionHandlerID].keys()) {
		SetActiveRadio(serverConnectionHandlerID, radio_id, false, -1);
	}
}

void KRTComms::Disconnected(uint64 serverConnectionHandlerID, anyID clientID) {
	foreach(int frequence, _activeRadios[serverConnectionHandlerID].values()) {
		RemoveFromFrequence(serverConnectionHandlerID, frequence, clientID);
	}
}

void KRTComms::RequestHotkeyInputDialog(int radio_id, QWidget *parent) {
	_keyword = "send_ch_" + QString::number(radio_id);
	_parent = parent;
	_ts3.requestHotkeyInputDialog(_pluginID, _keyword.toStdString().c_str(), 1, _parent);
}

void KRTComms::OnHotkeyRecordedEvent(QString keyword, QString key) {
	if (!_keyword.isEmpty()) {
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
		_ts3.requestHotkeyInputDialog(_pluginID, (_keyword + "_").toStdString().c_str(), 0, _parent);
		_keyword = "";
		_parent = NULL;
	}
}

void KRTComms::OnTalkStatusChangeEvent(uint64 serverConnectionHandlerID, int status, int isReceivedWhisper, anyID clientID) {
	//TODO Handle multiple STATUS_TALKING
	try {
		int clientFrequence = -1;
		foreach(int frequence, _targetClientIDs[serverConnectionHandlerID].keys()) {
			if (_targetClientIDs[serverConnectionHandlerID][frequence].contains(clientID)) {
				//_isClientWhispering[serverConnectionHandlerID][clientID] = isReceivedWhisper;
				clientFrequence = frequence;
				//TODO Support für multiple frequences
				break;
			}
		}

		//TODO Was wenn es einen Disconnect gibt wärenddessen sie STATUS_TALKING sind

		if (status == STATUS_TALKING) {
			Talkers::getInstance().Add(serverConnectionHandlerID, clientID, isReceivedWhisper, clientFrequence);
		} else
		if(status == STATUS_NOT_TALKING) {
			Talkers::getInstance().Remove(serverConnectionHandlerID, clientID, isReceivedWhisper);
		}
	} 
	catch (const std::exception& e) {
		_ts3.logMessage(e.what(), LogLevel_ERROR, "KRTC OnTalkStatusChangeEvent", serverConnectionHandlerID);
	}
	catch (...) {
		_ts3.logMessage("Unkown exception", LogLevel_ERROR, "KRTC OnTalkStatusChangeEvent", serverConnectionHandlerID);
	}
}

void KRTComms::OnEditPlaybackVoiceDataEvent(uint64 serverConnectionHandlerID, anyID clientID, short* samples, int sampleCount, int channels) {
		
	//Hier alles was den Ducker nicht braucht coden

	if (!Ducker::getInstance().IsEnabled()) return;
	Ducker::Type shouldDuck = Ducker::Type::NONE;

	if (Talkers::getInstance().IsAnyWhispering(serverConnectionHandlerID)) {
		if (!Talkers::getInstance().IsWhispering(serverConnectionHandlerID, clientID)) {
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
			if (_targetClientIDs[serverConnectionHandlerID][frequence].contains(clientID) && Talkers::getInstance().IsWhispering(serverConnectionHandlerID, clientID)) {
				
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
						auto sample = floatsSample[j][i / channels] * _gains[radio_id];
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

