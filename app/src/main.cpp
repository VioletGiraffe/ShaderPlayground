#include "shaderrenderwidget.h"
#include "mainwindow.h"
#include "assert/advanced_assert.h"

DISABLE_COMPILER_WARNINGS
#include <QApplication>
#include <QSurfaceFormat>
RESTORE_COMPILER_WARNINGS

extern "C"
{
	__declspec(dllexport) unsigned long NvOptimusEnablement = 1;
	__declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}

int main(int argc, char *argv[])
{
	qSetMessagePattern("%{function} (%{file}:%{line}): %{message}");

	AdvancedAssert::setLoggingFunc([](const char* message) {
		qDebug() << message;
	});

	qDebug() << "Running with Qt version" << qVersion();
	qDebug() << "Built with Qt version" << QT_VERSION_STR;

	assert_r(QString(qVersion()) == QT_VERSION_STR);

	QApplication::setAttribute(Qt::AA_UseDesktopOpenGL);

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
