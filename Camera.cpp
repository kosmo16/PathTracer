#include "Camera.h"

#include "BidirectionalPathTracer/BlinnPhongBrdf.h"
#include "BidirectionalPathTracer/NormalPdf.h"
#include "Math/randomUtils.h"
#include "PhotonMap.h"
#include "StreamPhotonMap.h"

#include <QTime>

#include <cfloat>
#include <fstream>
#include <iostream>
#include <windows.h>

using namespace std;

//miara czasu
double PCFreq = 0.0;
__int64 CounterStart = 0;
ofstream mojStrumien("wyniki.csv");

void StartCounter()
{
    LARGE_INTEGER li;
    if(!QueryPerformanceFrequency(&li))
        qDebug() << "QueryPerformanceFrequency failed!\n";

    PCFreq = double(li.QuadPart)/1000.0;

    QueryPerformanceCounter(&li);
    CounterStart = li.QuadPart;
}

double GetCounter()
{
    LARGE_INTEGER li;
    QueryPerformanceCounter(&li);
    return double(li.QuadPart-CounterStart)/PCFreq;
}
//miara czasu

#define BIAS 0.001f

Camera::Camera()
{
    position.Zero();
    target.Zero();
    target.z = 1;
    nearPlane = 0.001f;
    farPlane = 1000;
    fov = 54;
    img = 0;
    renderFileName = "render";
}

Camera::Camera(Vector3 positon, Vector3 target, float nearPlane, float farPlane, float fov) {
    this->position = positon;
    this->target = target;
    this->nearPlane = nearPlane;
    this->farPlane = farPlane;
    this->fov = fov;
    img = 0;
    renderFileName = "render";
}

/**
  Recalculates camera projection and view matrices.
  */
void Camera::Recalculate() {
    if(img) {
        projectionMatrix.SetPerspective(fov, ((float)img->GetWidth())/img->GetHeight(), nearPlane, farPlane);
        xFactor = 1.0f / projectionMatrix.entries[0];
        yFactor = 1.0f / projectionMatrix.entries[5];

        distance = 1.0f/tanf(fov/2);
        viewMatrix.LoadIdentity();

        Vector3 direction = target - position;
        direction.Normalize();

        //up vector is always up
        Vector3 up = Vector3(0,1,0);

        Vector3 left = up.CrossProduct(direction);
        left.Normalize();

        viewMatrix.SetRow(0, Vector4(left, 0.0f));
        viewMatrix.SetRow(1, Vector4(up, 0.0f));
        viewMatrix.SetRow(2, Vector4(direction, 0.0f));
        viewMatrix.SetRow(3, Vector4(0, 0, 0, 1));

        Matrix4x4 trans;
        trans.SetTranslation(-position);
        viewMatrix = viewMatrix*trans;

        vpMatrix = viewMatrix;
        invVPMatrix = vpMatrix.GetInverse();
    }
}

void Camera::RenderScene(Scene* scene, unsigned int ns) {
    StartCounter();

    Recalculate();

    float pixelW = 1.0f/img->GetWidth();
    float pixelH = 1.0f/img->GetHeight();

    int numSamples=ns;

    if(img) {
        img->Clear(LightIntensity(0,0,0));

#ifdef PARALLEL
#pragma omp parallel for schedule(dynamic, 50)
#endif
        for(unsigned int i=0;i<img->GetWidth()*img->GetHeight();i++) {
            float x = i % img->GetWidth();
            float y = i / img->GetWidth();

            LightIntensity currentPixel;
            for(int sY=-numSamples/2;sY<=numSamples/2;sY++) {
                for(int sX=-numSamples/2;sX<=numSamples/2;sX++) {

                    //cast ray into image
                    float px = 2.0f*((x+pixelW/numSamples*sX)/img->GetWidth()) - 1.0;
                    float py = 2.0f*((y+pixelH/numSamples*sY)/img->GetHeight()) - 1.0;
                    py = -py;
                    px *= xFactor;
                    py *= yFactor;

                    Vector4 origin(0,0,0,1);
                    Vector4 direction(px,py, 1, 0);

                    origin = invVPMatrix*Vector4(origin);
                    direction = invVPMatrix*Vector4(direction);

                    Ray ray(Vector3(origin.x, origin.y, origin.z), Vector3(direction.x, direction.y, direction.z));

                    //and trace it
                    //currentPixel += bidirectionalPathTracer.CalculateLightIntensity(scene, ray, position);
                    currentPixel+=rayTracer.TraceRay(ray, scene, position, 6);
                }
            }
            img->SetPixel(x,y,currentPixel/(numSamples*numSamples));
        }

        m_renderingTime = GetCounter();//(int)time.elapsed();
        mojStrumien<<"Rendering time " << m_renderingTime <<"ms"<<endl;

        img->SaveToFile(renderFileName);
    }
}

LightIntensity Camera::getPixelColor(float x, float y,
                                     float pxWidth, float pxHeight,
                                     const Scene* const &scene,
                                     const BidirectionalPathTracer &bidirectionalPathTracer) const
{
    float pX = x * pxWidth - 1.0f;
    float pY = y * pxHeight - 1.0f;
    pY = -pY;
    pX *= xFactor;
    pY *= yFactor;

    Vector4 origin(0, 0, 0, 1);
    Vector4 direction(pX, pY, 1, 0);

    origin = invVPMatrix * Vector4(origin);
    direction = invVPMatrix * Vector4(direction);

    Ray ray(origin, direction);
    // LightIntensity intensity = rayTracer.TraceRayStream(ray, scene, position, 6, 1050, &photonMap, &causticPhotonMap); // default exposure = 750
    LightIntensity intensity = bidirectionalPathTracer.TracePath(ray, scene, position);

    return intensity;
}

void Camera::RenderSceneStream(Scene* scene, unsigned int ns, unsigned int m_numEmittedGlobalPhotons,
                               unsigned int m_numEmittedCausticPhotons, int numAssociatedPhotons, float radius, int reflections) {
    QTime time;

    BlinnPhongBrdf brdf;
    NormalPdf pdf;
    BidirectionalPathTracer bidirectionalPathTracer(&brdf, &pdf);

    // liczenie czasu nowe
    StartCounter();
    Recalculate();

    int numSamples = ns;
    if(img) {
        img->Clear(LightIntensity(0,0,0));
        time.restart();
        StartCounter();
        StreamPhotonMap photonMap;
        photonMap.SetNumAssociatedPhotons(numAssociatedPhotons);
        photonMap.SetRadius(radius);

        qDebug()<<"generuje mape globalna";
        photonMap.GeneratePhotonMap(scene, m_numEmittedGlobalPhotons, reflections);

        // KAUSTYKA
        StreamPhotonMap causticPhotonMap;
        causticPhotonMap.SetNumAssociatedPhotons(numAssociatedPhotons);
        causticPhotonMap.SetRadius(radius);

        qDebug()<<"generuje mape kaustyczna";
        causticPhotonMap.GeneratePhotonMap(scene, m_numEmittedCausticPhotons, 4, true);

        m_renderingTime = GetCounter(); //time.elapsed();
        mojStrumien<< m_renderingTime <<";";

        time.restart();
        StartCounter();
        mojStrumien<<scene->geometry.count()<<";";
        // przed renderingiem usun sciane frontowa ktora sluzyla tylko do odbijania fotonow podczas propagacji aby nie uciekaly ze sceny
        scene->geometry.at(0)->deleteFrontWall();
        qDebug() << "zaczynam renderowac";

        float pxWidth = 2.0f / img->GetWidth();
        float pxHeight = 2.0f / img->GetHeight();

        qDebug() << "liczba probek na piksel: " << numSamples;
        float one_numSamples = 1.0f / numSamples;

        for(unsigned j=0;j<img->GetHeight();j++)
        {
            if(j % 25 == 0)
            {
                qDebug() << "j =" << j;
            }

            for(unsigned i=0;i<img->GetWidth();i++)
            {
                LightIntensity currentPixel;

                //one sample per pixel
                if(numSamples == 1)
                {
                    //dla kazdego piksela rzutuj promien przez jego srodek
                    float dx = 0.5f;
                    float dy = 0.5f;
                    LightIntensity intensity = getPixelColor(dx + i, dy + j, pxWidth, pxHeight, scene, bidirectionalPathTracer);
                    currentPixel += intensity;
                }
                //stochastic oversampling for multiple samples per pixel
                else
                {
                    //dla kazdego piksela rzutuj n promieni
                    for(int n=0;n<numSamples;n++)
                    {
                        //obliczanie losowej pozycji wewnatrz piksela
                        float dx = randomSignedFloat(1.0f);
                        float dy = randomSignedFloat(1.0f);
                        // float dx = floatRand();
                        // float dy = floatRand();
                        LightIntensity intensity = getPixelColor(dx + i, dy + j, pxWidth, pxHeight, scene, bidirectionalPathTracer);
                        currentPixel += intensity;
                    }
                }

                img->SetPixel(i, j, currentPixel * one_numSamples);
            }
        }

        m_renderingTime = time.elapsed();
        //mojStrumien<<"stary Rendering time " <<  GetCounter() <<"ms"<<endl;

        mojStrumien << GetCounter() << ";";
        mojStrumien << QString("%1;%2;%3;%4;%5")
                       .arg(ns)
                       .arg(m_numEmittedGlobalPhotons)
                       .arg(m_numEmittedCausticPhotons)
                       .arg(numAssociatedPhotons)
                       .arg(radius)
                       .toStdString() << endl;

        renderFileName = QString("kaustycznatrzywiecej odbic_spm_%1_%2_%3_%4_%5_%6")
                .arg(ns)
                .arg(m_numEmittedGlobalPhotons)
                .arg(m_numEmittedCausticPhotons)
                .arg(numAssociatedPhotons)
                .arg(radius)
                .arg(m_renderingTime);
        img->SaveToFile(renderFileName);
    }
}

void Camera::RenderScene(Scene* scene, unsigned ns, unsigned numGlobalMapPhotons, unsigned numCausticMapPhotons)
{
    Recalculate();
    StartCounter();

    float pixelW = 1.0f/img->GetWidth();
    float pixelH = 1.0f/img->GetHeight();

    int numSamples=ns;
    if(img) {
        img->Clear(LightIntensity(0,0,0));

        PhotonMap photonMap;

        photonMap.GeneratePhotonMap(scene, numGlobalMapPhotons, 6);

        PhotonMap causticPhotonMap;
        causticPhotonMap.GeneratePhotonMap(scene, numCausticMapPhotons, 2, true);

        StartCounter();
#ifdef PARALLEL
#pragma omp parallel for schedule(dynamic, 50)
#endif
        for(unsigned int i=0;i<img->GetWidth()*img->GetHeight();i++) {
            float x = i % img->GetWidth();
            float y = i / img->GetWidth();

            LightIntensity currentPixel;
            for(int sY=-numSamples/2;sY<=numSamples/2;sY++) {
                for(int sX=-numSamples/2;sX<=numSamples/2;sX++) {
                    float px = 2.0f*((x+pixelW/numSamples*sX)/img->GetWidth()) - 1.0;
                    float py = 2.0f*((y+pixelH/numSamples*sY)/img->GetHeight()) - 1.0;
                    py = -py;
                    px *= xFactor;
                    py *= yFactor;

                    Vector4 origin(0,0,0,1);
                    Vector4 direction(px,py, 1, 0);

                    origin = invVPMatrix*Vector4(origin);
                    direction = invVPMatrix*Vector4(direction);

                    Ray ray(Vector3(origin.x, origin.y, origin.z), Vector3(direction.x, direction.y, direction.z));

                    currentPixel += rayTracer.TraceRay(ray, scene, position, 6, 750, &photonMap, &causticPhotonMap);
                }
            }
            img->SetPixel(x,y,currentPixel/(numSamples*numSamples));
        }

        m_renderingTime = GetCounter();
        qDebug()<<"Rendering time " << m_renderingTime <<"ms"<<endl;

        renderFileName = QString("pm_%1_%2_%3_%4").arg(ns).arg(numGlobalMapPhotons).arg(numCausticMapPhotons).arg(m_renderingTime);
        img->SaveToFile(renderFileName);
    }
}

void Camera::VisualizePhotonMap(Scene *scene, int numPhotons, int maxReflections)
{
    QTime time;
    Recalculate();
    if(img) {
        img->Clear(LightIntensity(0,0,0));

        PhotonMap photonMap;

        photonMap.GeneratePhotonMap(scene, numPhotons, maxReflections, true);

        time.restart();
#ifdef PARALLEL
#pragma omp parallel for
#endif
        for(unsigned int i=0;i<img->GetWidth()*img->GetHeight();i++) {
            float x = i % img->GetWidth();
            float y = i / img->GetWidth();
            float X = x;
            float Y = y;

            x = 2.0f * x / img->GetWidth() - 1.0f;
            y = 2.0f * y / img->GetHeight() - 1.0f;
            y = -y;
            x *= xFactor;
            y *= yFactor;

            Vector4 origin(0,0,0,1);
            Vector4 direction(x,y,1,0);

            origin = invVPMatrix*Vector4(origin);
            direction = invVPMatrix*Vector4(direction);

            Ray ray(Vector3(origin.x, origin.y, origin.z), Vector3(direction.x, direction.y, direction.z));

            LightIntensity resultIntensity;

            int closest=-1;
            float closestDist=FLT_MAX;
            IntersectionResult closestIntersection;

            IntersectionResult result;
            for(int j=0;j<scene->geometry.count();j++) {
                result = scene->geometry.at(j)->Intersects(ray);
                if(result.type!=MISS) {
                    if(closestDist>result.distance) {
                        closestDist = result.distance;
                        closest = j;
                        closestIntersection = result;
                    }
                }
            }

            if(closest!=-1) {
                if(closestIntersection.object->GetMaterial()->type==REFLECTIVE) {
                    resultIntensity = LightIntensity(0,0,0);
                }
                else if(closestIntersection.object->GetMaterial()->type==REFRACTIVE) {
                    resultIntensity = LightIntensity(0,0,0);
                }
                else {
                    QList<Photon*> closest = photonMap.GetClosestPhotons(closestIntersection.LPOINT, 0.1, 2);

                    if(closest.count()!=0) {
                        float r = (closest[0]->position-closestIntersection.LPOINT).GetLength();

                        LightIntensity E;
                        for(int z=0;z<closest.count();z++) {
                            float dot = closestIntersection.intersectionLPOINTNormal.DotProduct(closest[z]->direction);
                            if(dot>0) {
                                E += 2000*closest[z]->energy*dot;
                            }
                        }

                        resultIntensity += (E/(M_PI*r*r));
                    }
                }
            }
            img->SetPixel(X,Y,resultIntensity);
        }

        m_renderingTime = time.elapsed();
        qDebug()<<"Rendering time " << m_renderingTime <<"ms";

        renderFileName = QString("vpm_%1_%2_%3")
                .arg(numPhotons)
                .arg(maxReflections)
                .arg(m_renderingTime);
        img->SaveToFile(renderFileName);
    }
}

void Camera::VisualizeStreamPhotonMap(Scene *scene, int numPhotons, int maxReflections, int numAssociatedPhotons, float radius)
{
    QTime time;
    Recalculate();
    if(img) {
        img->Clear(LightIntensity(0,0,0));

        StreamPhotonMap photonMap;
        photonMap.SetNumAssociatedPhotons(numAssociatedPhotons);
        photonMap.SetRadius(radius);

        photonMap.GeneratePhotonMap(scene, numPhotons, maxReflections, false);

        time.restart();
#ifdef PARALLEL
#pragma omp parallel for
#endif
        for(unsigned int i=0;i<img->GetWidth()*img->GetHeight();i++) {
            float x = i % img->GetWidth();
            float y = i / img->GetWidth();
            float X = x;
            float Y = y;

            x = 2.0f * x / img->GetWidth() - 1.0f;
            y = 2.0f * y / img->GetHeight() - 1.0f;
            y = -y;
            x *= xFactor;
            y *= yFactor;

            Vector4 origin(0,0,0,1);
            Vector4 direction(x,y, 1, 0);

            origin = invVPMatrix*Vector4(origin);
            direction = invVPMatrix*Vector4(direction);

            Ray ray(Vector3(origin.x, origin.y, origin.z), Vector3(direction.x, direction.y, direction.z));

            LightIntensity resultIntensity;

            int closest=-1;
            float closestDist=FLT_MAX;
            IntersectionResult closestIntersection;

            IntersectionResult result;
            for(int j=0;j<scene->geometry.count();j++) {
                result = scene->geometry.at(j)->Intersects(ray);
                if(result.type!=MISS) {
                    if(closestDist>result.distance) {
                        closestDist = result.distance;
                        closest = j;
                        closestIntersection = result;
                    }
                }
            }

            if(closest!=-1) {
                if(closestIntersection.object->GetMaterial()->type==REFLECTIVE) {
                    resultIntensity = LightIntensity(0,0,0);
                }
                else if(closestIntersection.object->GetMaterial()->type==REFRACTIVE) {

                    resultIntensity = LightIntensity(0,0,0);
                }
                else {
                    QList<Stream*> closest = photonMap.GetClosestPhotons(closestIntersection.LPOINT, 0.1f, 2); //second param is the searching radius -> size of the "visualized dot"

                    if(closest.count()!=0) {
                        float r = (closest[0]->position-closestIntersection.LPOINT).GetLength();

                        LightIntensity E;
                        for(int z=0;z<closest.count();z++) {
                            //qDebug()<<closest[z]->associatedPhoton.count();
                            for(int j=0;j<closest[z]->associatedPhoton.count();j++) {
                                float dot = closestIntersection.intersectionLPOINTNormal.DotProduct(closest[z]->associatedPhoton[j].direction);
                                if(dot>0) {
                                    E += 2000*closest[z]->associatedPhoton[j].energy*dot;
                                }
                            }

                            float dot = closestIntersection.intersectionLPOINTNormal.DotProduct(closest[z]->leadingPhoton.direction);
                            if(dot>0) {
                                E += 2000*closest[z]->leadingPhoton.energy*dot;
                            }
                        }

                        resultIntensity += (E/(M_PI*r*r));
                    }
                }
            }
            img->SetPixel(X,Y,resultIntensity);
        }

        m_renderingTime = time.elapsed();
        qDebug() << "Rendering time " << m_renderingTime << "ms";

        renderFileName = QString("vspm_%1_%2_%3_%4_%5")
                .arg(numPhotons)
                .arg(maxReflections)
                .arg(numAssociatedPhotons)
                .arg(radius)
                .arg(m_renderingTime);
        img->SaveToFile(renderFileName);
    }
}
