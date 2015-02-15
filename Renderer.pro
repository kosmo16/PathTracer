######################################################################
# Automatically generated by qmake (2.01a) sob. mar 10 10:49:58 2012
######################################################################

TEMPLATE = app
TARGET = Renderer
DEPENDPATH += .
QMAKE_CXXFLAGS += -openmp
QT += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
# Input
HEADERS += \
    BidirectionalPathTracer/BlinnPhongBrdf.h \
    BidirectionalPathTracer/brdf.h \
    BidirectionalPathTracer/dotBrdf.h \
    BidirectionalPathTracer/Node.h \
    BidirectionalPathTracer/outNormalBrdf.h \
    BidirectionalPathTracer/pdf.h \
    Geometry/Box.h \
    Geometry/CornellBox.h \
    Geometry/Geometry.h \
    Geometry/MS3DModel.h \
    Geometry/Sphere.h \
    Geometry/Triangle.h \
    Lights/AmbientLight.h \
    Lights/AreaLight.h \
    Lights/POINTLight.h \
    Math/Matrix4x4.h \
    Math/Plane.h \
    Math/randomUtils.h \
    Math/Ray.h \
    Math/Vector2.h \
    Math/Vector3.h \
    Math/Vector4.h \
    Camera.h \
    DisplayWindow.h \
    Image.h \
    KDTree.h \
    LightIntensity.h \
    MainWindow.h \
    Material.h \
    PhotonMap.h \
    RayTracer.h \
    rendereroptions.h \
    rendererpanel.h \
    Scene.h \
    StreamPhotonMap.h \
    Texture.h \
    BidirectionalPathTracer/pathtracer.h \
    BidirectionalPathTracer/semicirclePdf.h \
    BidirectionalPathTracer/hemispherePdf.h

SOURCES += main.cpp \
    BidirectionalPathTracer/BlinnPhongBrdf.cpp \
    BidirectionalPathTracer/brdf.cpp \
    BidirectionalPathTracer/dotBrdf.cpp \
    BidirectionalPathTracer/Node.cpp \
    BidirectionalPathTracer/outNormalBrdf.cpp \
    BidirectionalPathTracer/pdf.cpp \
    Geometry/Box.cpp \
    Geometry/CornellBox.cpp \
    Geometry/MS3DModel.cpp \
    Geometry/Sphere.cpp \
    Geometry/Triangle.cpp \
    Lights/AmbientLight.cpp \
    Lights/AreaLight.cpp \
    Lights/POINTLight.cpp \
    Math/Matrix4x4.cpp \
    Math/Plane.cpp \
    Math/randomUtils.cpp \
    Math/Ray.cpp \
    Math/Vector2.cpp \
    Math/Vector3.cpp \
    Math/Vector4.cpp \
    Camera.cpp \
    DisplayWindow.cpp \
    Image.cpp \
    KDTree.cpp \
    LightIntensity.cpp \
    MainWindow.cpp \
    Material.cpp \
    PhotonMap.cpp \
    RayTracer.cpp \
    rendereroptions.cpp \
    rendererpanel.cpp \
    Scene.cpp \
    StreamPhotonMap.cpp \
    Texture.cpp \
    BidirectionalPathTracer/pathtracer.cpp \
    BidirectionalPathTracer/semicirclePdf.cpp \
    BidirectionalPathTracer/hemispherePdf.cpp
