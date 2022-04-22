#pragma once

#include "compiler/compiler_warnings_control.h"

DISABLE_COMPILER_WARNINGS
#include <QString>
RESTORE_COMPILER_WARNINGS

class ShaderFramework
{
public:
	enum Framework {GLSL, ShaderToy};

	ShaderFramework();

	void setFrameworkMode(Framework framework);
	Framework frameworkMode() const;

	QString processedShaderSource(const QString& sourceCode) const;

	QString adjustLineNumbersInTheLog(const QString& log) const;

private:
	const QString _shaderTemplate;
	unsigned long _lineNumberOffset = 0;
	Framework _frameworkMode = GLSL;
};
