#pragma once
#include "QtWidgets/QSlider"

class QKSlider : public QSlider {
	Q_OBJECT
public:
	explicit QKSlider(Qt::Orientation orientation, QWidget* parent = nullptr);
	explicit QKSlider(QWidget* parent = nullptr);
	virtual ~QKSlider() {}



protected:
	void keyPressEvent(QKeyEvent *ev) {
		//DO NOTHING
	}
};

