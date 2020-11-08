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

	_doubleClickTimer.setSingleShot(true);

	QByteArray by = QByteArray::fromBase64("iVBORw0KGgoAAAANSUhEUgAAACAAAAAgCAYAAABzenr0AAAC3npUWHRSYXcgcHJvZmlsZSB0eXBlIGV4aWYAAHja7ZdBkuQoDEX3nGKOgCSExHEwmIi+wRx/PphyVmZVd8T09GIWacI2lsUX6MlkVTj//jHCXzioSAxJzXPJOeJIJRWu6Hi8jutOMa3rOni/wvOTPdwvGCbBXa7HfG7/Crs+Blja9uPZHqxtHd9CFJ9Cy4w8+9vPt5DwZaf9HMoeV9On5eyT25b9WNbLczIkoyv0hAOfQhJx9RlFrrNOG66YD5yiOPpJZNnz97kLd/cleXfvJXexbrs8pyLEvB3yS462nfTFLncYfpoRPSI/vTC5Q3zJ3Rjdxziv1dWUkakc9qI+lrJ6cDyQSlnDMprhVPRttYLmWGIDsQ6aB1oLVIiR2UGJOlUadK57o4YpJj7ZcGduLMvmYly4TQxIPxoNNinSA4iwNFATmPmeC624ZcVr5IjcCZ5MECOM+NLCd8bfabfQGLN0iaLfucK8eNY0pjHJzSu8AITGzqmu/K4WPtVN/ARWQFBXmh0LrPG4JA6lR23J4izw05hCvD4Nsr4FkCLEVkyGBARiJlHKFI3ZiJBHB5+KmbMkPkCAVLlTGGAjkgHHecbGGKPly8qXGVsLQKhkMaApUgErJUX9WHLUUFXRFFQ1q6lr0Zolp6w5Z8tzj6omlkwtm5lbseriydWzm7sXr4WLYAvTkouF4qWUWhG0QrpidIVHrQcfcqRDj3zY4Uc5akP5tNS05WbNW2m1c5eOz7/nbqF7L72edKKUznTqmU87/SxnHai1ISMNHXnY8FFGvaltqs/U6IXcr6nRpjaJpeVnD2owm31I0NxOdDIDMU4E4jYJoKB5MotOKfEkN5nFwvgolEGNdMLpNImBYDqJddDN7kHul9yCpn/FjX9GLkx0f4JcmOg2ua/cvqHW6/pFkQVofoUzp1EGNjY4nF7Z6/xN+u17+K8Cb6G30FvoLfQWegu9hf4/QgN/POBfzfAPimWRFrxZMLAAAAAGYktHRAD0APYA+THfCDgAAAAJcEhZcwAALiMAAC4jAXilP3YAAAAHdElNRQfkCgEHKxDdhXd4AAABgUlEQVRYw+2XPU8CQRCGn6gNsUASNBQ0Cv4BjR+1jRZ0NnaWtlQkVlJSqPgXbIgmFrT6F4yRjsIWPwoKOgMJYjOXTAh3Nwe7VEwyyezsOzPv3d7O5CC5FIA60AYGom3gTva8yiXQB0Yh2heMF6mpQgOgCVRFm+IL9muui5+o5O9AcQKmKHsB7tglgVdJ+gVkI3BZwYwkxolsqacqG/Blhd+MAy8ZEu4p+9mAf1H2vgsCaWV/GvCdkNipCWj5c4SZmoBzWRBYidjbBc6AI+V7BIYxOZeVfQFsAw/AWxJi1xH9flq9sRaveCgeaCWueAroeSTQkxqhH+GOpXnMIGmpEUogM4cPP2O9BXHyA9yLfQ7kXLArGc+yC+RVXF58ltiSi0b0NDZ0OuKbWydcNfq8ETgFDtX6QHxzI5ACbtW6Pn6/F9PQxTS0/CGty9UquCLwmyB2A/ieMILjJLJGTua9r2E0tHTMhkcCDcsrWgNaHoq3JLe5010BHzMeyVByVMM65T/0R+8iBIakyAAAAABJRU5ErkJggg==");
	_lock = QImage::fromData(by, "PNG");	
}

void QEditableLabel::resizeToContent() {
	QString text = this->text();

	int pixelsWide = this->fontMetrics().width(text) + 20;
	int pixelsHigh = this->fontMetrics().height();

	this->setFixedSize(pixelsWide, pixelsHigh);
}

void QEditableLabel::paintEvent(QPaintEvent * event) {

	if (_locked) {
		int w = height();
		int h = height();

		int lw = _lock.width();
		int lh = _lock.height();

		QPainter painter(this);
		painter.drawImage(QRectF(width() - w, 0, w, h), _lock, QRectF(0, 0, lw, lh));
		painter.end();
	}

	QLineEdit::paintEvent(event);
}