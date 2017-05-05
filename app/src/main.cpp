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

//	QApplication::setAttribute(Qt::AA_UseOpenGLES);
// 	QSurfaceFormat format;
// 	format.setVersion(3, 0);
// 	QSurfaceFormat::setDefaultFormat(format);

	QApplication app(argc, argv);
	app.setOrganizationName("GitHubSoft");
	app.setApplicationName("Shader Playground");

	MainWindow window;
	window.show();

	return app.exec();
}
