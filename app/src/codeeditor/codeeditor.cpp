#include "codeeditor.h"

#include "string/stringutils.h"
#include "assert/advanced_assert.h"
#include "math/math.hpp"

DISABLE_COMPILER_WARNINGS
#include <QPainter>
#include <QShortcut>
#include <QTextBlock>
#include <QVBoxLayout>
RESTORE_COMPILER_WARNINGS

#include <algorithm>


CodeEditorWithSearch::CodeEditorWithSearch(QWidget* parent) : QWidget(parent)
{
	_editor = new CodeEditor;
	_searchWidget = new CTextSearchWidget;
	_searchWidget->setCallbackReceiver(this);
	_searchWidget->hide();

	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->setContentsMargins(0, 0, 0, 0);
	layout->setSpacing(0);
	layout->addWidget(_editor);
	layout->addWidget(_searchWidget);

	setLayout(layout);

	auto shortcutShowSearchWidget = new QShortcut(QKeySequence("Ctrl+F"), this);
	connect(shortcutShowSearchWidget, &QShortcut::activated, this, [this]() {
		_searchWidget->setShowReplaceUI(false);
		_searchWidget->show();
	});
}

CodeEditor* CodeEditorWithSearch::editor() const
{
	return _editor;
}


void CodeEditorWithSearch::findPrevious(const QString& what, const TextSearchOptions options)
{
	QTextDocument::FindFlags flags = QTextDocument::FindBackward;
	if (options.caseSensitive) flags |= QTextDocument::FindCaseSensitively;
	_editor->find(what, flags);
}

void CodeEditorWithSearch::findNext(const QString& what, const TextSearchOptions options)
{
	QTextDocument::FindFlags flags;
	if (options.caseSensitive) flags |= QTextDocument::FindCaseSensitively;
	_editor->find(what, flags);
}

void CodeEditorWithSearch::findAll(const QString& what, const TextSearchOptions options)
{

}

void CodeEditorWithSearch::replaceNext(const QString& what, const QString& with, const TextSearchOptions options)
{

}

void CodeEditorWithSearch::replaceAll(const QString& what, const QString& with, const TextSearchOptions options)
{

}

CodeEditor::CodeEditor(QWidget *parent) : QPlainTextEdit(parent)
{
	_lineNumberArea = new LineNumberArea(this);

	connect(this, &QPlainTextEdit::blockCountChanged, this, &CodeEditor::updateLineNumberAreaWidth);
	connect(this, &QPlainTextEdit::updateRequest, this, &CodeEditor::updateLineNumberArea);
	connect(this, &QPlainTextEdit::cursorPositionChanged, this, &CodeEditor::highlightCurrentLine);
	connect(this, &QPlainTextEdit::textChanged, this, &CodeEditor::applyTextBackgroundColor); // Fix for #6

	updateLineNumberAreaWidth(0);
	highlightCurrentLine();
}

int CodeEditor::lineNumberAreaWidth()
{
	int numDigits = Math::ceil<int>(log10f((float)std::max(1, document()->lineCount())));

	const int digitWidth = fontMetrics().horizontalAdvance('9');
	return 10 + digitWidth * numDigits;
}

void CodeEditor::setTextBackgroundColor(const QColor& color)
{
	_textBgColor = color;
	applyTextBackgroundColor();

	auto lineNumberArea = _lineNumberArea->palette();
	lineNumberArea.setColor(QPalette::Window, color);
	_lineNumberArea->setPalette(lineNumberArea);
}

void CodeEditor::updateLineNumberAreaWidth(int /* newBlockCount */)
{
	setViewportMargins(lineNumberAreaWidth(), 0, 0, 0);
}

void CodeEditor::updateLineNumberArea(const QRect &rect, int dy)
{
	if (dy)
		_lineNumberArea->scroll(0, dy);
	else
		_lineNumberArea->update(0, rect.y(), _lineNumberArea->width(), rect.height());

	if (rect.contains(viewport()->rect()))
		updateLineNumberAreaWidth(0);
}

void CodeEditor::applyTextBackgroundColor()
{
	QTextCharFormat fmt;
	fmt.setBackground(QBrush(_textBgColor));
	mergeCurrentCharFormat(fmt);
}

void CodeEditor::resizeEvent(QResizeEvent *e)
{
	QPlainTextEdit::resizeEvent(e);

	QRect cr = contentsRect();
	_lineNumberArea->setGeometry(QRect(cr.left(), cr.top(), lineNumberAreaWidth(), cr.height()));
}

void CodeEditor::keyPressEvent(QKeyEvent* event)
{
	if (const auto key = event->key(); key == Qt::Key_Enter || key == Qt::Key_Return)
	{
		auto cursor = textCursor();
		const int pos = cursor.position();

		const QString text = toPlainText();
		
		const int lineStart = text.lastIndexOf('\n', std::max(pos - 1, 0)) + 1;
		assert_debug_only(pos >= lineStart);
		//QString lineBeforeCursor = text.mid(lineStart, pos - lineStart);
		const int tabulationEnd = std::find_if(text.begin() + lineStart, text.begin() + pos, [](const QChar c) {
			return c != ' ' && c != '\t';
		}) - text.begin();

		QPlainTextEdit::keyPressEvent(event);

		if (tabulationEnd > 0)
		{
			cursor = textCursor();
			assert_debug_only(tabulationEnd >= lineStart);
			cursor.insertText(text.mid(lineStart, tabulationEnd - lineStart));
		}
	}
	else
		QPlainTextEdit::keyPressEvent(event);
}

void CodeEditor::highlightCurrentLine()
{
	if (isReadOnly())
		return;

	const QColor lineColor = QColor(50, 50, 50, 127);

	QTextEdit::ExtraSelection selection;
	selection.format.setBackground(lineColor);
	selection.format.setProperty(QTextFormat::FullWidthSelection, true);
	selection.cursor = textCursor();
	selection.cursor.clearSelection();

	setExtraSelections(QList<QTextEdit::ExtraSelection>{selection});
}

void CodeEditor::lineNumberAreaPaintEvent(QPaintEvent *event)
{
	QPainter painter(_lineNumberArea);
	painter.fillRect(event->rect(), _lineNumberArea->palette().window());


	QTextBlock block = firstVisibleBlock();
	int blockNumber = block.blockNumber();
	int top = (int)blockBoundingGeometry(block).translated(contentOffset()).top();
	int bottom = top + (int)blockBoundingRect(block).height();

	while (block.isValid() && top <= event->rect().bottom())
	{
		if (block.isVisible() && bottom >= event->rect().top())
		{
			QString number = QString::number(blockNumber + 1);
			painter.setPen(_lineNumberArea->palette().text().color());
			painter.drawText(0, top, _lineNumberArea->width(), fontMetrics().height(),Qt::AlignCenter, number);
		}

		block = block.next();
		top = bottom;
		bottom = top + (int)blockBoundingRect(block).height();
		++blockNumber;
	}
}
