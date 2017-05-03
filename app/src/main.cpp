#include "shaderrenderwidget.h"
#include "mainwindow.h"
#include "assert/advanced_assert.h"

DISABLE_COMPILER_WARNINGS
#include <QApplication>
#include <QSurfaceFormat>
RESTORE_COMPILER_WARNINGS

int main(int argc, char *argv[])
{
	qSetMessagePattern("%{function} (%{file}:%{line}): %{message}");

	AdvancedAssert::setLoggingFunc([](const char* message) {
		qDebug() << message;
	});

	QApplication app(argc, argv);
	app.setOrganizationName("GitHubSoft");
	app.setApplicationName("Shader Playground");

//	QSurfaceFormat format;
//	// Request OpenGL 3.3 compatibility or OpenGL ES 3.0.
//	if (QOpenGLContext::openGLModuleType() == QOpenGLContext::LibGL)
//	{
//		qDebug() << "Requesting 3.3 compatibility context";
//		format.setVersion(3, 3);
//		format.setProfile(QSurfaceFormat::CompatibilityProfile);
//	}
//	else
//	{
//		qDebug() << "Requesting 3.0 context";
//		format.setVersion(3, 0);
//	}

//	QSurfaceFormat::setDefaultFormat(format);

	MainWindow window;
	window.show();

	return app.exec();
}
