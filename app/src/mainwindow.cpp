#include "mainwindow.h"

#include "shaderrenderwidget.h"
#include "codeeditor/codeeditor.h"
#include "shadersyntaxhighlighter.h"
#include "errorlogsyntaxhighlighter.h"
#include "colorschemeeditor.h"

#include "widgets/layouts/coverlaylayout.h"
#include "settings/csettings.h"
#include "aboutdialog/caboutdialog.h"
#include "widgets/cpersistentwindow.h"
#include "widgets/widgetutils.h"
#include "utils/resources.h"

DISABLE_COMPILER_WARNINGS
#include "ui_mainwindow.h"

#include <QActionGroup>
#include <QDate>
#include <QFile>
#include <QMessageBox>
#include <QStringBuilder>
RESTORE_COMPILER_WARNINGS

namespace Settings {
	static constexpr const char SHADER_FRAMEWORK[] = "Ui/ShaderFamework";
	static constexpr const char WRAP_TEXT[] = "Ui/WrapText";

	static constexpr const char WINDOWGEOMETRY[] = "Ui/MainWindow";
	static constexpr const char LAST_OPEN_DOCUMENT[] = "Ui/LastOpenDocument";
}

static constexpr const char defaultDocumentExtension[] =  "Shader Playground files(*.fsh);;All files (*.*)";

MainWindow::MainWindow(QWidget* parent) :
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
	new ErrorLogSyntaxHighlighter(ui->output->document());

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

	{
		// Editor font
		QFont editorFont;
		editorFont.setFamily("Consolas");
		editorFont.setFixedPitch(true);
		editorFont.setPointSize(10);

		_shaderEditorWidget->setFont(editorFont);
	}

	{
		// Output font
		QFont f;
		f.setFamily("Consolas");
		f.setFixedPitch(true);
		f.setPointSize(11);
		ui->output->setFont(f);
	}

	// "color: white" only really affects the line numbers, since everything else is colored with the ColorScheme
	_shaderEditorWidget->setStyleSheet("color: white; selection-background-color: rgba(250, 100, 0, 200); selection-color: white;");
	_shaderEditorWidget->setFrameStyle(QFrame::NoFrame);
	auto editorPalette = _shaderEditorWidget->palette();
	editorPalette.setColor(QPalette::Active, QPalette::Base, Qt::transparent);
	editorPalette.setColor(QPalette::Inactive, QPalette::Base, Qt::transparent);
	_shaderEditorWidget->setPalette(editorPalette);
	_shaderEditorWidget->setTextBackgroundColor(QColor(20, 20, 20, 160));

	_shaderEditorWidget->setTabStopDistance(static_cast<qreal>(4ull * _shaderEditorWidget->fontMetrics().horizontalAdvance(' ')));

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
		_documentHandler.saveAs(defaultDocumentExtension);
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

	connect(ui->action_Word_wrap, &QAction::toggled, this, [this] (bool wrap) {
		_shaderEditorWidget->setLineWrapMode(wrap ? QPlainTextEdit::WidgetWidth : QPlainTextEdit::NoWrap);
		QSettings{}.setValue(Settings::WRAP_TEXT, wrap);
	});

	ui->action_Word_wrap->setChecked(QSettings{}.value(Settings::WRAP_TEXT, true).toBool());

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
		const auto buildDate = QLocale(QLocale::English, QLocale::UnitedStates).toDate(__DATE__, "MMM d yyyy");
		CAboutDialog(buildDate.toString("yy.MM"), this, "2017").exec();
	});

	connect(&_fpsUpdaterTimer, &QTimer::timeout, this, &MainWindow::updateWindowTitle);
	_fpsUpdaterTimer.start(200);
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
			_documentHandler.saveAs(defaultDocumentExtension);
		}
	}

	_documentHandler.open(defaultDocumentExtension);

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
		"[*] - " % _renderWidget->gpuName() % ", frame time: " %
		QString::number(_renderWidget->frameTimeMs(), 'f', 1) % " ms"
	);
}

