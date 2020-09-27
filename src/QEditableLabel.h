#pragma once
#include "QtWidgets/QLineEdit"
#include "QtGui/QMouseEvent"

class QEditableLabel : public QLineEdit {
	Q_OBJECT
public:
	explicit QEditableLabel(QWidget* parent = nullptr, const QString& text = "Rename Me");
	virtual ~QEditableLabel() {}

	void resizeToContent();
signals:
	void clicked();
protected:
	void mousePressEvent(QMouseEvent *) { emit clicked(); }
	void mouseDoubleClickEvent(QMouseEvent *) {
		this->setReadOnly(false);
		this->selectAll();
		this->setFocus();
	}
};

