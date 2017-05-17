#pragma once

#include "csaveabledocument.h"

class QWidget;

class CDocumentHandler
{
public:
	explicit CDocumentHandler(QWidget* parent = nullptr);

	// Supplies a path for the document already obtained it through some other means.
	void setDocumentPath(const QString& path);
	QString documentPath() const;
	QString documentName() const;

	void setContents(const QByteArray& contents);
	QByteArray contents() const;

	bool hasUnsavedChanges() const;

	// Lets the user freely choose a document to open (Ctrl+O).
	bool open();
	// Returns all the contents of a previously open document. If no valid document is associated with this handler, fails.
	bool loadContents();
	// Saves a document. If no path had been specified for it, prompts the user with a save dialog (Ctrl+S).
	bool save();
	// Saves a document under the supplied name (Ctrl+Shift+S).
	bool saveAs();

private:
	void promptToSaveChanges();

private:
	CSaveableDocument _document;
	QWidget* _parent = nullptr;
};
