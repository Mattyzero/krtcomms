#pragma once
#include "QtWidgets/QLineEdit"
#include "QtGui/QMouseEvent"
#include <QtCore/QTimer>
#include <QtGui/QPainter>
#include <QtGui/QPixmap>
#include <QtGui/QImage>

class QEditableLabel : public QLineEdit {
	Q_OBJECT
public:
	explicit QEditableLabel(QWidget* parent = nullptr, const QString& text = "Rename Me");
	virtual ~QEditableLabel() {}

	void resizeToContent();
	void setLocked(bool locked) {
		_locked = locked;
		update();
	}
	void setTextColor(QColor color);
signals:
	void clicked();
protected:
	void paintEvent(QPaintEvent * event);
	void mousePressEvent(QMouseEvent * event) {

		if (!this->isReadOnly()) {
			QLineEdit::mousePressEvent(event);
			return;
		}

		if (_doubleClickConnection != NULL) {
			QObject::disconnect(*_doubleClickConnection);
			delete _doubleClickConnection;
			_doubleClickConnection = NULL;
		}

		if (_doubleClickTimer.isActive()) {
			_doubleClickTimer.stop();
		}

		_doubleClickConnection = new QMetaObject::Connection;
		*_doubleClickConnection = QObject::connect(&_doubleClickTimer, &QTimer::timeout, [this]() {
			emit clicked();

			QObject::disconnect(*_doubleClickConnection);
			delete _doubleClickConnection;
			_doubleClickConnection = NULL;
		});
		_doubleClickTimer.start(200);
	}
	void mouseDoubleClickEvent(QMouseEvent *) {

		if (_doubleClickConnection != NULL) {
			QObject::disconnect(*_doubleClickConnection);
			delete _doubleClickConnection;
			_doubleClickConnection = NULL;
		}

		if (_doubleClickTimer.isActive()) {
			_doubleClickTimer.stop();
		}
		

		this->setReadOnly(false);
		this->selectAll();
		this->setFocus();
	}
private:
	QMetaObject::Connection * _doubleClickConnection = NULL;
	QTimer _doubleClickTimer;
	QImage _lock;
	bool _locked = false;
	QString _styleSheet;
};

