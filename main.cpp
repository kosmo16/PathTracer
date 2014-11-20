#include <QApplication>
#include <QDebug>
#include "MainWindow.h"

#include "Math/Vector2.h"
#include "Math/Vector3.h"
#include "Math/Vector4.h"
#include "Math/Ray.h"
#include "Math/Plane.h"
#include "Geometry/Sphere.h"
#include "Geometry/Triangle.h"
#include "Geometry/CornellBox.h"
#include "Camera.h"
#include "Image.h"
#include "Material.h"
#include "Lights/POINTLight.h"
#include "Lights/AreaLight.h"
#include "Lights/AmbientLight.h"
#include "Scene.h"
#include "Geometry/MS3DModel.h"
#include "Geometry/Box.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    MainWindow window;
    window.show();

    return a.exec();
}

