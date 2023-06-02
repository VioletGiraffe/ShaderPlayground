#include "mainwindow.h"
#include "assert/advanced_assert.h"

DISABLE_COMPILER_WARNINGS
#include <QApplication>
#include <QDebug>
#include <QSurfaceFormat>
RESTORE_COMPILER_WARNINGS

// These declaration tell the video driver to provide maximum graphics performance for this application
extern "C"
{
	__declspec(dllexport) uint32_t NvOptimusEnablement = 1;
	__declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}

static QtMessageHandler g_qtMessageHandler = nullptr;

static void myMessageOutput(QtMsgType type, const QMessageLogContext& context, const QString& msg)
{
	g_qtMessageHandler(type, context, msg);
}

int main(int argc, char *argv[])
{
	g_qtMessageHandler = qInstallMessageHandler(&myMessageOutput);
	qSetMessagePattern("%{function}: %{message}");

	AdvancedAssert::setLoggingFunc([](const char* message) {
		qDebug() << message;
	});

	qDebug() << "Built with Qt version" << QT_VERSION_STR;

	if(QStringLiteral(QT_VERSION_STR) != qVersion())
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
