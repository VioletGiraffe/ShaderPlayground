#include <QApplication>
#include <QSurfaceFormat>

#include "mainwidget.h"

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);

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

	MainWidget widget;
	widget.show();

	return app.exec();
}
