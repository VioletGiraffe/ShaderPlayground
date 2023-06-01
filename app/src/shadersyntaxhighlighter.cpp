#include "shadersyntaxhighlighter.h"
#include "assert/advanced_assert.h"

DISABLE_COMPILER_WARNINGS
#include <QDebug>
RESTORE_COMPILER_WARNINGS

// Adapted from https://github.com/GLSL-Debugger/GLSL-Debugger/blob/master/glsldb/glslSyntaxHighlighter.cpp

static QTextCharFormat toTextCharFormat(const ColorScheme::Format& f)
{
	QTextCharFormat format;
	format.setForeground(QBrush(f._color));
	format.setFontWeight(f._weight);
	return format;
}

ShaderSyntaxHighlighter::ShaderSyntaxHighlighter(ColorScheme colors, QTextDocument* parent) :
	QSyntaxHighlighter(parent)
{
	setColorScheme(std::move(colors));
}

void ShaderSyntaxHighlighter::addPatternFromList(const QStringList& list, const QTextCharFormat& format)
{
	for (const QString& pattern: list)
		_highlightingRules.push_back(HighlightingRule{QRegularExpression(QStringLiteral("\\b") + pattern + QStringLiteral("\\b")), format});
}

void ShaderSyntaxHighlighter::setColorScheme(ColorScheme colors)
{
	_colorScheme = std::move(colors);

	HighlightingRule rule;

	//////////////////////////////////////////////
	//                 RULES
	//////////////////////////////////////////////

	QStringList glslConditional;
	glslConditional.append("if");
	glslConditional.append("else");
	addPatternFromList(glslConditional, toTextCharFormat(_colorScheme._statementFormat));

	QStringList glslStatement;
	glslStatement.append("break");
	glslStatement.append("return");
	glslStatement.append("continue");
	glslStatement.append("discard");
	addPatternFromList(glslStatement, toTextCharFormat(_colorScheme._statementFormat));

	QStringList glslRepeat;
	glslRepeat.append("while");
	glslRepeat.append("for");
	glslRepeat.append("do");
	addPatternFromList(glslRepeat, toTextCharFormat(_colorScheme._statementFormat));

	/* Numbers */
	rule.pattern = QRegularExpression("\\b\\d+(u{,1}l{0,2}|ll{,1}u)\\b");
	rule.format = toTextCharFormat(_colorScheme._numberFormat);
	_highlightingRules.push_back(rule);

	rule.pattern = QRegularExpression("\\b0x\\x+(u{,1}l{0,2}|ll{,1}u)\\b");
	rule.format = toTextCharFormat(_colorScheme._numberFormat);
	_highlightingRules.push_back(rule);

	rule.pattern = QRegularExpression("\\b\\d+f\\b");
	rule.format = toTextCharFormat(_colorScheme._numberFormat);
	_highlightingRules.push_back(rule);

	rule.pattern = QRegularExpression("\\b\\d+\\.\\d*(e[-+]{,1}\\d+){,1}[fl]{,1}\\b");
	rule.format = toTextCharFormat(_colorScheme._numberFormat);
	_highlightingRules.push_back(rule);

	rule.pattern = QRegularExpression("\\b\\.\\d+(e[-+]{,1}\\d+){,1}[fl]{,1}\\b");
	rule.format = toTextCharFormat(_colorScheme._numberFormat);
	_highlightingRules.push_back(rule);

	rule.pattern = QRegularExpression("\\b\\d+e[-+]{,1}\\d+[fl]{,1}\\b");
	rule.format = toTextCharFormat(_colorScheme._numberFormat);
	_highlightingRules.push_back(rule);

	rule.pattern = QRegularExpression("\\b0\\o*[89]\\d*\\b");
	rule.format = toTextCharFormat(_colorScheme._numberFormat);
	_highlightingRules.push_back(rule);

	/* Swizzles */
	rule.pattern = QRegularExpression("\\.[xyzw]{1,4}\\b");
	rule.format = toTextCharFormat(_colorScheme._swizzleFormat);
	_highlightingRules.push_back(rule);

	rule.pattern = QRegularExpression("\\.[rgba]{1,4}\\b");
	rule.format = toTextCharFormat(_colorScheme._swizzleFormat);
	_highlightingRules.push_back(rule);

	rule.pattern = QRegularExpression("\\.[stpq]{1,4}\\b");
	rule.format = toTextCharFormat(_colorScheme._swizzleFormat);
	_highlightingRules.push_back(rule);

	/* Types */
	QStringList glslTypes;
	glslTypes.append("void");
	glslTypes.append("bool");
	glslTypes.append("bvec2");
	glslTypes.append("bvec3");
	glslTypes.append("bvec4");
	glslTypes.append("int");
	glslTypes.append("ivec2");
	glslTypes.append("ivec3");
	glslTypes.append("ivec4");
	glslTypes.append("float");
	glslTypes.append("vec2");
	glslTypes.append("vec3");
	glslTypes.append("vec4");
	glslTypes.append("sampler1D");
	glslTypes.append("sampler2D");
	glslTypes.append("sampler3D");
	glslTypes.append("samplerCUBE");
	glslTypes.append("sampler1DShadow");
	glslTypes.append("sampler2DShadow");
	glslTypes.append("vec4");
	glslTypes.append("vec4");
	glslTypes.append("struct");
	addPatternFromList(glslTypes, toTextCharFormat(_colorScheme._typesFormat));

	/* Storage class */
	QStringList glslStorageClass;
	glslStorageClass.append("const");
	glslStorageClass.append("attribute");
	glslStorageClass.append("varying");
	glslStorageClass.append("uniform");
	glslStorageClass.append("in");
	glslStorageClass.append("out");
	glslStorageClass.append("inout");
	addPatternFromList(glslStorageClass, toTextCharFormat(_colorScheme._typesFormat));

	/* Functions */
	QStringList glslFunc;
	glslFunc.append("radians");
	glslFunc.append("degrees");
	glslFunc.append("sin");
	glslFunc.append("cos");
	glslFunc.append("tan");
	glslFunc.append("asin");
	glslFunc.append("acos");
	glslFunc.append("atan");
	glslFunc.append("pow");
	glslFunc.append("exp2");
	glslFunc.append("log2");
	glslFunc.append("sqrt");
	glslFunc.append("inversesqrt");
	glslFunc.append("abs");
	glslFunc.append("sign");
	glslFunc.append("floor");
	glslFunc.append("ceil");
	glslFunc.append("fract");
	glslFunc.append("mod");
	glslFunc.append("min");
	glslFunc.append("max");
	glslFunc.append("clamp");
	glslFunc.append("mix");
	glslFunc.append("step");
	glslFunc.append("smoothstep");
	glslFunc.append("length");
	glslFunc.append("distance");
	glslFunc.append("dot");
	glslFunc.append("cross");
	glslFunc.append("normalize");
	glslFunc.append("ftransform");
	glslFunc.append("faceforward");
	glslFunc.append("reflect");
	glslFunc.append("matrixcompmult");
	glslFunc.append("lessThan");
	glslFunc.append("lessThanEqual");
	glslFunc.append("greaterThan");
	glslFunc.append("greaterThanEqual");
	glslFunc.append("equal");
	glslFunc.append("notEqual");
	glslFunc.append("any");
	glslFunc.append("all");
	glslFunc.append("not");
	glslFunc.append("texture1D");
	glslFunc.append("texture1DProj");
	glslFunc.append("texture1DLod");
	glslFunc.append("texture1DProjLod");
	glslFunc.append("texture2D");
	glslFunc.append("texture2DProj");
	glslFunc.append("texture2DLod");
	glslFunc.append("texture2DProjLod");
	glslFunc.append("texture3D");
	glslFunc.append("texture3DProj");
	glslFunc.append("texture3DLod");
	glslFunc.append("texture3DProjLod");
	glslFunc.append("textureCube");
	glslFunc.append("textureCubeLod");
	glslFunc.append("shadow1D");
	glslFunc.append("shadow1DProj");
	glslFunc.append("shadow1DLod");
	glslFunc.append("shadow1DProjLod");
	glslFunc.append("shadow2D");
	glslFunc.append("shadow2DProj");
	glslFunc.append("shadow2DLod");
	glslFunc.append("shadow2DProjLod");
	glslFunc.append("dFdx");
	glslFunc.append("dFdy");
	glslFunc.append("fwidth");
	glslFunc.append("noise1");
	glslFunc.append("noise2");
	glslFunc.append("noise3");
	glslFunc.append("noise4");
	glslFunc.append("refract");
	glslFunc.append("exp");
	glslFunc.append("log");
	addPatternFromList(glslFunc, toTextCharFormat(_colorScheme._statementFormat));

	/* States */
	QStringList glslState;
	glslState.append("gl_Position");
	glslState.append("gl_PointSize");
	glslState.append("gl_ClipVertex");
	glslState.append("gl_FragCoord");
	glslState.append("gl_FrontFacing");
	glslState.append("gl_FragColor");
	glslState.append("gl_FragData");
	glslState.append("gl_FragDepth");
	glslState.append("gl_Color");
	glslState.append("gl_SecondaryColor");
	glslState.append("gl_Normal");
	glslState.append("gl_Vertex");
	glslState.append("gl_FogCoord");
	glslState.append("gl_FrontColor");
	glslState.append("gl_BackColor");
	glslState.append("gl_FrontSecondaryColor");
	glslState.append("gl_BackSecondaryColor");
	glslState.append("gl_TexCoord");
	glslState.append("gl_FogFragCoord");
	glslState.append("gl_MultiTexCoord\\d+");
	addPatternFromList(glslState, toTextCharFormat(_colorScheme._typesFormat));

	/* Uniforms */
	QStringList glslUniform;
	glslUniform.append("gl_ModelViewMatrix");
	glslUniform.append("gl_ProjectionMatrix");
	glslUniform.append("gl_ModelViewProjectionMatrix");
	glslUniform.append("gl_NormalMatrix");
	glslUniform.append("gl_TextureMatrix");
	glslUniform.append("gl_NormalScale");
	glslUniform.append("gl_DepthRange");
	glslUniform.append("gl_ClipPlane");
	glslUniform.append("gl_Point");
	glslUniform.append("gl_FrontMaterial");
	glslUniform.append("gl_BackMaterial");
	glslUniform.append("gl_LightSource");
	glslUniform.append("gl_LightModel");
	glslUniform.append("gl_FrontLightModelProduct");
	glslUniform.append("gl_BackLightModelProduct");
	glslUniform.append("gl_FrontLightProduct");
	glslUniform.append("gl_BackLightProduct");
	glslUniform.append("glTextureEnvColor");
	glslUniform.append("gl_TextureEnvColor");
	glslUniform.append("gl_Fog");
	glslUniform.append("gl_ModelViewMatrixInverse");
	glslUniform.append("gl_ProjectionMatrixInverse");
	glslUniform.append("gl_ModelViewProjectionMatrixInverse");
	glslUniform.append("gl_TextureMatrixInverse");
	glslUniform.append("gl_ModelViewMatrixTranspose");
	glslUniform.append("gl_ProjectionMatrixTranspose");
	glslUniform.append("gl_ModelViewProjectionMatrixTranspose");
	glslUniform.append("gl_TextureMatrixTranspose");
	glslUniform.append("gl_ModelViewMatrixInverseTranspose");
	glslUniform.append("gl_ProjectionMatrixInverseTranspose");
	glslUniform.append("gl_ModelViewProjectionMatrixInverseTranspose");
	glslUniform.append("gl_TextureMatrixInverseTranspose");
	glslUniform.append("gl_EyePlane");
	glslUniform.append("gl_ObjectPlane");
	addPatternFromList(glslUniform, toTextCharFormat(_colorScheme._typesFormat));

	/* preprocessor */
	//rule.pattern = QRegularExpression("\\s*#\\.*\\n");
	rule.pattern = QRegularExpression("#.*");
	rule.format = toTextCharFormat(_colorScheme._preprocessorFormat);
	_highlightingRules.push_back(rule);

	/* single line comments */
	rule.pattern = QRegularExpression("//.*");
	rule.format = toTextCharFormat(_colorScheme._commentFormat);
	_highlightingRules.push_back(rule);

	/* multi line comments */
	_commentStartExpression = QRegularExpression("/\\*");
	_commentEndExpression = QRegularExpression("\\*/");

	rehighlight();
}

void ShaderSyntaxHighlighter::highlightBlock(const QString& text)
{
	setFormat(0, text.length(), toTextCharFormat(_colorScheme._otherText));

	int lastMatchIndex = -1, lastMatchLength = -1;
	for (const auto& rule: _highlightingRules)
	{
		const auto& expression = rule.pattern;
		auto match = expression.match(text);
		qsizetype index = match.capturedStart();
		while (index >= 0)
		{
			const int length = match.capturedLength();
			assert_r(length > 0);

			lastMatchIndex = index;
			lastMatchLength = length;

			setFormat(index, length, rule.format);
			match = expression.match(text, index + length);
			index = match.capturedStart();
		}
	}

	setCurrentBlockState(0);

	int commentStartIndex = 0;
	if (previousBlockState() != 1) // Comment?
	{
		auto match = _commentStartExpression.match(lastMatchIndex != -1 ? text.mid(lastMatchIndex + lastMatchLength) : text);
		commentStartIndex = match.capturedStart();
	}

	while (commentStartIndex >= 0)
	{
		auto match = _commentEndExpression.match(text, commentStartIndex);
		const int endIndex = match.capturedStart();
		int commentLength = 0;
		if (endIndex == -1)
		{
			setCurrentBlockState(1); // Spanning comment
			commentLength = text.length() - commentStartIndex;
		}
		else
		{
			commentLength = endIndex - commentStartIndex + match.capturedLength();
		}

		setFormat(commentStartIndex, commentLength, toTextCharFormat(_colorScheme._commentFormat));

		match = _commentStartExpression.match(text, commentStartIndex + commentLength);
		commentStartIndex = match.capturedStart();
	}
}
