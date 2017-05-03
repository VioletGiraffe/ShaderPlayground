#include "mainwindow.h"

#include "shaderrenderwidget.h"
#include "widgets/layouts/coverlaylayout.h"
#include "codeeditor.h"
#include "shadersyntaxhighlighter.h"
#include "assert/advanced_assert.h"

DISABLE_COMPILER_WARNINGS
#include "ui_mainwindow.h"

#include <QActionGroup>
#include <QStringBuilder>
RESTORE_COMPILER_WARNINGS

inline QString textFromResource(const char* resourcePath)
{
	QFile resourceFile(resourcePath);
	assert(resourceFile.exists());
	assert_r(resourceFile.open(QFile::ReadOnly));

	return resourceFile.readAll();
}

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);

	auto overlayLayout = new COverlayLayout(ui->shaderWidgetsHost);
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
	_shaderEditorWidget->setStyleSheet("color: white; selection-background-color: rgba(200, 200, 200, 150); selection-color: rgb(30, 30, 30);");
	_shaderEditorWidget->setFrameStyle(QFrame::NoFrame);
	auto editorPalette = _shaderEditorWidget->palette();
	editorPalette.setColor(QPalette::Active, QPalette::Base, Qt::transparent);
	editorPalette.setColor(QPalette::Inactive, QPalette::Base, Qt::transparent);
	_shaderEditorWidget->setPalette(editorPalette);

	_shaderEditorWidget->setTextBackgroundColor(QColor(20, 20, 20, 130));

	_shaderEditorWidget->setPlainText(textFromResource(":/resources/default_fragment_shader.fsh"));
	_shaderEditorWidget->setLineWrapMode(QPlainTextEdit::NoWrap);
	_shaderEditorWidget->setTabStopWidth(4 * _shaderEditorWidget->fontMetrics().width(' '));
	connect(_shaderEditorWidget, &QPlainTextEdit::textChanged, this, &MainWindow::updateFragmentShader);

	ui->mainSplitter->setStretchFactor(0, 1);
	ui->mainSplitter->setStretchFactor(1, 0);
	ui->mainSplitter->setSizes({0, 100});

	connect(ui->actionToggle_fullscreen_mode, &QAction::triggered, this, [this](bool checked) {
		if (checked)
			showFullScreen();
		else
			showNormal();
	});

	auto shaderFrameworkModeMenuGroup = new QActionGroup(this);
	shaderFrameworkModeMenuGroup->addAction(ui->actionBarebone_GLSL);
	shaderFrameworkModeMenuGroup->addAction(ui->actionShadertoy_compatibility);

	connect(ui->actionExit, &QAction::triggered, qApp, &QApplication::quit);

	connect(&_fpsUpdaterTimer, &QTimer::timeout, this, &MainWindow::updateWindowTitle);
	_fpsUpdaterTimer.start(100);
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
	ui->output->setPlainText(log);
}

void MainWindow::updateWindowTitle()
{
	setWindowTitle("Shader Playground - " % QString::number((int)(1000.0f / _renderWidget->frameRenderingPeriod())) % " fps");
}
