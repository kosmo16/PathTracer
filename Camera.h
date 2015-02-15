#ifndef CAMERA_H
#define CAMERA_H

#include "BidirectionalPathTracer/BiDirectionalPathTracer.h"
#include "Math/Matrix4x4.h"
#include "Math/Ray.h"
#include "Math/Vector3.h"
#include "Image.h"
#include "RayTracer.h"
#include "Scene.h"

#include <QList>

class PhotonMap;

//#define PARALLEL

/**
  Camera class
  */
class Camera
{
public:
    Camera();
    /**
      Creates camera with given position and target LPOINT, near and far plane and fov angle
      */
    Camera(Vector3 positon, Vector3 target, float nearPlane, float farPlane, float fov);

    /**
      Returns image to which camera renders
      */
    Image* GetImage() const {
        return img;
    }

    /**
      Sets image to which camera will render
      */
    void SetImage(Image* image) {
        img = image;
    }

    /**
      Returns position of camera
      */
    Vector3 GetPosition() const {
        return position;
    }

    /**
      Sets position of camera
      */
    void SetPosition(Vector3 p) {
        position = p;
    }

    /**
      Returns target LPOINT of camera
      */
    Vector3 GetTarget() const {
        return target;
    }

    /**
      Sets target LPOINT of camera
      */
    void SetTarget(Vector3 t) {
        target = t;
    }

    /**
      Returns near plane of camera
      */
    float GetNearPlane() const {
        return nearPlane;
    }

    /**
      Sets near plane of camera
      */
    void SetNearPlane(float np) {
        nearPlane = np;
    }

    /**
      Returns far plane of camera
      */
    float GetFarPlane() const {
        return farPlane;
    }

    /**
      Set far plane of camera
      */
    void SetFarPlane(float fp) {
        farPlane = fp;
    }

    /** Renders scene without use of photon mapping
        \param scene scene
        \param numSamples number of samples per pixel
    */
    void RenderScene(Scene* scene, unsigned int numSamples=1);

    /** Renders scene with photon mapping
        \param scene scene
        \param numSamples number of samples per pixel
        \param numGlobalMapPhotons number of global map photons
        \param numCausticMapPhotons number of caustic map photons
    */
    void RenderScene(Scene* scene, unsigned int numSamples, unsigned int numGlobalMapPhotons,
                     unsigned int numCausticMapPhotons);

    /** Renders scene with streamed photon mapping
        \param scene scene
        \param numSamples number of samples per pixel
        \param numGlobalMapPhotons number of global map photons
        \param numCausticMapPhotons number of caustic map photons
        \param numAssociatedPhotons number of associated photons
        \param radius radius of photons stream
    */
    void RenderSceneStream(Scene *scene, unsigned ns, unsigned numEmittedGlobalPhotons, unsigned m_numEmittedCausticPhotons, int numAssociatedPhotons, float radius, int reflections);

    void VisualizePhotonMap(Scene* scene, int numPhotons, int maxReflections);
    void VisualizeStreamPhotonMap(Scene* scene, int numPhotons, int maxReflections, int numAssociatedPhotons, float radius);

    void SetResultFileName(const char* filename) {
        renderFileName = QString(filename);
    }

    void SetPhotonMappingParams(float globalMapRadius, int globalMapNumPhotons,
                                float causticMapRadius, int causticMapNumPhotons) {
        rayTracer.SetPhotonMappingParams(globalMapRadius, globalMapNumPhotons,
                                         causticMapRadius, causticMapNumPhotons);
    }

    int renderingTime() const { return m_renderingTime; } // default time in ms /divide by 1000 to get s

    LightIntensity getPixelColor(float x, float y,
                                 float pxWidth, float pxHeight,
                                  Scene * scene);
private:
    Vector3 position;
    Vector3 target;

    float nearPlane;
    float farPlane;
    float fov;
    Image* img;

    float distance;

    Matrix4x4 projectionMatrix;
    Matrix4x4 viewMatrix;
    Matrix4x4 vpMatrix;
    Matrix4x4 invVPMatrix;

    float xFactor;
    float yFactor;

    RayTracer rayTracer;

    void Recalculate();

    QString renderFileName;

    int m_renderingTime;
};

#endif // CAMERA_H
