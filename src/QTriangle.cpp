#include "QTriangle.h"

#include <QtWidgets/QLabel>
#include <QtGui/QPainter>

QTriangle::QTriangle(QWidget *parent) : QLabel(parent) {
	_color = Qt::white;
	_direction = Direction::TOP;	
}

void QTriangle::SetColor(QColor color) {
	_color = color;
	update();
}

void QTriangle::SetDirection(Direction direction) {
	_direction = direction;
	update();
}

void QTriangle::paintEvent(QPaintEvent * e) {	
	
	QPainter p(this);
	p.setPen(QPen(Qt::black, 1, Qt::SolidLine, Qt::RoundCap));
	p.setBrush(QBrush(_color, Qt::SolidPattern));
	QRectF rect = QRectF(0, 0, width() - 1, height() - 1);

	QPainterPath path;
	if (_direction == Direction::TOP) {
		path.moveTo(rect.left() + (rect.width() / 2), rect.top());
		path.lineTo(rect.bottomLeft());
		path.lineTo(rect.bottomRight());
		path.lineTo(rect.left() + (rect.width() / 2), rect.top());
	}
	if (_direction == Direction::BOTTOM) {
		path.moveTo(rect.left(), rect.top());
		path.lineTo(rect.right(), rect.top());
		path.lineTo(rect.left() + (rect.width() / 2), rect.bottom());
		path.lineTo(rect.left(), rect.top());
	}
	p.drawPath(path);
	p.end();
	QLabel::paintEvent(e);
}