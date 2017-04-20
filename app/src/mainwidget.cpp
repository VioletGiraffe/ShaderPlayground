#include "mainwidget.h"

#include <QDebug>
#include <QMatrix4x4>
#include <QMouseEvent>

#define LogGlError \
{\
	const auto err = glGetError();	\
	if (err != GL_NO_ERROR)	\
		qDebug() << "GL error" << err << "at" << __FUNCTION__ << __LINE__;	\
}

MainWidget::MainWidget(QWidget *parent) : QOpenGLWidget(parent)
{
}

MainWidget::~MainWidget()
{
}

static const char* vshader =
	"#ifdef GL_ES\n"
	"precision highp int;\n"
	"precision highp float;\n"
	"#endif\n"

	"attribute highp vec4 vertexPosition;\n"
	"uniform highp mat4 matrix;\n"

	"void main()\n"
	"{\n"
	"gl_Position = matrix * vertexPosition;\n"
	"}\n";


void MainWidget::setFragmentShader(const QString& shaderSource)
{
	QMutexLocker locker(&m);

	if (_program)
	{
		_program->release();
		_program->removeAllShaders();
	}

	_program = std::make_unique<QOpenGLShaderProgram>();
	_fragmentShader = std::make_unique<QOpenGLShader>(QOpenGLShader::Fragment);


	if (!_fragmentShader->compileSourceCode(shaderSource))
	{
		qDebug() << "Error compiling fragment shader:";
		qDebug() << _fragmentShader->log();
		return;
	}

	if (!_program->addShader(_fragmentShader.get()))
		qDebug() << "Failed to add fragment shader:\n" << _program->log();

	// Compile vertex shader
	if (!_program->addShaderFromSourceCode(QOpenGLShader::Vertex, vshader))
		qDebug() << "Failed to add vertex shader:\n" << _program->log();

	if (!_program->link())
		qDebug() << "Failed to link the program\n:" << _program->log();

	if (!_program->bind())
		qDebug() << "Failed to bind the program\n:" << _program->log();
}

void MainWidget::mouseMoveEvent(QMouseEvent* e)
{
	mousePosition = QVector2D(e->localPos());
}

void MainWidget::initializeGL()
{
	initializeOpenGLFunctions();

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	LogGlError;

	connect(&timer, &QTimer::timeout, this, [this](){update();});
	timer.start(10);
}

void MainWidget::resizeGL(int w, int h)
{
	// TODO: manually calling glViewport() has no effect in QOpenGLWidget?
//	const auto scale = devicePixelRatio();
//	glViewport(0, 0, scale * w, scale * h);
}

void MainWidget::paintGL()
{
	QMutexLocker locker(&m);

	if (!_program || !_program->isLinked())
		return;

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

	glDrawArrays(GL_TRIANGLES, 0, 6);
	LogGlError;

	_program->disableAttributeArray("vertexPosition");
	LogGlError;
}
