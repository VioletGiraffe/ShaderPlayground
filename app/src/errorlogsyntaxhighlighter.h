#pragma once

#include "compiler/compiler_warnings_control.h"

DISABLE_COMPILER_WARNINGS
#include <QSyntaxHighlighter>
RESTORE_COMPILER_WARNINGS

class ErrorLogSyntaxHighlighter final : QSyntaxHighlighter
{
public:
	using QSyntaxHighlighter::QSyntaxHighlighter;

protected:
	void highlightBlock(const QString &text) override;
};
