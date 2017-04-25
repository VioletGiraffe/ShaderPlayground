#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "shaderrenderwidget.h"
#include "overlaylayout.h"
#include "codeeditor.h"
#include "shadersyntaxhighlighter.h"

static const char* fshader =
	"#ifdef GL_ES\n"
	"precision highp int;\n"
	"precision highp float;\n"
	"#endif\n\n"

	"varying highp vec2 pixelPosition;\n\n"

	"void main()\n"
	"{\n"
		"\tgl_FragColor = pixelPosition.x > 100.0 ? vec4(1.0, 0.95, 0.0, 1.0) : vec4(0.0, 0.85, 1.0, 1.0);\n"
	"}\n";

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);

	auto overlayLayout = new OverlayLayout(ui->shaderWidgetsHost);
	_renderWidget = new ShaderRenderWidget();
	overlayLayout->addWidget(_renderWidget);

	_shaderEditorWidget = new CodeEditor();
	overlayLayout->addWidget(_shaderEditorWidget);
	new ShaderSyntaxHighlighter(_shaderEditorWidget->document());

	ui->shaderWidgetsHost->setLayout(overlayLayout);

	QFont editorFont;
	editorFont.setFamily("Consolas");
	editorFont.setFixedPitch(true);
	editorFont.setPointSize(10);

	_shaderEditorWidget->setFont(editorFont);
	_shaderEditorWidget->setStyleSheet("color: white; selection-background-color: rgba(80, 80, 80, 130);");
	_shaderEditorWidget->setFrameStyle(QFrame::NoFrame);
	auto editorPalette = _shaderEditorWidget->palette();
	editorPalette.setColor(QPalette::Active, QPalette::Base, Qt::transparent);
	editorPalette.setColor(QPalette::Inactive, QPalette::Base, Qt::transparent);
	_shaderEditorWidget->setPalette(editorPalette);

	QTextCharFormat fmt;
	fmt.setBackground(QBrush(QColor(20, 20, 20, 130)));
	_shaderEditorWidget->mergeCurrentCharFormat(fmt);

	_shaderEditorWidget->setPlainText(fshader);
	_shaderEditorWidget->setLineWrapMode(QPlainTextEdit::NoWrap);
	_shaderEditorWidget->setTabStopWidth(4 * _shaderEditorWidget->fontMetrics().width(' '));
	connect(_shaderEditorWidget, &QPlainTextEdit::textChanged, this, &MainWindow::updateFragmentShader);

	ui->mainSplitter->setStretchFactor(0, 1);
	ui->mainSplitter->setStretchFactor(1, 0);
	ui->mainSplitter->setSizes({0, 100});
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
