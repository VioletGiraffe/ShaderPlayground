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
	[[nodiscard]] QByteArray contents() const;

	[[nodiscard]] bool hasUnsavedChanges() const;

	void setFilePath(const QString& filePath);
	void newDocument(const QByteArray& contents = QByteArray());
	[[nodiscard]] bool load(const QString& filePath);
	[[nodiscard]] bool load();
	[[nodiscard]] bool save();
	[[nodiscard]] bool saveAs(const QString& fileName);

	[[nodiscard]] QString filePath() const;
	[[nodiscard]] QString name() const;

private:
	QByteArray _contents;
	QString _filePath;
	bool _hasUnsavedChanges = false;
};
