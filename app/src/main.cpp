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

	qDebug() << "Built with Qt version" << QT_VERSION_STR;

	if(QString(qVersion()) != QT_VERSION_STR)
	{
		qDebug() << "Running with Qt version" << qVersion();
		assert_unconditional_r("Current Qt version doesn't match the one application was built with.");
	}

	QApplication::setAttribute(Qt::AA_UseDesktopOpenGL);

	QSurfaceFormat format;
	format.setSwapInterval(0);
	QSurfaceFormat::setDefaultFormat(format);

	QApplication app(argc, argv);
	app.setOrganizationName("GitHubSoft");
	app.setApplicationName("Shader Playground");

	MainWindow window;
	window.show();

	return app.exec();
}
