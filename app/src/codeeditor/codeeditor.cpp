#include "codeeditor.h"

#include "string/stringutils.h"
#include "assert/advanced_assert.h"
#include "math/math.hpp"
#include "container/algorithms.hpp"

DISABLE_COMPILER_WARNINGS
#include <QAbstractItemView>
#include <QCompleter>
#include <QDebug>
#include <QPainter>
#include <QRegularExpression>
#include <QScrollBar>
#include <QShortcut>
#include <QStringListModel>
#include <QTextBlock>
#include <QVBoxLayout>
RESTORE_COMPILER_WARNINGS

#include <algorithm>

static const QString tokenDelimiters = QStringLiteral(R"([~!@#$%?()[]"+-*/.,;:|&<>=^{} )");


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
		_searchWidget->grabFocus();
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
	connect(document(), &QTextDocument::contentsChange, this, [this](int position, int /*charsRemoved*/, int charsAdded) {
		if (charsAdded <= 0)
			return;

		applyTextBackgroundColor(); // Fix for #6, but not really

		const QString text = toPlainText();
		if (charsAdded > 1)
		{
			updateTokensList(text);
		}
		else
		{
			const auto cursor = textCursor();
			const auto pos = cursor.position();
			if (pos - 1 >= text.length())
				return;

			const auto charAtCursor = text[pos - 1];
			if (tokenDelimiters.contains(charAtCursor))
				updateTokensList(text);
		}
	}); 

	updateLineNumberAreaWidth(0);
	highlightCurrentLine();

	_completer = new QCompleter(this);
	_completer->setWidget(this);
	_completer->setCompletionMode(QCompleter::PopupCompletion);
	_completer->setCaseSensitivity(Qt::CaseInsensitive);
	_completer->setModel(new QStringListModel(_completer));
	QObject::connect(_completer, QOverload<const QString&>::of(&QCompleter::activated),	this, &CodeEditor::insertCompletion);
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

void CodeEditor::setPlainText(const QString& text)
{
	QPlainTextEdit::setPlainText(text);
	updateTokensList(text);
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
	setCurrentCharFormat(fmt);
}

inline void CodeEditor::insertCompletion(const QString& completion)
{
	if (_completer->widget() != this)
		return;

	QTextCursor tc = textCursor();
	const auto extra = completion.length() - _completer->completionPrefix().length();
	tc.movePosition(QTextCursor::Left);
	tc.movePosition(QTextCursor::EndOfWord);
	tc.insertText(completion.right(extra));
	setTextCursor(tc);
}

void CodeEditor::updateTokensList(const QString& text)
{
	static const QRegularExpression regex("["
		+ QRegularExpression::escape(tokenDelimiters) + R"(\n\t)"
		+ "]+");

	auto* model = reinterpret_cast<QStringListModel*>(_completer->model());

	QStringList tokens = text.split(regex, Qt::SkipEmptyParts);

	ContainerAlgorithms::erase_if(tokens, [](const QString& s) {
		static const QRegularExpression notANumber{ "[a-zA-Z]+" };
		return s.length() <= 2 || !s.contains(notANumber);
	});
	tokens.append(model->stringList());
	tokens.sort();
	tokens.removeDuplicates();

	//for (auto it = tokens.begin(), end = tokens.end(); it != end; ++it)
	//{
	//	for (auto itNested = it + 1; itNested != end; ++itNested)
	//	{
	//		if (itNested->startsWith(*it))
	//		{
	//			it->clear();
	//			break;
	//		}
	//	}
	//}

	model->setStringList(tokens);
}

void CodeEditor::resizeEvent(QResizeEvent *e)
{
	QPlainTextEdit::resizeEvent(e);

	const QRect cr = contentsRect();
	_lineNumberArea->setGeometry(QRect(cr.left(), cr.top(), lineNumberAreaWidth(), cr.height()));
}

void CodeEditor::keyPressEvent(QKeyEvent* event)
{
	if (_completer->popup()->isVisible())
	{
		// The following keys are forwarded by the completer to the widget
		switch (event->key()) {
		case Qt::Key_Enter:
		case Qt::Key_Return:
		case Qt::Key_Escape:
		case Qt::Key_Tab:
		case Qt::Key_Backtab:
			event->ignore();
			return; // let the completer do default behavior
		default:
			break;
		}
	}

	static constexpr auto isTabulation = [](QChar c) {
		return c == '\t' || c == ' ';
	};

	const auto tabulationForCurrentLine = [this](const QString& text) -> std::pair<qsizetype /* startPos */, QString> {
		auto cursor = textCursor();
		const int pos = cursor.position();

		const auto lineStart = text.lastIndexOf('\n', std::max(pos - 1, 0)) + 1;
		if (pos < lineStart) // Can happen when pressing Enter at the start of the file
			return { -1, QString{} };

		const qsizetype tabulationEnd = static_cast<qsizetype>(
			std::find_if(text.begin() + lineStart, text.begin() + pos, [](const QChar c) {
				return !isTabulation(c);
			}) - text.begin()
		);

		return { lineStart, text.mid(lineStart, tabulationEnd - lineStart) };
	};

	if (const auto key = event->key(); key == Qt::Key_Enter || key == Qt::Key_Return)
	{
		const QString text = toPlainText();
		const auto pos = textCursor().position();
		const bool addNewTab = pos > 0 && text[pos - 1] == '{';

		const auto [lineStartPos, tabulation] = tabulationForCurrentLine(text);

		QPlainTextEdit::keyPressEvent(event);

		if (!tabulation.isEmpty())
		{
			auto cursor = textCursor();
			cursor.insertText(tabulation);
		}

		if (addNewTab)
			textCursor().insertText(QString{ '\t' });
	}
	else if (event->key() == Qt::Key_Backtab)
	{
		auto cursor = textCursor();
		const auto text = toPlainText();
		if (cursor.movePosition(QTextCursor::StartOfLine))
		{
			const auto charAtCursor = text.at(cursor.position());
			if (isTabulation(charAtCursor))
			{
				cursor.deleteChar();
			}
		}
		else if (!text.isEmpty())
		{
			const auto charAtCursor = text.at(cursor.position());
			if (isTabulation(charAtCursor))
				cursor.deleteChar();
		}

		event->accept();
	}
	else if (event->key() == Qt::Key_BraceRight)
	{
		const auto text = toPlainText();
		const auto [lineStartPos, tabulation] = tabulationForCurrentLine(text);
		auto cursor = textCursor();
		if (tabulation.length() > 0 && lineStartPos + tabulation.length() == cursor.position())
			cursor.deletePreviousChar();

		QPlainTextEdit::keyPressEvent(event);
	}
	else
		QPlainTextEdit::keyPressEvent(event);

	// Completer handling
	const auto mods = event->modifiers();

	static const QString eow("~!@#$%^&|*()_+{}|:\"<>?,./;'[]\\-="); // end of word

	QTextCursor tc = textCursor();
	tc.select(QTextCursor::WordUnderCursor);
	const QString completionPrefix = tc.selectedText();

	bool isShortcut = false;
	const bool hasModifier = mods != Qt::NoModifier;
	if (!isShortcut && (hasModifier || event->text().isEmpty() || completionPrefix.length() < 1
		|| eow.contains(event->text().right(1))))
	{
		_completer->popup()->hide();
		return;
	}

	if (completionPrefix != _completer->completionPrefix())
	{
		_completer->setCompletionPrefix(completionPrefix);
		_completer->popup()->setCurrentIndex(_completer->completionModel()->index(0, 0));
	}

	QRect cr = cursorRect();
	cr.setWidth(_completer->popup()->sizeHintForColumn(0) + _completer->popup()->verticalScrollBar()->sizeHint().width());
	_completer->complete(cr); // popup it up!
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
