#include "shaderrenderwidget.h"

#include <QDebug>
#include <QMatrix4x4>
#include <QMouseEvent>

#define LogGlError \
{\
	const auto err = glGetError();	\
	if (err != GL_NO_ERROR)	\
		qDebug() << "GL error" << err << "at" << __FUNCTION__ << __LINE__;	\
}

ShaderRenderWidget::ShaderRenderWidget(QWidget *parent) : QOpenGLWidget(parent)
{
}

ShaderRenderWidget::~ShaderRenderWidget()
{
}

QString ShaderRenderWidget::setFragmentShader(const QString& shaderSource)
{
	QMutexLocker locker(&_shaderProgramMutex);

	if (_program->isLinked())
		_program->removeShader(_fragmentShader.get());

	if (!_fragmentShader->compileSourceCode(shaderSource))
		return _fragmentShader->log();

	if (!_program->addShader(_fragmentShader.get()))
		qDebug() << "Failed to add fragment shader:\n" << _program->log();

	if (!_program->link())
		qDebug() << "Failed to link the program\n:" << _program->log();

	return _program->log();
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

	connect(&timer, &QTimer::timeout, this, [this](){update();});
	timer.start(10);
}

void ShaderRenderWidget::resizeGL(int w, int h)
{
	// TODO: manually calling glViewport() has no effect in QOpenGLWidget?
//	const auto scale = devicePixelRatio();
//	glViewport(0, 0, scale * w, scale * h);
}

void ShaderRenderWidget::paintGL()
{
#ifndef _WIN32
	QMutexLocker locker(&_shaderProgramMutex);
#endif

	if (!_program || !_program->isLinked())
		return;

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
	_program->setUniformValue("screenSize", size());
	LogGlError;
	_program->setUniformValue("mousePosition", mapFromGlobal(QCursor::pos()));
	LogGlError;

	_timeSinceLastFrame.start();
	_program->setUniformValue("frameTime", (uint32_t)_timeSinceLastFrame.elapsed());
	LogGlError;
	_program->setUniformValue("totalTime", (uint32_t)_totalRunTime.elapsed());
	LogGlError;

	glDrawArrays(GL_TRIANGLES, 0, 6);
	LogGlError;

	_program->disableAttributeArray("vertexPosition");
	LogGlError;
}
