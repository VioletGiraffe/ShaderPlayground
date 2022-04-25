#pragma once

#include "compiler/compiler_warnings_control.h"

DISABLE_COMPILER_WARNINGS
#include <QColor>
#include <QFont>
RESTORE_COMPILER_WARNINGS

class ColorScheme
{
public:
	[[nodiscard]] static ColorScheme fromYaml(const QString& yaml);

	struct Format {
		QColor _color;
		QFont::Weight _weight = QFont::Normal;
	};

	Format _statementFormat;
	Format _typesFormat;
	Format _numberFormat;
	Format _swizzleFormat;
	Format _preprocessorFormat;
	Format _commentFormat;
	Format _otherText;
};
