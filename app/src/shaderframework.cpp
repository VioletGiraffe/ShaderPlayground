#include "shaderframework.h"

#include "assert/advanced_assert.h"
#include "regex/regex_helpers.hpp"

DISABLE_COMPILER_WARNINGS
#include <QFile>
RESTORE_COMPILER_WARNINGS

#include <algorithm>

static QString textFromResource(const char* resourcePath)
{
	QFile resourceFile(resourcePath);
	assert(resourceFile.exists());
	assert_r(resourceFile.open(QFile::ReadOnly));

	return resourceFile.readAll();
}

ShaderFramework::ShaderFramework() :
	_shaderTemplate{ textFromResource(":/resources/default_shadertoy_template.fsh") }
{
	const QString placeholder = "%1%";
	const auto placeholderPosition = _shaderTemplate.indexOf(placeholder);
	const auto linesBeforePlaceholder = std::count(_shaderTemplate.begin(), _shaderTemplate.begin() + placeholderPosition, '\n');
	_lineNumberOffset = (decltype(_lineNumberOffset))linesBeforePlaceholder + 1 /* why is this required? */;
}

void ShaderFramework::setFrameworkMode(Framework framework)
{
	_frameworkMode = framework;
}

ShaderFramework::Framework ShaderFramework::frameworkMode() const
{
	return _frameworkMode;
}

QString ShaderFramework::processedShaderSource(const QString& sourceCode) const
{
	if (_frameworkMode == GLSL)
		return sourceCode;
	else if (_frameworkMode == ShaderToy)
		return QString(_shaderTemplate).replace("%1%", sourceCode);
	else
		assert_and_return_unconditional_r("Unknown framework", sourceCode);
}

QString ShaderFramework::adjustLineNumbersInTheLog(const QString& log) const
{
	if (_frameworkMode != ShaderToy)
		return log;

	static const std::wregex line_number_regex(L"\\(([0-9]+)\\) :");
	auto wlog = log.toStdWString();
	wlog = regex_helpers::regex_replace(wlog, line_number_regex, [this](const std::match_results<typename std::wstring::const_iterator>& match) {
		const auto lineNumber = std::stoul(match.str(1));
		return L'(' + std::to_wstring(lineNumber - _lineNumberOffset) + L')';
	});

	return QString::fromStdWString(wlog);
}