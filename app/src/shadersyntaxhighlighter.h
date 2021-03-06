#pragma once

#include "compiler/compiler_warnings_control.h"

DISABLE_COMPILER_WARNINGS
#include <QSyntaxHighlighter>
RESTORE_COMPILER_WARNINGS

#include <vector>

class ShaderSyntaxHighlighter: public QSyntaxHighlighter
{
public:
	ShaderSyntaxHighlighter(QTextDocument *parent = 0);

protected:
	void highlightBlock(const QString &text) override;

private:
	void addPatternFromList(QStringList &list, QTextCharFormat &format);

	struct HighlightingRule {
		QRegExp pattern;
		QTextCharFormat format;
	};

	std::vector<HighlightingRule> _highlightingRules;

	QRegExp _commentStartExpression;
	QRegExp _commentEndExpression;

	QTextCharFormat _statementFormat;
	QTextCharFormat _commentFormat;
	QTextCharFormat _preprocessorFormat;
	QTextCharFormat _numberFormat;
	QTextCharFormat _typesFormat;
	QTextCharFormat _swizzleFormat;
};
