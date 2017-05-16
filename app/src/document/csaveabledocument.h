#pragma once

#include "compiler/compiler_warnings_control.h"

DISABLE_COMPILER_WARNINGS
#include <QString>
RESTORE_COMPILER_WARNINGS

class CSaveableDocument
{
public:
	bool hasUnsavedChanges() const;
	void markAsModified(bool modified = true);

	struct FileLoadResult {
		QByteArray data;
		bool loadedSuccessfully;
	};

	void setFilePath(const QString& filePath);
	FileLoadResult load(const QString& filePath);
	FileLoadResult load();
	bool save(const QByteArray& data);
	bool saveAs(const QByteArray& data, const QString& fileName);

	QString filePath() const;
	QString name() const;

private:
	QString _filePath;
	bool _hasUnsavedChanges = false;
};
