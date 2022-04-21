#include "shaderrenderwidget.h"
#include "regex/regex_helpers.hpp"

DISABLE_COMPILER_WARNINGS
#include <QApplication>
#include <QDate>
#include <QDebug>
#include <QMatrix4x4>
#include <QMouseEvent>
#include <QOpenGLShader>
#include <QOpenGLShaderProgram>
#include <QTimer>
#include <QVector3D>
RESTORE_COMPILER_WARNINGS

#define LogGlError \
{\
	const auto err = this->glGetError(); \
	if (err != GL_NO_ERROR) \
		qDebug() << "GL error" << err << "at" << __FUNCTION__ << __LINE__;\
}

ShaderRenderWidget::ShaderRenderWidget(QWidget *parent) :
	QOpenGLWidget(parent)
{
	const auto date = QDate::currentDate();
	_day = static_cast<float>(date.day());
	_month = static_cast<float>(date.month());
	_year = static_cast<float>(date.year());

	_timer = new QTimer(this);
}

ShaderRenderWidget::~ShaderRenderWidget()
{
}

// Returns the compilation error message, if any
QString ShaderRenderWidget::setFragmentShader(const QString& shaderSource)
{
#ifndef _WIN32
	std::lock_guard locker{ _shaderProgramMutex };
#endif

	const QString oldCode = _fragmentShader->sourceCode();

	if (_program->isLinked())
		_program->removeShader(_fragmentShader.get());

	QString shaderCompilationError;
	if (!_fragmentShader->compileSourceCode(shaderSource))
	{
		shaderCompilationError = adjustLineNumbersInTheLog(_fragmentShader->log());
		_fragmentShader->compileSourceCode(oldCode);
	}

	if (!_program->addShader(_fragmentShader.get()))
		return shaderCompilationError + "\nFailed to add fragment shader.\n\n" + adjustLineNumbersInTheLog(_program->log());
	else if (!_program->link())
		return shaderCompilationError + "\nFailed to link the program.\n\n" + adjustLineNumbersInTheLog(_program->log());

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
		<< (const char*)this->glGetString(GL_RENDERER) << '\n'
		<< (const char*)this->glGetString(GL_VERSION);

	connect(_timer, &QTimer::timeout, this, (void (ShaderRenderWidget::*)()) &ShaderRenderWidget::update);
	_timer->start(10);
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
	std::lock_guard locker{ _shaderProgramMutex };
#endif

	_frameRenderingPeriod = _timeSinceLastFrame.elapsed<std::chrono::microseconds>() / 1000.0f;
	_timeSinceLastFrame.start();

	if (!_program || !_program->isLinked())
	{
		this->glClear(GL_COLOR_BUFFER_BIT);
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

	_program->setUniformValue("iDate", QVector4D(_year, _month, _day, (float)(time(nullptr) % (24 * 3600))));
	LogGlError;

	this->glDrawArrays(GL_TRIANGLES, 0, 6);
	LogGlError;

	_program->disableAttributeArray("vertexPosition");
	LogGlError;
}

QString ShaderRenderWidget::adjustLineNumbersInTheLog(const QString& log) const
{
	static const std::wregex line_number_regex(L"\\(([0-9]+)\\) :");
	auto wlog = log.toStdWString();
	regex_helpers::regex_replace(wlog.begin(), wlog.end(), line_number_regex, [](const std::match_results<typename std::wstring::iterator>& match) {
		const auto lineNumber = QString::fromStdWString(match.str(1)).toInt();
		return QString::number(lineNumber - 10).toStdWString();
	});

	return QString::fromStdWString(wlog);
}
