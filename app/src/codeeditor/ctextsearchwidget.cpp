#include "ctextsearchwidget.h"

DISABLE_COMPILER_WARNINGS
#include "ui_ctextsearchwidget.h"

#include <QDebug>
#include <QPaintEvent>
#include <QPainter>
RESTORE_COMPILER_WARNINGS

CTextSearchWidget::CTextSearchWidget(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::CTextSearchWidget)
{
	ui->setupUi(this);

	setShowReplaceUI(false);

	ui->_findNext->setShortcut(QKeySequence("F3"));
	connect(ui->_findNext, &QPushButton::clicked, this, [this]() {
		if (_callbackReceiver) _callbackReceiver->findNext(ui->_searchFor->text(), currentOptions());
	});

	ui->_findPrevious->setShortcut(QKeySequence("Shift+F3"));
	connect(ui->_findPrevious, &QPushButton::clicked, this, [this]() {
		if (_callbackReceiver) _callbackReceiver->findPrevious(ui->_searchFor->text(), currentOptions());
	});

	connect(ui->_findAll, &QPushButton::clicked, this, [this]() {
		if (_callbackReceiver) _callbackReceiver->findAll(ui->_searchFor->text(), currentOptions());
	});

	connect(ui->_replace, &QPushButton::clicked, this, [this]() {
		if (_callbackReceiver) _callbackReceiver->replaceNext(ui->_searchFor->text(), ui->_replaceWith->text(),currentOptions());
	});

	connect(ui->_replaceAll, &QPushButton::clicked, this, [this]() {
		if (_callbackReceiver) _callbackReceiver->replaceAll(ui->_searchFor->text(), ui->_replaceWith->text(), currentOptions());
	});
}

CTextSearchWidget::~CTextSearchWidget()
{
	delete ui;
}

void CTextSearchWidget::setCallbackReceiver(TextSearchCallbacks* receiver)
{
	_callbackReceiver = receiver;
}

void CTextSearchWidget::setShowReplaceUI(bool show)
{
	for (int col = 0; col < ui->gridLayout->columnCount(); ++col)
	{
		auto item = ui->gridLayout->itemAtPosition(1, col);
		if (item)
			item->widget()->setVisible(show);
	}
} 

TextSearchOptions CTextSearchWidget::currentOptions() const
{
	TextSearchOptions options;
	options.caseSensitive = ui->_caseSensitive->isChecked();

	return options;
}
