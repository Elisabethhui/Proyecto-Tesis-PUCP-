#ifndef ANALYSISBAR_H
#define ANALYSISBAR_H

#include "SimpleMesh/mesh.h"
// #include "mathlab/bin/generatedCode/first/faceDemoMissingData.h";

class AnalysisBar : public SimpleMesh::Mesh
{
protected:
    unsigned int mNumSectors;
    unsigned int mNumPartitions;
    float mRadius;
    float scale;
public:
    AnalysisBar();
    AnalysisBar(unsigned int pNumSectors, unsigned int pNumPartitions, float pRadius);
    ~AnalysisBar();

    void setNumberSectors(unsigned int pNumSectors){mNumSectors = pNumSectors;}
    void setNumPartitions(unsigned int pNumPartitions){mNumPartitions = pNumPartitions;}
    void setRadius(float pRadius){mRadius = pRadius;}

    void proceduralInit(SimpleMesh::Mesh *mesh, float bbox[][3]);
    void proceduralInitY(SimpleMesh::Mesh *mesh, float bbox[][3]);
    void proceduralInitZ(SimpleMesh::Mesh *mesh, float bbox[][3]);
    void set_scale(float pScale);


};

#endif // ANALYSISBAR_H
