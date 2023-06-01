#include "errorlogsyntaxhighlighter.h"

DISABLE_COMPILER_WARNINGS
#include <QStringRef>
RESTORE_COMPILER_WARNINGS

void ErrorLogSyntaxHighlighter::highlightBlock(const QString &text)
{
	const auto lines = QStringRef{ &text }.split('\n', Qt::SkipEmptyParts);
	QTextCharFormat error;
	error.setForeground(Qt::white);
	error.setBackground(Qt::red);

	QTextCharFormat warning;
	warning.setBackground(QColor{ 255, 215, 0 });

	for (const auto& s: lines)
	{
		if (s.contains(QStringLiteral("error"), Qt::CaseInsensitive) && s.contains(QStringLiteral(": ")))
			setFormat(s.position(), s.length(), error);
		else if (s.contains(QStringLiteral("warning"), Qt::CaseInsensitive) && s.contains(QStringLiteral(": ")))
			setFormat(s.position(), s.length(), warning);
	}
}
