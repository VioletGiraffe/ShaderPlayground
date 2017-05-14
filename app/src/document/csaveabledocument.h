#pragma once

#include "compiler/compiler_warnings_control.h"

DISABLE_COMPILER_WARNINGS
#include <QString>
RESTORE_COMPILER_WARNINGS

class CSaveableDocument
{
public:
	bool hasUnsavedChanges() const;
	void markAsUnsaved();

	struct FileLoadResult {
		QByteArray data;
		bool loadedSuccessfully;
	};

	FileLoadResult load(const QString& filePath);
	bool save(const QByteArray& data);
	bool saveAs(const QByteArray& data, const QString& fileName);

	QString filePath() const;
	QString name();

private:
	QString _filePath;
	bool _hasUnsavedChanges = false;
};
