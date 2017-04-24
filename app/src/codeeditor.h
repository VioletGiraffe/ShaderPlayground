#pragma once

#include <QPlainTextEdit>

class QPaintEvent;
class QResizeEvent;
class QSize;
class QWidget;

class LineNumberArea;


class CodeEditor : public QPlainTextEdit
{
public:
	CodeEditor(QWidget *parent = 0);

	void lineNumberAreaPaintEvent(QPaintEvent *event);
	int lineNumberAreaWidth();

protected:
	void resizeEvent(QResizeEvent *event) override;

private slots:
	void updateLineNumberAreaWidth(int newBlockCount);
	void highlightCurrentLine();
	void updateLineNumberArea(const QRect &, int);

private:
	QWidget* lineNumberArea = nullptr;
};


class LineNumberArea : public QWidget
{
public:
	inline LineNumberArea(CodeEditor *editor) : QWidget(editor), codeEditor(editor) {
	}

	inline QSize sizeHint() const override {
		return QSize(codeEditor->lineNumberAreaWidth(), 0);
	}

protected:
	inline void paintEvent(QPaintEvent *event) override {
		codeEditor->lineNumberAreaPaintEvent(event);
	}

private:
	CodeEditor* const codeEditor = nullptr;
};

