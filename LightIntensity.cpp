#include "LightIntensity.h"

LightIntensity LightIntensity::operator+(const LightIntensity& rhs) const {
    return LightIntensity(r+rhs.r, g+rhs.g, b+rhs.b);
}

LightIntensity LightIntensity::operator-(const LightIntensity& rhs) const {
    return LightIntensity(r-rhs.r, g-rhs.g, b-rhs.b);
}

LightIntensity LightIntensity::operator*(float num) const {
    return LightIntensity(r*num, g*num, b*num);
}

ostream& operator<<(ostream &str, const LightIntensity &intensity)
{
    str << "17. LI[" << intensity.r << ", " << intensity.g << ", " << intensity.b << "]";
    if(intensity.r != intensity.r)
    {
        cerr << "\n20. r = NaN\n";
        throw 1u;
//        exit(0);
    }
    if(intensity.g != intensity.g)
    {
        cerr << "\n26. g = NaN\n";
        throw 1l;
//        exit(0);
    }
    if(intensity.b != intensity.b)
    {
        cerr << "\n32. b = NaN\n";
        throw 1ul;
//        exit(0);
    }
    return str;
}

QDebug operator<<(QDebug dbg, const LightIntensity &intensity) {
    dbg.nospace() << "40. LI[" << intensity.r << ", " << intensity.g << ", " << intensity.b << "]";
    if(intensity.r != intensity.r)
    {
        // cerr << intensity << "\n43. r = NaN\n";
        throw 1u;
//        exit(0);
    }
    if(intensity.g != intensity.g)
    {
        // cerr << intensity << "\n49. g = NaN\n";
        throw 1l;
//        exit(0);
    }
    if(intensity.b != intensity.b)
    {
        // cerr << intensity << "\n55. b = NaN\n";
        throw 1ul;
//        exit(0);
    }
    return dbg.space();
}

LightIntensity LightIntensity::operator*(const LightIntensity& rhs) const {
    return LightIntensity(r*rhs.r, g*rhs.g, b*rhs.b);
}

LightIntensity LightIntensity::operator/(float num) const {
    return LightIntensity(r/num, g/num, b/num);
}

LightIntensity& LightIntensity::operator+=(const LightIntensity& rhs) {
    r+=rhs.r;
    g+=rhs.g;
    b+=rhs.b;

    return (*this);
}

LightIntensity& LightIntensity::operator-=(const LightIntensity& rhs) {
    r-=rhs.r;
    g-=rhs.g;
    b-=rhs.b;

    return (*this);
}

LightIntensity& LightIntensity::operator*=(const LightIntensity& rhs) {
    r*=rhs.r;
    g*=rhs.g;
    b*=rhs.b;

    return (*this);
}

LightIntensity& LightIntensity::operator*=(float num) {
    r*=num;
    g*=num;
    b*=num;

    return (*this);
}

LightIntensity& LightIntensity::operator/=(float num) {
    r/=num;
    g/=num;
    b/=num;

    return (*this);
}

LightIntensity operator*(float num, const LightIntensity& li) {
    return LightIntensity(num*li.r, num*li.g, num*li.b);
}
bool LightIntensity::operator ==(const LightIntensity& rhs) const {
    if(r == rhs.r && g==rhs.g && b==rhs.b)
        return true;
    return false;
}
