#include "mainwindow.h"

#include "shaderrenderwidget.h"
#include "codeeditor.h"
#include "shadersyntaxhighlighter.h"

#include "widgets/layouts/coverlaylayout.h"
#include "assert/advanced_assert.h"
#include "settings/csettings.h"
#include "aboutdialog/caboutdialog.h"

DISABLE_COMPILER_WARNINGS
#include "ui_mainwindow.h"

#include <QActionGroup>
#include <QMessageBox>
#include <QStringBuilder>
RESTORE_COMPILER_WARNINGS

// Keys for storing option in the settings
#define SETTINGS_KEY_UI_SHADER_FRAMEWORK QStringLiteral("Ui/ShaderFamework")
#define SETTINGS_KEY_UI_WINDOWGEOMETRY QStringLiteral("Ui/WindowGeometry")
#define SETTINGS_KEY_UI_WINDOWSTATE QStringLiteral("Ui/WindowState")

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

	auto shaderFrameworkModeMenuGroup = new QActionGroup(this);
	shaderFrameworkModeMenuGroup->addAction(ui->actionBarebone_GLSL);
	shaderFrameworkModeMenuGroup->addAction(ui->actionShadertoy_compatibility);

	const auto currentFramework = (ShaderFramework::Framework)CSettings().value(SETTINGS_KEY_UI_SHADER_FRAMEWORK, ShaderFramework::ShaderToy).toInt();
	_shaderFramework.setFrameworkMode(currentFramework);
	if (currentFramework == ShaderFramework::GLSL)
		ui->actionBarebone_GLSL->setChecked(true);
	else
		ui->actionShadertoy_compatibility->setChecked(true);

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

	_shaderEditorWidget->setPlainText(textFromResource(currentFramework == ShaderFramework::ShaderToy ? ":/resources/default_fragment_shader_shadertoy.fsh" : ":/resources/default_fragment_shader_barebone.fsh"));
	_shaderEditorWidget->setLineWrapMode(QPlainTextEdit::NoWrap);
	_shaderEditorWidget->setTabStopWidth(4 * _shaderEditorWidget->fontMetrics().width(' '));
	connect(_shaderEditorWidget, &QPlainTextEdit::textChanged, this, &MainWindow::updateFragmentShader);

	ui->mainSplitter->setStretchFactor(0, 1);
	ui->mainSplitter->setStretchFactor(1, 0);
	ui->mainSplitter->setSizes({0, 100});

	connect(ui->actionOpen, &QAction::triggered, this, [this]() {
		if (!_documentHandler.open())
		{
			QMessageBox::warning(this, "Failed to open file", "Failed to open the selected file.");
			return;
		}

		const auto result = _documentHandler.loadContents();
		if (!result.loadedSuccessfully)
		{
			QMessageBox::warning(this, "Failed to load file", "Failed to load the file " % _documentHandler.documentName());
			return;
		}

		_shaderEditorWidget->setPlainText(QString::fromUtf8(result.data));
	});

	connect(ui->actionSave, &QAction::triggered, this, [this]() {
		_documentHandler.save(_shaderEditorWidget->toPlainText().toUtf8());
	});

	connect(ui->actionSave_as, &QAction::triggered, this, [this]() {
		_documentHandler.saveAs(_shaderEditorWidget->toPlainText().toUtf8());
	});

	connect(ui->actionExit, &QAction::triggered, qApp, &QApplication::quit);

	connect(ui->actionToggle_fullscreen_mode, &QAction::triggered, this, [this](bool checked) {
		if (checked)
			showFullScreen();
		else
			showNormal();
	});

	connect(ui->actionBarebone_GLSL, &QAction::triggered, this, [this]() {
		setShaderFramework(ShaderFramework::GLSL);
	});

	connect(ui->actionShadertoy_compatibility, &QAction::triggered, this, [this]() {
		setShaderFramework(ShaderFramework::ShaderToy);
	});

	connect(ui->actionAbout, &QAction::triggered, this, [this]() {
		CAboutDialog(this).exec();
	});

	connect(&_fpsUpdaterTimer, &QTimer::timeout, this, &MainWindow::updateWindowTitle);
	_fpsUpdaterTimer.start(100);
}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::showEvent(QShowEvent* e)
{
	QMainWindow::showEvent(e);
	updateFragmentShader();
}

void MainWindow::setShaderFramework(ShaderFramework::Framework framework)
{
	CSettings().setValue(SETTINGS_KEY_UI_SHADER_FRAMEWORK, framework);
	_shaderFramework.setFrameworkMode(framework);
	updateFragmentShader();
}

void MainWindow::updateFragmentShader()
{
	const QString log = _renderWidget->setFragmentShader(_shaderFramework.processedShaderSource(_shaderEditorWidget->toPlainText()));
	ui->output->setPlainText(log);
}

void MainWindow::updateWindowTitle()
{
	setWindowTitle("Shader Playground - " % QString::number((int)(1000.0f / _renderWidget->frameRenderingPeriod())) % " fps");
}
