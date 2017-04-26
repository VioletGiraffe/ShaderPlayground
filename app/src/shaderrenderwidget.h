#pragma once

#include "system/ctimeelapsed.h"

#include <QMutex>
#include <QOpenGLFunctions>
#include <QOpenGLShader>
#include <QOpenGLShaderProgram>
#include <QOpenGLWidget>
#include <QTimer>

#include <memory>

class ShaderRenderWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
public:
	explicit ShaderRenderWidget(QWidget *parent = 0);
	~ShaderRenderWidget();

	QString setFragmentShader(const QString& shaderSource);

protected:
	void showEvent(QShowEvent *event) override;

	void initializeGL() override;
	void resizeGL(int w, int h) override;
	void paintGL() override;

private:
	QMutex _shaderProgramMutex;

	QTimer timer;
	CTimeElapsed _timeSinceLastFrame, _totalRunTime;
	std::unique_ptr<QOpenGLShaderProgram> _program;
	std::unique_ptr<QOpenGLShader> _fragmentShader;
};
