TEMPLATE = subdirs

SUBDIRS = app cpputils cpp-template-utils qtutils

app.depends = qtutils cpputils
cpputils.depends = cpp-template-utils
qtutils.depends = cpputils
