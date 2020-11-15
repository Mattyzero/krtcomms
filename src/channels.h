#pragma once
#include <QtWidgets/QDialog>
#pragma comment (lib, "Qt5Widgets")
#pragma comment (lib, "Qt5Core")
#include <QtCore/QSettings>
#include <QtCore/QStringListModel>
#include <QtGui/QCursor>
#include <memory>
#include <QtGui/QFont>
#include <QtCore/QMap>
#include <QtCore/QList>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QListWidgetItem>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QDial>
#include <QtWidgets/QSlider>
#include "QTriangle.h"
#include "QEditableLabel.h"
#include "QEditableLCDNumber.h"
#include "teamspeak/public_definitions.h"
#include "ts3_functions.h"

namespace Ui {
	class channelsui;
}
class channels : public QDialog
{
	Q_OBJECT
public:
	channels(const QString& configLocation, char* pluginID, TS3Functions ts3Functions, QWidget *parent = nullptr);
	~channels();
	channels(const channels& other) = delete;
	channels& operator=(const channels& other) = delete;

	void set(const QString& option, const QVariant& value);
	QVariant get(const QString& option) const;
	QVariant get(const QString& option, const QVariant& defaultValue) const;

	void OnStartup();
	void GetHotkeysFromKeywords();

	void EnableSendLamp(int radio_id);
	void DisableSendLamp(int radio_id);
	void EnableReceiveLamp(int radio_id);
	void DisableReceiveLamp(int radio_id);
	void MuteReceiveLamp(int radio_id);
	void UnMuteReceiveLamp(int radio_id);
	void ChangeChannelMuted(bool checked);
	void SetFrequence(int radio_id, double frequence);
	void ToggleRadio(int radio_id);
	void Load();

	bool onDifferentKey(QString keyword, QString key, QWidget *parent);

protected:
	void showEvent(QShowEvent* e) override;
	void resizeEvent(QResizeEvent* e) override;

private:
	std::unique_ptr<Ui::channelsui> _ui;
	std::unique_ptr<QSettings> _settings;
	QComboBox* _profiles = NULL;
	QString _profile = "";

	char* _pluginID;
	TS3Functions _ts3;
	uint64 _serverConnectionHandlerID;

	QString _oldItemName = "";
	bool _elgatoStreamDeck = false;
	QMap<int, QMap<int, int>> _lampStatus;
	QString _configLocation = "";
	QString _statusLocation = "";

	QList<QCheckBox*> _channels;
	QList<void(channels::*)(int state)> _channelEvents;
	QList<QPushButton*> _channelHotkeys;
	QList<void(channels::*)(bool checked)> _channelHotkeyEvents;

	QList<QEditableLabel*> _lineEdits;
	QList<void(channels::*)()> _editingFinisheds;

	QList<QDial*> _panChannels;
	QList<void(channels::*)(int value)> _panChannelEvents;

	QList<QSlider*> _volumeGains;
	QList<void(channels::*)(int value)> _volumeGainEvents;

	QList<QPushButton*> _setFrequences;
	QList<void(channels::*)(bool checked)> _setFrequenceEvents;

	QList<QTriangle*> _sendLamps;
	QList<QTriangle*> _receiveLamps;

	QList<QEditableLCDNumber*> _frequences;


	

	void save();
	void load();
	void load(bool onStartup);
	void load(bool onStartup, bool reload);

	void uncheckAll();

	void onChange1(int state);
	void onChange2(int state);
	void onChange3(int state);
	void onChange4(int state);

	void onChange5(int state);
	void onChange6(int state);
	void onChange7(int state);
	void onChange8(int state);

	void onClick1(bool checked);
	void onClick2(bool checked);
	void onClick3(bool checked);
	void onClick4(bool checked);

	void onClick5(bool checked);
	void onClick6(bool checked);
	void onClick7(bool checked);
	void onClick8(bool checked);

	void onSetFrequence1(bool checked);
	void onSetFrequence2(bool checked);
	void onSetFrequence3(bool checked);
	void onSetFrequence4(bool checked);

	void onSetFrequence5(bool checked);
	void onSetFrequence6(bool checked);
	void onSetFrequence7(bool checked);
	void onSetFrequence8(bool checked);

	void onPanChanged1(int value);
	void onPanChanged2(int value);
	void onPanChanged3(int value);
	void onPanChanged4(int value);

	void onPanChanged5(int value);
	void onPanChanged6(int value);
	void onPanChanged7(int value);
	void onPanChanged8(int value);

	void onVolumeGainChanged1(int value);
	void onVolumeGainChanged2(int value);
	void onVolumeGainChanged3(int value);
	void onVolumeGainChanged4(int value);

	void onVolumeGainChanged5(int value);
	void onVolumeGainChanged6(int value);
	void onVolumeGainChanged7(int value);
	void onVolumeGainChanged8(int value);

	void onReset(bool checked);
	void onDebug(int state);
	void onAdvanced(int state);

	void onChannelDuckingChanged(int state);
	void onFreq1yz_abDuckingChanged(int state);
	void onFreqxy1_abDuckingChanged(int state);
	void onFreqxyz_1bDuckingChanged(int state);

	void onChannelDuckingSliderChanged(int value);
	void onFreq1yz_abDuckingSliderChanged(int value);
	void onFreqxy1_abDuckingSliderChanged(int value);
	void onFreqxyz_1bDuckingSliderChanged(int value);

	void onRadio1PasswordChanged(QString value);
	void onSetRadio1Password(bool checked);

	void onRadio2PasswordChanged(QString value);
	void onSetRadio2Password(bool checked);

	void onRadio3PasswordChanged(QString value);
	void onSetRadio3Password(bool checked);

	void onRadio4PasswordChanged(QString value);
	void onSetRadio4Password(bool checked);

	void onPushToMuteAllClick(bool checked);
	void onPushToMuteChannelClick(bool checked);
	void onToggleMuteClick(bool checked);
	void onToggleRadioClick(bool checked);

	void onBeepSoundChanged(int state);
	void onActivateRadioOnStartupChanged(int state);
	void onSetFreqByChannelnameChanged(int state);
	void onSetFreqByChannelnameRadioChanged(int index);
	void onElgatoStreamDeckChanged(int state);

	void onProfileChanged(int index);
	void onProfilesListItemActivated(QListWidgetItem *item);
	void onProfilesListCurrentTextChanged(QString text);
	void onProfilesListItemChanged(QListWidgetItem *item);
	void onProfileNewClicked(bool checked);
	void onProfileDeleteClicked(bool checked);
	void onProfileRenameClicked(bool checked);

	void onLineEdit1EditingFinished();
	void onLineEdit2EditingFinished();
	void onLineEdit3EditingFinished();
	void onLineEdit4EditingFinished();

	void onLineEdit5EditingFinished();
	void onLineEdit6EditingFinished();
	void onLineEdit7EditingFinished();
	void onLineEdit8EditingFinished();


	int toInt(double frequence);
	void renameGroup(QString currentName, QString newName);

	void writeStatus();
};

