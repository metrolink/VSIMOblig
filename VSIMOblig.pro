QT          += core gui widgets

TEMPLATE    = app
CONFIG      += c++17

TARGET      = 3D-eksamen

INCLUDEPATH +=  ./GSL
#INCLUDEPATH += $(LASTOOLS_HOME)\\LASlib\\inc
#INCLUDEPATH += $(LASTOOLS_HOME)\\LASzip\\src
#LIBS *= $(LASTOOLS_HOME)\\LASlib\\lib\\LASlib.lib

SOURCES += main.cpp \
    GSL/matrix2x2.cpp \
    GSL/matrix3x3.cpp \
    GSL/matrix4x4.cpp \
    GSL/vector2d.cpp \
    GSL/vector3d.cpp \
    GSL/vector4d.cpp \
    GSL/gsl_math.cpp \
    collision.cpp \
    lasmap.cpp \
    renderwindow.cpp \
    rollingstone.cpp \
    shader.cpp \
    mainwindow.cpp \
    trianglearray.cpp \
    texture.cpp \
    vertex.cpp \
    xyz.cpp \
    trianglesurface.cpp \
    input.cpp \
    visualobject.cpp \
    camera.cpp \
    octahedronball.cpp \
    tree.cpp \
    plane.cpp \
    beziercurve.cpp

HEADERS += \
    GSL/matrix2x2.h \
    GSL/matrix3x3.h \
    GSL/matrix4x4.h \
    GSL/vector2d.h \
    GSL/vector3d.h \
    GSL/vector4d.h \
    GSL/gsl_math.h \
    GSL/math_constants.h \
    LASLoader.h \
    collision.h \
    lasmap.h \
    renderwindow.h \
    rollingstone.h \
    shader.h \
    mainwindow.h \
    trianglearray.h \
    texture.h \
    vertex.h \
    xyz.h \
    gltypes.h \
    trianglesurface.h \
    input.h \
    visualobject.h \
    camera.h \
    octahedronball.h \
    tree.h \
    plane.h \
    beziercurve.h

FORMS += \
    mainwindow.ui

DISTFILES += \
    plainfragment.frag \
    plainvertex.vert \
    texturefragmet.frag \
    texturevertex.vert
