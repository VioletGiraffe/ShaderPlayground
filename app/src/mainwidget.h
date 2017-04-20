#pragma once

#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLWidget>
#include <QTimer>
#include <QVector2D>

class MainWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
public:
	explicit MainWidget(QWidget *parent = 0);
	~MainWidget();

protected:
	void mouseMoveEvent(QMouseEvent *e) override;

	void initializeGL() override;
	void resizeGL(int w, int h) override;
	void paintGL() override;

	void initShaders();

private:
	QTimer timer;
	QOpenGLShaderProgram program;

	QVector2D mousePosition;
};
