#pragma once
#include "ts3_functions.h"
#include <QtWidgets/QDialog>
#include <QtCore/QMap>
#include <QtCore/QList>
#include <QtCore/QTimer>
#include "channels.h"

#define RADIO_COUNT 8

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
	bool AddToFrequence(uint64 serverConnectionHandlerID, int frequence, anyID clientID, const char* nickname, bool shouldUpdate);
	bool RemoveFromFrequence(uint64 serverConnectionHandlerID, int frequence, anyID clientID);
	void RemoveAllFromFrequence(uint64 serverConnectionHandlerID, int frequence);

	bool AnswerTheCall(uint64 serverConnectionHandlerID, int frequence, anyID clientID);

	void WhisperToRadio(uint64 serverConnectionHandlerID, int radio_id);
	void UpdateWhisperTo(uint64 serverConnectionHandlerID, int frequence);
	void WhisperTo(uint64 serverConnectionHandlerID, QList<uint64> targetChannelIDArray, QList<anyID> targetClientIDArray);

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
	void RequestHotkeyInputDialog(QString keyword, QWidget *parent = nullptr);
	void OnHotkeyRecordedEvent(QString keyword, QString key);
	void OnTalkStatusChangeEvent(uint64 serverConnectionHandlerID, int status, int isReceivedWhisper, anyID clientID);
	void OnEditPlaybackVoiceDataEvent(uint64 serverConnectionHandlerID, anyID clientID, short* samples, int sampleCount, int channels);
	void OnEditPostProcessVoiceDataEvent(uint64 serverConnectionHandlerID, anyID clientID, short* samples, int sampleCount, int channels, const unsigned int* channelSpeakerArray, unsigned int* channelFillMask);
	void OnEditCapturedVoiceDataEvent(uint64 serverConnectionHandlerID, short* samples, int sampleCount, int channels, int* edited);
	void OnHotkeyEvent(uint64 serverConnectionHandlerID, int radio_id);
	void OnTimerTimeout();

	void PushToMute(uint64 serverConnectionHandlerID, QString type);
	void PushToMuteAll(uint64 serverConnectionHandlerID);
	void PushToMuteChannel(uint64 serverConnectionHandlerID);
	void ToggleMute(uint64 serverConnectionHandlerID);
	void ToggleMute(uint64 serverConnectionHandlerID, int radio_id);
	bool IsFrequenceMuted(uint64 serverConnectionHandlerID, int frequence);
	QList<int> GetMutedFrequences(uint64 serverConnectionHandlerID);
	int OnServerErrorEvent(uint64 serverConnectionHandlerID, const char* errorMessage, unsigned int error, const char* returnCode, const char* extraMessage);
	void OnClientMoveTimeoutEvent(uint64 serverConnectionHandlerID, anyID clientID, uint64 oldChannelID, uint64 newChannelID, int visibility, const char* timeoutMessage);

	void SetSoundsEnabled(bool enabled);
private:

	QMap<uint64, QMap<int, int>> _activeRadios;
	
	QString _keyword;
	QString _key;
	QWidget* _parent;

	bool _debug = false;
	QMap<int, bool> _isWhispering;
	bool _pttActive = false;
	bool _vadActive = false;
	bool _inputActive = false;

	QMap<uint64, QMap<int, QList<uint64>>> _targetChannelIDs;
	QMap<uint64, QMap<int, QList<anyID>>>  _targetClientIDs;
	QMap<uint64, QMap<anyID, QString>> _nicknames;

	float _pans[RADIO_COUNT];
	float _gains[RADIO_COUNT];

	char* _pluginID;
	struct TS3Functions _ts3;
	channels* _channels;
	bool _soundsEnabled = true;
	QString _soundsPath[RADIO_COUNT][2];

	QTimer _doubleClickTimer[RADIO_COUNT];
	int _doubleClickCount[RADIO_COUNT];
	bool _muted[RADIO_COUNT];
	bool _allMuted = false;
	bool _channelMuted = false;
	bool _toggleMute = false;
	bool _toggleMuted[RADIO_COUNT];

	QMetaObject::Connection * _doubleClickConnection[RADIO_COUNT];
};

