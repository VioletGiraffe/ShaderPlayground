#include "shaderframework.h"
#include "assert/advanced_assert.h"

DISABLE_COMPILER_WARNINGS
#include <QFile>
RESTORE_COMPILER_WARNINGS

inline QString textFromResource(const char* resourcePath)
{
	QFile resourceFile(resourcePath);
	assert(resourceFile.exists());
	assert_r(resourceFile.open(QFile::ReadOnly));

	return resourceFile.readAll();
}

void ShaderFramework::setFrameworkMode(Framework framework)
{
	_frameworkMode = framework;
}

QString ShaderFramework::processedShaderSource(const QString& sourceCode) const
{
	if (_frameworkMode == GLSL)
		return sourceCode;
	else if (_frameworkMode == ShaderToy)
	{
		static const QString shaderSourceTemplate = textFromResource(":/resources/default_shadertoy_template.fsh");
		return QString(shaderSourceTemplate).replace("%1%", sourceCode);
	}
	else
		assert_and_return_unconditional_r("Unknown framework", sourceCode);
}
