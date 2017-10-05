#pragma once

#include "system/ctimeelapsed.h"
#include "compiler/compiler_warnings_control.h"

DISABLE_COMPILER_WARNINGS
#include <QMutex>
#include <QOpenGLFunctions>
#include <QOpenGLShader>
#include <QOpenGLShaderProgram>
#include <QOpenGLWidget>
#include <QTimer>
RESTORE_COMPILER_WARNINGS

#include <memory>

class ShaderRenderWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
public:
	explicit ShaderRenderWidget(QWidget *parent = 0);
	~ShaderRenderWidget();

	// Returns the compilation error message, if any
	QString setFragmentShader(const QString& shaderSource);

	float frameRenderingPeriod() const;

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

	float _frameRenderingPeriod = 0.0f;
};
