#include "overlaylayout.h"

#include <assert.h>

OverlayLayout::OverlayLayout(QWidget *parent) : QLayout (parent)
{
}

OverlayLayout::~OverlayLayout()
{
	for (auto item : _items)
		delete item;
}

void OverlayLayout::addItem(QLayoutItem *item)
{
	_items.push_back(item);
}

int OverlayLayout::count() const
{
	return _items.size();
}

QLayoutItem* OverlayLayout::itemAt(int index) const
{
	return index >= 0 && index < count() ? _items[index] : nullptr;
}

QLayoutItem* OverlayLayout::takeAt(int index)
{
	return index >= 0 && index < count() ? _items.takeAt(index) : nullptr;
}

void OverlayLayout::setGeometry(const QRect & rect)
{
	QLayout::setGeometry(rect);

	for (QLayoutItem* item : _items)
		item->setGeometry(rect);
}

QSize OverlayLayout::sizeHint() const
{
	QSize size;
	for (const QLayoutItem* item: _items)
		size = size.expandedTo(item->minimumSize());

	return size;
}

Qt::Orientations OverlayLayout::expandingDirections() const
{
	return Qt::Horizontal | Qt::Vertical;
}
