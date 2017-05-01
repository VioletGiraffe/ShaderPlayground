#pragma once

#include "compiler/compiler_warnings_control.h"

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
	void updateFragmentShader();
	void updateWindowTitle();

private:
	Ui::MainWindow *ui;

	CodeEditor* _shaderEditorWidget;
	ShaderRenderWidget* _renderWidget;

	QTimer _fpsUpdaterTimer;
};

