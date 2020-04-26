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

protected:
	void showEvent(QShowEvent* e) override;

private:
	std::unique_ptr<Ui::channelsui> _ui;
	std::unique_ptr<QSettings> _settings;

	char* _pluginID;
	TS3Functions _ts3;
	uint64 _serverConnectionHandlerID;

	void save();
	void load();

	void onChange1(int state);
	void onChange2(int state);
	void onChange3(int state);
	void onChange4(int state);

	void onClick1(bool checked);
	void onClick2(bool checked);
	void onClick3(bool checked);
	void onClick4(bool checked);

	void onSetFrequence1(bool checked);
	void onSetFrequence2(bool checked);
	void onSetFrequence3(bool checked);
	void onSetFrequence4(bool checked);

	void onPanChanged1(int value);
	void onPanChanged2(int value);
	void onPanChanged3(int value);
	void onPanChanged4(int value);

	void onVolumeGainChanged1(int value);
	void onVolumeGainChanged2(int value);
	void onVolumeGainChanged3(int value);
	void onVolumeGainChanged4(int value);

	void onReset(bool checked);
	void onDebug(int state);
	void onDuckChannel(int state);
};

