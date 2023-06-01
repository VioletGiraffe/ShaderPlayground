#pragma once

#include "compiler/compiler_warnings_control.h"
#include "codeeditor/colorscheme.h"

DISABLE_COMPILER_WARNINGS
#include <QRegularExpression>
#include <QSyntaxHighlighter>
RESTORE_COMPILER_WARNINGS

#include <vector>

class ShaderSyntaxHighlighter final: public QSyntaxHighlighter
{
public:
	ShaderSyntaxHighlighter(ColorScheme colors, QTextDocument *parent = 0);
	void setColorScheme(ColorScheme colors);

protected:
	void highlightBlock(const QString &text) override;

private:
	void addPatternFromList(const QStringList &list, const QTextCharFormat &format);

private:
	ColorScheme _colorScheme;

	struct HighlightingRule {
		QRegularExpression pattern;
		QTextCharFormat format;
	};

	std::vector<HighlightingRule> _highlightingRules;

	QRegularExpression _commentStartExpression;
	QRegularExpression _commentEndExpression;
};
