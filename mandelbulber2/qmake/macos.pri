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

docFiles.files = $$ROOT/deploy/doc
docFiles.path = Contents/Resources
QMAKE_BUNDLE_DATA += docFiles

newsFiles.files = $$ROOT/deploy/NEWS
newsFiles.path = Contents/Resources/doc
QMAKE_BUNDLE_DATA += newsFiles

languageFiles.files = $$ROOT/language
languageFiles.path = Contents/Resources
QMAKE_BUNDLE_DATA += languageFiles
