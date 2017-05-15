#include "csaveabledocument.h"

#include "assert/advanced_assert.h"

DISABLE_COMPILER_WARNINGS
#include <QFile>
#include <QFileInfo>
RESTORE_COMPILER_WARNINGS

bool CSaveableDocument::hasUnsavedChanges() const
{
	return _hasUnsavedChanges;
}

void CSaveableDocument::markAsUnsaved()
{
	_hasUnsavedChanges = true;
}

void CSaveableDocument::setFilePath(const QString& filePath)
{
	_filePath = filePath;
	_hasUnsavedChanges = !QFileInfo(filePath).exists();
}

CSaveableDocument::FileLoadResult CSaveableDocument::load(const QString& filePath)
{
	QFile file(filePath);
	const auto failure = FileLoadResult{QByteArray(), false};
	assert_and_return_r(file.exists(), failure);
	assert_and_return_r(file.open(QFile::ReadOnly), failure);

	_filePath = filePath;
	return FileLoadResult{file.readAll(), true};
}

CSaveableDocument::FileLoadResult CSaveableDocument::load()
{
	return load(_filePath);
}

inline bool save(const QByteArray& data, const QString& filePath)
{
	QFile file(filePath);
	assert_and_return_r(file.open(QFile::WriteOnly), false);

	return file.write(data) == (qint64)data.size();
}

bool CSaveableDocument::save(const QByteArray& data)
{
	if (::save(data, _filePath))
	{
		_hasUnsavedChanges = false;
		return true;
	}
	else
		return false;
}

bool CSaveableDocument::saveAs(const QByteArray& data, const QString& fileName)
{
	if (::save(data, fileName))
	{
		_hasUnsavedChanges = false;
		_filePath = fileName;
		return true;
	}
	else
		return false;
}

QString CSaveableDocument::filePath() const
{
	return _filePath;
}

QString CSaveableDocument::name() const
{
	return QFileInfo(_filePath).fileName();
}
