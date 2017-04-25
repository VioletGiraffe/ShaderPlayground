#include "shadersyntaxhighlighter.h"

#include <QDebug>

// Adapted from https://github.com/GLSL-Debugger/GLSL-Debugger/blob/master/glsldb/glslSyntaxHighlighter.cpp

ShaderSyntaxHighlighter::ShaderSyntaxHighlighter(QTextDocument* parent) :
	QSyntaxHighlighter(parent)
{
	HighlightingRule rule;

	//////////////////////////////////////////////
	//                FORMATS
	//////////////////////////////////////////////

	_statementFormat.setForeground(QColor(255, 0, 0));
	_statementFormat.setFontWeight(QFont::Bold);

	_commentFormat.setForeground(QColor(0, 0, 205));

	_preprocessorFormat.setForeground(QColor(205, 0, 205));

	_numberFormat.setForeground(QColor(205, 0, 0));

	_typesFormat.setForeground(QColor(0, 0, 255));
	_typesFormat.setFontWeight(QFont::Bold);

	_swizzleFormat.setForeground(QColor(205, 0, 205));

	//////////////////////////////////////////////
	//                 RULES
	//////////////////////////////////////////////

	QStringList glslConditional;
	glslConditional.append("if");
	glslConditional.append("else");
	addPatternFromList(glslConditional, _statementFormat);

	QStringList glslStatement;
	glslStatement.append("break");
	glslStatement.append("return");
	glslStatement.append("continue");
	glslStatement.append("discard");
	addPatternFromList(glslStatement, _statementFormat);

	QStringList glslRepeat;
	glslRepeat.append("while");
	glslRepeat.append("for");
	glslRepeat.append("do");
	addPatternFromList(glslRepeat, _statementFormat);

	/* Numbers */
	rule.pattern = QRegExp("\\b\\d+(u{,1}l{0,2}|ll{,1}u)\\b");
	rule.format = _numberFormat;
	_highlightingRules.push_back(rule);

	rule.pattern = QRegExp("\\b0x\\x+(u{,1}l{0,2}|ll{,1}u)\\b");
	rule.format = _numberFormat;
	_highlightingRules.push_back(rule);

	rule.pattern = QRegExp("\\b\\d+f\\b");
	rule.format = _numberFormat;
	_highlightingRules.push_back(rule);

	rule.pattern = QRegExp("\\b\\d+\\.\\d*(e[-+]{,1}\\d+){,1}[fl]{,1}\\b");
	rule.format = _numberFormat;
	_highlightingRules.push_back(rule);

	rule.pattern = QRegExp("\\b\\.\\d+(e[-+]{,1}\\d+){,1}[fl]{,1}\\b");
	rule.format = _numberFormat;
	_highlightingRules.push_back(rule);

	rule.pattern = QRegExp("\\b\\d+e[-+]{,1}\\d+[fl]{,1}\\b");
	rule.format = _numberFormat;
	_highlightingRules.push_back(rule);

	rule.pattern = QRegExp("\\b0\\o*[89]\\d*\\b");
	rule.format = _numberFormat;
	_highlightingRules.push_back(rule);

	/* Swizzles */
	rule.pattern = QRegExp("\\.[xyzw]{1,4}\\b");
	rule.format = _swizzleFormat;
	_highlightingRules.push_back(rule);

	rule.pattern = QRegExp("\\.[rgba]{1,4}\\b");
	rule.format = _swizzleFormat;
	_highlightingRules.push_back(rule);

	rule.pattern = QRegExp("\\.[stpq]{1,4}\\b");
	rule.format = _swizzleFormat;
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
	addPatternFromList(glslTypes, _typesFormat);

	/* Storage class */
	QStringList glslStorageClass;
	glslStorageClass.append("const");
	glslStorageClass.append("attribute");
	glslStorageClass.append("varying");
	glslStorageClass.append("uniform");
	glslStorageClass.append("in");
	glslStorageClass.append("out");
	glslStorageClass.append("inout");
	addPatternFromList(glslStorageClass, _typesFormat);

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
	addPatternFromList(glslFunc, _statementFormat);

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
	addPatternFromList(glslState, _typesFormat);

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
	addPatternFromList(glslUniform, _typesFormat);

	/* preprocessor */
	//rule.pattern = QRegExp("\\s*#\\.*\\n");
	rule.pattern = QRegExp("#.*");
	rule.format = _preprocessorFormat;
	_highlightingRules.push_back(rule);

	/* single line comments */
	rule.pattern = QRegExp("//.*");
	rule.format = _commentFormat;
	_highlightingRules.push_back(rule);

	/* multi line comments */
	_commentStartExpression = QRegExp("/\\*");
	_commentEndExpression = QRegExp("\\*/");
}

void ShaderSyntaxHighlighter::addPatternFromList(QStringList& list, QTextCharFormat& format)
{
	for (const QString& pattern: list)
		_highlightingRules.push_back(HighlightingRule{QRegExp(QString("\\b") + pattern + QString("\\b")), format});
}

void ShaderSyntaxHighlighter::highlightBlock(const QString& text)
{
	for (const auto& rule: _highlightingRules)
	{
		const auto& expression = rule.pattern;
		int index = rule.pattern.indexIn(text);
		while (index >= 0)
		{
			int length = expression.matchedLength();
			if (length <= 0)
				qDebug() << "length<=0 for" << expression.pattern() << "mathcing in" << text << "at" << index;

			setFormat(index, length, rule.format);
			index = expression.indexIn(text, index + length);
		}
	}

	setCurrentBlockState(0);

	int startIndex = 0;
	if (previousBlockState() != 1)
		startIndex = _commentStartExpression.indexIn(text);

	while (startIndex >= 0)
	{
		int endIndex = _commentEndExpression.indexIn(text, startIndex);
		int commentLength;
		if (endIndex == -1)
		{
			setCurrentBlockState(1);
			commentLength = text.length() - startIndex;
		}
		else
		{
			commentLength = endIndex - startIndex + _commentEndExpression.matchedLength();
		}

		setFormat(startIndex, commentLength, _commentFormat);
		startIndex = _commentStartExpression.indexIn(text, startIndex + commentLength);
	}
}
