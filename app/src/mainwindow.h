#pragma once

#include "compiler/compiler_warnings_control.h"
#include "shaderframework.h"

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

private:
	void setShaderFramework(ShaderFramework::Framework framework);
	void updateFragmentShader();
	void updateWindowTitle();

private:
	Ui::MainWindow *ui;

	CodeEditor* _shaderEditorWidget;
	ShaderRenderWidget* _renderWidget;

	ShaderFramework _shaderFramework;

	QTimer _fpsUpdaterTimer;
};

