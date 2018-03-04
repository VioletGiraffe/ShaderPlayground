#include "shaderrenderwidget.h"
#include "utility/on_scope_exit.hpp"

DISABLE_COMPILER_WARNINGS
#include <QApplication>
#include <QDebug>
#include <QMatrix4x4>
#include <QMouseEvent>
#include <QStringBuilder>
#include <QVector3D>
RESTORE_COMPILER_WARNINGS

#include <regex>

#define LogGlError \
{\
	const auto err = glGetError();	\
	if (err != GL_NO_ERROR)	\
		qDebug() << "GL error" << err << "at" << __FUNCTION__ << __LINE__;\
}

ShaderRenderWidget::ShaderRenderWidget(QWidget *parent) : QOpenGLWidget(parent)
{
}

ShaderRenderWidget::~ShaderRenderWidget()
{
}

// Returns the compilation error message, if any
QString ShaderRenderWidget::setFragmentShader(const QString& shaderSource)
{
	QMutexLocker locker(&_shaderProgramMutex);

	const QString oldCode = _fragmentShader->sourceCode();

	if (_program->isLinked())
		_program->removeShader(_fragmentShader.get());

	QString shaderCompilationError;
	if (!_fragmentShader->compileSourceCode(shaderSource))
	{
		shaderCompilationError = _fragmentShader->log();
		_fragmentShader->compileSourceCode(oldCode);
	}

	if (!_program->addShader(_fragmentShader.get()))
		return "Failed to add fragment shader.\n\n" % _program->log();

	if (!_program->link())
		return "Failed to link the program.\n\n" % _program->log();

	return shaderCompilationError;
}

float ShaderRenderWidget::frameRenderingPeriod() const
{
	return _frameRenderingPeriod;
}

void ShaderRenderWidget::showEvent(QShowEvent *event)
{
	QOpenGLWidget::showEvent(event);
	_totalRunTime.start();
	_timeSinceLastFrame.start();
}

void ShaderRenderWidget::initializeGL()
{
	initializeOpenGLFunctions();

	_program = std::make_unique<QOpenGLShaderProgram>();
	_fragmentShader = std::make_unique<QOpenGLShader>(QOpenGLShader::Fragment);

	if (!_program->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/resources/default_vertex_shader.vsh"))
		qDebug() << "Failed to add vertex shader:\n" << _program->log();

	qDebug() << '\n'
		<< (const char*)glGetString(GL_RENDERER) << '\n'
		<< (const char*)glGetString(GL_VERSION);

	connect(&timer, &QTimer::timeout, this, [this](){update();});
	timer.start(10);
}

void ShaderRenderWidget::resizeGL(int w, int h)
{
	QOpenGLWidget::resizeGL(w, h);
	// TODO: manually calling glViewport() has no effect in QOpenGLWidget?
//	const auto scale = devicePixelRatio();
//	glViewport(0, 0, scale * w, scale * h);
}

void ShaderRenderWidget::paintGL()
{
#ifndef _WIN32
	QMutexLocker locker(&_shaderProgramMutex);
#endif

	_frameRenderingPeriod = _timeSinceLastFrame.elapsed<std::chrono::microseconds>() / 1000.0f;
	_timeSinceLastFrame.start();

	if (!_program || !_program->isLinked())
	{
		glClear(GL_COLOR_BUFFER_BIT);
		return;
	}

	if (!_program->bind())
		qDebug() << "Failed to bind the program\n:" << _program->log();

	const float w = (float)width(), h = (float)height();
	const GLfloat vertices[2 * 3 * 3] = {
		0.0f, 0.0f, 0.0f,
		0.0f, h, 0.0f,
		w, h, 0.0f,
		w, h, 0.0f,
		w, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f
	};

	QMatrix4x4 pmvMatrix;
	pmvMatrix.ortho(rect());

	_program->enableAttributeArray("vertexPosition");
	LogGlError;

	_program->setAttributeArray("vertexPosition", vertices, 3);
	LogGlError;
	_program->setUniformValue("matrix", pmvMatrix);
	LogGlError;
	_program->setUniformValue("iResolution", size());
	LogGlError;
	auto mouseStatus = QVector4D(QVector2D(mapFromGlobal(QCursor::pos())), -1.0f, -1.0f);
	if (QApplication::mouseButtons() & (Qt::LeftButton | Qt::RightButton))
	{
		mouseStatus.setZ(mouseStatus.x());
		mouseStatus.setW(mouseStatus.y());
	}

	_program->setUniformValue("mousePosition", mouseStatus);
	LogGlError;

	_program->setUniformValue("frameTime", _frameRenderingPeriod);
	LogGlError;
	_program->setUniformValue("totalTime", (float)_totalRunTime.elapsed());
	LogGlError;

	_program->setUniformValue("iFrame", _frameCounter++);
	LogGlError;

	_program->setUniformValue("iDate", QVector4D((float)_date.year(), (float)_date.month(), (float)_date.day(), (float)(time(nullptr) % (24 * 3600))));
	LogGlError;

	glDrawArrays(GL_TRIANGLES, 0, 6);
	LogGlError;

	_program->disableAttributeArray("vertexPosition");
	LogGlError;
}

QString adjustLineNumbersInTheLog(const QString& log)
{
	static const std::wregex line_number_regex(L"\\(([0-9]+)\\) :");
	std::wcmatch m;
	std::regex_search((const wchar_t*)log.utf16(), m, line_number_regex);
	if (m.size() == 2)
	{
		const auto lineNumber = QString::fromUtf16((const char16_t*)m[1].first, m[1].second - m[1].first).toInt();
	}

	return log;
}
