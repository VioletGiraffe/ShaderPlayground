#include "csaveabledocument.h"

#include "assert/advanced_assert.h"

DISABLE_COMPILER_WARNINGS
#include <QFile>
#include <QFileInfo>
RESTORE_COMPILER_WARNINGS

CSaveableDocument& CSaveableDocument::operator=(const CSaveableDocument& other)
{
	_filePath = other._filePath;
	_hasUnsavedChanges = other._hasUnsavedChanges;

	return *this;
}

void CSaveableDocument::setContents(const QByteArray& contents)
{
	_hasUnsavedChanges = _contents != contents;
	_contents = contents;
}

QByteArray CSaveableDocument::contents() const
{
	return _contents;
}

bool CSaveableDocument::hasUnsavedChanges() const
{
	return _hasUnsavedChanges;
}

void CSaveableDocument::setFilePath(const QString& filePath)
{
	_filePath = filePath;
	_hasUnsavedChanges = !QFileInfo(filePath).exists();
}

bool CSaveableDocument::load(const QString& filePath)
{
	QFile file(filePath);
	assert_and_return_r(file.exists(), false);
	assert_and_return_r(file.open(QFile::ReadOnly), false);

	_filePath = filePath;
	_contents = file.readAll();
	_hasUnsavedChanges = false;
	return true;
}

bool CSaveableDocument::load()
{
	return load(_filePath);
}

inline bool save(const QByteArray& data, const QString& filePath)
{
	QFile file(filePath);
	assert_and_return_r(file.open(QFile::WriteOnly), false);

	return file.write(data) == (qint64)data.size();
}

bool CSaveableDocument::save()
{
	if (::save(_contents, _filePath))
	{
		_hasUnsavedChanges = false;
		return true;
	}
	else
		return false;
}

bool CSaveableDocument::saveAs(const QString& fileName)
{
	if (::save(_contents, fileName))
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
