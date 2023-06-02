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

	QStringList list;

	//////////////////////////////////////////////
	//                 RULES
	//////////////////////////////////////////////

	list.clear();
	list.append("if");
	list.append("else");
	addPatternFromList(list, toTextCharFormat(_colorScheme._statementFormat));

	list.clear();
	list.append("break");
	list.append("return");
	list.append("continue");
	list.append("discard");
	addPatternFromList(list, toTextCharFormat(_colorScheme._statementFormat));

	list.clear();
	list.append("while");
	list.append("for");
	list.append("do");
	addPatternFromList(list, toTextCharFormat(_colorScheme._statementFormat));

	HighlightingRule rule;

	/* Numbers */
	rule.pattern = QRegularExpression(R"([+-]?\d+(U|u)?(\.\d+)?)"); // Integer and fixed-point literals
	rule.format = toTextCharFormat(_colorScheme._numberFormat);
	_highlightingRules.push_back(std::move(rule));

	rule.pattern = QRegularExpression(R"([+-]?\d+(\.\d*)?([eE][+-]?\d+)?f?)");
	rule.format = toTextCharFormat(_colorScheme._numberFormat);
	_highlightingRules.push_back(std::move(rule));

	/* Swizzles */
	rule.pattern = QRegularExpression("\\.[xyzw]{1,4}\\b");
	rule.format = toTextCharFormat(_colorScheme._swizzleFormat);
	_highlightingRules.push_back(std::move(rule));

	rule.pattern = QRegularExpression("\\.[rgba]{1,4}\\b");
	rule.format = toTextCharFormat(_colorScheme._swizzleFormat);
	_highlightingRules.push_back(std::move(rule));

	rule.pattern = QRegularExpression("\\.[stpq]{1,4}\\b");
	rule.format = toTextCharFormat(_colorScheme._swizzleFormat);
	_highlightingRules.push_back(std::move(rule));

	/* Types */
	list.clear();
	list.append("void");
	list.append("bool");
	list.append("bvec2");
	list.append("bvec3");
	list.append("bvec4");
	list.append("int");
	list.append("ivec2");
	list.append("ivec3");
	list.append("ivec4");
	list.append("float");
	list.append("vec2");
	list.append("vec3");
	list.append("vec4");
	list.append("sampler1D");
	list.append("sampler2D");
	list.append("sampler3D");
	list.append("samplerCUBE");
	list.append("sampler1DShadow");
	list.append("sampler2DShadow");
	list.append("vec4");
	list.append("vec4");
	list.append("struct");
	addPatternFromList(list, toTextCharFormat(_colorScheme._typesFormat));

	/* Storage class */
	list.clear();
	list.append("const");
	list.append("attribute");
	list.append("varying");
	list.append("uniform");
	list.append("in");
	list.append("out");
	list.append("inout");
	addPatternFromList(list, toTextCharFormat(_colorScheme._typesFormat));

	/* Functions */
	list.clear();
	list.append("radians");
	list.append("degrees");
	list.append("sin");
	list.append("cos");
	list.append("tan");
	list.append("asin");
	list.append("acos");
	list.append("atan");
	list.append("pow");
	list.append("exp2");
	list.append("log2");
	list.append("sqrt");
	list.append("inversesqrt");
	list.append("abs");
	list.append("sign");
	list.append("floor");
	list.append("ceil");
	list.append("fract");
	list.append("mod");
	list.append("min");
	list.append("max");
	list.append("clamp");
	list.append("mix");
	list.append("step");
	list.append("smoothstep");
	list.append("length");
	list.append("distance");
	list.append("dot");
	list.append("cross");
	list.append("normalize");
	list.append("ftransform");
	list.append("faceforward");
	list.append("reflect");
	list.append("matrixcompmult");
	list.append("lessThan");
	list.append("lessThanEqual");
	list.append("greaterThan");
	list.append("greaterThanEqual");
	list.append("equal");
	list.append("notEqual");
	list.append("any");
	list.append("all");
	list.append("not");
	list.append("texture1D");
	list.append("texture1DProj");
	list.append("texture1DLod");
	list.append("texture1DProjLod");
	list.append("texture2D");
	list.append("texture2DProj");
	list.append("texture2DLod");
	list.append("texture2DProjLod");
	list.append("texture3D");
	list.append("texture3DProj");
	list.append("texture3DLod");
	list.append("texture3DProjLod");
	list.append("textureCube");
	list.append("textureCubeLod");
	list.append("shadow1D");
	list.append("shadow1DProj");
	list.append("shadow1DLod");
	list.append("shadow1DProjLod");
	list.append("shadow2D");
	list.append("shadow2DProj");
	list.append("shadow2DLod");
	list.append("shadow2DProjLod");
	list.append("dFdx");
	list.append("dFdy");
	list.append("fwidth");
	list.append("noise1");
	list.append("noise2");
	list.append("noise3");
	list.append("noise4");
	list.append("refract");
	list.append("exp");
	list.append("log");
	addPatternFromList(list, toTextCharFormat(_colorScheme._statementFormat));

	/* States */
	list.clear();
	list.append("gl_Position");
	list.append("gl_PointSize");
	list.append("gl_ClipVertex");
	list.append("gl_FragCoord");
	list.append("gl_FrontFacing");
	list.append("gl_FragColor");
	list.append("gl_FragData");
	list.append("gl_FragDepth");
	list.append("gl_Color");
	list.append("gl_SecondaryColor");
	list.append("gl_Normal");
	list.append("gl_Vertex");
	list.append("gl_FogCoord");
	list.append("gl_FrontColor");
	list.append("gl_BackColor");
	list.append("gl_FrontSecondaryColor");
	list.append("gl_BackSecondaryColor");
	list.append("gl_TexCoord");
	list.append("gl_FogFragCoord");
	list.append("gl_MultiTexCoord\\d+");
	addPatternFromList(list, toTextCharFormat(_colorScheme._typesFormat));

	/* Uniforms */
	list.clear();
	list.append("gl_ModelViewMatrix");
	list.append("gl_ProjectionMatrix");
	list.append("gl_ModelViewProjectionMatrix");
	list.append("gl_NormalMatrix");
	list.append("gl_TextureMatrix");
	list.append("gl_NormalScale");
	list.append("gl_DepthRange");
	list.append("gl_ClipPlane");
	list.append("gl_Point");
	list.append("gl_FrontMaterial");
	list.append("gl_BackMaterial");
	list.append("gl_LightSource");
	list.append("gl_LightModel");
	list.append("gl_FrontLightModelProduct");
	list.append("gl_BackLightModelProduct");
	list.append("gl_FrontLightProduct");
	list.append("gl_BackLightProduct");
	list.append("glTextureEnvColor");
	list.append("gl_TextureEnvColor");
	list.append("gl_Fog");
	list.append("gl_ModelViewMatrixInverse");
	list.append("gl_ProjectionMatrixInverse");
	list.append("gl_ModelViewProjectionMatrixInverse");
	list.append("gl_TextureMatrixInverse");
	list.append("gl_ModelViewMatrixTranspose");
	list.append("gl_ProjectionMatrixTranspose");
	list.append("gl_ModelViewProjectionMatrixTranspose");
	list.append("gl_TextureMatrixTranspose");
	list.append("gl_ModelViewMatrixInverseTranspose");
	list.append("gl_ProjectionMatrixInverseTranspose");
	list.append("gl_ModelViewProjectionMatrixInverseTranspose");
	list.append("gl_TextureMatrixInverseTranspose");
	list.append("gl_EyePlane");
	list.append("gl_ObjectPlane");
	addPatternFromList(list, toTextCharFormat(_colorScheme._typesFormat));

	/* preprocessor */
	//rule.pattern = QRegularExpression("\\s*#\\.*\\n");
	rule.pattern = QRegularExpression("#.*");
	rule.format = toTextCharFormat(_colorScheme._preprocessorFormat);
	_highlightingRules.push_back(std::move(rule));

	/* single line comments */
	rule.pattern = QRegularExpression("//.*");
	rule.format = toTextCharFormat(_colorScheme._commentFormat);
	_highlightingRules.push_back(std::move(rule));

	/* multi line comments */
	_commentStartExpression = QRegularExpression("/\\*");
	_commentEndExpression = QRegularExpression("\\*/");

	for (const auto& r : _highlightingRules)
	{
		assert(r.pattern.isValid());
	}

	rehighlight();
}

void ShaderSyntaxHighlighter::highlightBlock(const QString& text)
{
	setFormat(0, (int)text.length(), toTextCharFormat(_colorScheme._otherText));

	qsizetype lastMatchIndex = -1, lastMatchLength = -1;
	for (const auto& rule: _highlightingRules)
	{
		const auto& expression = rule.pattern;
		auto match = expression.match(text);
		qsizetype index = match.capturedStart();
		while (index >= 0)
		{
			const qsizetype length = match.capturedLength();
			assert_r(length > 0);

			lastMatchIndex = index;
			lastMatchLength = length;

			setFormat((int)index, (int)length, rule.format);
			match = expression.match(text, index + length);
			index = match.capturedStart();
		}
	}

	setCurrentBlockState(0);

	qsizetype commentStartIndex = 0;
	if (previousBlockState() != 1) // Comment?
	{
		auto match = _commentStartExpression.match(lastMatchIndex != -1 ? text.mid(lastMatchIndex + lastMatchLength) : text);
		commentStartIndex = match.capturedStart();
	}

	while (commentStartIndex >= 0)
	{
		auto match = _commentEndExpression.match(text, commentStartIndex);
		const qsizetype endIndex = match.capturedStart();
		qsizetype commentLength = 0;
		if (endIndex == -1)
		{
			setCurrentBlockState(1); // Spanning comment
			commentLength = text.length() - commentStartIndex;
		}
		else
		{
			commentLength = endIndex - commentStartIndex + match.capturedLength();
		}

		setFormat((int)commentStartIndex, (int)commentLength, toTextCharFormat(_colorScheme._commentFormat));

		match = _commentStartExpression.match(text, commentStartIndex + commentLength);
		commentStartIndex = match.capturedStart();
	}
}
