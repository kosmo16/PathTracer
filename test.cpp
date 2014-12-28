
#include "BidirectionalPathTracer/normalPdf.h"
#include "BidirectionalPathTracer/BlinnPhongBrdf.h"

#include <fstream>

int main(int argc, char *argv[])
{
    Pdf * pdf = new NormalPdf();
    Brdf * brdf = new BlinnPhongBrdf();

    Vector3 V(1.0f, -1.0f, 1.0f);
    Vector3 N(0.0f, 1.0f, 0.0f);

    fstream outputFile;

    outputFile.open("brdf.dat", ios_base::out);

    if(!outputFile)
    {
        return 1;
    }

    for(int i = 0; i < 1000; ++i)
    {
        Vector3 W = pdf->computeDirection(V, N);
        float ratio = brdf->computeRatio(V, W, N);
        cout << "ratio: " << ratio << "\t";
        W *= ratio;
        outputFile << W.x << " " << W.y << " " << W.z << "\n";
        cout << W.x << " " << W.y << " " << W.z << "\n";
    }

    outputFile.close();

    delete pdf;

    return 0;
}

