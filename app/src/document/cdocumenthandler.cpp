#include "cdocumenthandler.h"

DISABLE_COMPILER_WARNINGS
#include <QFileDialog>
#include <QMessageBox>
#include <QStringBuilder>
RESTORE_COMPILER_WARNINGS

CDocumentHandler::CDocumentHandler(QWidget* parent) : _parent(parent)
{
}

void CDocumentHandler::setDocumentPath(const QString& path)
{
	_document.setFilePath(path);
}

// Creates a new document (empty, or with the optional supplied contents); gracefully handles unsaved changes
void CDocumentHandler::newDocument(const QByteArray& contents)
{
	handleUnsavedChanges();
	_document.newDocument(contents);
}

bool CDocumentHandler::open()
{
	const QString path = QFileDialog::getOpenFileName(_parent, "", QFileInfo(_document.filePath()).absolutePath());
	if (path.isEmpty())
		return false;

	_document.setFilePath(path);
	return true;
}

bool CDocumentHandler::loadContents()
{
	hasUnsavedChanges();
	return _document.load();
}

bool CDocumentHandler::save()
{
	if (!_document.filePath().isEmpty())
		return _document.save();
	else
		return saveAs();
}

bool CDocumentHandler::saveAs()
{
	const QString path = QFileDialog::getSaveFileName(_parent, "", QFileInfo(_document.filePath()).absolutePath());
	if (path.isEmpty())
		return false;

	return _document.saveAs(path);
}

QString CDocumentHandler::documentPath() const
{
	return _document.filePath();
}

QString CDocumentHandler::documentName() const
{
	return _document.name();
}

void CDocumentHandler::setContents(const QByteArray& contents)
{
	_document.setContents(contents);
}

QByteArray CDocumentHandler::contents() const
{
	return _document.contents();
}

bool CDocumentHandler::hasUnsavedChanges() const
{
	return _document.hasUnsavedChanges();
}

void CDocumentHandler::handleUnsavedChanges()
{
	if (!_document.hasUnsavedChanges())
		return;

	if (QMessageBox::question(_parent, "Unsaved changes detected", "There are unsaved changes in " % _document.name() % ". Do you want to save them?") == QMessageBox::Yes)
		save();
}
