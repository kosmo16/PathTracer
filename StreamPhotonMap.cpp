#include "Math/randomUtils.h"
#include "Lights/AreaLight.h"
#include "KDTree.h"
#include "StreamPhotonMap.h"

#include <cfloat>

#define BIAS 0.001f

const float RADIUS=2.5;

StreamPhotonMap::StreamPhotonMap() {
    maxPhotons=9000000;
    kdTree = new KDTree<Stream>();
}

StreamPhotonMap::~StreamPhotonMap() {
    qDeleteAll(photons);
    photons.clear();

    delete kdTree;
}

void StreamPhotonMap::GeneratePhotonMap(const Scene* const &scene, int numPhotons, int maxReflections, bool caustic) {
    int numPOINTLights=0;
    //count how much LPOINTs lights are in scene
    for(int i=0;i<scene->lights.count();i++) {
        // qDebug() << "swiatlo " << i << "/" << scene->lights.count() << "\n";
        if(scene->lights.at(i)->type==LPOINT || scene->lights.at(i)->type == AREA) {
            numPOINTLights++;
            if(caustic) {
                //if we generate caustic map, we must generate first projection map for each light
                qDebug()<<"generuje mape projekcji dla swiatla";
                scene->lights.at(i)->CreateProjectionMap(scene);
            }
        }
    }
    qDebug()<<"liczba swiatel w scenie "<<numPOINTLights;

    //adjust number of photons for each light
    numPhotons/=numPOINTLights;

    for(int i=0;i<scene->lights.count();i++) {
        if(scene->lights.at(i)->type==LPOINT || scene->lights.at(i)->type==AREA)
            //generate photons for each light in scene

            qDebug()<<"emituje fotony dla zrodla";

            qDebug()<<"dla kazdego fotonu ze zrodla ma byc " <<maxReflections << "odbic";
            GeneratePhotons(scene->lights.at(i), &scene->geometry, numPhotons, caustic, maxReflections);
    }

    //build kdtree from photons
    kdTree->Build(photons);
    photons.clear();
}

//generate streams for given light
void StreamPhotonMap::GeneratePhotons(AmbientLight *light, const QList<Geometry*>* const &geometry, int numPhotons, bool caustic, int maxReflections) {

    QList<Stream*> tempPhotons;
    //qDebug()<<"mam wyemitowac" <<numPhotons <<"fotonow";
    int emittedPhotons = 0;

    //while(tempPhotons.count()<numPhotons && photons.count()+tempPhotons.count()<maxPhotons) {
    //while(tempPhotons.count()<numPhotons) {
    while(emittedPhotons<numPhotons && photons.count()+tempPhotons.count()<maxPhotons) {
        Ray photonRay;
        //qDebug()<<tempPhotons.count();
        //generate lead photon
        photonRay = light->GetPhoton(caustic);

        //qDebug()<<"wyemitowalem foton wiodacy";
        Stream startStream;
        //jesli jest to foton niekaustyczny to wygeneruj fotony stowarzyszone w strumieniu
        if(!caustic)
        {
            //add associated photons.
            //At begining all associated photons have the same position as lead photon and energy = 1 / assoc photons count
            for(int j=0;j<numAssociatedPhotons;j++) {
                SinglePhoton sp;
                sp.position = photonRay.origin;
                sp.energy = light->color/numAssociatedPhotons;
                startStream.associatedPhoton.push_back(sp);
            }
        }
        //qDebug()<<"Poczatkowo strumien ma"<<startStream.associatedPhoton.count()<<"fotonow stowarzyszonych";
        TracePhoton(light->color, photonRay, geometry, &tempPhotons, &startStream, maxReflections);
        //qDebug()<<"koniec "<<tempPhotons.count();
        emittedPhotons++;
    }
    qDebug()<<"Wyemitowalem "<<emittedPhotons<<" fotonow wiodacych";

    //przeskaluj energie kazdego fotonu przez liczbe zapisanych do mapy fotonow
    float scale;
    if(caustic) {
        if(light->type == LPOINT || light->type == AREA)
            scale = (1.0f/tempPhotons.count())*light->GetProjectionMapRatio();
    }
    else
        scale = 1.0f/tempPhotons.count();

    for(int i=0;i<tempPhotons.count();i++) {
        tempPhotons[i]->leadingPhoton.energy*=scale;
        for(int j=0;j<tempPhotons[i]->associatedPhoton.count();j++)
            tempPhotons[i]->associatedPhoton[j].energy*=scale;
    }

    photons.append(tempPhotons);
    qDebug()<<"Mapa fotonowa zawiera "<<photons.count()<<"fotonow (wyemitowane + odbite)";
}

//traces photon stream
void StreamPhotonMap::TracePhoton(LightIntensity photonEnergy, const Ray &startRay, const QList<Geometry*>* const &geometry,
                                  QList<Stream *> *photons, Stream* parent, int reflections) {

    int closest=-1;
    float closestDist=FLT_MAX;
    IntersectionResult closestIntersection;

    IntersectionResult result;
    //check intersection of lead photon with geometry in scene
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

    if(closest!=-1) {
        //if photon intersects with reflective object and we can still reflect photon
        if(closestIntersection.object->GetMaterial()->type==REFLECTIVE && reflections>0) {
            Vector3 reflected = startRay.direction.Reflect(closestIntersection.intersectionLPOINTNormal);
            reflected.Normalize();
            Ray newRay(closestIntersection.LPOINT+reflected*BIAS, reflected);

            //zlicz energie stowarzyszonych fotonow i przekaz ja do wiodacego fotonu (koncentracja energii dla uwydatnienia kaustyki)
            LightIntensity tempE;
            for(int j=0;j<parent->associatedPhoton.count();j++) {
                tempE += parent->associatedPhoton[j].energy;
            }
            photonEnergy += tempE;

            //temporary parent stream with no associated photons
            Stream tempStream;

            TracePhoton(photonEnergy, newRay, geometry, photons, &tempStream, reflections-1);
        }
        else if(closestIntersection.object->GetMaterial()->type==REFRACTIVE && reflections>0) {
            RefractiveMaterial* mat = (RefractiveMaterial*)closestIntersection.object->GetMaterial();

            Vector3 refracted;
            if(closestIntersection.type==HIT)
                refracted = startRay.direction.Refract(closestIntersection.intersectionLPOINTNormal, mat->etaRate);
            else
                refracted = startRay.direction.Refract(-closestIntersection.intersectionLPOINTNormal, 1.0f/mat->etaRate);
            refracted.Normalize();

            Ray newRay(closestIntersection.LPOINT+refracted*BIAS, refracted);

            //zlicz energie stowarzyszonych fotonow i przekaz ja do wiodacego fotonu (koncentracja energii dla uwydatnienia kaustyki)
            LightIntensity tempE;
            for(int j=0;j<parent->associatedPhoton.count();j++) {
                tempE += parent->associatedPhoton[j].energy;
            }
            photonEnergy += tempE;
            //temporary parent stream with no associated photons
            Stream tempStream;

            TracePhoton(photonEnergy, newRay, geometry, photons, &tempStream, reflections-1);
        }
        else {
            //we hit diffuse geometry
            //qDebug()<<"wiodacy trafil w dyfuzyjna";

            DiffuseMaterial* mat = (DiffuseMaterial*)closestIntersection.object->GetMaterial();

            //new photon stream to store in stream map
            Stream* newStream = new Stream;
            newStream->leadingPhoton.energy = photonEnergy;
            newStream->leadingPhoton.position = closestIntersection.LPOINT;
            newStream->position = closestIntersection.LPOINT;
            newStream->leadingPhoton.direction = -startRay.direction;

            //tymczasowa lista do przechowywania odbitych fotonow stowarzyszonych
            QVector<SinglePhoton> reflectedAssocPhotons;

            //qDebug()<<"energia zapisanego fotonu wiodacego: "<<photonEnergy.r<<photonEnergy.g<<photonEnergy.b;

            //generate new associated photons
            int ileAssoc = 0;
            //qDebug()<<"dotychczas w strumieniu bylo: "<<parent->associatedPhoton.count();
            //qDebug()<<"dotychczasowy promien strumienia: "<<radius;
            for(int j=0;j<parent->associatedPhoton.count();j++) {
                float x,y,z;
                do {
                    x = randomSignedFloat();
                    y = randomSignedFloat();
                    z = randomSignedFloat();
                } while(x*x+y*y+z*z>1);
                //pozycja punktu koncowego wektora wyznaczajacego nowy foton stowarzyszony
                Vector3 newAssociatedPos(x,y,z);
                newAssociatedPos*=radius;
                newAssociatedPos += closestIntersection.LPOINT;

                //check if ray from old associated photon to new intersects something
                /*
                Ray oldAssociatedToNew(parent->associatedPhoton[j].position, newAssociatedPos - parent->associatedPhoton[j].position);
                IntersectionResult iresult;
                for(int k=0;k<geometry->count();k++) {
                    iresult = geometry->at(k)->Intersects(oldAssociatedToNew, (newAssociatedPos - parent->associatedPhoton[j].position).GetLength());
                    if(iresult.type!=MISS) {
                       break;
                    }
                }
                */

                //check if ray from old associated photon to new temp LPOINT intersects something
                float closestDistance=FLT_MAX;
                IntersectionResult tempClosestIntersection;
                Ray oldAssociatedToNew(parent->associatedPhoton[j].position, newAssociatedPos - parent->associatedPhoton[j].position);
                IntersectionResult iresult;
                for(int k=0;k<geometry->count();k++) {
                    iresult = geometry->at(k)->Intersects(oldAssociatedToNew);//, (newAssociatedPos - parent->associatedPhoton[j].position).GetLength());
                    if(iresult.type!=MISS) {
                        if(closestDistance>iresult.distance) {
                            closestDistance = iresult.distance;
                            tempClosestIntersection = iresult;
                        }
                    }
                }

                //jesli promien nowego fotonu trafil w geometrie
                if(tempClosestIntersection.type!=MISS)
                {
                    //qDebug()<<"znalazlem przeciecie stowarzyszonego";
                    DiffuseMaterial* tempMat = (DiffuseMaterial*)tempClosestIntersection.object->GetMaterial();

                    //sprawdz czy przeciecie najblizsze wzgledem starej pozycji fotonu stowarzyszonego
                    //jest w odleglosci wiekszej niz promien strumienia od nowej pozycji fotonu wiodacego
                    if( (tempClosestIntersection.LPOINT - newStream->leadingPhoton.position).GetLength() > radius)
                    {
                        //qDebug()<<"foton stowarzyszony zablokowany";
                        continue;
                    }
                    //jesli przeciecie w odleglosci mniejszej niz promieñ strumienia to sprawdz material
                    else if(tempMat->type == DIFFUSE)
                    {
                        //qDebug()<<"foton stowarzyszony trafil w powierzchnie dyfuzyjna"<<tempMat->diffuse.r<<tempMat->diffuse.g<<tempMat->diffuse.b;

                        //utworz nowy foton stowarzyszony w znalezionym punkcie przeciecia
                        SinglePhoton newAssociated;
                        newAssociated.position = tempClosestIntersection.LPOINT;
                        newAssociated.direction = -oldAssociatedToNew.direction;
                        newAssociated.energy = parent->associatedPhoton[j].energy;
                        newStream->associatedPhoton.push_back(newAssociated);
                        //jego energia po ewentualnym odbiciu bedzie rowna
                        SinglePhoton reflectedAssociated;
                        reflectedAssociated.position = tempClosestIntersection.LPOINT;
                        reflectedAssociated.direction = -oldAssociatedToNew.direction;
                        reflectedAssociated.energy = newAssociated.energy*tempMat->diffuse;
                        reflectedAssocPhotons.push_back(reflectedAssociated);
                        //qDebug()<<"poprzednia energia stowarzyszonego fotonu"<<newAssociated.energy.r<<newAssociated.energy.g<<newAssociated.energy.b;
                        //qDebug()<<"energia po odbiciu stowarzyszonego fotonu"<<reflectedAssociated.energy.r<<reflectedAssociated.energy.g<<reflectedAssociated.energy.b;
                        ileAssoc++;
                    }
                    else{
                        //qDebug()<<"foton stowarzyszony trafil w powierzchnie niedyfuzyjna";
                    }
                }
            }
            //qDebug()<<"wyemitowalem"<<ileAssoc<<"fotonow stowarzyszonych";

            /*
            if(newStream->associatedPhoton.count())
                newStream->leadingPhoton.energy = photonEnergy/newStream->associatedPhoton.count();
            for(int j=0;j<newStream->associatedPhoton.count();j++)
                newStream->associatedPhoton[j].energy = photonEnergy/newStream->associatedPhoton.count();
            */

            //zapisz strumien fotonow do mapy strumieni
            photons->append(newStream);

            //jesli nastapilo odbicie
            if(PropabilityOfAbsorption()>mat->absorptionPropability && reflections>0) {
                //heurystyka Jensena bazujaca na mechanizmie rosyjskiej ruletki
                //float avgDiff = (mat->diffuse.r+mat->diffuse.g+mat->diffuse.b)/3;
                //photonEnergy *= (mat->diffuse/avgDiff);

                //przeskaluj energie fotonu po odbiciu od materialu oraz fotonow stowarzyszonych
                photonEnergy *= mat->diffuse;
                newStream->associatedPhoton = reflectedAssocPhotons;

                // odbij tylko jesli foton po odbiciu ma energie wieksza od zera (aby nie sledzic i zapisywac fotonow z energia 0,0,0 w rezultacie odbicia np czerwonego od niebieskiego)
                if (photonEnergy.r+photonEnergy.g+photonEnergy.b != 0)
                {
                    Vector3 reflected = LambertReflectionDirection(closestIntersection);
                    Ray newRay(closestIntersection.LPOINT+reflected*BIAS, reflected);
                    TracePhoton(photonEnergy, newRay, geometry, photons, newStream, reflections-1);
                }
            }
        }
    }
    else{//jesli nie trafilismy w nic lub blad numeryczny szukania przeciecia
        //qDebug()<<"pudlo";
    }
}

float StreamPhotonMap::PropabilityOfAbsorption() {
    return randomUnsignedFloat();
}

Vector3 StreamPhotonMap::LambertReflectionDirection(const IntersectionResult &ir) {

    float x, y, z;
    do {
        x = randomSignedFloat();
        y = randomSignedFloat();
        z = randomSignedFloat();
    } while(x*x + y*y + z*z > 1.0f);

    Vector3 direction(x,y,z);

    if(direction.DotProduct(ir.intersectionLPOINTNormal)<0)
        direction*=-1;

    return direction;
}

QList<Stream*> StreamPhotonMap::GetClosestPhotons(const Vector3 &LPOINT, float radius, int maxPhotons) {
    return kdTree->FindClosest(LPOINT, radius, maxPhotons);
}
