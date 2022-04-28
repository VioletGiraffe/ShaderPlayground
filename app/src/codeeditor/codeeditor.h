#pragma once

#include "compiler/compiler_warnings_control.h"
#include "ctextsearchwidget.h"

DISABLE_COMPILER_WARNINGS
#include <QPlainTextEdit>
RESTORE_COMPILER_WARNINGS

class QPaintEvent;
class QResizeEvent;
class QSize;
class QWidget;

class LineNumberArea;
class CodeEditor;
class CTextSearchWidget;

class CodeEditorWithSearch final : public QWidget, public TextSearchCallbacks
{
public:
	explicit CodeEditorWithSearch(QWidget* parent = nullptr);
	CodeEditor* editor() const;

	void findPrevious(const QString& what, const TextSearchOptions options = TextSearchOptions()) override;
	void findNext(const QString& what, const TextSearchOptions options = TextSearchOptions()) override;

	void replaceNext(const QString& what, const QString& with, const TextSearchOptions options = TextSearchOptions()) override;
	void replaceAll(const QString& what, const QString& with, const TextSearchOptions options = TextSearchOptions()) override;

private:
	CodeEditor* _editor = nullptr;
	CTextSearchWidget* _searchWidget = nullptr;
};


class CodeEditor final : public QPlainTextEdit
{
public:
	CodeEditor(QWidget *parent = nullptr);

	void lineNumberAreaPaintEvent(QPaintEvent *event);
	int lineNumberAreaWidth();

	void setTextBackgroundColor(const QColor& color);

protected:
	void resizeEvent(QResizeEvent *event) override;
	void keyPressEvent(QKeyEvent* event) override;

private:
	void updateLineNumberAreaWidth(int newBlockCount);
	void highlightCurrentLine();
	void updateLineNumberArea(const QRect &, int);

	void applyTextBackgroundColor();

private:
	QColor _textBgColor;
	QWidget* _lineNumberArea = nullptr;
};


class LineNumberArea final : public QWidget
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

