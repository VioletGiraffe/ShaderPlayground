#include "mainwindow.h"
#include "ui_mainwindow.h"

static const char* fshader =
	"#ifdef GL_ES\n"
	"precision highp int;\n"
	"precision highp float;\n"
	"#endif\n"

	"void main()\n"
	"{\n"
		"\tgl_FragColor = vec4(1.0, 1.0, 0.0, 1.0);\n"
	"}\n";

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);

	ui->fragmentShaderText->setText(fshader);
	ui->fragmentShaderText->setTabStopWidth(3 * QFontMetrics(ui->fragmentShaderText->font()).width(' '));
	connect(ui->fragmentShaderText, &QTextEdit::textChanged, this, &MainWindow::updateFragmentShader);
}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::showEvent(QShowEvent* e)
{
	updateFragmentShader();
	QMainWindow::showEvent(e);
}

void MainWindow::updateFragmentShader()
{
	ui->glWidget->setFragmentShader(ui->fragmentShaderText->toPlainText());
}
