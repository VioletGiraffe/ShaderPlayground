#include "errorlogsyntaxhighlighter.h"

void ErrorLogSyntaxHighlighter::highlightBlock(const QString &text)
{
	const auto lines = text.splitRef('\n', Qt::SkipEmptyParts);
	QTextCharFormat error;
	error.setForeground(Qt::white);
	error.setBackground(Qt::red);

	QTextCharFormat warning;
	warning.setBackground(QColor::fromRgb(0xffd700U));

	for (const auto& s: lines)
	{
		if (s.contains("error", Qt::CaseInsensitive) && s.contains(": "))
			setFormat(s.position(), s.length(), error);
		else if (s.contains("warning", Qt::CaseInsensitive) && s.contains(": "))
			setFormat(s.position(), s.length(), warning);
	}
}
