#pragma once

#include "compiler/compiler_warnings_control.h"

DISABLE_COMPILER_WARNINGS
#include <QString>
RESTORE_COMPILER_WARNINGS

class ShaderFramework
{
public:
	enum Framework {GLSL, ShaderToy};

	void setFrameworkMode(Framework framework);
	Framework frameworkMode() const;

	QString processedShaderSource(const QString& sourceCode) const;

private:
	Framework _frameworkMode = GLSL;
};
