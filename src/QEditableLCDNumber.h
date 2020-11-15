#pragma once

#include <QtWidgets/QLCDNumber>
#include <QtGui/QPaintEvent>
#include <QtGui/QKeyEvent>
#include <QtCore/QRectF>
#include <QtGui/QWheelEvent>
#include <QtGui/QBrush>

class QEditableLCDNumber : public QLCDNumber
{
	Q_OBJECT
public:
	explicit QEditableLCDNumber(QWidget *parent = 0);
	void Deselect();

	void QEditableLCDNumber::FormatDisplay(double value);
	void QEditableLCDNumber::FormatDisplay(QString value);

signals:
	void submitted(bool checked = false);

public slots:
	void onSubmit(bool);

protected:
	void mousePressEvent(QMouseEvent * e);
	void keyPressEvent(QKeyEvent * event);
	void paintEvent(QPaintEvent * e);
	void wheelEvent(QWheelEvent * event);

private:
	void setDigitAreaPressed(QMouseEvent * e, const int areaId = -1);
	void positionValueIncrement(const int numSteps);
	QRectF mDigitRect;
	int mDigitIdToChange;
};

