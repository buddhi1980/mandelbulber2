ROOT = $$PWD/..
SHARED = $$ROOT/deploy/share/mandelbulber2

CONFIG += app_bundle

formulaFiles.files = $$ROOT/formula
formulaFiles.path = Contents/Resources
QMAKE_BUNDLE_DATA += formulaFiles

openClFiles.files = $$ROOT/opencl
openClFiles.path = Contents/Resources
QMAKE_BUNDLE_DATA += openClFiles

openSharedFiles.files = $$SHARED/examples $$SHARED/materials $$SHARED/icons $$SHARED/sounds $$SHARED/textures $$SHARED/toolbar $$SHARED/data
openSharedFiles.path = Contents/Resources
QMAKE_BUNDLE_DATA += openSharedFiles

# one rule fills Contents/Resources/doc: with a second rule for NEWS, a parallel
# make could create the folder in both rules at once, and one of the copies failed
docFiles.files = $$files($$ROOT/deploy/doc/*) $$ROOT/deploy/NEWS
docFiles.path = Contents/Resources/doc
QMAKE_BUNDLE_DATA += docFiles

languageFiles.files = $$ROOT/language
languageFiles.path = Contents/Resources
QMAKE_BUNDLE_DATA += languageFiles
