cmake_minimum_required(VERSION 3.5)

project(SecNote LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 需要在Clion的设置中的CMAKE设置中添加一个变量来指定QT的CMAKE目录：
# -DCMAKE_PREFIX_PATH="C:/Qt/5.15.2/msvc2019_64/lib/cmake"

IF (MSVC)
    SET(OPTIONS WIN32)
    add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
    add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")
ENDIF (MSVC)

find_package(Qt5 COMPONENTS REQUIRED Widgets LinguistTools Gui Core)

set(TS_FILES SafetyNotebook_zh_CN.ts.ts)
set(CPP_FILES
        main.cpp
        mainwindow.cpp
        newdirdialog.cpp
        openonlinedialog.cpp
        passworddialog.cpp
        settings.cpp
        settingsdialog.cpp
        loader.cpp
        onlinefileinfomodel.cpp
        qaesencryption.cpp
        PasswordGenerateDialog.cpp
        mtextedit.cpp)

set(H_FILES
        mainwindow.h
        newdirdialog.h
        openonlinedialog.h
        passworddialog.h
        settings.h
        settingsdialog.h
        loader.h
        onlinefileinfomodel.h
        qaesencryption.h
        PasswordGenerateDialog.h
        mtextedit.h)

set(UI_FILES
        mainwindow.ui
        newdirdialog.ui
        openonlinedialog.ui
        passworddialog.ui
        settingsdialog.ui
        passwordgeneratedialog.ui)

set(QRC_FILES
        resource.qrc)

set(QRC_SRC
        main.cpp SNoteItem.cpp SNoteItem.h)

qt5_wrap_ui(UI_SRC ${UI_FILES})
qt5_wrap_cpp(MOC_SRC ${H_FILES})
qt5_add_resources(QRC_SRC ${QRC_FILES})

add_executable(SafetyNotebook ${OPTIONS}
        ${CPP_FILES}
        ${H_FILES}
        ${UI_SRC}
        ${MOC_SRC}
        ${QRC_SRC}
        #${TS_FILES}
        )

#add_executable(
#        SecNote
#        ${CPP_FILES}
#        ${H_FILES}
#        ${UI_FILES}
#        ${QRC_FILES}
#)

target_link_libraries(SafetyNotebook PRIVATE Qt5::Widgets Qt5::Core Qt5::Gui)

#qt5_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
