#pragma once

#include "compiler/compiler_warnings_control.h"
#include "shaderframework.h"
#include "document/cdocumenthandler.h"

DISABLE_COMPILER_WARNINGS
#include <QMainWindow>
#include <QTimer>
RESTORE_COMPILER_WARNINGS

class CodeEditor;
class ShaderRenderWidget;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
public:
	explicit MainWindow(QWidget *parent = 0);
	~MainWindow();

	void showEvent(QShowEvent* e) override;
	void closeEvent(QCloseEvent* e) override;

private:
	// Action handlers
	void onOpenDocument();

	void loadSampleShaders();

	void setShaderFramework(ShaderFramework::Framework framework);
	void updateFragmentShader();
	void updateWindowTitle();

private:
	Ui::MainWindow *ui;

	CodeEditor* _shaderEditorWidget;
	ShaderRenderWidget* _renderWidget;

	ShaderFramework _shaderFramework;
	CDocumentHandler _documentHandler;

	QTimer _fpsUpdaterTimer;
};

