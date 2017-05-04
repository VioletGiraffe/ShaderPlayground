#pragma once

#include "compiler/compiler_warnings_control.h"

DISABLE_COMPILER_WARNINGS
#include <QPlainTextEdit>
RESTORE_COMPILER_WARNINGS

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

	void setTextBackgroundColor(const QColor& color);

protected:
	void resizeEvent(QResizeEvent *event) override;

private:
	void updateLineNumberAreaWidth(int newBlockCount);
	void highlightCurrentLine();
	void updateLineNumberArea(const QRect &, int);

	void applyTextBackgroundColor();

private:
	QColor _textBgColor;
	QWidget* _lineNumberArea = nullptr;
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

