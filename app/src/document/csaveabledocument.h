#pragma once

#include "compiler/compiler_warnings_control.h"

DISABLE_COMPILER_WARNINGS
#include <QString>
RESTORE_COMPILER_WARNINGS

class CSaveableDocument
{
public:
	CSaveableDocument& operator=(const CSaveableDocument& other);

	void setContents(const QByteArray& contents);
	QByteArray contents() const;

	bool hasUnsavedChanges() const;

	void setFilePath(const QString& filePath);
	bool load(const QString& filePath);
	bool load();
	bool save();
	bool saveAs(const QString& fileName);

	QString filePath() const;
	QString name() const;

private:
	QByteArray _contents;
	QString _filePath;
	bool _hasUnsavedChanges = false;
};
