#include "PhotonMap.h"

#include "Lights/AreaLight.h"
#include "Math/randomUtils.h"
#include "KDTree.h"

#include <cfloat>

#define BIAS 0.001f

PhotonMap::PhotonMap() {
    maxPhotons = 5000000;
    kdTree = new KDTree<Photon>();
}

PhotonMap::~PhotonMap() {
    qDeleteAll(photons);
    photons.clear();

    delete kdTree;
}


void PhotonMap::GeneratePhotonMap(Scene *scene, int numPhotons, int maxReflections, bool caustic) {
    int numPOINTLights=0;
    //count how much LPOINT lights are in scene
    for(int i=0;i<scene->lights.count();i++) {
        if(scene->lights.at(i)->type==LPOINT || scene->lights.at(i)->type == AREA) {
            numPOINTLights++;
            if(caustic) {
                //if we generate caustic map, we must generate first projection map for each light
                scene->lights.at(i)->CreateProjectionMap(scene);
            }
        }
    }
    qDebug()<<"liczba swiatel punktowych w scenie "<<numPOINTLights;
    //adjust number of photons for each light
    numPhotons/=numPOINTLights;

    for(int i=0;i<scene->lights.count();i++) {
        if(scene->lights.at(i)->type==LPOINT || scene->lights.at(i)->type==AREA)
            //generate photons for each light in scene
            GeneratePhotons(scene->lights.at(i), &scene->geometry, numPhotons, caustic, maxReflections);
    }


    //build kdtree from photons
    kdTree->Build(photons);
    photons.clear();
}


//generates photons for given light
void PhotonMap::GeneratePhotons(AmbientLight *light, QList<Geometry*>* geometry, int numPhotons, bool caustic, int maxReflections) {

    QList<Photon*> tempPhotons;
    int emittedPhotons = 0;

    //while(tempPhotons.count()<numPhotons && photons.count()+tempPhotons.count()<maxPhotons) {
    while(emittedPhotons<numPhotons && photons.count()+tempPhotons.count()<maxPhotons) {
    Ray photonRay;

        //generate photon from light
        photonRay = light->GetPhoton(caustic);

        //trace photon
        TracePhoton(light->color, photonRay, geometry, &tempPhotons, maxReflections);
        emittedPhotons++;
    }
    qDebug()<<"Wyemitowalem "<<emittedPhotons<<" fotonow";

    //scale energy of photons
    float scale;// TODO initialize scale
    if(caustic) {
        if(light->type == LPOINT || light->type == AREA)
            scale = (1.0f/tempPhotons.count())*light->GetProjectionMapRatio();
    }
    else
        scale = 1.0f/tempPhotons.count();

    for(int i=0;i<tempPhotons.count();i++) {

        tempPhotons[i]->energy*=scale;
    }

    photons.append(tempPhotons);
    qDebug()<<"Mapa fotonowa zawiera "<<photons.count()<<"fotonow (wyemitowane + odbite)";
}


//Traces way of photon in scene
void PhotonMap::TracePhoton(LightIntensity photonEnergy, const Ray &startRay, QList<Geometry *> *geometry,
                            QList<Photon *> *photons, int reflections) {

    int closest=-1;
    float closestDist=FLT_MAX;
    IntersectionResult closestIntersection;

    IntersectionResult result;
    //check intersection of ray with geometry in scene
    for(int j=0;j<geometry->count();j++) {
        result = geometry->at(j)->Intersects(startRay);
        if(result.type!=MISS) {
            if(closestDist>result.distance) {
                closestDist = result.distance;
                closest = j;
                closestIntersection = result;
            }
        }
    }
    //get the closest intersection
    if(closest!=-1) {

        //if photon intersects with reflective object and we can still reflect photon
        if(closestIntersection.object->GetMaterial()->type==REFLECTIVE && reflections>0) {
            //calculate reflected vector
            Vector3 reflected = startRay.direction.Reflect(closestIntersection.intersectionLPOINTNormal);
            reflected.Normalize();
            //create new ray, slighty biased to avoid artifacts
            Ray newRay(closestIntersection.LPOINT+reflected*BIAS, reflected);

            //trace new reflected photon
            TracePhoton(photonEnergy, newRay, geometry, photons, reflections-1);
        }
        //if photon intersects with refractive object and we can still reflect photon
        else if(closestIntersection.object->GetMaterial()->type==REFRACTIVE && reflections>0) {
            RefractiveMaterial* mat = (RefractiveMaterial*)closestIntersection.object->GetMaterial();

            //calculate refracted vector
            Vector3 refracted;
            //if we hit outside of geometry
            if(closestIntersection.type==HIT)
                refracted = startRay.direction.Refract(closestIntersection.intersectionLPOINTNormal,
                                                  mat->etaRate);
            //if we hit inside of geometry
            else
                refracted = startRay.direction.Refract(-closestIntersection.intersectionLPOINTNormal,
                                                  1.0f/mat->etaRate);
            refracted.Normalize();

            Ray newRay(closestIntersection.LPOINT+refracted*BIAS, refracted);

            TracePhoton(photonEnergy, newRay, geometry, photons, reflections-1);
        }
        //we hit diffuse geometry
        else {

            DiffuseMaterial* mat = (DiffuseMaterial*)closestIntersection.object->GetMaterial();

            //new photon to store in photon map
            Photon* p = new Photon;
            //current photon energy
            p->energy = photonEnergy;
            //position of photon
            p->position = closestIntersection.LPOINT;
            //direction of income
            p->direction = -startRay.direction;
            photons->append(p);

            //russian rullette
            if(PropabilityOfAbsorption()<mat->absorptionPropability && reflections>0) {
                float avgDiff = (mat->diffuse.r+mat->diffuse.g+mat->diffuse.b)/3;
                //update photon energy
                photonEnergy *= (mat->diffuse/avgDiff);
                //get new photon direction
                Vector3 reflected = LambertReflectionDirection(closestIntersection);
                Ray newRay(closestIntersection.LPOINT+reflected*BIAS, reflected);
                TracePhoton(photonEnergy, newRay, geometry, photons, reflections-1);
            }
        }
    }
}

float PhotonMap::PropabilityOfAbsorption() {
    return randomUnsignedFloat(1.0f);
}

Vector3 PhotonMap::LambertReflectionDirection(const IntersectionResult &ir) {

    float x,y,z;
    do {
        x = randomSignedFloat(1.0f);
        y = randomSignedFloat(1.0f);
        z = randomSignedFloat(1.0f);
    } while(x*x + y*y + z*z > 1.0f);

    Vector3 direction(x,y,z);

    if(direction.DotProduct(ir.intersectionLPOINTNormal)<0)
        direction*=-1;

    return direction;
}

QList<Photon*> PhotonMap::GetClosestPhotons(Vector3 LPOINT, float radius, int maxPhotons) {
    return kdTree->FindClosest(LPOINT, radius, maxPhotons);
}
