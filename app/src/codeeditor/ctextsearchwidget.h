#pragma once

#include "compiler/compiler_warnings_control.h"

DISABLE_COMPILER_WARNINGS
#include <QWidget>
RESTORE_COMPILER_WARNINGS

namespace Ui {
class CTextSearchWidget;
}

struct TextSearchOptions
{
	bool caseSensitive = false;
};

struct TextSearchCallbacks
{
	virtual void findPrevious(const QString& what, const TextSearchOptions options = TextSearchOptions()) = 0;
	virtual void findNext(const QString& what, const TextSearchOptions options = TextSearchOptions()) = 0;
	virtual void findAll(const QString& what, const TextSearchOptions options = TextSearchOptions()) = 0;

	virtual void replaceNext(const QString& what, const QString& with, const TextSearchOptions options = TextSearchOptions()) = 0;
	virtual void replaceAll(const QString& what, const QString& with, const TextSearchOptions options = TextSearchOptions()) = 0;
};

class CTextSearchWidget : public QWidget
{
public:
	explicit CTextSearchWidget(QWidget *parent = nullptr);
	~CTextSearchWidget();

	void setCallbackReceiver(TextSearchCallbacks* receiver);

	void setShowReplaceUI(bool show);

private:
	TextSearchOptions currentOptions() const;

private:
	Ui::CTextSearchWidget *ui = nullptr;
	TextSearchCallbacks* _callbackReceiver = nullptr;
};
