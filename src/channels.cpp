#include "channels.h"
#include "ui_channels.h"

#include "teamspeak/public_rare_definitions.h"
#include "teamspeak/public_definitions.h"
#include "ts3_functions.h"
#include "KRTComms.h"
#include "Ducker.h"

#include <QtGui/QCursor>

channels::channels(const QString& configLocation, char* pluginID, TS3Functions ts3Functions, QWidget* parent /* = nullptr */) : QDialog(parent),
	_ui(std::make_unique<Ui::channelsui>()),
	_settings(std::make_unique<QSettings>(configLocation, QSettings::IniFormat, this)) {

	_pluginID = pluginID;
	_ts3 = ts3Functions;
	

	_ui->setupUi(this);

	setWindowTitle("KRT Comms Radios");

	this->adjustSize();

	connect(_ui->buttonBox, &QDialogButtonBox::accepted, this, &channels::save);
	connect(_ui->buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

	connect(_ui->channel_1, &QCheckBox::stateChanged, this, &channels::onChange1);
	connect(_ui->channel_2, &QCheckBox::stateChanged, this, &channels::onChange2);
	connect(_ui->channel_3, &QCheckBox::stateChanged, this, &channels::onChange3);
	connect(_ui->channel_4, &QCheckBox::stateChanged, this, &channels::onChange4);

	connect(_ui->ch_1_hotkey, &QPushButton::clicked, this, &channels::onClick1);
	connect(_ui->ch_2_hotkey, &QPushButton::clicked, this, &channels::onClick2);
	connect(_ui->ch_3_hotkey, &QPushButton::clicked, this, &channels::onClick3);
	connect(_ui->ch_4_hotkey, &QPushButton::clicked, this, &channels::onClick4);

	connect(_ui->pan_ch_1, &QDial::valueChanged, this, &channels::onPanChanged1);
	connect(_ui->pan_ch_2, &QDial::valueChanged, this, &channels::onPanChanged2);
	connect(_ui->pan_ch_3, &QDial::valueChanged, this, &channels::onPanChanged3);
	connect(_ui->pan_ch_4, &QDial::valueChanged, this, &channels::onPanChanged4);

	connect(_ui->volume_gain_ch_1, &QDial::valueChanged, this, &channels::onVolumeGainChanged1);
	connect(_ui->volume_gain_ch_2, &QDial::valueChanged, this, &channels::onVolumeGainChanged2);
	connect(_ui->volume_gain_ch_3, &QDial::valueChanged, this, &channels::onVolumeGainChanged3);
	connect(_ui->volume_gain_ch_4, &QDial::valueChanged, this, &channels::onVolumeGainChanged4);

	connect(_ui->reset, &QPushButton::clicked, this, &channels::onReset);
	connect(_ui->debug, &QCheckBox::stateChanged, this, &channels::onDebug);
	connect(_ui->duck_channel, &QCheckBox::stateChanged, this, &channels::onDuckChannel);

	connect(_ui->set_frequence_1, &QPushButton::clicked, this, &channels::onSetFrequence1);
	connect(_ui->set_frequence_2, &QPushButton::clicked, this, &channels::onSetFrequence2);
	connect(_ui->set_frequence_3, &QPushButton::clicked, this, &channels::onSetFrequence3);
	connect(_ui->set_frequence_4, &QPushButton::clicked, this, &channels::onSetFrequence4);
}

channels::~channels() {
	_settings->sync();
}

void channels::set(const QString& option, const QVariant& value) {
	_settings->setValue(option, value);
}

QVariant channels::get(const QString& option) const {
	return _settings->value(option);
}

void channels::showEvent(QShowEvent* /* e */) {
	load();
}

void channels::save() {
	set("freq_1", _ui->frequence_1->value());
	set("freq_2", _ui->frequence_2->value());
	set("freq_3", _ui->frequence_3->value());
	set("freq_4", _ui->frequence_4->value());

	close();
}

void channels::load() {
	_serverConnectionHandlerID = _ts3.getCurrentServerConnectionHandlerID();

	_ui->channel_1->setChecked(KRTComms::getInstance().ActiveInRadio(_serverConnectionHandlerID, 0));
	_ui->channel_2->setChecked(KRTComms::getInstance().ActiveInRadio(_serverConnectionHandlerID, 1));
	_ui->channel_3->setChecked(KRTComms::getInstance().ActiveInRadio(_serverConnectionHandlerID, 2));
	_ui->channel_4->setChecked(KRTComms::getInstance().ActiveInRadio(_serverConnectionHandlerID, 3));

	_ui->frequence_1->FormatDisplay(get("freq_1").toDouble());
	_ui->frequence_2->FormatDisplay(get("freq_2").toDouble());
	_ui->frequence_3->FormatDisplay(get("freq_3").toDouble());
	_ui->frequence_4->FormatDisplay(get("freq_4").toDouble());

	_ui->pan_ch_1->setValue(get("pan_1").toInt());
	_ui->pan_ch_2->setValue(get("pan_2").toInt());
	_ui->pan_ch_3->setValue(get("pan_3").toInt());
	_ui->pan_ch_4->setValue(get("pan_4").toInt());

	_ui->volume_gain_ch_1->setValue(get("gain_1").toInt());
	_ui->volume_gain_ch_2->setValue(get("gain_2").toInt());
	_ui->volume_gain_ch_3->setValue(get("gain_3").toInt());
	_ui->volume_gain_ch_4->setValue(get("gain_4").toInt());

	_ui->duck_channel->setChecked(get("duck_channel").toBool());
}

void channels::onChange1(int state) {
	KRTComms::getInstance().SetActiveRadio(_serverConnectionHandlerID, 0, state == 2, (int)(_ui->frequence_1->value() * 100));
}

void channels::onChange2(int state) {
	KRTComms::getInstance().SetActiveRadio(_serverConnectionHandlerID, 1, state == 2, (int)(_ui->frequence_2->value() * 100));
}

void channels::onChange3(int state) {
	KRTComms::getInstance().SetActiveRadio(_serverConnectionHandlerID, 2, state == 2, (int)(_ui->frequence_3->value() * 100));
}

void channels::onChange4(int state) {
	KRTComms::getInstance().SetActiveRadio(_serverConnectionHandlerID, 3, state == 2, (int)(_ui->frequence_4->value() * 100));
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

void channels::onSetFrequence1(bool checked) {
	KRTComms::getInstance().SetActiveRadio(_serverConnectionHandlerID, 0, _ui->channel_1->isChecked(), (int)(_ui->frequence_1->value() * 100));
	_ui->frequence_1->Deselect();
	set("freq_1", _ui->frequence_1->value());
}

void channels::onSetFrequence2(bool checked) {
	KRTComms::getInstance().SetActiveRadio(_serverConnectionHandlerID, 1, _ui->channel_2->isChecked(), (int)(_ui->frequence_2->value() * 100));
	_ui->frequence_2->Deselect();
	set("freq_2", _ui->frequence_2->value());
}

void channels::onSetFrequence3(bool checked) {
	KRTComms::getInstance().SetActiveRadio(_serverConnectionHandlerID, 2, _ui->channel_3->isChecked(), (int)(_ui->frequence_3->value() * 100));
	_ui->frequence_3->Deselect();
	set("freq_3", _ui->frequence_3->value());
}

void channels::onSetFrequence4(bool checked) {
	KRTComms::getInstance().SetActiveRadio(_serverConnectionHandlerID, 3, _ui->channel_4->isChecked(), (int)(_ui->frequence_4->value() * 100));
	_ui->frequence_4->Deselect();
	set("freq_4", _ui->frequence_4->value());
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

void channels::onReset(bool checked) {
	KRTComms::getInstance().Reset(_serverConnectionHandlerID);
}

void channels::onDebug(int state) {
	KRTComms::getInstance().SetDebug(state == 2);
}

void channels::onDuckChannel(int state) {
	Ducker::getInstance().SetEnabled(state == 2);
	set("duck_channel", state == 2);
}


