#include "ColorSchemeEditor.h"

DISABLE_COMPILER_WARNINGS
#include "ui_ColorSchemeEditor.h"

#include <QSettings>
RESTORE_COMPILER_WARNINGS

static constexpr const char ColorSchemeKey[] = "ColorScheme";

static constexpr const char DefaultScheme[] =
R"(text:
  color: "#ECECEC"
statements:
  color: "#ef9300"
  font: bold
numbers:
  color: "#CB83EA"
preprocessor:
  color: "#c5ab95"
types:
  color: "#3CC1E6"
swizzle:
  color: "#ef9300"
comments:
  color: "#639fd4")";

ColorSchemeEditor::ColorSchemeEditor(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::ColorShemeEditor)
{
	ui->setupUi(this);
	ui->text->setPlainText(colorScheme());
}

ColorSchemeEditor::~ColorSchemeEditor()
{
	delete ui;
}

QString ColorSchemeEditor::colorScheme()
{
	return QSettings{}.value(ColorSchemeKey, DefaultScheme).toString();
}

void ColorSchemeEditor::accept()
{
	QSettings{}.setValue(ColorSchemeKey, ui->text->toPlainText());
	QDialog::accept();
}
