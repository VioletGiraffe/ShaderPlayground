#pragma once

#include "system/ctimeelapsed.h"
#include "compiler/compiler_warnings_control.h"

DISABLE_COMPILER_WARNINGS
#include <QOpenGLFunctions>
#include <QOpenGLWidget>
RESTORE_COMPILER_WARNINGS

#include <memory>
#include <mutex>

class QOpenGLShader;
class QOpenGLShaderProgram;
class QTimer;

class ShaderRenderWidget final : public QOpenGLWidget, protected QOpenGLFunctions
{
public:
	explicit ShaderRenderWidget(QWidget *parent = nullptr);
	~ShaderRenderWidget() override; // Do not remove, required because of forward-declaring unique_ptr type

	// Returns the compilation error message, if any
	[[nodiscard]] QString setFragmentShader(const QString& shaderSource);

	[[nodiscard]] float frameRenderingPeriod() const;
	[[nodiscard]] QString gpuName() const;

protected:
	void showEvent(QShowEvent *event) override;

	void initializeGL() override;
	void paintGL() override;

private:
	std::mutex _shaderProgramMutex;

	QString _gpuName;

	QString _lastWorkingShaderCode;

	CTimeElapsed _timeSinceLastFrame, _totalRunTime;
	std::unique_ptr<QOpenGLShaderProgram> _program;
	std::unique_ptr<QOpenGLShader> _fragmentShader;

	QTimer* _timer = nullptr;

	float _frameRenderingPeriod = 0.0f;
	int _frameCounter = 0;

	float _day, _month, _year;
};
