#include "channels.h"
#include "ui_channels.h"

#include "teamspeak/public_rare_definitions.h"
#include "teamspeak/public_definitions.h"
#include "teamspeak/public_errors.h"
#include "ts3_functions.h"
#include "KRTComms.h"
#include "Ducker.h"
#include "Encrypter.h"

#include <QtGui/QCursor>
#include <QtGui/QPainter>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QListWidget>
#include <QtCore/QTextStream>

channels::channels(const QString& configLocation, char* pluginID, TS3Functions ts3Functions, QWidget* parent /* = nullptr */) : QDialog(parent),
	_ui(std::make_unique<Ui::channelsui>()),
	_settings(std::make_unique<QSettings>(configLocation, QSettings::IniFormat, this)) {

	_settings->setIniCodec("UTF-8");

	_pluginID = pluginID;
	_ts3 = ts3Functions;

	_configLocation = configLocation;
	_statusLocation = QString(configLocation).replace(".cnf", ".status");
	

	_ui->setupUi(this);

	setWindowTitle((QString("KRT Comms Radios v") + KRTComms::version));

	//this->adjustSize();
		
	connect(_ui->buttonBox, &QDialogButtonBox::accepted, this, &channels::save);
	connect(_ui->buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

	connect(_ui->channel_1, &QCheckBox::stateChanged, this, &channels::onChange1);
	connect(_ui->channel_2, &QCheckBox::stateChanged, this, &channels::onChange2);
	connect(_ui->channel_3, &QCheckBox::stateChanged, this, &channels::onChange3);
	connect(_ui->channel_4, &QCheckBox::stateChanged, this, &channels::onChange4);

	connect(_ui->channel_5, &QCheckBox::stateChanged, this, &channels::onChange5);
	connect(_ui->channel_6, &QCheckBox::stateChanged, this, &channels::onChange6);
	connect(_ui->channel_7, &QCheckBox::stateChanged, this, &channels::onChange7);
	connect(_ui->channel_8, &QCheckBox::stateChanged, this, &channels::onChange8);

	connect(_ui->lineedit_1, &QEditableLabel::clicked, _ui->channel_1, &QCheckBox::toggle);
	connect(_ui->lineedit_2, &QEditableLabel::clicked, _ui->channel_2, &QCheckBox::toggle);
	connect(_ui->lineedit_3, &QEditableLabel::clicked, _ui->channel_3, &QCheckBox::toggle);
	connect(_ui->lineedit_4, &QEditableLabel::clicked, _ui->channel_4, &QCheckBox::toggle);

	connect(_ui->lineedit_5, &QEditableLabel::clicked, _ui->channel_5, &QCheckBox::toggle);
	connect(_ui->lineedit_6, &QEditableLabel::clicked, _ui->channel_6, &QCheckBox::toggle);
	connect(_ui->lineedit_7, &QEditableLabel::clicked, _ui->channel_7, &QCheckBox::toggle);
	connect(_ui->lineedit_8, &QEditableLabel::clicked, _ui->channel_8, &QCheckBox::toggle);

	connect(_ui->lineedit_1, &QEditableLabel::editingFinished, this, &channels::onLineEdit1EditingFinished);
	connect(_ui->lineedit_2, &QEditableLabel::editingFinished, this, &channels::onLineEdit2EditingFinished);
	connect(_ui->lineedit_3, &QEditableLabel::editingFinished, this, &channels::onLineEdit3EditingFinished);
	connect(_ui->lineedit_4, &QEditableLabel::editingFinished, this, &channels::onLineEdit4EditingFinished);

	connect(_ui->lineedit_5, &QEditableLabel::editingFinished, this, &channels::onLineEdit5EditingFinished);
	connect(_ui->lineedit_6, &QEditableLabel::editingFinished, this, &channels::onLineEdit6EditingFinished);
	connect(_ui->lineedit_7, &QEditableLabel::editingFinished, this, &channels::onLineEdit7EditingFinished);
	connect(_ui->lineedit_8, &QEditableLabel::editingFinished, this, &channels::onLineEdit8EditingFinished);

	connect(_ui->ch_1_hotkey, &QPushButton::clicked, this, &channels::onClick1);
	connect(_ui->ch_2_hotkey, &QPushButton::clicked, this, &channels::onClick2);
	connect(_ui->ch_3_hotkey, &QPushButton::clicked, this, &channels::onClick3);
	connect(_ui->ch_4_hotkey, &QPushButton::clicked, this, &channels::onClick4);

	connect(_ui->ch_5_hotkey, &QPushButton::clicked, this, &channels::onClick5);
	connect(_ui->ch_6_hotkey, &QPushButton::clicked, this, &channels::onClick6);
	connect(_ui->ch_7_hotkey, &QPushButton::clicked, this, &channels::onClick7);
	connect(_ui->ch_8_hotkey, &QPushButton::clicked, this, &channels::onClick8);

	connect(_ui->pan_ch_1, &QDial::valueChanged, this, &channels::onPanChanged1);
	connect(_ui->pan_ch_2, &QDial::valueChanged, this, &channels::onPanChanged2);
	connect(_ui->pan_ch_3, &QDial::valueChanged, this, &channels::onPanChanged3);
	connect(_ui->pan_ch_4, &QDial::valueChanged, this, &channels::onPanChanged4);

	connect(_ui->pan_ch_5, &QDial::valueChanged, this, &channels::onPanChanged5);
	connect(_ui->pan_ch_6, &QDial::valueChanged, this, &channels::onPanChanged6);
	connect(_ui->pan_ch_7, &QDial::valueChanged, this, &channels::onPanChanged7);
	connect(_ui->pan_ch_8, &QDial::valueChanged, this, &channels::onPanChanged8);

	connect(_ui->volume_gain_ch_1, &QSlider::valueChanged, this, &channels::onVolumeGainChanged1);
	connect(_ui->volume_gain_ch_2, &QSlider::valueChanged, this, &channels::onVolumeGainChanged2);
	connect(_ui->volume_gain_ch_3, &QSlider::valueChanged, this, &channels::onVolumeGainChanged3);
	connect(_ui->volume_gain_ch_4, &QSlider::valueChanged, this, &channels::onVolumeGainChanged4);

	connect(_ui->volume_gain_ch_5, &QSlider::valueChanged, this, &channels::onVolumeGainChanged5);
	connect(_ui->volume_gain_ch_6, &QSlider::valueChanged, this, &channels::onVolumeGainChanged6);
	connect(_ui->volume_gain_ch_7, &QSlider::valueChanged, this, &channels::onVolumeGainChanged7);
	connect(_ui->volume_gain_ch_8, &QSlider::valueChanged, this, &channels::onVolumeGainChanged8);

	connect(_ui->reset, &QPushButton::clicked, this, &channels::onReset);
	connect(_ui->debug, &QCheckBox::stateChanged, this, &channels::onDebug);
	connect(_ui->advanced, &QCheckBox::stateChanged, this, &channels::onAdvanced);

	connect(_ui->set_frequence_1, &QPushButton::clicked, this, &channels::onSetFrequence1);
	connect(_ui->set_frequence_2, &QPushButton::clicked, this, &channels::onSetFrequence2);
	connect(_ui->set_frequence_3, &QPushButton::clicked, this, &channels::onSetFrequence3);
	connect(_ui->set_frequence_4, &QPushButton::clicked, this, &channels::onSetFrequence4);

	connect(_ui->set_frequence_5, &QPushButton::clicked, this, &channels::onSetFrequence5);
	connect(_ui->set_frequence_6, &QPushButton::clicked, this, &channels::onSetFrequence6);
	connect(_ui->set_frequence_7, &QPushButton::clicked, this, &channels::onSetFrequence7);
	connect(_ui->set_frequence_8, &QPushButton::clicked, this, &channels::onSetFrequence8);


	connect(_ui->channel_ducking, &QCheckBox::stateChanged, this, &channels::onChannelDuckingChanged);
	connect(_ui->freq_ducking_1yz_ab, &QCheckBox::stateChanged, this, &channels::onFreq1yz_abDuckingChanged);
	connect(_ui->freq_ducking_xy1_ab, &QCheckBox::stateChanged, this, &channels::onFreqxy1_abDuckingChanged);
	connect(_ui->freq_ducking_xyz_1b, &QCheckBox::stateChanged, this, &channels::onFreqxyz_1bDuckingChanged);

	connect(_ui->channel_ducking_slider, &QSlider::valueChanged, this, &channels::onChannelDuckingSliderChanged);
	connect(_ui->freq_ducking_slider_1yz_ab, &QSlider::valueChanged, this, &channels::onFreq1yz_abDuckingSliderChanged);
	connect(_ui->freq_ducking_slider_xy1_ab, &QSlider::valueChanged, this, &channels::onFreqxy1_abDuckingSliderChanged);
	connect(_ui->freq_ducking_slider_xyz_1b, &QSlider::valueChanged, this, &channels::onFreqxyz_1bDuckingSliderChanged);

	connect(_ui->radio_1_password, &QLineEdit::textChanged, this, &channels::onRadio1PasswordChanged);
	connect(_ui->set_radio_1_password, &QPushButton::clicked, this, &channels::onSetRadio1Password);

	connect(_ui->radio_2_password, &QLineEdit::textChanged, this, &channels::onRadio2PasswordChanged);
	connect(_ui->set_radio_2_password, &QPushButton::clicked, this, &channels::onSetRadio2Password);

	connect(_ui->radio_3_password, &QLineEdit::textChanged, this, &channels::onRadio3PasswordChanged);
	connect(_ui->set_radio_3_password, &QPushButton::clicked, this, &channels::onSetRadio3Password);

	connect(_ui->radio_4_password, &QLineEdit::textChanged, this, &channels::onRadio4PasswordChanged);
	connect(_ui->set_radio_4_password, &QPushButton::clicked, this, &channels::onSetRadio4Password);


	_ui->receive_lamp_1->SetDirection(QTriangle::Direction::BOTTOM);
	_ui->receive_lamp_2->SetDirection(QTriangle::Direction::BOTTOM);
	_ui->receive_lamp_3->SetDirection(QTriangle::Direction::BOTTOM);
	_ui->receive_lamp_4->SetDirection(QTriangle::Direction::BOTTOM);

	_ui->receive_lamp_5->SetDirection(QTriangle::Direction::BOTTOM);
	_ui->receive_lamp_6->SetDirection(QTriangle::Direction::BOTTOM);
	_ui->receive_lamp_7->SetDirection(QTriangle::Direction::BOTTOM);
	_ui->receive_lamp_8->SetDirection(QTriangle::Direction::BOTTOM);

	_ui->radio_5_8->hide();

	connect(_ui->push_to_mute_all, &QPushButton::clicked, this, &channels::onPushToMuteAllClick);
	connect(_ui->push_to_mute_channel, &QPushButton::clicked, this, &channels::onPushToMuteChannelClick);
	connect(_ui->toggle_mute, &QPushButton::clicked, this, &channels::onToggleMuteClick);

	_ui->label_channel_muted->hide();

	connect(_ui->beep_sound, &QCheckBox::stateChanged, this, &channels::onBeepSoundChanged); 
	connect(_ui->activate_radio_on_startup, &QCheckBox::stateChanged, this, &channels::onActivateRadioOnStartupChanged);
	connect(_ui->set_freq_by_channelname, &QCheckBox::stateChanged, this, &channels::onSetFreqByChannelnameChanged);
	connect(_ui->set_freq_by_channelname_radio, QOverload<int>::of(&QComboBox::activated), this, &channels::onSetFreqByChannelnameRadioChanged);
	connect(_ui->elgato_streamdeck, &QCheckBox::stateChanged, this, &channels::onElgatoStreamDeckChanged);


	//connect(_ui->profiles_list, &QListWidget::itemActivated, this, &channels::onProfilesListItemActivated);
	connect(_ui->profiles_list, &QListWidget::currentTextChanged, this, &channels::onProfilesListCurrentTextChanged);
	connect(_ui->profiles_list, &QListWidget::itemChanged, this, &channels::onProfilesListItemChanged);

	connect(_ui->profile_new, &QPushButton::clicked, this, &channels::onProfileNewClicked);
	connect(_ui->profile_delete, &QPushButton::clicked, this, &channels::onProfileDeleteClicked);
	connect(_ui->profile_rename, &QPushButton::clicked, this, &channels::onProfileRenameClicked);

	QStringList groups = _settings->childGroups();

	_ui->profiles_list->addItems(groups);

	_profiles = new QComboBox(this);
	_profiles->addItem("General");
	_profiles->addItems(groups);
	_ui->tabWidget->setCornerWidget(_profiles, Qt::TopRightCorner);

	connect(_profiles, QOverload<int>::of(&QComboBox::activated), this, &channels::onProfileChanged);
}

channels::~channels() {
	_settings->sync();
}

void channels::set(const QString& option, const QVariant& value) {
	_settings->beginGroup(_profile);
	_settings->setValue(option, value);
	_settings->endGroup();
}

QVariant channels::get(const QString& option) const {	
	_settings->beginGroup(_profile);
	QVariant ret = _settings->value(option);
	_settings->endGroup();

	return ret;
}

QVariant channels::get(const QString& option, const QVariant& defaultValue) const {
	_settings->beginGroup(_profile);
	QVariant ret = _settings->value(option, defaultValue);
	_settings->endGroup();

	return ret;
}

void channels::OnStartup() {
	if (get("activate_radio_on_startup").toBool()) {
		//_ui->debug->setChecked("true");
		load(true);
	}
	
	GetHotkeysFromKeywords();
}

void channels::GetHotkeysFromKeywords() {
	const size_t arrayLen = 11;
	const size_t hotkeyBufSize = 50;
	const char* keywords[hotkeyBufSize] = {
		"send_ch_0",
		"send_ch_1",
		"send_ch_2",
		"send_ch_3",
		"send_ch_4",
		"send_ch_5",
		"send_ch_6",
		"send_ch_7",

		"push_to_mute_all",
		"push_to_mute_channel",
		"toggle_mute",
	};

	char** hotkeys = (char**)malloc(sizeof(char*) * arrayLen);
	for (int i = 0; i < arrayLen; i++) {
		hotkeys[i] = (char*)malloc(sizeof(char) * hotkeyBufSize);
	}

	unsigned int result = _ts3.getHotkeyFromKeyword(_pluginID, keywords, hotkeys, arrayLen, hotkeyBufSize);

	if (result == ERROR_ok) {
		QString ch_1_hotkey(hotkeys[0]);
		if (!ch_1_hotkey.isEmpty()) _ui->ch_1_hotkey->setToolTip(ch_1_hotkey);

		QString ch_2_hotkey(hotkeys[1]);
		if (!ch_2_hotkey.isEmpty()) _ui->ch_2_hotkey->setToolTip(ch_2_hotkey);

		QString ch_3_hotkey(hotkeys[2]);
		if (!ch_3_hotkey.isEmpty()) _ui->ch_3_hotkey->setToolTip(ch_3_hotkey);

		QString ch_4_hotkey(hotkeys[3]);
		if (!ch_4_hotkey.isEmpty()) _ui->ch_4_hotkey->setToolTip(ch_4_hotkey);

		QString ch_5_hotkey(hotkeys[4]);
		if (!ch_5_hotkey.isEmpty()) _ui->ch_5_hotkey->setToolTip(ch_5_hotkey);

		QString ch_6_hotkey(hotkeys[5]);
		if (!ch_6_hotkey.isEmpty()) _ui->ch_6_hotkey->setToolTip(ch_6_hotkey);

		QString ch_7_hotkey(hotkeys[6]);
		if (!ch_7_hotkey.isEmpty()) _ui->ch_7_hotkey->setToolTip(ch_7_hotkey);

		QString ch_8_hotkey(hotkeys[7]);
		if (!ch_8_hotkey.isEmpty()) _ui->ch_8_hotkey->setToolTip(ch_8_hotkey);

		QString push_to_mute_all(hotkeys[8]);
		if (!push_to_mute_all.isEmpty()) _ui->push_to_mute_all->setToolTip(push_to_mute_all);

		QString push_to_mute_channel(hotkeys[9]);
		if (!push_to_mute_channel.isEmpty()) _ui->push_to_mute_channel->setToolTip(push_to_mute_channel);

		QString toggle_mute(hotkeys[10]);
		if (!toggle_mute.isEmpty()) _ui->toggle_mute->setToolTip(toggle_mute);
	}

	for (int i = 0; i < arrayLen; i++) {
		//_ts3.printMessageToCurrentTab(hotkeys[i]);
		free(hotkeys[i]);
	}
	free(hotkeys);
}

void channels::EnableSendLamp(int radio_id) {
	switch (radio_id) {
	case 0:
		_ui->send_lamp_1->SetColor(QColor("green"));
		break;
	case 1:
		_ui->send_lamp_2->SetColor(QColor("green"));
		break;
	case 2:
		_ui->send_lamp_3->SetColor(QColor("green"));
		break;	
	case 3:
		_ui->send_lamp_4->SetColor(QColor("green"));
		break;

	case 4:
		_ui->send_lamp_5->SetColor(QColor("green"));
		break;
	case 5:
		_ui->send_lamp_6->SetColor(QColor("green"));
		break;
	case 6:
		_ui->send_lamp_7->SetColor(QColor("green"));
		break;
	case 7:
		_ui->send_lamp_8->SetColor(QColor("green"));
		break;
	}
	if (_elgatoStreamDeck) {
		_lampStatus[radio_id][0] = 1;
		writeStatus();
	}
}

void channels::DisableSendLamp(int radio_id) {
	switch (radio_id) {
	case 0:
		_ui->send_lamp_1->SetColor(QColor("white"));
		break;
	case 1:
		_ui->send_lamp_2->SetColor(QColor("white"));
		break;
	case 2:
		_ui->send_lamp_3->SetColor(QColor("white"));
		break;
	case 3:
		_ui->send_lamp_4->SetColor(QColor("white"));
		break;

	case 4:
		_ui->send_lamp_5->SetColor(QColor("white"));
		break;
	case 5:
		_ui->send_lamp_6->SetColor(QColor("white"));
		break;
	case 6:
		_ui->send_lamp_7->SetColor(QColor("white"));
		break;
	case 7:
		_ui->send_lamp_8->SetColor(QColor("white"));
		break;
	}
	if (_elgatoStreamDeck) {
		_lampStatus[radio_id][0] = 0;
		writeStatus();
	}
}

void channels::EnableReceiveLamp(int radio_id) {
	switch (radio_id) {
	case 0:
		_ui->receive_lamp_1->SetColor(QColor("red"));
		break;
	case 1:
		_ui->receive_lamp_2->SetColor(QColor("red"));
		break;
	case 2:
		_ui->receive_lamp_3->SetColor(QColor("red"));
		break;
	case 3:
		_ui->receive_lamp_4->SetColor(QColor("red"));
		break;

	case 4:
		_ui->receive_lamp_5->SetColor(QColor("red"));
		break;
	case 5:
		_ui->receive_lamp_6->SetColor(QColor("red"));
		break;
	case 6:
		_ui->receive_lamp_7->SetColor(QColor("red"));
		break;
	case 7:
		_ui->receive_lamp_8->SetColor(QColor("red"));
		break;
	}
	if (_elgatoStreamDeck) {
		_lampStatus[radio_id][1] = 1;
		writeStatus();
	}
}

void channels::DisableReceiveLamp(int radio_id) {
	switch (radio_id) {
	case 0:
		_ui->receive_lamp_1->SetColor(QColor("white"));
		break;
	case 1:
		_ui->receive_lamp_2->SetColor(QColor("white"));
		break;
	case 2:
		_ui->receive_lamp_3->SetColor(QColor("white"));
		break;
	case 3:
		_ui->receive_lamp_4->SetColor(QColor("white"));
		break;

	case 4:
		_ui->receive_lamp_5->SetColor(QColor("white"));
		break;
	case 5:
		_ui->receive_lamp_6->SetColor(QColor("white"));
		break;
	case 6:
		_ui->receive_lamp_7->SetColor(QColor("white"));
		break;
	case 7:
		_ui->receive_lamp_8->SetColor(QColor("white"));
		break;
	}
	if (_elgatoStreamDeck) {
		_lampStatus[radio_id][1] = 0;
		writeStatus();
	}
}

void channels::MuteReceiveLamp(int radio_id) {
	switch (radio_id) {
	case 0:
		//_ui->receive_lamp_1->SetColor(QColor("black"));
		_ui->receive_lamp_1->SetDirection(QTriangle::Direction::LEFT);
		break;
	case 1:
		//_ui->receive_lamp_2->SetColor(QColor("black"));
		_ui->receive_lamp_2->SetDirection(QTriangle::Direction::LEFT);
		break;
	case 2:
		//_ui->receive_lamp_3->SetColor(QColor("black"));
		_ui->receive_lamp_3->SetDirection(QTriangle::Direction::LEFT);
		break;
	case 3:
		//_ui->receive_lamp_4->SetColor(QColor("black"));
		_ui->receive_lamp_4->SetDirection(QTriangle::Direction::LEFT);
		break;

	case 4:
		//_ui->receive_lamp_5->SetColor(QColor("black"));
		_ui->receive_lamp_5->SetDirection(QTriangle::Direction::LEFT);
		break;
	case 5:
		//_ui->receive_lamp_6->SetColor(QColor("black"));
		_ui->receive_lamp_6->SetDirection(QTriangle::Direction::LEFT);
		break;
	case 6:
		//_ui->receive_lamp_7->SetColor(QColor("black"));
		_ui->receive_lamp_7->SetDirection(QTriangle::Direction::LEFT);
		break;
	case 7:
		//_ui->receive_lamp_8->SetColor(QColor("black"));
		_ui->receive_lamp_8->SetDirection(QTriangle::Direction::LEFT);
		break;
	}
}

void channels::UnMuteReceiveLamp(int radio_id) {
	switch (radio_id) {
	case 0:
		//_ui->receive_lamp_1->SetColor(QColor("white"));
		_ui->receive_lamp_1->SetDirection(QTriangle::Direction::BOTTOM);
		break;
	case 1:
		//_ui->receive_lamp_2->SetColor(QColor("white"));
		_ui->receive_lamp_2->SetDirection(QTriangle::Direction::BOTTOM);
		break;
	case 2:
		//_ui->receive_lamp_3->SetColor(QColor("white"));
		_ui->receive_lamp_3->SetDirection(QTriangle::Direction::BOTTOM);
		break;
	case 3:
		//_ui->receive_lamp_4->SetColor(QColor("white"));
		_ui->receive_lamp_4->SetDirection(QTriangle::Direction::BOTTOM);
		break;

	case 4:
		//_ui->receive_lamp_5->SetColor(QColor("white"));
		_ui->receive_lamp_5->SetDirection(QTriangle::Direction::BOTTOM);
		break;
	case 5:
		//_ui->receive_lamp_6->SetColor(QColor("white"));
		_ui->receive_lamp_6->SetDirection(QTriangle::Direction::BOTTOM);
		break;
	case 6:
		//_ui->receive_lamp_7->SetColor(QColor("white"));
		_ui->receive_lamp_7->SetDirection(QTriangle::Direction::BOTTOM);
		break;
	case 7:
		//_ui->receive_lamp_8->SetColor(QColor("white"));
		_ui->receive_lamp_8->SetDirection(QTriangle::Direction::BOTTOM);
		break;
	}	
}

void channels::ChangeChannelMuted(bool checked) {
	if (checked) {
		_ui->label_channel_muted->show();
	}
	else {
		_ui->label_channel_muted->hide();
	}
}

void channels::SetFrequence(int radio_id, double frequence) {
	switch (radio_id) {
	case 0:
		_ui->frequence_1->FormatDisplay(frequence);
		onSetFrequence1(true);
		break;
	case 1:
		_ui->frequence_2->FormatDisplay(frequence);
		onSetFrequence2(true);
		break;
	case 2:
		_ui->frequence_3->FormatDisplay(frequence);
		onSetFrequence3(true);
		break;
	case 3:
		_ui->frequence_4->FormatDisplay(frequence);
		onSetFrequence4(true);
		break;

	case 4:
		_ui->frequence_5->FormatDisplay(frequence);
		onSetFrequence5(true);
		break;
	case 5:
		_ui->frequence_6->FormatDisplay(frequence);
		onSetFrequence6(true);
		break;
	case 6:
		_ui->frequence_7->FormatDisplay(frequence);
		onSetFrequence7(true);
		break;
	case 7:
		_ui->frequence_8->FormatDisplay(frequence);
		onSetFrequence8(true);
		break;
	}
}

void channels::Load() {
	load();
}

void channels::showEvent(QShowEvent* /* e */) {
	load();
}

void channels::resizeEvent(QResizeEvent* e) {
	QDialog::resizeEvent(e);
	if (!e->oldSize().isEmpty()) {
		set("win_size", e->size());
	}
}

void channels::save() {

	close();
}

void channels::load() {
	load(false);
}

void channels::load(bool onStartup) {	
	_serverConnectionHandlerID = _ts3.getCurrentServerConnectionHandlerID();
	if (_serverConnectionHandlerID <= 0) return;
	
	resize(get("win_size").toSize());
	//TODO pointer array zu jeweiligen variablen damit das _Zahl ding nicht ewig weiter geht
	//Falls jemand weis wie man das easy umsetzen kann bitte mich kontaktieren oder ein PR DANKE
	
	_ui->frequence_1->FormatDisplay(get("freq_1").toDouble());
	_ui->frequence_2->FormatDisplay(get("freq_2").toDouble());
	_ui->frequence_3->FormatDisplay(get("freq_3").toDouble());
	_ui->frequence_4->FormatDisplay(get("freq_4").toDouble());

	_ui->frequence_5->FormatDisplay(get("freq_5").toDouble());
	_ui->frequence_6->FormatDisplay(get("freq_6").toDouble());
	_ui->frequence_7->FormatDisplay(get("freq_7").toDouble());
	_ui->frequence_8->FormatDisplay(get("freq_8").toDouble());

	_ui->pan_ch_1->setValue(get("pan_1").toInt());
	_ui->pan_ch_2->setValue(get("pan_2").toInt());
	_ui->pan_ch_3->setValue(get("pan_3").toInt());
	_ui->pan_ch_4->setValue(get("pan_4").toInt());

	_ui->pan_ch_5->setValue(get("pan_5").toInt());
	_ui->pan_ch_6->setValue(get("pan_6").toInt());
	_ui->pan_ch_7->setValue(get("pan_7").toInt());
	_ui->pan_ch_8->setValue(get("pan_8").toInt());

	_ui->volume_gain_ch_1->setValue(get("gain_1").toInt());
	_ui->volume_gain_ch_2->setValue(get("gain_2").toInt());
	_ui->volume_gain_ch_3->setValue(get("gain_3").toInt());
	_ui->volume_gain_ch_4->setValue(get("gain_4").toInt());

	_ui->volume_gain_ch_5->setValue(get("gain_5").toInt());
	_ui->volume_gain_ch_6->setValue(get("gain_6").toInt());
	_ui->volume_gain_ch_7->setValue(get("gain_7").toInt());
	_ui->volume_gain_ch_8->setValue(get("gain_8").toInt());

	_ui->advanced->setChecked(get("advanced").toBool());
	
	_ui->channel_ducking->setChecked(get("duck_channel").toBool());
	_ui->freq_ducking_1yz_ab->setChecked(get("duck_freq_1yz_ab").toBool());
	_ui->freq_ducking_xy1_ab->setChecked(get("duck_freq_xy1_ab").toBool());
	_ui->freq_ducking_xyz_1b->setChecked(get("duck_freq_xyz_1b").toBool());

	_ui->channel_ducking_slider->setValue(get("channel_ducking").toInt());
	_ui->freq_ducking_slider_1yz_ab->setValue(get("freq_1yz_ab_ducking").toInt());
	_ui->freq_ducking_slider_xy1_ab->setValue(get("freq_xy1_ab_ducking").toInt());
	_ui->freq_ducking_slider_xyz_1b->setValue(get("freq_xyz_1b_ducking").toInt());

	_ui->radio_1_password->setText(get("radio_1_password").toString());
	Encrypter::SetPassword(0, get("radio_1_password").toString());
	_ui->lineedit_1->setLocked(!get("radio_1_password").toString().isEmpty());

	_ui->radio_2_password->setText(get("radio_2_password").toString());
	Encrypter::SetPassword(1, get("radio_2_password").toString());
	_ui->lineedit_2->setLocked(!get("radio_2_password").toString().isEmpty());

	_ui->radio_3_password->setText(get("radio_3_password").toString());
	Encrypter::SetPassword(2, get("radio_3_password").toString());
	_ui->lineedit_3->setLocked(!get("radio_3_password").toString().isEmpty());

	_ui->radio_4_password->setText(get("radio_4_password").toString());
	Encrypter::SetPassword(3, get("radio_4_password").toString());
	_ui->lineedit_4->setLocked(!get("radio_4_password").toString().isEmpty());

	_ui->beep_sound->setChecked(get("beep_sound", true).toBool());
	_ui->activate_radio_on_startup->setChecked(get("activate_radio_on_startup", false).toBool());
	_ui->set_freq_by_channelname->setChecked(get("set_freq_by_channelname", false).toBool());
	_ui->elgato_streamdeck->setChecked(get("elgato_streamdeck", false).toBool());

	

	if (onStartup) {
		bool radio_on = false;
		radio_on = get("radio_1").toBool();
		if (_ui->channel_1->isChecked() && radio_on) {
			onChange1(get("radio_1").toBool() ? 2 : 0);
		}
		else {
			_ui->channel_1->setChecked(radio_on);
		}
		

		radio_on = get("radio_2").toBool();
		if (_ui->channel_2->isChecked() && radio_on) {
			onChange2(2);
		}
		else {
			_ui->channel_2->setChecked(radio_on);
		}

		radio_on = get("radio_3").toBool();
		if (_ui->channel_3->isChecked() && radio_on) {
			onChange3(2);
		}
		else {
			_ui->channel_3->setChecked(radio_on);
		}

		radio_on = get("radio_4").toBool();
		if (_ui->channel_4->isChecked() && radio_on) {
			onChange4(2);
		}
		else {
			_ui->channel_4->setChecked(radio_on);
		}

		radio_on = get("radio_5").toBool();
		if (_ui->channel_5->isChecked() && radio_on) {
			onChange5(2);
		}
		else {
			_ui->channel_5->setChecked(radio_on);
		}

		radio_on = get("radio_6").toBool();
		if (_ui->channel_6->isChecked() && radio_on) {
			onChange6(2);
		}
		else {
			_ui->channel_6->setChecked(radio_on);
		}

		radio_on = get("radio_7").toBool();
		if (_ui->channel_7->isChecked() && radio_on) {
			onChange7(2);
		}
		else {
			_ui->channel_7->setChecked(radio_on);
		}

		radio_on = get("radio_8").toBool();
		if (_ui->channel_8->isChecked() && radio_on) {
			onChange8(2);
		}
		else {
			_ui->channel_8->setChecked(radio_on);
		}
		
	}
	else {
		_ui->channel_1->setChecked(KRTComms::getInstance().ActiveInRadio(_serverConnectionHandlerID, 0));
		_ui->channel_2->setChecked(KRTComms::getInstance().ActiveInRadio(_serverConnectionHandlerID, 1));
		_ui->channel_3->setChecked(KRTComms::getInstance().ActiveInRadio(_serverConnectionHandlerID, 2));
		_ui->channel_4->setChecked(KRTComms::getInstance().ActiveInRadio(_serverConnectionHandlerID, 3));

		_ui->channel_5->setChecked(KRTComms::getInstance().ActiveInRadio(_serverConnectionHandlerID, 4));
		_ui->channel_6->setChecked(KRTComms::getInstance().ActiveInRadio(_serverConnectionHandlerID, 5));
		_ui->channel_7->setChecked(KRTComms::getInstance().ActiveInRadio(_serverConnectionHandlerID, 6));
		_ui->channel_8->setChecked(KRTComms::getInstance().ActiveInRadio(_serverConnectionHandlerID, 7));
	}

	_ui->lineedit_1->setText(get("channel_1_text", "Radio 1").toString());
	_ui->lineedit_2->setText(get("channel_2_text", "Radio 2").toString());
	_ui->lineedit_3->setText(get("channel_3_text", "Radio 3").toString());
	_ui->lineedit_4->setText(get("channel_4_text", "Radio 4").toString());
	_ui->lineedit_5->setText(get("channel_5_text", "Radio 5").toString());
	_ui->lineedit_6->setText(get("channel_6_text", "Radio 6").toString());
	_ui->lineedit_7->setText(get("channel_7_text", "Radio 7").toString());
	_ui->lineedit_8->setText(get("channel_8_text", "Radio 8").toString());

	_ui->lineedit_1->resizeToContent();
	_ui->lineedit_2->resizeToContent();
	_ui->lineedit_3->resizeToContent();
	_ui->lineedit_4->resizeToContent();
	_ui->lineedit_5->resizeToContent();
	_ui->lineedit_6->resizeToContent();
	_ui->lineedit_7->resizeToContent();
	_ui->lineedit_8->resizeToContent();

	_ui->set_freq_by_channelname_radio->clear();

	for (int i = 0; i < RADIO_COUNT; i++) {
		_ui->set_freq_by_channelname_radio->addItem(get("channel_" + QString::number(i + 1) + "_text", "Radio " + QString::number(i + 1)).toString());
	}

	_ui->set_freq_by_channelname_radio->setCurrentIndex(get("set_freq_by_channelname_radio", 2).toInt());

	_ui->buttonBox->button(QDialogButtonBox::Ok)->setDefault(false);
	_ui->buttonBox->button(QDialogButtonBox::Ok)->setAutoDefault(false);
}

void channels::onChange1(int state) {
	KRTComms::getInstance().SetActiveRadio(_serverConnectionHandlerID, 0, state == 2, toInt(_ui->frequence_1->value()));
	DisableSendLamp(0);
	DisableReceiveLamp(0);
	set("radio_1", state == 2);
}

void channels::onChange2(int state) {
	KRTComms::getInstance().SetActiveRadio(_serverConnectionHandlerID, 1, state == 2, toInt(_ui->frequence_2->value()));
	DisableSendLamp(1);
	DisableReceiveLamp(1);
	set("radio_2", state == 2);
}

void channels::onChange3(int state) {
	KRTComms::getInstance().SetActiveRadio(_serverConnectionHandlerID, 2, state == 2, toInt(_ui->frequence_3->value()));
	DisableSendLamp(2);
	DisableReceiveLamp(2);
	set("radio_3", state == 2);
}

void channels::onChange4(int state) {
	KRTComms::getInstance().SetActiveRadio(_serverConnectionHandlerID, 3, state == 2, toInt(_ui->frequence_4->value()));
	DisableSendLamp(3);
	DisableReceiveLamp(3);
	set("radio_4", state == 2);
}

void channels::onChange5(int state) {
	KRTComms::getInstance().SetActiveRadio(_serverConnectionHandlerID, 4, state == 2, toInt(_ui->frequence_5->value()));
	DisableSendLamp(4);
	DisableReceiveLamp(4);
	set("radio_5", state == 2);
}

void channels::onChange6(int state) {
	KRTComms::getInstance().SetActiveRadio(_serverConnectionHandlerID, 5, state == 2, toInt(_ui->frequence_6->value()));
	DisableSendLamp(5);
	DisableReceiveLamp(5);
	set("radio_6", state == 2);
}

void channels::onChange7(int state) {
	KRTComms::getInstance().SetActiveRadio(_serverConnectionHandlerID, 6, state == 2, toInt(_ui->frequence_7->value()));
	DisableSendLamp(6);
	DisableReceiveLamp(6);
	set("radio_7", state == 2);
}

void channels::onChange8(int state) {
	KRTComms::getInstance().SetActiveRadio(_serverConnectionHandlerID, 7, state == 2, toInt(_ui->frequence_8->value()));
	DisableSendLamp(7);
	DisableReceiveLamp(7);
	set("radio_8", state == 2);
}

void channels::onClick1(bool checked) {
	KRTComms::getInstance().RequestHotkeyInputDialog(0, this);
}

void channels::onClick2(bool checked) {
	KRTComms::getInstance().RequestHotkeyInputDialog(1, this);
}

void channels::onClick3(bool checked) {
	KRTComms::getInstance().RequestHotkeyInputDialog(2, this);
}

void channels::onClick4(bool checked) {
	KRTComms::getInstance().RequestHotkeyInputDialog(3, this);
}

void channels::onClick5(bool checked) {
	KRTComms::getInstance().RequestHotkeyInputDialog(4, this);
}

void channels::onClick6(bool checked) {
	KRTComms::getInstance().RequestHotkeyInputDialog(5, this);
}

void channels::onClick7(bool checked) {
	KRTComms::getInstance().RequestHotkeyInputDialog(6, this);
}

void channels::onClick8(bool checked) {
	KRTComms::getInstance().RequestHotkeyInputDialog(7, this);
}

void channels::onSetFrequence1(bool checked) {
	if (_ui->channel_1->isChecked()) {
		KRTComms::getInstance().SetActiveRadio(_serverConnectionHandlerID, 0, true, toInt(_ui->frequence_1->value()));
	}
	else {
		_ui->channel_1->setChecked(true);
	}
	_ui->frequence_1->Deselect();
	set("freq_1", _ui->frequence_1->value());
}

void channels::onSetFrequence2(bool checked) {
	if (_ui->channel_2->isChecked()) {
		KRTComms::getInstance().SetActiveRadio(_serverConnectionHandlerID, 1, true, toInt(_ui->frequence_2->value()));
	}
	else {
		_ui->channel_2->setChecked(true);
	}
	_ui->frequence_2->Deselect();
	set("freq_2", _ui->frequence_2->value());
}

void channels::onSetFrequence3(bool checked) {	
	if (_ui->channel_3->isChecked()) {
		KRTComms::getInstance().SetActiveRadio(_serverConnectionHandlerID, 2, true, toInt(_ui->frequence_3->value()));
	}
	else {
		_ui->channel_3->setChecked(true);
	}
	_ui->frequence_3->Deselect();
	set("freq_3", _ui->frequence_3->value());
}

void channels::onSetFrequence4(bool checked) {	
	if (_ui->channel_4->isChecked()) {
		KRTComms::getInstance().SetActiveRadio(_serverConnectionHandlerID, 3, true, toInt(_ui->frequence_4->value()));
	}
	else {
		_ui->channel_4->setChecked(true);
	}
	_ui->frequence_4->Deselect();
	set("freq_4", _ui->frequence_4->value());
}

void channels::onSetFrequence5(bool checked) {
	if (_ui->channel_5->isChecked()) {
		KRTComms::getInstance().SetActiveRadio(_serverConnectionHandlerID, 4, true, toInt(_ui->frequence_5->value()));
	}
	else {
		_ui->channel_5->setChecked(true);
	}
	_ui->frequence_5->Deselect();
	set("freq_5", _ui->frequence_5->value());
}

void channels::onSetFrequence6(bool checked) {	
	if (_ui->channel_6->isChecked()) {
		KRTComms::getInstance().SetActiveRadio(_serverConnectionHandlerID, 5, true, toInt(_ui->frequence_6->value()));
	}
	else {
		_ui->channel_6->setChecked(true);
	}
	_ui->frequence_6->Deselect();
	set("freq_6", _ui->frequence_6->value());
}

void channels::onSetFrequence7(bool checked) {	
	if (_ui->channel_7->isChecked()) {
		KRTComms::getInstance().SetActiveRadio(_serverConnectionHandlerID, 6, true, toInt(_ui->frequence_7->value()));
	}
	else {
		_ui->channel_7->setChecked(true);
	}
	_ui->frequence_7->Deselect();
	set("freq_7", _ui->frequence_7->value());
}

void channels::onSetFrequence8(bool checked) {	
	if (_ui->channel_8->isChecked()) {
		KRTComms::getInstance().SetActiveRadio(_serverConnectionHandlerID, 7, true, toInt(_ui->frequence_8->value()));
	}
	else {
		_ui->channel_8->setChecked(true);
	}
	_ui->frequence_8->Deselect();
	set("freq_8", _ui->frequence_8->value());
}

void channels::onPanChanged1(int value) {
	KRTComms::getInstance().SetPan(0, value / 10.0f);
	set("pan_1", value);
}

void channels::onPanChanged2(int value) {
	KRTComms::getInstance().SetPan(1, value / 10.0f);
	set("pan_2", value);
}

void channels::onPanChanged3(int value) {
	KRTComms::getInstance().SetPan(2, value / 10.0f);
	set("pan_3", value);
}

void channels::onPanChanged4(int value) {
	KRTComms::getInstance().SetPan(3, value / 10.0f);
	set("pan_4", value);
}

void channels::onPanChanged5(int value) {
	KRTComms::getInstance().SetPan(4, value / 10.0f);
	set("pan_5", value);
}

void channels::onPanChanged6(int value) {
	KRTComms::getInstance().SetPan(5, value / 10.0f);
	set("pan_6", value);
}

void channels::onPanChanged7(int value) {
	KRTComms::getInstance().SetPan(6, value / 10.0f);
	set("pan_7", value);
}

void channels::onPanChanged8(int value) {
	KRTComms::getInstance().SetPan(7, value / 10.0f);
	set("pan_8", value);
}

void channels::onVolumeGainChanged1(int value) {
	KRTComms::getInstance().SetVolumeGain(0, value / 10.0f);
	set("gain_1", value);
}

void channels::onVolumeGainChanged2(int value) {
	KRTComms::getInstance().SetVolumeGain(1, value / 10.0f);
	set("gain_2", value);
}

void channels::onVolumeGainChanged3(int value) {
	KRTComms::getInstance().SetVolumeGain(2, value / 10.0f);
	set("gain_3", value);
}

void channels::onVolumeGainChanged4(int value) {
	KRTComms::getInstance().SetVolumeGain(3, value / 10.0f);
	set("gain_4", value);
}

void channels::onVolumeGainChanged5(int value) {
	KRTComms::getInstance().SetVolumeGain(4, value / 10.0f);
	set("gain_5", value);
}

void channels::onVolumeGainChanged6(int value) {
	KRTComms::getInstance().SetVolumeGain(5, value / 10.0f);
	set("gain_6", value);
}

void channels::onVolumeGainChanged7(int value) {
	KRTComms::getInstance().SetVolumeGain(6, value / 10.0f);
	set("gain_7", value);
}

void channels::onVolumeGainChanged8(int value) {
	KRTComms::getInstance().SetVolumeGain(7, value / 10.0f);
	set("gain_8", value);
}

void channels::onReset(bool checked) {
	//DEBUG CRASH
	//*((unsigned int*)0) = 0xDEAD;
	//END
	KRTComms::getInstance().Reset(_serverConnectionHandlerID);
}

void channels::onDebug(int state) {
	KRTComms::getInstance().SetDebug(state == 2);
}

void channels::onAdvanced(int state) {
	if (state != 0) {
		_ui->radio_5_8->show();
	}
	else {
		_ui->radio_5_8->hide();
	}

	set("advanced", state == 2);
}

void channels::onChannelDuckingChanged(int state) {
	Ducker::getInstance().SetEnabled(Ducker::Type::CHANNEL, state == 2);
	set("duck_channel", state == 2);
}

void channels::onFreq1yz_abDuckingChanged(int state) {
	Ducker::getInstance().SetEnabled(Ducker::Type::FREQ_1YZ_AB, state == 2);
	set("duck_freq_1yz_ab", state == 2);
}

void channels::onFreqxy1_abDuckingChanged(int state) {
	Ducker::getInstance().SetEnabled(Ducker::Type::FREQ_XY1_AB, state == 2);
	set("duck_freq_xy1_ab", state == 2);
}

void channels::onFreqxyz_1bDuckingChanged(int state) {
	Ducker::getInstance().SetEnabled(Ducker::Type::FREQ_XYZ_1B, state == 2);
	set("duck_freq_xyz_1b", state == 2);
}

void channels::onChannelDuckingSliderChanged(int value) {
	Ducker::getInstance().SetGain(Ducker::Type::CHANNEL, value / 100.0f);
	set("channel_ducking", value);
}

void channels::onFreq1yz_abDuckingSliderChanged(int value) {
	Ducker::getInstance().SetGain(Ducker::Type::FREQ_1YZ_AB, value / 100.0f);
	set("freq_1yz_ab_ducking", value);
}

void channels::onFreqxy1_abDuckingSliderChanged(int value) {
	Ducker::getInstance().SetGain(Ducker::Type::FREQ_XY1_AB, value / 100.0f);
	set("freq_xy1_ab_ducking", value);
}

void channels::onFreqxyz_1bDuckingSliderChanged(int value) {
	Ducker::getInstance().SetGain(Ducker::Type::FREQ_XYZ_1B, value / 100.0f);
	set("freq_xyz_1b_ducking", value);
}

void channels::onRadio1PasswordChanged(QString value) {
	
	set("radio_1_password", value);
}

void channels::onSetRadio1Password(bool checked) {
	KRTComms::getInstance().SetActiveRadio(_serverConnectionHandlerID, 0, false, -1);
	Encrypter::SetPassword(0, get("radio_1_password").toString());
	_ui->lineedit_1->setLocked(!get("radio_1_password").toString().isEmpty());
	onSetFrequence1(true);
}

void channels::onRadio2PasswordChanged(QString value) {

	set("radio_2_password", value);
}

void channels::onSetRadio2Password(bool checked) {
	KRTComms::getInstance().SetActiveRadio(_serverConnectionHandlerID, 1, false, -1);
	Encrypter::SetPassword(1, get("radio_2_password").toString());
	_ui->lineedit_2->setLocked(!get("radio_2_password").toString().isEmpty());
	onSetFrequence2(true);
}

void channels::onRadio3PasswordChanged(QString value) {

	set("radio_3_password", value);
}

void channels::onSetRadio3Password(bool checked) {
	KRTComms::getInstance().SetActiveRadio(_serverConnectionHandlerID, 2, false, -1);
	Encrypter::SetPassword(2, get("radio_3_password").toString());
	_ui->lineedit_3->setLocked(!get("radio_3_password").toString().isEmpty());
	onSetFrequence3(true);
}

void channels::onRadio4PasswordChanged(QString value) {

	set("radio_4_password", value);
}

void channels::onSetRadio4Password(bool checked) {
	KRTComms::getInstance().SetActiveRadio(_serverConnectionHandlerID, 3, false, -1);
	Encrypter::SetPassword(3, get("radio_4_password").toString());
	_ui->lineedit_4->setLocked(!get("radio_4_password").toString().isEmpty());
	onSetFrequence4(true);
}

bool channels::onDifferentKey(QString keyword, QString key, QWidget *parent) {
	QMessageBox msgBox;
	msgBox.setText("Unterschiedliche Hotkeys erkannt!");
	msgBox.setInformativeText(QLatin1String("Bitte richte deine Tastenbelegung nach dem Drücken auf \"OK\" erneut ein."));
	msgBox.setStandardButtons(QMessageBox::Ok);
	msgBox.setDefaultButton(QMessageBox::Ok);
	int ret = msgBox.exec();

	switch (ret) {
	case QMessageBox::Ok:
		KRTComms::getInstance().RequestHotkeyInputDialog(keyword, parent);
		return true; //lass ich daweil so falls ich mich umentscheide und doch wieder 2 Knöpfe einbau
		break;
	}

	return false;
}

void channels::onPushToMuteAllClick(bool checked) {
	KRTComms::getInstance().RequestHotkeyInputDialog("push_to_mute_all", this);
}

void channels::onPushToMuteChannelClick(bool checked) {
	KRTComms::getInstance().RequestHotkeyInputDialog("push_to_mute_channel", this);
}

void channels::onToggleMuteClick(bool checked) {
	KRTComms::getInstance().RequestHotkeyInputDialog("toggle_mute", this);
}

void channels::onBeepSoundChanged(int state) {
	KRTComms::getInstance().SetSoundsEnabled(state == 2);
	set("beep_sound", state == 2);
}

void channels::onActivateRadioOnStartupChanged(int state) {
	set("activate_radio_on_startup", state == 2);
}

void channels::onSetFreqByChannelnameChanged(int state) {
	KRTComms::getInstance().SetFreqByChannelname(state == 2 ? get("set_freq_by_channelname_radio", 2).toInt() : -1);
	set("set_freq_by_channelname", state == 2);
}

void channels::onSetFreqByChannelnameRadioChanged(int index) {
	set("set_freq_by_channelname_radio", index);
	onSetFreqByChannelnameChanged(_ui->set_freq_by_channelname->isChecked() ? 2 : 0);
}

void channels::onElgatoStreamDeckChanged(int state) {
	set("elgato_streamdeck", state == 2);
	_elgatoStreamDeck = state == 2;

	//debug mode
	if (_ui->debug->isChecked() && state == 2) {
		_ts3.printMessageToCurrentTab(_configLocation.toStdString().c_str());
	}
}

void channels::onProfileChanged(int index) {
	_profile = _profiles->currentText();
	if (_profile == "General") _profile = "";
	load();
}

void channels::onProfilesListItemActivated(QListWidgetItem *item) {

}

void channels::onProfilesListCurrentTextChanged(QString text) {
	
}

void channels::onProfilesListItemChanged(QListWidgetItem *item) {
	if (_oldItemName == item->text() || _oldItemName.isEmpty()) return;

	renameGroup(_oldItemName, item->text());

	int index = _profiles->findText(_oldItemName);
	if (index == -1) {
		_profiles->addItem(item->text());
	}
	else {
		_profiles->setItemText(index, item->text());
	}
	

	_oldItemName = "";
}

void channels::onProfileNewClicked(bool checked) {
	QList<QListWidgetItem *> found = _ui->profiles_list->findItems("New Profile", Qt::MatchExactly);
	if (found.length() > 0) return;

	_ui->profiles_list->addItem("New Profile");
	_settings->beginGroup("New Profile");
	_settings->setValue("freq_1", 1);
	_settings->endGroup();
}

void channels::onProfileDeleteClicked(bool checked) {
	QListWidgetItem *item = _ui->profiles_list->currentItem();
	if (item == nullptr) return;

	_settings->remove(item->text());
	_profiles->removeItem(_profiles->findText(item->text()));

	_ui->profiles_list->takeItem(_ui->profiles_list->row(item));
}

void channels::onProfileRenameClicked(bool checked) {
	QListWidgetItem *item = _ui->profiles_list->currentItem();
	item->setFlags(item->flags() | Qt::ItemIsEditable);
	_oldItemName = item->text();
	_ui->profiles_list->editItem(item);

	
}

void channels::onLineEdit1EditingFinished() {
	set("channel_1_text", _ui->lineedit_1->text());
}

void channels::onLineEdit2EditingFinished() {
	set("channel_2_text", _ui->lineedit_2->text());
}

void channels::onLineEdit3EditingFinished() {
	set("channel_3_text", _ui->lineedit_3->text());
}

void channels::onLineEdit4EditingFinished() {
	set("channel_4_text", _ui->lineedit_4->text());
}

void channels::onLineEdit5EditingFinished() {
	set("channel_5_text", _ui->lineedit_5->text());
}

void channels::onLineEdit6EditingFinished() {
	set("channel_6_text", _ui->lineedit_6->text());
}

void channels::onLineEdit7EditingFinished() {
	set("channel_7_text", _ui->lineedit_7->text());
}

void channels::onLineEdit8EditingFinished() {
	set("channel_8_text", _ui->lineedit_8->text());
}


int channels::toInt(double frequence) {
	return frequence * 100.0 + 0.1; //floating-point fix
}

void channels::renameGroup(QString currentName, QString newName) {

	//QString logmessage = "Try to rename Profile from " + currentName + " to " + newName;
	//_ts3.printMessageToCurrentTab(logmessage.toStdString().c_str());

	QHash<QString, QVariant> hash;
	_settings->beginGroup(currentName);
	foreach(const QString &key, _settings->childKeys()) {
		hash[key] = _settings->value(key);
	}
	_settings->endGroup();

	//_ts3.printMessageToCurrentTab(QString::number(hash.size()).toStdString().c_str());

	_settings->beginGroup(newName);
	foreach(const QString &key, hash.keys()) {
		_settings->setValue(key, hash[key]);
	}
	_settings->endGroup();

	_settings->remove(currentName);
}

void channels::writeStatus() {	
	QFile file(_statusLocation);
	if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
		QTextStream stream(&file);

		foreach(int radio_id, _lampStatus.keys()) {
			stream << "r" << (radio_id+1) << ":" << _lampStatus[radio_id][0] << ":" << _lampStatus[radio_id][1] << "\n";
		}

		file.close();
	}
}

