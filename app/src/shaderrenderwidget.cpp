#include "shaderrenderwidget.h"

#include "utils/resources.h"

#include "assert/advanced_assert.h"

DISABLE_COMPILER_WARNINGS
#include <QApplication>
#include <QDate>
#include <QDebug>
#include <QElapsedTimer>
#include <QMatrix4x4>
#include <QMouseEvent>
#include <QOpenGLDebugLogger>
#include <QOpenGLShader>
#include <QOpenGLShaderProgram>
#include <QTimer>
#include <QVector3D>
RESTORE_COMPILER_WARNINGS

static constexpr int TargetFPS = 60;

//#define LogGlError \
//{\
//	const auto err = this->glGetError(); \
//	if (err != GL_NO_ERROR) \
//		qDebug() << "GL error" << err << "at" << __FUNCTION__ << __LINE__;\
//}

#define LogGlError (void)0

ShaderRenderWidget::ShaderRenderWidget(QWidget *parent) :
	QOpenGLWidget(parent)
{
	const auto date = QDate::currentDate();
	_day = static_cast<float>(date.day());
	_month = static_cast<float>(date.month());
	_year = static_cast<float>(date.year());

	_timer = new QTimer(this);

	auto fmt = QSurfaceFormat::defaultFormat();
	fmt.setProfile(QSurfaceFormat::CompatibilityProfile);
	fmt.setVersion(3, 1);
#ifdef _DEBUG
	fmt.setOption(QSurfaceFormat::DebugContext);
#endif
	setFormat(fmt);

	if (const auto actualFmt = format(); actualFmt != fmt)
	{
		qDebug() << "Failed to set the requested format; actual format:" << actualFmt;
		assert_unconditional_r("Failed to set the requested format");
	}

	_lastWorkingShaderCode = textFromResource(":/resources/blank_shader.fsh");
}

ShaderRenderWidget::~ShaderRenderWidget()
{
	// Unbind the manual binds
	this->glBindVertexArray(0);
	this->glBindBuffer(GL_ARRAY_BUFFER, 0);
}

// Returns the compilation error message, if any
QString ShaderRenderWidget::setFragmentShader(const QString& shaderSource)
{
	std::lock_guard locker{ _shaderProgramMutex };

	// Intel driver crashes when calling program->bind() after updating the frag shader, so I must re-create the whole program instead.
	// It is, unfortunately, quite slow and makes typing difficult, especially since the mutex has to be locked for the duration.

#ifdef _WIN32
	static constexpr bool isWindows = true;
#else
	static constexpr bool isWindows = false;
#endif

	if (isWindows && _gpuName.contains("Intel"))
	{
		_program = std::make_unique<QOpenGLShaderProgram>();
		assert_r(_program->addShader(_vertexShader.get()));
	}
	else // The normal way - reuse the same program object and only replace the frag shader
	{
		if (_program->isLinked())
			_program->removeShader(_fragmentShader.get());
	}

	QString shaderCompilationError;
	if (!_fragmentShader->compileSourceCode(shaderSource))
	{
		shaderCompilationError = _fragmentShader->log();
		_fragmentShader->compileSourceCode(_lastWorkingShaderCode);
	}
	else
		_lastWorkingShaderCode = shaderSource;

	if (!_program->addShader(_fragmentShader.get()))
		return shaderCompilationError + "\nFailed to add fragment shader.\n\n" + _program->log();

	if (!_program->link())
		return shaderCompilationError + "\nFailed to link the program.\n\n" + _program->log();

	return shaderCompilationError;
}

float ShaderRenderWidget::frameRenderingPeriod() const
{
	return _frameRenderingPeriod;
}

float ShaderRenderWidget::frameTimeMs() const noexcept
{
	return _frameTimeNanosec * 1e-6f;
}

QString ShaderRenderWidget::gpuName() const
{
	return _gpuName;
}

void ShaderRenderWidget::showEvent(QShowEvent *event)
{
	QOpenGLWidget::showEvent(event);
	_totalRunTime.start();
	_timeSinceLastFrame.start();
}

void ShaderRenderWidget::initializeGL()
{
	std::lock_guard locker{ _shaderProgramMutex };

	assert_r(initializeOpenGLFunctions());

	_gpuName = (const char*)this->glGetString(GL_RENDERER);

	qDebug() << '\n'
		<< _gpuName << '\n'
		<< (const char*)this->glGetString(GL_VERSION);

	_vertexShader = std::make_unique<QOpenGLShader>(QOpenGLShader::Vertex);
	if (!_vertexShader->compileSourceFile(":/resources/default_vertex_shader.vsh"))
		qDebug() << "Failed to add vertex shader:\n" << _program->log();

	_program = std::make_unique<QOpenGLShaderProgram>();
	assert_r(_program->addShader(_vertexShader.get()));

	_fragmentShader = std::make_unique<QOpenGLShader>(QOpenGLShader::Fragment);

	connect(_timer, &QTimer::timeout, this, (void (ShaderRenderWidget::*)()) &ShaderRenderWidget::update);
	_timer->start(1000 / TargetFPS);

#ifdef _DEBUG
	auto* ctx = QOpenGLContext::currentContext();
	qInfo() << ctx->format();
	if (ctx->hasExtension(QByteArrayLiteral("GL_KHR_debug")))
	{
		QOpenGLDebugLogger* logger = new QOpenGLDebugLogger(this);
		logger->initialize(); // initializes in the current context, i.e. ctx
		connect(logger, &QOpenGLDebugLogger::messageLogged, this, [](const QOpenGLDebugMessage& msg) -> int {
			qWarning() << msg;
			return 0;
		}, Qt::DirectConnection);
		logger->startLogging(QOpenGLDebugLogger::SynchronousLogging);
	}
#endif

	this->glGenBuffers(1, &_vbo);
	this->glGenVertexArrays(1, &_vao);
}

void ShaderRenderWidget::resizeGL(int w, int h)
{
	if (_lastMousePosWithButtonPressed.manhattanLength() == 0)
		_lastMousePosWithButtonPressed = QPoint{ w / 2, h / 2 };

	QOpenGLWidget::resizeGL(w, h);
}

void ShaderRenderWidget::paintGL()
{
	QElapsedTimer timer;
	timer.start();

	if (!_shaderProgramMutex.try_lock())
		return;

	std::lock_guard lock{ _shaderProgramMutex, std::adopt_lock };

	_frameRenderingPeriod = _timeSinceLastFrame.elapsed<std::chrono::microseconds>() / 1000.0f;
	_timeSinceLastFrame.start();

	if (!_program || !_program->isLinked())
	{
		this->glClear(GL_COLOR_BUFFER_BIT);
		return;
	}

	if (!_program->bind())
		qDebug() << "Failed to bind the program\n:" << _program->log();

	this->glBindBuffer(GL_ARRAY_BUFFER, _vbo);
	this->glBindVertexArray(_vao);

	const float w = (float)width(), h = (float)height();
	const GLfloat vertices[2 * 3 * 3] {
		0.0f, 0.0f, 0.0f,
		0.0f, h, 0.0f,
		w, h, 0.0f,
		w, h, 0.0f,
		w, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f
	};

	this->glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);
	this->glEnableVertexAttribArray(0);
	this->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	QMatrix4x4 pmvMatrix;
	pmvMatrix.ortho(rect());
	_program->setUniformValue("matrix", pmvMatrix);
	LogGlError;

	_program->setUniformValue("iResolution", size());
	LogGlError;

	const auto mousePos = mapFromGlobal(QCursor::pos());
	if (QApplication::mouseButtons() & (Qt::LeftButton | Qt::RightButton))
	{
		_lastMousePosWithButtonPressed = mousePos;
	}

	const auto mouseStatus = QVector4D(
		(float)mousePos.x(), (float)mousePos.y(),
		(float)_lastMousePosWithButtonPressed.x(), (float)_lastMousePosWithButtonPressed.y()
	);
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
	//this->glUseProgram(_program->programId());
	LogGlError;

	this->glDisableVertexAttribArray(0);
	LogGlError;

	// Unbind the program
	this->glUseProgram(0);

	this->glBindVertexArray(0);
	this->glBindBuffer(GL_ARRAY_BUFFER, 0);

	_frameTimeNanosec = timer.nsecsElapsed();
}
