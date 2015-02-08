#include "POINTLight.h"

#include "Math/randomUtils.h"
#include "Scene.h"

#include <cfloat>

POINTLight::POINTLight() : AmbientLight()
{
    position.Zero();
    attenuation.One();
    type = LPOINT;
    projectionMap=0;
}

POINTLight::POINTLight(Vector3 position, Color color, Vector3 attenuation) : AmbientLight() {
    this->position = position;
    this->color = color;
    this->attenuation = attenuation;
    type = LPOINT;
    projectionMap=0;
}

POINTLight::~POINTLight() {
    if(projectionMap) {
        delete projectionMap;
        projectionMap=0;
    }
}

bool POINTLight::IsInShadow(const IntersectionResult* const &ir, const QList<Geometry *> &geometry) const {
    //check if ray from light to intersection LPOINT intersects some geometry
    Ray lightToLPOINT(position, ir->LPOINT-position);
    float dist = (position-ir->LPOINT).GetLength() - 0.001;

    for(int i=0;i<geometry.count();i++) {
        IntersectionResult intersect = geometry.at(i)->Intersects(lightToLPOINT);
        if(intersect.type==HIT && intersect.distance<dist && intersect.object!=ir->object)
            return true;
    }

    return false;
}

Vector3 POINTLight::GetPosition() const {
    return position;
}

LightIntensity POINTLight::GetLightIntensity(const Vector3 &cameraPosition,
                                             const IntersectionResult* const &ir,
                                             const QList<Geometry *> &geometry) const {
//    qDebug() << "PL";
    LightIntensity result(0,0,0);

    if(IsInShadow(ir, geometry)){ //commenting this disable sharp shadows (hence know geometry blocking is being checked)
        LightIntensity shadow(0.0,0.0,0.0);
        return shadow;
    }

    Vector3 normal = ir->intersectionLPOINTNormal;
    Vector3 light(position - ir->LPOINT);

    float lightDistance = light.GetLength();
    normal.Normalize();
    light.Normalize();

    Material* mat = ir->object->GetMaterial();
    //if geometry has diffuse material calculate phong lighting
    if(mat->type==DIFFUSE) {
        DiffuseMaterial* diffMat = (DiffuseMaterial*)mat;
        float diffuseFactor = normal.DotProduct(light);

        if(diffuseFactor > 0.0f) {
            float specPower = diffMat->specularCoeff;
            Vector3 eye = cameraPosition - ir->LPOINT;
            eye.Normalize();
            Vector3 reflected = (-light).Reflect(normal);
            float specFactor = pow(max(reflected.DotProduct(eye), 0.0f), specPower);

            result += diffMat->diffuse*color*diffuseFactor;
            result += diffMat->specular*color*specFactor;
        }
        result /= (attenuation.x+attenuation.y*lightDistance+attenuation.z*lightDistance*lightDistance);
    }

    return result;
}

LightIntensity POINTLight::GetLightIntensity(const Vector3 &cameraPosition,
                                         const IntersectionResult* const &ir,
                                         const QList<Geometry *> &geometry,
                                         const Vector3 &position,
                                         const LightIntensity &color) const
{
    //    qDebug() << "PL";
        LightIntensity result(0,0,0);

        if(IsInShadow(ir, geometry)){ //commenting this disable sharp shadows (hence know geometry blocking is being checked)
            LightIntensity shadow(0.0,0.0,0.0);
            return shadow;
        }

        Vector3 normal = ir->intersectionLPOINTNormal;
        Vector3 light(position - ir->LPOINT);

        normal.Normalize();
        light.Normalize();

        Material* mat = ir->object->GetMaterial();
        //if geometry has diffuse material calculate phong lighting
        if(mat->type==DIFFUSE) {
            DiffuseMaterial* diffMat = (DiffuseMaterial*)mat;
            float diffuseFactor = normal.DotProduct(light);

            if(diffuseFactor > 0.0f) {
                float specPower = diffMat->specularCoeff;
                Vector3 eye = cameraPosition - ir->LPOINT;
                eye.Normalize();
                Vector3 reflected = (-light).Reflect(normal);
                float specFactor = pow(max(reflected.DotProduct(eye), 0.0f), specPower);

                result += diffMat->diffuse*color*diffuseFactor;
                result += diffMat->specular*color*specFactor;
            }
            //result /= (attenuation.x+attenuation.y*lightDistance+attenuation.z*lightDistance*lightDistance);
        }

        return result;
}

//generate photon from unit sphere
Ray POINTLight::GetPhoton(bool useProjectionMap) const {
    float x,y,z;
    bool ok=false;
    do {
        x = randomSignedFloat();
        y = randomSignedFloat();
        z = randomSignedFloat();

        if(x*x + y*y + z*z <= 1.0f) {
            if(useProjectionMap && projectionMap) {
                Vector3 dir(x, y, z);
                dir.Normalize();

                /*if(hemisphere) {
                    if(dir.DotProduct(Vector3(0,-1,0)))
                        dir*=-1;
                }
*/
                //if we use projection map
                //generated photon must be directed into reflective/refractive geometry
                if(projectionMap->SampleSpherical(dir) == Color(1, 1, 1))
                    ok = true;
            }
            else {
                ok = true;
            }
        }
    }
    while(!ok);
    
    return Ray(position, Vector3(x, y, z));
}

void POINTLight::CreateProjectionMap(const Scene* scene) {
    if(projectionMap)
        delete projectionMap;

    const QList<Geometry*> &geometry = scene->geometry;
    const int geometryCount = geometry.count();
    projectionMap = new Texture(512,512);

    int height = projectionMap->GetHeight();
    int width = projectionMap->GetWidth();
    float one_height = 1.0f / height;
    float one_width = 1.0f / width;

    //for each pixel in projection map
    for(int y=0;y<height;y++) {
        for(int x=0;x<width;x++) {
            float u = one_height * x;
            float v = one_width * y;

            u = 1.0f - u;
            v = 1.0f - v;

            //calculate spherical coordinates
            float phi = u * 2.0f * M_PI;
            float theta = (1.0f - v) * M_PI;

            float sin_phi = sin(phi);
            float cos_phi = cos(phi);
            float sin_theta = sin(theta);
            float cos_theta = cos(theta);

            Vector3 direction(sin_theta * sin_phi, cos_theta, sin_theta * cos_phi);

            //chec if ray for given pixel intersects reflective/refractive geometry
            Ray ray(position, direction);

            int closest=-1;
            float closestDist=FLT_MAX;
            IntersectionResult closestIntersection;
            for(int j=0;j<geometryCount;j++) {
                IntersectionResult result = geometry.at(j)->Intersects(ray);
                if(result.type!=MISS) {
                    if(closestDist>result.distance) {
                        closestDist = result.distance;
                        closest = j;
                        closestIntersection = result;
                    }
                }
            }

            if(closest != -1)
            {
                const MaterialType &material = closestIntersection.object->GetMaterial()->type;
                if(material == REFRACTIVE || material == REFLECTIVE)
                {
                    projectionMap->SetPixel(x,y,Color(1,1,1));
                }
                else
                {
                    projectionMap->SetPixel(x,y,Color(0,0,0));
                }
            }
        }
    }

//    projectionMap->SaveToFile("projectionMap.png");
}

float POINTLight::GetProjectionMapRatio() const {
    if(projectionMap)
        return projectionMap->GetWhiteToBlackPixelRatio();
    return 0;
}
