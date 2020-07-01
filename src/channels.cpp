#include "channels.h"
#include "ui_channels.h"

#include "teamspeak/public_rare_definitions.h"
#include "teamspeak/public_definitions.h"
#include "ts3_functions.h"
#include "KRTComms.h"
#include "Ducker.h"
#include "Encrypter.h"

#include <QtGui/QCursor>
#include <QtGui/QPainter>
#include <QtWidgets/QMessageBox>

channels::channels(const QString& configLocation, char* pluginID, TS3Functions ts3Functions, QWidget* parent /* = nullptr */) : QDialog(parent),
	_ui(std::make_unique<Ui::channelsui>()),
	_settings(std::make_unique<QSettings>(configLocation, QSettings::IniFormat, this)) {

	_pluginID = pluginID;
	_ts3 = ts3Functions;
	

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
	set("freq_1", _ui->frequence_1->value());
	set("freq_2", _ui->frequence_2->value());
	set("freq_3", _ui->frequence_3->value());
	set("freq_4", _ui->frequence_4->value());

	set("freq_5", _ui->frequence_5->value());
	set("freq_6", _ui->frequence_6->value());
	set("freq_7", _ui->frequence_7->value());
	set("freq_8", _ui->frequence_8->value());

	close();
}

void channels::load() {
	resize(get("win_size").toSize());
	_serverConnectionHandlerID = _ts3.getCurrentServerConnectionHandlerID();
	//TODO pointer array zu jeweiligen variablen damit das _Zahl ding nicht ewig weiter geht
	//Falls jemand weis wie man das easy umsetzen kann bitte mich kontaktieren oder ein PR DANKE

	_ui->channel_1->setChecked(KRTComms::getInstance().ActiveInRadio(_serverConnectionHandlerID, 0));
	_ui->channel_2->setChecked(KRTComms::getInstance().ActiveInRadio(_serverConnectionHandlerID, 1));
	_ui->channel_3->setChecked(KRTComms::getInstance().ActiveInRadio(_serverConnectionHandlerID, 2));
	_ui->channel_4->setChecked(KRTComms::getInstance().ActiveInRadio(_serverConnectionHandlerID, 3));

	_ui->channel_5->setChecked(KRTComms::getInstance().ActiveInRadio(_serverConnectionHandlerID, 4));
	_ui->channel_6->setChecked(KRTComms::getInstance().ActiveInRadio(_serverConnectionHandlerID, 5));
	_ui->channel_7->setChecked(KRTComms::getInstance().ActiveInRadio(_serverConnectionHandlerID, 6));
	_ui->channel_8->setChecked(KRTComms::getInstance().ActiveInRadio(_serverConnectionHandlerID, 7));

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

	_ui->radio_2_password->setText(get("radio_2_password").toString());
	Encrypter::SetPassword(1, get("radio_2_password").toString());

	_ui->radio_3_password->setText(get("radio_3_password").toString());
	Encrypter::SetPassword(2, get("radio_3_password").toString());

	_ui->radio_4_password->setText(get("radio_4_password").toString());
	Encrypter::SetPassword(3, get("radio_4_password").toString());
}

void channels::onChange1(int state) {
	KRTComms::getInstance().SetActiveRadio(_serverConnectionHandlerID, 0, state == 2, (int)(_ui->frequence_1->value() * 100));
	DisableSendLamp(0);
	DisableReceiveLamp(0);
}

void channels::onChange2(int state) {
	KRTComms::getInstance().SetActiveRadio(_serverConnectionHandlerID, 1, state == 2, (int)(_ui->frequence_2->value() * 100));
	DisableSendLamp(1);
	DisableReceiveLamp(1);
}

void channels::onChange3(int state) {
	KRTComms::getInstance().SetActiveRadio(_serverConnectionHandlerID, 2, state == 2, (int)(_ui->frequence_3->value() * 100));
	DisableSendLamp(2);
	DisableReceiveLamp(2);
}

void channels::onChange4(int state) {
	KRTComms::getInstance().SetActiveRadio(_serverConnectionHandlerID, 3, state == 2, (int)(_ui->frequence_4->value() * 100));
	DisableSendLamp(3);
	DisableReceiveLamp(3);
}

void channels::onChange5(int state) {
	KRTComms::getInstance().SetActiveRadio(_serverConnectionHandlerID, 4, state == 2, (int)(_ui->frequence_5->value() * 100));
	DisableSendLamp(4);
	DisableReceiveLamp(4);
}

void channels::onChange6(int state) {
	KRTComms::getInstance().SetActiveRadio(_serverConnectionHandlerID, 5, state == 2, (int)(_ui->frequence_6->value() * 100));
	DisableSendLamp(5);
	DisableReceiveLamp(5);
}

void channels::onChange7(int state) {
	KRTComms::getInstance().SetActiveRadio(_serverConnectionHandlerID, 6, state == 2, (int)(_ui->frequence_7->value() * 100));
	DisableSendLamp(6);
	DisableReceiveLamp(6);
}

void channels::onChange8(int state) {
	KRTComms::getInstance().SetActiveRadio(_serverConnectionHandlerID, 7, state == 2, (int)(_ui->frequence_8->value() * 100));
	DisableSendLamp(7);
	DisableReceiveLamp(7);
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
		KRTComms::getInstance().SetActiveRadio(_serverConnectionHandlerID, 0, true, (int)(_ui->frequence_1->value() * 100));
	}
	else {
		_ui->channel_1->setChecked(true);
	}
	_ui->frequence_1->Deselect();
	set("freq_1", _ui->frequence_1->value());
}

void channels::onSetFrequence2(bool checked) {
	if (_ui->channel_2->isChecked()) {
		KRTComms::getInstance().SetActiveRadio(_serverConnectionHandlerID, 1, true, (int)(_ui->frequence_2->value() * 100));
	}
	else {
		_ui->channel_2->setChecked(true);
	}
	_ui->frequence_2->Deselect();
	set("freq_2", _ui->frequence_2->value());
}

void channels::onSetFrequence3(bool checked) {	
	if (_ui->channel_3->isChecked()) {
		KRTComms::getInstance().SetActiveRadio(_serverConnectionHandlerID, 2, true, (int)(_ui->frequence_3->value() * 100));
	}
	else {
		_ui->channel_3->setChecked(true);
	}
	_ui->frequence_3->Deselect();
	set("freq_3", _ui->frequence_3->value());
}

void channels::onSetFrequence4(bool checked) {	
	if (_ui->channel_4->isChecked()) {
		KRTComms::getInstance().SetActiveRadio(_serverConnectionHandlerID, 3, true, (int)(_ui->frequence_4->value() * 100));
	}
	else {
		_ui->channel_4->setChecked(true);
	}
	_ui->frequence_4->Deselect();
	set("freq_4", _ui->frequence_4->value());
}

void channels::onSetFrequence5(bool checked) {
	if (_ui->channel_5->isChecked()) {
		KRTComms::getInstance().SetActiveRadio(_serverConnectionHandlerID, 4, true, (int)(_ui->frequence_5->value() * 100));
	}
	else {
		_ui->channel_5->setChecked(true);
	}
	_ui->frequence_5->Deselect();
	set("freq_5", _ui->frequence_5->value());
}

void channels::onSetFrequence6(bool checked) {	
	if (_ui->channel_6->isChecked()) {
		KRTComms::getInstance().SetActiveRadio(_serverConnectionHandlerID, 5, true, (int)(_ui->frequence_6->value() * 100));
	}
	else {
		_ui->channel_6->setChecked(true);
	}
	_ui->frequence_6->Deselect();
	set("freq_6", _ui->frequence_6->value());
}

void channels::onSetFrequence7(bool checked) {	
	if (_ui->channel_7->isChecked()) {
		KRTComms::getInstance().SetActiveRadio(_serverConnectionHandlerID, 6, true, (int)(_ui->frequence_7->value() * 100));
	}
	else {
		_ui->channel_7->setChecked(true);
	}
	_ui->frequence_7->Deselect();
	set("freq_7", _ui->frequence_7->value());
}

void channels::onSetFrequence8(bool checked) {	
	if (_ui->channel_8->isChecked()) {
		KRTComms::getInstance().SetActiveRadio(_serverConnectionHandlerID, 7, true, (int)(_ui->frequence_8->value() * 100));
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
	KRTComms::getInstance().SetActiveRadio(_serverConnectionHandlerID, 0, true, (int)(_ui->frequence_1->value() * 100));
}

void channels::onRadio2PasswordChanged(QString value) {

	set("radio_2_password", value);
}

void channels::onSetRadio2Password(bool checked) {
	KRTComms::getInstance().SetActiveRadio(_serverConnectionHandlerID, 1, false, -1);
	Encrypter::SetPassword(1, get("radio_2_password").toString());
	KRTComms::getInstance().SetActiveRadio(_serverConnectionHandlerID, 1, true, (int)(_ui->frequence_2->value() * 100));
}

void channels::onRadio3PasswordChanged(QString value) {

	set("radio_3_password", value);
}

void channels::onSetRadio3Password(bool checked) {
	KRTComms::getInstance().SetActiveRadio(_serverConnectionHandlerID, 2, false, -1);
	Encrypter::SetPassword(2, get("radio_3_password").toString());
	KRTComms::getInstance().SetActiveRadio(_serverConnectionHandlerID, 2, true, (int)(_ui->frequence_3->value() * 100));
}

void channels::onRadio4PasswordChanged(QString value) {

	set("radio_4_password", value);
}

void channels::onSetRadio4Password(bool checked) {
	KRTComms::getInstance().SetActiveRadio(_serverConnectionHandlerID, 3, false, -1);
	Encrypter::SetPassword(3, get("radio_2_password").toString());
	KRTComms::getInstance().SetActiveRadio(_serverConnectionHandlerID, 3, true, (int)(_ui->frequence_4->value() * 100));
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
		KRTComms::getInstance().RequestHotkeyInputDialog(keyword.replace("send_ch_", "").replace("_", "").toInt(), parent);
		return true; //lass ich daweil so falls ich mich umentscheide und doch wieder 2 Knöpfe einbau
		break;
	}

	return false;
}


