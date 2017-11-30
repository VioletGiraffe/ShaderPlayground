#include "ctextsearchwidget.h"

DISABLE_COMPILER_WARNINGS
#include "ui_ctextsearchwidget.h"
RESTORE_COMPILER_WARNINGS

CTextSearchWidget::CTextSearchWidget(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::CTextSearchWidget)
{
	ui->setupUi(this);

	setShowReplaceUI(false);
}

CTextSearchWidget::~CTextSearchWidget()
{
	delete ui;
}

void CTextSearchWidget::setShowReplaceUI(bool show)
{
	for (int col = 0; col < ui->gridLayout->columnCount(); ++col)
		ui->gridLayout->itemAtPosition(1, col)->widget()->setVisible(show);
}
