#include "mainwindow.h"

#include "shaderrenderwidget.h"
#include "codeeditor/codeeditor.h"
#include "shadersyntaxhighlighter.h"
#include "colorschemeeditor.h"

#include "widgets/layouts/coverlaylayout.h"
#include "assert/advanced_assert.h"
#include "settings/csettings.h"
#include "aboutdialog/caboutdialog.h"
#include "widgets/cpersistentwindow.h"
#include "widgets/widgetutils.h"

DISABLE_COMPILER_WARNINGS
#include "ui_mainwindow.h"

#include <QActionGroup>
#include <QMessageBox>
#include <QStringBuilder>
RESTORE_COMPILER_WARNINGS

namespace Settings {
	static constexpr const char SHADER_FRAMEWORK[] = "Ui/ShaderFamework";
	static constexpr const char WRAP_TEXT[] = "Ui/WrapText";
	static constexpr const char COLOR_SCHEME[] = "Ui/ColorScheme";

	static constexpr const char WINDOWGEOMETRY[] = "Ui/MainWindow";
	static constexpr const char LAST_OPEN_DOCUMENT[] = "Ui/LastOpenDocument";
}

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

	installEventFilter(new CPersistenceEnabler(Settings::WINDOWGEOMETRY, this));

	auto overlayLayout = new COverlayLayout(ui->shaderWidgetsHost);
	overlayLayout->setObjectName("Code / graphics view overlay layout");
	_renderWidget = new ShaderRenderWidget();
	overlayLayout->addWidget(_renderWidget);

	auto shaderEditor = new CodeEditorWithSearch;
	_shaderEditorWidget = shaderEditor->editor();
	overlayLayout->addWidget(shaderEditor);
	_syntaxHighlighter = new ShaderSyntaxHighlighter(ColorScheme::fromYaml(ColorSchemeEditor::colorScheme()), _shaderEditorWidget->document());

	ui->shaderWidgetsHost->setLayout(overlayLayout);

	auto shaderFrameworkModeMenuGroup = new QActionGroup(this);
	shaderFrameworkModeMenuGroup->addAction(ui->actionBarebone_GLSL);
	shaderFrameworkModeMenuGroup->addAction(ui->actionShadertoy_compatibility);

	const auto currentFramework = (ShaderFramework::Framework)CSettings().value(Settings::SHADER_FRAMEWORK, ShaderFramework::ShaderToy).toInt();
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

	_shaderEditorWidget->setLineWrapMode(QPlainTextEdit::NoWrap);
	_shaderEditorWidget->setTabStopDistance(static_cast<qreal>(4 * _shaderEditorWidget->fontMetrics().horizontalAdvance(' ')));

	ui->mainSplitter->setStretchFactor(0, 1);
	ui->mainSplitter->setStretchFactor(1, 0);
	ui->mainSplitter->setSizes({0, 100});

	// Loading the last open shader document
	const QString lastOpenDocumentPath = CSettings().value(Settings::LAST_OPEN_DOCUMENT).toString();
	if (!lastOpenDocumentPath.isEmpty() && QFile::exists(lastOpenDocumentPath))
	{
		_documentHandler.setDocumentPath(lastOpenDocumentPath);
		if (!_documentHandler.loadContents())
		{
			QMessageBox::warning(this, "Error loading file", "Failed to load the last used file " % lastOpenDocumentPath);
			loadSampleShaders();
		}
		else
			_shaderEditorWidget->setPlainText(QString::fromUtf8(_documentHandler.contents()));
	}
	else
		loadSampleShaders();

	// This can only be done after the initial shader had been loaded
	connect(_shaderEditorWidget, &QPlainTextEdit::textChanged, this, &MainWindow::updateFragmentShader);


	connect(ui->actionNew, &QAction::triggered, this, [this](){
		loadSampleShaders();
	});

	connect(ui->actionOpen, &QAction::triggered, this, &MainWindow::onOpenDocument);

	connect(ui->actionSave, &QAction::triggered, this, [this]() {
		_documentHandler.save();
		CSettings().setValue(Settings::LAST_OPEN_DOCUMENT, _documentHandler.documentPath());
		setWindowModified(_documentHandler.hasUnsavedChanges());
	});

	connect(ui->actionSave_as, &QAction::triggered, this, [this]() {
		_documentHandler.saveAs();
		CSettings().setValue(Settings::LAST_OPEN_DOCUMENT, _documentHandler.documentPath());
		setWindowModified(_documentHandler.hasUnsavedChanges());
	});

	connect(ui->actionShow_actual_shader_code, &QAction::triggered, this, [this](){
		QDialog dlg{ this };
		QVBoxLayout l;
		QTextEdit viewer;
		viewer.setReadOnly(true);
		viewer.setPlainText(_shaderFramework.processedShaderSource(_shaderEditorWidget->toPlainText()));
		l.addWidget(&viewer);
		dlg.setLayout(&l);
		WidgetUtils::centerWidgetInParent(&dlg, 0.7);
		dlg.exec();
	});

	connect(ui->actionExit, &QAction::triggered, qApp, &QApplication::quit);

	connect(ui->actionToggle_fullscreen_mode, &QAction::triggered, this, [this](bool checked) {
		if (checked)
			showFullScreen();
		else
			showNormal();
	});

	connect(ui->action_Color_scheme, &QAction::triggered, this, [this] {
		ColorSchemeEditor editor(this);
		if (editor.exec() == QDialog::Accepted)
			_syntaxHighlighter->setColorScheme(ColorScheme::fromYaml(ColorSchemeEditor::colorScheme()));
	});

	connect(ui->actionBarebone_GLSL, &QAction::triggered, this, [this]() {
		setShaderFramework(ShaderFramework::GLSL);
	});

	connect(ui->actionShadertoy_compatibility, &QAction::triggered, this, [this]() {
		setShaderFramework(ShaderFramework::ShaderToy);
	});

	connect(ui->actionAbout, &QAction::triggered, this, [this]() {
		CAboutDialog("0.1", this, "2017").exec();
	});

	connect(&_fpsUpdaterTimer, &QTimer::timeout, this, &MainWindow::updateWindowTitle);
	_fpsUpdaterTimer.start(100);

	updateWindowTitle();
}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::showEvent(QShowEvent* e)
{
	QMainWindow::showEvent(e);
	// TODO: is this correct?
	updateFragmentShader();
}

void MainWindow::closeEvent(QCloseEvent* e)
{
	if (_documentHandler.hasUnsavedChanges())
	{
		const auto result = QMessageBox::question(this, tr("Unsaved changes"), tr("There are unsaved changes in the current document. Do you want to save them?"), QMessageBox::Yes | QMessageBox::Cancel | QMessageBox::No, QMessageBox::Cancel);
		if (result == QMessageBox::Yes)
			_documentHandler.save();
		else if (result == QMessageBox::Cancel)
		{
			e->ignore();
			return;
		}
	}

	QMainWindow::closeEvent(e);
}

void MainWindow::onOpenDocument()
{
	if (_documentHandler.hasUnsavedChanges())
	{
		if (QMessageBox::question(this,
			"Save changes?",
			"There are unsaved changes, do you want to save them before opening a new file?")
			== QMessageBox::Yes)
		{
			_documentHandler.saveAs();
		}
	}

	_documentHandler.open();

	if (!_documentHandler.loadContents())
	{
		QMessageBox::critical(this, "Failed to load file", "Failed to load the file " + _documentHandler.documentName());
		return;
	}

	CSettings().setValue(Settings::LAST_OPEN_DOCUMENT, _documentHandler.documentPath());
	_shaderEditorWidget->setPlainText(QString::fromUtf8(_documentHandler.contents()));
}

void MainWindow::loadSampleShaders()
{
	const QString defaultShader = textFromResource(_shaderFramework.frameworkMode() == ShaderFramework::ShaderToy ? ":/resources/default_fragment_shader_shadertoy.fsh" : ":/resources/default_fragment_shader_barebone.fsh");
	_documentHandler.newDocument(defaultShader.toUtf8());
	_shaderEditorWidget->setPlainText(defaultShader);
}

void MainWindow::setShaderFramework(ShaderFramework::Framework framework)
{
	CSettings().setValue(Settings::SHADER_FRAMEWORK, framework);
	_shaderFramework.setFrameworkMode(framework);
	updateFragmentShader();
}

// TODO: why is this called in showEvent?
void MainWindow::updateFragmentShader()
{
	const QString& log = _renderWidget->setFragmentShader(_shaderFramework.processedShaderSource(_shaderEditorWidget->toPlainText()));
	const QString adjustedLog = _shaderFramework.adjustLineNumbersInTheLog(log);
	ui->output->setPlainText(adjustedLog);

	_documentHandler.setContents(_shaderEditorWidget->toPlainText().toUtf8());
	setWindowModified(_documentHandler.hasUnsavedChanges());
}

void MainWindow::updateWindowTitle()
{
	const QString documentName = !_documentHandler.documentName().isEmpty() ? _documentHandler.documentName() : "Untitled shader";
	setWindowTitle(documentName %
		"[*] - " % _renderWidget->gpuName() % ", FPS: " %
		QString::number((int)(1000.0f / _renderWidget->frameRenderingPeriod()))
	);
}

