QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    mainwindow.h \
    archiver.h \
    check_directory_properties.h \
    create_parent_dirs.h \
    get_directory_structure.h \
    haffman_tree_builder.h \
    haffman_tree_packer.h \
    haffman_tree_restorer.h \
    stack.h \
    unarchiver.h

LIBS += "C:\Users\Fox and Dragon\Documents\Archiver\libarchiver.dll" \
    "C:\Users\Fox and Dragon\Documents\Archiver\libcheck_directory_properties.dll" \
    "C:\Users\Fox and Dragon\Documents\Archiver\libcreate_parent_dirs.dll" \
    "C:\Users\Fox and Dragon\Documents\Archiver\libget_directory_structure.dll" \
    "C:\Users\Fox and Dragon\Documents\Archiver\libunarchiver.dll"

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

#win32: LIBS += -L$$PWD/./ -larchiver

#INCLUDEPATH += $$PWD/.
#DEPENDPATH += $$PWD/.

#win32:!win32-g++: PRE_TARGETDEPS += $$PWD/archiver.lib
#else:win32-g++: PRE_TARGETDEPS += $$PWD/libarchiver.a


