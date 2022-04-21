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

	// Creates a new document (empty, or with the optional supplied contents); gracefully handles unsaved changes
	void newDocument(const QByteArray& contents = QByteArray());
	// Lets the user freely choose a document to open (Ctrl+O).
	void open();
	// Returns all the contents of a previously open document. If no valid document is associated with this handler, fails.
	bool loadContents();
	// Saves a document. If no path had been specified for it, prompts the user with a save dialog (Ctrl+S).
	bool save();
	// Saves a document under the supplied name (Ctrl+Shift+S).
	bool saveAs();

private:
	void handleUnsavedChanges();

private:
	CSaveableDocument _document;
	QWidget* _parent = nullptr;
};
