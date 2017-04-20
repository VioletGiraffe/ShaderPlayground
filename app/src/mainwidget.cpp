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

void MainWidget::mouseMoveEvent(QMouseEvent* e)
{
	mousePosition = QVector2D(e->localPos());
}


void MainWidget::initializeGL()
{
	initializeOpenGLFunctions();

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	initShaders();

	connect(&timer, &QTimer::timeout, this, [this](){update();});
	timer.start(10);
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

static const char* fshader =
	"#ifdef GL_ES\n"
	"precision highp int;\n"
	"precision highp float;\n"
	"#endif\n"

	"void main()\n"
	"{\n"
		"gl_FragColor = vec4(1.0, 1.0, 0.0, 1.0);\n"
	"}\n";



void MainWidget::initShaders()
{
	// Compile vertex shader
	if (!program.addShaderFromSourceCode(QOpenGLShader::Vertex, vshader))
		close();

	// Compile fragment shader
	if (!program.addShaderFromSourceCode(QOpenGLShader::Fragment, fshader))
		close();

	// Link shader pipeline
	if (!program.link())
		close();

	const QString log = program.log();
	if (!log.isEmpty())
		qDebug() << log;

	// Bind shader pipeline for use
	if (!program.bind())
		close();
}

void MainWidget::resizeGL(int w, int h)
{
	// TODO: has no effect in QOpenGLWidget?
//	const auto scale = devicePixelRatio();
//	glViewport(0, 0, scale * w, scale * h);
}

void MainWidget::paintGL()
{
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

	program.enableAttributeArray("vertexPosition");
	LogGlError;

	program.setAttributeArray("vertexPosition", vertices, 3);
	LogGlError;
	program.setUniformValue("matrix", pmvMatrix);
	LogGlError;

	glDrawArrays(GL_TRIANGLES, 0, 6);
	LogGlError;

	program.disableAttributeArray("vertexPosition");
	LogGlError;
}
