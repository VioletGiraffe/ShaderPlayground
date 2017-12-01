#include "codeeditor.h"
#include "ctextsearchwidget.h"

DISABLE_COMPILER_WARNINGS
#include <QPainter>
#include <QTextBlock>
#include <QVBoxLayout>
RESTORE_COMPILER_WARNINGS


CodeEditorWithSearch::CodeEditorWithSearch(QWidget* parent) : QWidget(parent)
{
	_editor = new CodeEditor;
	_searchWidget = new CTextSearchWidget;
	_searchWidget->setVisible(false);

	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->setContentsMargins(0, 0, 0, 0);
	layout->setSpacing(0);
	layout->addWidget(_editor);
	layout->addWidget(_searchWidget);

	setLayout(layout);
}

CodeEditor* CodeEditorWithSearch::editor() const
{
	return _editor;
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
	int numDigits = 1;
	for (int max = qMax(1, blockCount()); max >= 10; max /= 10)
		++numDigits;

	const int digitWidth = fontMetrics().width(QLatin1Char('9'));
	return 10 + digitWidth * numDigits;
}

void CodeEditor::setTextBackgroundColor(const QColor& color)
{
	_textBgColor = color;
	applyTextBackgroundColor();

	auto lineNumberArea = _lineNumberArea->palette();
	lineNumberArea.setColor(QPalette::Background, color);
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
	painter.fillRect(event->rect(), _lineNumberArea->palette().background());


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
