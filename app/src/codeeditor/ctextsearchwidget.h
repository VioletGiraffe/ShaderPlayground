#pragma once

#include "compiler/compiler_warnings_control.h"

DISABLE_COMPILER_WARNINGS
#include <QWidget>
RESTORE_COMPILER_WARNINGS

namespace Ui {
class CTextSearchWidget;
}

class CTextSearchWidget : public QWidget
{
public:
	explicit CTextSearchWidget(QWidget *parent = 0);
	~CTextSearchWidget();

	void setShowReplaceUI(bool show);

private:
	Ui::CTextSearchWidget *ui;
};
