#include "QEditableLabel.h"

QEditableLabel::QEditableLabel(QWidget* parent, const QString& text)
	: QLineEdit(text, parent)
{
	setReadOnly(true);
	setStyleSheet("QLineEdit:read-only {"
		"    border: none;"
		"    background: transparent; }"
		"QLineEdit {"
		"    background: white; "
		"    padding: 0; "
		"}");
	connect(this, &QLineEdit::editingFinished, [this] {
		this->unsetCursor();
		this->setSelection(0, 0);
		this->setReadOnly(true); 
	
		resizeToContent();
	});

	connect(this, &QLineEdit::returnPressed, [this] {

	});
}

void QEditableLabel::resizeToContent() {
	QString text = this->text();

	int pixelsWide = this->fontMetrics().width(text) + 10;
	int pixelsHigh = this->fontMetrics().height();

	this->setFixedSize(pixelsWide, pixelsHigh);
}