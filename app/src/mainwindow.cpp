#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "shaderrenderwidget.h"

#include <QTextEdit>

static const char* fshader =
	"#ifdef GL_ES\n"
	"precision highp int;\n"
	"precision highp float;\n"
	"#endif\n"

	"varying highp vec2 pixelPosition;\n"

	"void main()\n"
	"{\n"
		"if (pixelPosition.x > 100.0)\n"
			"\tgl_FragColor = vec4(1.0, 1.0, 0.0, 1.0);\n"
		"else\n"
			"\tgl_FragColor = vec4(0.0, 1.0, 0.0, 1.0);\n"
	"}\n";

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);

	_renderWidget = new ShaderRenderWidget(ui->shaderWidgetHost);
	_shaderEditorWidget = new QTextEdit(ui->shaderWidgetHost);

	_shaderEditorWidget->setLineWrapMode(QTextEdit::NoWrap);
	auto editorPalette = _shaderEditorWidget->palette();
	editorPalette.setColor(QPalette::Background, Qt::transparent);
	_shaderEditorWidget->setPalette(editorPalette);

	_shaderEditorWidget->setText(fshader);
	_shaderEditorWidget->setTabStopWidth(3 * _shaderEditorWidget->fontMetrics().width(' '));
	connect(_shaderEditorWidget, &QTextEdit::textChanged, this, &MainWindow::updateFragmentShader);
}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::showEvent(QShowEvent* e)
{
	updateFragmentShader();
	QMainWindow::showEvent(e);
}

void MainWindow::updateFragmentShader()
{
	const QString log = _renderWidget->setFragmentShader(_shaderEditorWidget->toPlainText());
	if (!log.isEmpty())
		ui->output->setPlainText(log);
}
