#pragma once

#include <QLayout>
#include <QList>

class OverlayLayout : public QLayout
{
public:
	OverlayLayout(QWidget *parent);
	~OverlayLayout();

	void addItem(QLayoutItem *item) override;
	int count() const override;
	QLayoutItem* itemAt(int index) const override;
	QLayoutItem* takeAt(int index) override;

	void setGeometry(const QRect &rect) override;

	QSize sizeHint(void) const override;

	Qt::Orientations expandingDirections() const override;

private:
	QList<QLayoutItem*> _items;
};
