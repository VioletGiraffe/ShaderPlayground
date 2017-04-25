#include "shadersyntaxhighlighter.h"

ShaderSyntaxHighlighter::ShaderSyntaxHighlighter(QTextDocument *parent)
	: QSyntaxHighlighter(parent)
{
	_keywordFormat.setForeground(Qt::darkBlue);
	_keywordFormat.setFontWeight(QFont::Bold);
	QStringList keywordPatterns;
	keywordPatterns << "\\bchar\\b" << "\\bclass\\b" << "\\bconst\\b"
					<< "\\bdouble\\b" << "\\benum\\b" << "\\bexplicit\\b"
					<< "\\bfriend\\b" << "\\binline\\b" << "\\bint\\b"
					<< "\\blong\\b" << "\\bnamespace\\b" << "\\boperator\\b"
					<< "\\bprivate\\b" << "\\bprotected\\b" << "\\bpublic\\b"
					<< "\\bshort\\b" << "\\bsignals\\b" << "\\bsigned\\b"
					<< "\\bslots\\b" << "\\bstatic\\b" << "\\bstruct\\b"
					<< "\\btemplate\\b" << "\\btypedef\\b" << "\\btypename\\b"
					<< "\\bunion\\b" << "\\bunsigned\\b" << "\\bvirtual\\b"
					<< "\\bvoid\\b" << "\\bvolatile\\b";

	for (const QString &pattern: keywordPatterns)
		_highlightingRules.emplace_back(HighlightingRule{QRegExp(pattern), _keywordFormat});

	HighlightingRule rule;

	_classFormat.setFontWeight(QFont::Bold);
	_classFormat.setForeground(Qt::darkMagenta);
	rule.pattern = QRegExp("\\bQ[A-Za-z]+\\b");
	rule.format = _classFormat;
	_highlightingRules.push_back(rule);

	_singleLineCommentFormat.setForeground(Qt::red);
	rule.pattern = QRegExp("//[^\n]*");
	rule.format = _singleLineCommentFormat;
	_highlightingRules.push_back(rule);

	_multiLineCommentFormat.setForeground(Qt::red);

	_quotationFormat.setForeground(Qt::darkGreen);
	rule.pattern = QRegExp("\".*\"");
	rule.format = _quotationFormat;
	_highlightingRules.push_back(rule);

	_functionFormat.setFontItalic(true);
	_functionFormat.setForeground(Qt::blue);
	rule.pattern = QRegExp("\\b[A-Za-z0-9_]+(?=\\()");
	rule.format = _functionFormat;
	_highlightingRules.push_back(rule);

	_commentStartExpression = QRegExp("/\\*");
	_commentEndExpression = QRegExp("\\*/");
}

void ShaderSyntaxHighlighter::highlightBlock(const QString &text)
{
	for (const HighlightingRule &rule: _highlightingRules)
	{
		QRegExp expression(rule.pattern);
		int index = expression.indexIn(text);
		while (index >= 0)
		{
			int length = expression.matchedLength();
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

		setFormat(startIndex, commentLength, _multiLineCommentFormat);
		startIndex = _commentStartExpression.indexIn(text, startIndex + commentLength);
	}
}
