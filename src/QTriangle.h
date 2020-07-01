#pragma once
#include <QtWidgets/QLabel>

class QTriangle : public QLabel {
	Q_OBJECT
public:
	explicit QTriangle(QWidget *parent = 0);

	enum Direction {
		TOP,
		BOTTOM,
		LEFT,
		RIGHT
	};

	void SetColor(QColor color);
	void SetDirection(Direction direction);

protected:
	void paintEvent(QPaintEvent * e);

private:
	QColor _color;
	Direction _direction;
};

