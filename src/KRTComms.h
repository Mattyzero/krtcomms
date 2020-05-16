#pragma once
#include "ts3_functions.h"
#include <QtWidgets/QDialog>
#include <QtCore/QMap>
#include <QtCore/QList>
#include "channels.h"

class KRTComms {
public:
	static KRTComms& getInstance();
	static void Log(QString message);
	static char* version;

	KRTComms();
	~KRTComms() = default;

	void Init(const struct TS3Functions funcs, char* pluginID, channels* channels);

	void SetDebug(bool debug);

	void InfoData(uint64 serverConnectionHandlerID, uint64 id, enum PluginItemType type, char** data);

	void SendPluginCommand(uint64 serverConnectionHandlerID, const char* pluginID, QString command, int targetMode, const anyID* targetIDs, const char* returnCode);

	void ProcessPluginCommand(uint64 serverConnectionHandlerID, const char* pluginCommand, anyID invokerClientID, const char* invokerName);

	void SetActiveRadio(uint64 serverConnectionHandlerID, int radio_id, bool state, int frequence);
	bool ActiveInRadio(uint64 serverConnectionHandlerID, int radio_id);
	bool ActiveInFrequence(uint64 serverConnectionHandlerID, int frequence);
	bool AddToFrequence(uint64 serverConnectionHandlerID, int frequence, anyID clientID, const char* nickname);
	bool RemoveFromFrequence(uint64 serverConnectionHandlerID, int frequence, anyID clientID);
	void RemoveAllFromFrequence(uint64 serverConnectionHandlerID, int frequence);

	bool AnswerTheCall(uint64 serverConnectionHandlerID, int frequence, anyID clientID);

	void WhisperToRadio(uint64 serverConnectionHandlerID, int radio_id);
	void WhisperTo(uint64 serverConnectionHandlerID, QList<uint64> targetChannelIDArray, int targetChannelIDArrayLength, QList<anyID> targetClientIDArray, int targetClientIDArrayLength);

	void SetPushToTalk(uint64 serverConnectionHandlerID, bool shouldTalk);
	void Reset(uint64 serverConnectionHandlerID);

	void SetPan(int radio_id, float pan);
	void SetVolumeGain(int radio_id, float gain);

	int GetFrequence(uint64 serverConnectionHandlerID, int radio_id);
	int GetRadioId(uint64 serverConnectionHandlerID, int frequence);

	void Disconnect();
	void Disconnect(uint64 serverConnectionHandlerID);
	void Disconnected(uint64 serverConnectionHandlerID, anyID clientID);

	void RequestHotkeyInputDialog(int radio_id, QWidget *parent = nullptr);
	void OnHotkeyRecordedEvent(QString keyword, QString key);
	void OnTalkStatusChangeEvent(uint64 serverConnectionHandlerID, int status, int isReceivedWhisper, anyID clientID);
	void OnEditPlaybackVoiceDataEvent(uint64 serverConnectionHandlerID, anyID clientID, short* samples, int sampleCount, int channels);
	void OnEditPostProcessVoiceDataEvent(uint64 serverConnectionHandlerID, anyID clientID, short* samples, int sampleCount, int channels, const unsigned int* channelSpeakerArray, unsigned int* channelFillMask);
	void OnEditCapturedVoiceDataEvent(uint64 serverConnectionHandlerID, short* samples, int sampleCount, int channels, int* edited);
private:
	
	bool _firstX = true;
	bool _firstY = true;

	QMap<uint64, QMap<int, int>> _activeRadios;
	
	QString _keyword;
	QWidget* _parent;

	bool _debug = false;
	QMap<int, bool> _isWhispering;
	bool _pttActive = false;
	bool _vadActive = false;
	bool _inputActive = false;

	QMap<uint64, QMap<int, QList<uint64>>> _targetChannelIDs;
	QMap<uint64, QMap<int, QList<anyID>>>  _targetClientIDs;
	QMap<uint64, QMap<anyID, QString>> _nicknames;

	float _pans[4];
	float _gains[4];

	char* _pluginID;
	struct TS3Functions _ts3;
	channels* _channels;
};

