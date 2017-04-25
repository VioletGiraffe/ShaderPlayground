#pragma once

#include <QSyntaxHighlighter>
#include <QTextCharFormat>

#include <vector>

class QTextDocument;

class ShaderSyntaxHighlighter : public QSyntaxHighlighter
{
public:
	ShaderSyntaxHighlighter(QTextDocument *parent = 0);

protected:
	void highlightBlock(const QString &text) override;

private:
	struct HighlightingRule
	{
		QRegExp pattern;
		QTextCharFormat format;
	};

	std::vector<HighlightingRule> _highlightingRules;

	QRegExp _commentStartExpression;
	QRegExp _commentEndExpression;

	QTextCharFormat _keywordFormat;
	QTextCharFormat _classFormat;
	QTextCharFormat _singleLineCommentFormat;
	QTextCharFormat _multiLineCommentFormat;
	QTextCharFormat _quotationFormat;
	QTextCharFormat _functionFormat;
};
