#pragma once
#include "compiler/compiler_warnings_control.h"

DISABLE_COMPILER_WARNINGS
#include <QDialog>
RESTORE_COMPILER_WARNINGS

namespace Ui {
class ColorShemeEditor;
}

class ColorSchemeEditor final : public QDialog
{
public:
	explicit ColorSchemeEditor(QWidget *parent);
	~ColorSchemeEditor() override;

	static QString colorScheme();

protected:
	void accept() override;

private:
	Ui::ColorShemeEditor *ui;
};

