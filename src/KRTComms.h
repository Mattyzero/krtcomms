#pragma once
#include "ts3_functions.h"
#include <QtWidgets/QDialog>
#include <QtCore/QMap>
#include <QtCore/QList>
#include <QtCore/QTimer>
#include "channels.h"

#define RADIO_COUNT 8

#define LOCAL_START 0
#define LOCAL_END 1
#define REMOTE_START 2
#define REMOTE_END 3

class KRTComms {
public:
	static KRTComms& getInstance();
	static void Log(QString message);
	static char* version;

	KRTComms();
	~KRTComms() = default;

	void Init(const struct TS3Functions funcs, char* pluginID, channels* channels);

	void Connected(uint64 serverConnectionHandlerID);
	void InitVars(uint64 serverConnectionHandlerID);

	void SetDebug(bool debug);

	void InfoData(uint64 serverConnectionHandlerID, uint64 id, enum PluginItemType type, char** data);

	void SendPluginCommand(uint64 serverConnectionHandlerID, const char* pluginID, QString command, int targetMode, const anyID* targetIDs, const char* returnCode);

	void ProcessPluginCommand(uint64 serverConnectionHandlerID, const char* pluginCommand, anyID invokerClientID, const char* invokerName);

	void SetActiveRadio(uint64 serverConnectionHandlerID, int radio_id, bool state, int frequence);
	void Left(uint64 serverConnectionHandlerID, int radio_id, int old_frequence);
	bool ActiveInRadio(uint64 serverConnectionHandlerID, int radio_id);
	bool ActiveInFrequence(uint64 serverConnectionHandlerID, int frequence);
	bool AddToFrequence(uint64 serverConnectionHandlerID, int frequence, anyID clientID, const char* nickname, bool shouldUpdate);
	bool RemoveFromFrequence(uint64 serverConnectionHandlerID, int frequence, anyID clientID);
	void RemoveAllFromFrequence(uint64 serverConnectionHandlerID, int frequence);

	bool AnswerTheCall(uint64 serverConnectionHandlerID, int frequence, anyID clientID);

	void WhisperToRadio(uint64 serverConnectionHandlerID, int radio_id);
	void UpdateWhisperTo(uint64 serverConnectionHandlerID);
	void WhisperTo(uint64 serverConnectionHandlerID, QList<uint64> targetChannelIDArray, QList<anyID> targetClientIDArray);

	void SetPushToTalk(uint64 serverConnectionHandlerID, bool shouldTalk);
	void Reset(uint64 serverConnectionHandlerID);

	void SetPan(int radio_id, float pan);
	void SetVolumeGain(int radio_id, float gain);

	int GetFrequence(uint64 serverConnectionHandlerID, int radio_id);
	int GetRadioId(uint64 serverConnectionHandlerID, int frequence);
	QList<int> GetRadioIds(uint64 serverConnectionHandlerID, int frequence);

	void Disconnect(bool shutdown);
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
	void ToggleRadio(uint64 serverConnectionHandlerID);
	void ToggleRadio(uint64 serverConnectionHandlerID, int radio_id);
	void FreqUpDown(uint64 serverConnectionHandlerID, QString direction);
	int OnServerErrorEvent(uint64 serverConnectionHandlerID, const char* errorMessage, unsigned int error, const char* returnCode, const char* extraMessage);
	void OnClientMoveTimeoutEvent(uint64 serverConnectionHandlerID, anyID clientID, uint64 oldChannelID, uint64 newChannelID, int visibility, const char* timeoutMessage);

	void SetSoundsEnabled(bool enabled);
	void SetFreqByChannelname(int radio_id);
	bool IsInSameBroadcast(uint64 serverConnectionHandlerID, int frequence);
	bool IsBroadcastFrequence(int frequence);
	void OnUpdateChannelEditedEvent(uint64 serverConnectionHandlerID, uint64 channelID, anyID invokerID, const char* invokerName, const char* invokerUniqueIdentifier);
	void OnClientMoveEvent(uint64 serverConnectionHandlerID, anyID clientID, uint64 oldChannelID, uint64 newChannelID, int visibility, const char* moveMessage);
	void OnClientMoveMovedEvent(uint64 serverConnectionHandlerID, anyID clientID, uint64 oldChannelID, uint64 newChannelID, int visibility, anyID moverID, const char* moverName, const char* moverUniqueIdentifier, const char* moveMessage);
	void SetFreqByChannelname(uint64 serverConnectionHandlerID, anyID clientID, uint64 newChannelID);
	void EnsureStreamerGroup(uint64 serverConnectionHandlerID, anyID clientID, uint64 newChannelID);
	void OnClientChannelGroupChangedEvent(uint64 serverConnectionHandlerID, uint64 channelGroupID, uint64 channelID, anyID clientID, anyID invokerClientID, const char* invokerName, const char* invokerUniqueIdentity);
	void OnChannelGroupListEvent(uint64 serverConnectionHandlerID, uint64 channelGroupID, const char* name, int type, int iconID, int saveDB);
	void OnChannelGroupListFinishedEvent(uint64 serverConnectionHandlerID);

	void ReloadConfig(uint64 serverConnectionHandlerID);
	QList<anyID> RemoveDuplicates(QList<anyID> list);
	QList<uint64> RemoveDuplicates(QList<uint64> list);
	void RemoveInvalidClients(uint64 serverConnectionHandlerID);
	void RequestServerGroupsByClientID(uint64 serverConnectionHandlerID);
	void RequestServerGroupsByClientID(uint64 serverConnectionHandlerID, anyID clientID);
	void RequestServerGroupsByClientID(uint64 serverConnectionHandlerID, uint64 clientDatabaseID);
	void OnServerGroupByClientIDEvent(uint64 serverConnectionHandlerID, const char* name, uint64 serverGroupList, uint64 clientDatabaseID);

	void MuteInStream(uint64 serverConnectionhandlerID, anyID clientID, bool mute);
private:

	QMap<uint64, QMap<int, int>> _activeRadios;
	
	QString _keyword;
	QString _key;
	QWidget* _parent;

	bool _shutdown = false;
	bool _debug = false;
	QMap<uint64, QMap<int, bool>> _isWhispering;
	QMap<uint64, bool> _pttActive;
	QMap<uint64, bool> _vadActive;
	QMap<uint64, bool> _inputActive;
	QMap<uint64, bool> _agcActive;

	QMap<uint64, QMap<int, QList<uint64>>> _targetChannelIDs;
	QMap<uint64, QMap<int, QList<anyID>>>  _targetClientIDs;
	QMap<uint64, QMap<anyID, QString>> _nicknames;

	QMap<uint64, QList<anyID>> _muteInStream;

	float _pans[RADIO_COUNT];
	float _gains[RADIO_COUNT];

	char* _pluginID;
	struct TS3Functions _ts3;
	channels* _channels;
	bool _soundsEnabled = true;
	QString _soundsPath[RADIO_COUNT][4];

	QTimer _doubleClickTimer[RADIO_COUNT];
	int _doubleClickCount[RADIO_COUNT];
	bool _muted[RADIO_COUNT];
	bool _allMuted = false;
	bool _channelMuted = false;
	bool _toggleMute = false;
	bool _toggleMuted[RADIO_COUNT];
	int _setFreqByChannelname = -1;
	bool _toggleRadio = false;
	bool _markAsStreamer = false;
	QMap <uint64, uint64> _streamerChannelGroupID;

	QMetaObject::Connection * _doubleClickConnection[RADIO_COUNT];
};

