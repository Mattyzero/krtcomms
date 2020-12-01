#pragma once
#include "QtWidgets/QDial"
class QKDial : public QDial {
	Q_OBJECT
public:
	explicit QKDial(QWidget* parent = nullptr);
	virtual ~QKDial() {}



protected:
	void keyPressEvent(QKeyEvent *ev) {
		//DO NOTHING
	}
};

