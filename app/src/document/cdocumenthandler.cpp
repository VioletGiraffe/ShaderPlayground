#include "cdocumenthandler.h"

DISABLE_COMPILER_WARNINGS
#include <QFileDialog>
RESTORE_COMPILER_WARNINGS

CDocumentHandler::CDocumentHandler(QWidget* parent) : _parent(parent)
{

}

void CDocumentHandler::setDocumentPath(const QString& path)
{
	_document.setFilePath(path);
}

bool CDocumentHandler::open()
{
	const QString path = QFileDialog::getOpenFileName(_parent, "", QFileInfo(_document.filePath()).absolutePath());
	if (path.isEmpty())
		return false;

	_document.setFilePath(path);
	return true;
}

CSaveableDocument::FileLoadResult CDocumentHandler::loadContents()
{
	return _document.load();
}

bool CDocumentHandler::save(const QByteArray& contents)
{
	if (!_document.filePath().isEmpty())
		return _document.save(contents);
	else
		return saveAs(contents);
}

bool CDocumentHandler::saveAs(const QByteArray& contents)
{
	const QString path = QFileDialog::getSaveFileName(_parent, "", QFileInfo(_document.filePath()).absolutePath());
	if (path.isEmpty())
		return false;

	return _document.saveAs(contents, path);
}

QString CDocumentHandler::documentPath() const
{
	return _document.filePath();
}

QString CDocumentHandler::documentName() const
{
	return _document.name();
}

bool CDocumentHandler::hasUnsavedChanges() const
{
	return _document.hasUnsavedChanges();
}

void CDocumentHandler::markAsModified(bool modified)
{
	_document.markAsModified(modified);
}
