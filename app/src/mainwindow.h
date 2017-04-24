#pragma once

#include <QMainWindow>

class QTextEdit;
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

private:
	Ui::MainWindow *ui;

	QTextEdit* _shaderEditorWidget;
	ShaderRenderWidget* _renderWidget;
};

