#pragma once

#include <QOpenGLFunctions>
#include <QOpenGLShader>
#include <QOpenGLShaderProgram>
#include <QOpenGLWidget>
#include <QTimer>
#include <QVector2D>

#include <QMutex>

#include <memory>

class MainWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
public:
	explicit MainWidget(QWidget *parent = 0);
	~MainWidget();

	void setFragmentShader(const QString& shaderSource);

protected:
	void mouseMoveEvent(QMouseEvent *e) override;

	void initializeGL() override;
	void resizeGL(int w, int h) override;
	void paintGL() override;

private:
	QMutex m;

	QTimer timer;
	std::unique_ptr<QOpenGLShaderProgram> _program;
	std::unique_ptr<QOpenGLShader> _fragmentShader;

	QVector2D mousePosition;
};
