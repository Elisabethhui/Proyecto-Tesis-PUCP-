    #include <cassert>
    #include "analysisbar.h"
    #include <iostream>
    #include "SimpleMesh/mesh.h"

    AnalysisBar::AnalysisBar():
        Mesh()
    {
        mNumSectors = 0;
        mNumPartitions = 0;
    }

    AnalysisBar::AnalysisBar(unsigned int pNumSectors, unsigned int pNumPartitions, float pRadius):
        Mesh()
    {
        mNumSectors = pNumSectors;
        mNumPartitions = pNumPartitions;
        mRadius = pRadius;
    }

    AnalysisBar::~AnalysisBar(){

    }

    void AnalysisBar::set_scale(float pScale){
        scale = pScale;
    }

    /*This method builds a cylinder based on the number of sectors and partitions we provide in the constructor
    The pose of the cylinder is canonical, so it must be transformed after initialization*/
    void AnalysisBar::proceduralInit(SimpleMesh::Mesh *mesh,float bbox[2][3]){

        set_number_vertices(3*(2*mNumSectors + 2));

        //Number of triangles is numSectors x numPartitions x 2
        set_number_triangles(3*(2*mNumSectors + mNumSectors + mNumSectors));

        float angle = -(2*3.141592)/mNumSectors;

        //Para el caso del eje X
        float offset = (bbox[1][2] - bbox[0][2]);

        SimpleMesh::Vertex  initPoint(bbox[0][0] - mRadius, bbox[1][1] + mRadius, bbox[0][2]);
        int currentVertex = 0;

        //Populate the first level
        SimpleMesh::Vertex auxPoint;
        for(int i = 0; i < (mNumSectors); i++){
            //auxPoint.setX(initPoint.x()*cos(angle*(i+1)) - initPoint.y()*sin(angle*(i+1)));
            //auxPoint.setY(initPoint.x()*sin(angle*(i+1)) + initPoint.y()*cos(angle*(i+1)));
            auxPoint.setX(initPoint.x() + (mRadius * cos(angle*(i+1))));
            auxPoint.setY(initPoint.y() + (mRadius * sin(angle*(i+1))));
            auxPoint.setZ(initPoint.z());

            add_vertex(currentVertex++, auxPoint);
            add_color(currentVertex-1, 1.0, 0.0, 0.0);
        }

        int longuitudPrimeraTapa = currentVertex;

        //Populate the next levels
        for(int i = 0; i < longuitudPrimeraTapa; i++){
            auxPoint.setX(vertices[i].x());
            auxPoint.setY(vertices[i].y());
            auxPoint.setZ(vertices[i].z() + offset);

            add_vertex(currentVertex++, auxPoint);
            add_color(currentVertex-1, 1.0, 0.0, 0.0);
        }

        //Contorno del cilindro antes

        //Luego los centros del cilindro el ambas tapas
        int longuitudDosTapas = currentVertex;

        int currentTriangle = 0;
        int j = (longuitudDosTapas / 2);
        int valor = (longuitudDosTapas / 2);
        //Now it is the turn for the triangle population
        for(int i = 0; i < valor; i++){
            if (i < (valor - 1)){
                add_triangle(currentTriangle++, i, j, j + 1);
                add_triangle(currentTriangle++, i, j + 1, i + 1);
                j = j + 1;
            }
            else{//para encerrar la parte lateral
                add_triangle(currentTriangle++, i, j, valor);
                add_triangle(currentTriangle++, i, valor , 0);
                i++;
                j = j + 1;
            }
        }

        add_vertex(currentVertex++, bbox[0][0] - mRadius, bbox[1][1] + mRadius, bbox[0][2]);
        add_color(currentVertex-1, 1.0, 0.0, 0.0);

        int indice_tapa_arriba = currentVertex - 1;

        //add_vertex(currentVertex++, 0.0,0.0,offset*(mNumPartitions+1));
        add_vertex(currentVertex++, bbox[0][0] - mRadius,bbox[1][1] + mRadius,bbox[0][2] + offset);
        //add_vertex(currentVertex++, 0.0, 0.0, 1.0);
        add_color(currentVertex-1, 1.0, 0.0, 0.0);

        int indice_tapa_abajo = currentVertex - 1;

        //Ahora las tapas
        //Primero la de arriba
        for(int i = 0 ; i < mNumSectors; i++){
            if(i < (mNumSectors - 1)){
                add_triangle(currentTriangle++, indice_tapa_arriba, i, i + 1);
            }else{//Es el ultimo
                add_triangle(currentTriangle++, indice_tapa_arriba, i, 0);
            }
        }
        //luego la tapa de abajo
        for(int i = 0 ; i < mNumSectors; i++){
            int indice = i + mNumSectors;
            if(i < (mNumSectors - 1)){
                add_triangle(currentTriangle++, indice_tapa_abajo, indice + 1, indice);
            }else{//Es el ultimo
                add_triangle(currentTriangle++, indice_tapa_abajo, mNumSectors ,indice);
            }
        }

        //-----------------------------------------------------------------------------------------------
        //Para el caso de el eje Y
        offset = (bbox[1][1] - bbox[0][1]);
        SimpleMesh::Vertex  initPointEjeY(bbox[0][0] - mRadius,bbox[0][1], bbox[0][2] - mRadius);
        //We start with point (1,0,0) and start the population transforming this point
        int valor_inicial = currentVertex;

        for(int i = 0; i < (mNumSectors); i++){
            auxPoint.setX(initPointEjeY.x() + mRadius*cos(angle*(i+1)));
            auxPoint.setY(initPointEjeY.y());
            auxPoint.setZ(initPointEjeY.z() + mRadius*sin(angle*(i+1)));

            add_vertex(currentVertex++, auxPoint);
            add_color(currentVertex-1, 1.0, 0.0, 0.0);
        }

        longuitudPrimeraTapa = currentVertex;

        //Populate the next levels
        for(int i = valor_inicial; i < longuitudPrimeraTapa; i++){
            auxPoint.setX(vertices[i].x());
            auxPoint.setY(vertices[i].y() + offset);
            auxPoint.setZ(vertices[i].z());

            add_vertex(currentVertex++, auxPoint);
            add_color(currentVertex-1, 1.0, 0.0, 0.0);
        }

        longuitudDosTapas = currentVertex;

        j = longuitudDosTapas - mNumSectors;
        valor = longuitudDosTapas - mNumSectors;

        //Now it is the turn for the triangle population
        for(int i = valor_inicial; i < valor; i++){
            if (i < (valor - 1)){
                add_triangle(currentTriangle++, i, j, j + 1);
                add_triangle(currentTriangle++, i, j + 1, i + 1);
                j = j + 1;
            }
            else{//para encerrar la parte lateral
                add_triangle(currentTriangle++, i, j, valor);
                add_triangle(currentTriangle++, i, valor , valor_inicial);
                i++;
                j = j + 1;
            }
        }

        add_vertex(currentVertex++, bbox[0][0], bbox[0][1], bbox[0][2] - mRadius);
        add_color(currentVertex-1, 1.0, 0.0, 0.0);

        indice_tapa_arriba = currentVertex - 1;

        add_vertex(currentVertex++, bbox[0][0],bbox[0][1]+offset,bbox[0][2] - mRadius);
        add_color(currentVertex-1, 1.0, 0.0, 0.0);

        indice_tapa_abajo = currentVertex - 1;

        //Ahora las tapas
        //Primero la de arriba
        for(int i = 0 ; i < mNumSectors; i++){
            int indice = i + valor_inicial;
            int inicio = valor_inicial;
            if(i < (mNumSectors - 1)){
                add_triangle(currentTriangle++, indice_tapa_arriba, indice, indice + 1);
            }else{//Es el ultimo
                add_triangle(currentTriangle++, indice_tapa_arriba, indice, inicio);
            }
        }

        //luego la tapa de abajo
        for(int i = 0 ; i < mNumSectors; i++){
            int indice = i + valor_inicial + mNumSectors;
            int inicio = valor_inicial + mNumSectors;
            if(i < (mNumSectors - 1)){
                add_triangle(currentTriangle++, indice_tapa_abajo, indice + 1, indice);
            }else{//Es el ultimo
                add_triangle(currentTriangle++, indice_tapa_abajo, inicio ,indice);
            }
        }

        //-----------------------------------------------------------------------------------------------
        //Para el caso de el eje Z
        offset = bbox[1][0] - bbox[0][0];
        SimpleMesh::Vertex  initPointEjeZ(bbox[0][0],bbox[0][1], bbox[0][2]);
        valor_inicial = currentVertex;

        for(int i = 0; i < (mNumSectors); i++){
            auxPoint.setX(initPointEjeZ.x());
            auxPoint.setY(initPointEjeZ.y() + mRadius*cos(angle*(i+1)));
            auxPoint.setZ(initPointEjeZ.z() + mRadius*sin(angle*(i+1)));

            add_vertex(currentVertex++, auxPoint);
            add_color(currentVertex-1, 1.0, 0.0, 0.0);
        }

        longuitudPrimeraTapa = currentVertex;

        //Populate the next levels
        for(int i = valor_inicial; i < longuitudPrimeraTapa; i++){
            auxPoint.setX(vertices[i].x() + offset);
            auxPoint.setY(vertices[i].y());
            auxPoint.setZ(vertices[i].z());

            add_vertex(currentVertex++, auxPoint);
            add_color(currentVertex-1, 1.0, 0.0, 0.0);
        }

        longuitudDosTapas = currentVertex;
        j = longuitudDosTapas - mNumSectors;
        valor = longuitudDosTapas - mNumSectors;

        for(int i = valor_inicial; i < valor; i++){
            if (i < (valor - 1)){
                add_triangle(currentTriangle++, i, j, j + 1);
                add_triangle(currentTriangle++, i, j + 1, i + 1);
                j = j + 1;
            }
            else{//para encerrar la parte lateral
                add_triangle(currentTriangle++, i, j, valor);
                add_triangle(currentTriangle++, i, valor , valor_inicial);
                i++;
                j = j + 1;
            }
        }

        add_vertex(currentVertex++, bbox[0][0],bbox[0][1], bbox[0][2]);
        add_color(currentVertex-1, 1.0, 0.0, 0.0);

        indice_tapa_arriba = currentVertex - 1;

        add_vertex(currentVertex++,bbox[0][0] + offset,bbox[0][1], bbox[0][2]);
        add_color(currentVertex-1, 1.0, 0.0, 0.0);

        indice_tapa_abajo = currentVertex - 1;

        //Ahora las tapas
        //Primero la de arriba
        for(int i = 0 ; i < mNumSectors; i++){
            int indice = i + valor_inicial;
            int inicio = valor_inicial;
            if(i < (mNumSectors - 1)){
                add_triangle(currentTriangle++, indice_tapa_arriba, indice, indice + 1);
            }else{//Es el ultimo
                add_triangle(currentTriangle++, indice_tapa_arriba, indice, inicio);
            }
        }
        //luego la tapa de abajo
        for(int i = 0 ; i < mNumSectors; i++){
            int indice = i + valor_inicial + mNumSectors;
            int inicio = valor_inicial + mNumSectors;
            if(i < (mNumSectors - 1)){
                add_triangle(currentTriangle++, indice_tapa_abajo, indice + 1, indice);
            }else{//Es el ultimo
                add_triangle(currentTriangle++, indice_tapa_abajo, inicio ,indice);
            }
        }

        compute_normals();
    }

    void AnalysisBar::proceduralInitY(SimpleMesh::Mesh *mesh, float bbox[2][3]){

        set_number_vertices(2*mNumSectors + 2);

        //Number of triangles is numSectors x numPartitions x 2
        set_number_triangles(2*mNumSectors + mNumSectors + mNumSectors);

        float angle = -(2*3.141592)/mNumSectors;
        float offset = bbox[1][1] - bbox[0][1];

        SimpleMesh::Vertex  initPoint(bbox[0][0] - mRadius,bbox[0][1], bbox[0][2]);
        //We start with point (1,0,0) and start the population transforming this point
        int currentVertex = 0;

        //add_vertex(currentVertex++, initPoint);
        //add_color(currentVertex-1, 1.0, 0.0, 0.0);

        //Populate the first level
        SimpleMesh::Vertex auxPoint;
        for(int i = 0; i < (mNumSectors); i++){
            auxPoint.setX(initPoint.x() + mRadius*cos(angle*(i+1)));
            auxPoint.setY(initPoint.y());
            auxPoint.setZ(initPoint.z() + mRadius*sin(angle*(i+1)));

            add_vertex(currentVertex++, auxPoint);
            add_color(currentVertex-1, 1.0, 0.0, 0.0);
        }

        int longuitudPrimeraTapa = currentVertex;

        //Populate the next levels
        for(int i = 0; i < longuitudPrimeraTapa; i++){
            auxPoint.setX(vertices[i].x());
            auxPoint.setY(vertices[i].y() + offset);
            auxPoint.setZ(vertices[i].z());

            add_vertex(currentVertex++, auxPoint);
            add_color(currentVertex-1, 1.0, 0.0, 0.0);
        }

        //Contorno del cilindro antes

        //Luego los centros del cilindro el ambas tapas
        int longuitudDosTapas = currentVertex;

        //Check number of vertices
        assert(currentVertex == num_vertices);

        int currentTriangle = 0;
        int j = (longuitudDosTapas / 2);
        int valor = (longuitudDosTapas / 2);
        //Now it is the turn for the triangle population
        for(int i = 0; i < valor; i++){

            if (i < (valor - 1)){

                add_triangle(currentTriangle++, i, j, j + 1);
                add_triangle(currentTriangle++, i, j + 1, i + 1);

                //i++;
                j = j + 1;
            }
            else{//para encerrar la parte lateral
                add_triangle(currentTriangle++, i, j, valor);
                add_triangle(currentTriangle++, i, valor , 0);
                i++;
                j = j + 1;
            }
        }

        add_vertex(currentVertex++, bbox[0][0], bbox[0][1], bbox[0][2]);
        add_color(currentVertex-1, 1.0, 0.0, 0.0);

        int indice_tapa_arriba = currentVertex - 1;

        add_vertex(currentVertex++, bbox[0][0],bbox[0][1]+offset,bbox[0][2]);
        //add_vertex(currentVertex++, 0.0, 0.0, 1.0);
        add_color(currentVertex-1, 1.0, 0.0, 0.0);

        int indice_tapa_abajo = currentVertex - 1;

        //Ahora las tapas
        //Primero la de arriba
        for(int i = 0 ; i < mNumSectors; i++){
            if(i < (mNumSectors - 1)){
                add_triangle(currentTriangle++, indice_tapa_arriba, i, i + 1);
            }else{//Es el ultimo
                add_triangle(currentTriangle++, indice_tapa_arriba, i, 0);
            }
        }
        //luego la tapa de abajo
        for(int i = 0 ; i < mNumSectors; i++){
            int indice = i + mNumSectors;
            if(i < (mNumSectors - 1)){
                add_triangle(currentTriangle++, indice_tapa_abajo, indice + 1, indice);
            }else{//Es el ultimo
                add_triangle(currentTriangle++, indice_tapa_abajo, mNumSectors ,indice);
            }
        }
        compute_normals();
    }

    void AnalysisBar::proceduralInitZ(SimpleMesh::Mesh *mesh, float bbox[2][3]){

        set_number_vertices(2*mNumSectors + 2);

        //Number of triangles is numSectors x numPartitions x 2
        set_number_triangles(2*mNumSectors + mNumSectors + mNumSectors);

        float angle = -(2*3.141592)/mNumSectors;
        float offset = /*1.0/*/mNumPartitions;
        //float offset = (bbox[1][2] - bbox[0][2])*scale;

        SimpleMesh::Vertex  initPoint(mRadius,0.0, 0.0);
        //We start with point (1,0,0) and start the population transforming this point
        int currentVertex = 0;

        //add_vertex(currentVertex++, initPoint);
        //add_color(currentVertex-1, 1.0, 0.0, 0.0);

        //Populate the first level
        SimpleMesh::Vertex auxPoint;
        for(int i = 0; i < (mNumSectors); i++){
            auxPoint.setX(initPoint.z());
            auxPoint.setY(initPoint.x()*cos(angle*(i+1)) - initPoint.y()*sin(angle*(i+1)));
            auxPoint.setZ(initPoint.x()*sin(angle*(i+1)) + initPoint.y()*cos(angle*(i+1)));

            add_vertex(currentVertex++, auxPoint);
            add_color(currentVertex-1, 1.0, 0.0, 0.0);
        }

        int longuitudPrimeraTapa = currentVertex;

        //Populate the next levels
        for(int i = 0; i < longuitudPrimeraTapa; i++){
            auxPoint.setX(vertices[i].x() + offset*(mNumPartitions+1));
            auxPoint.setY(vertices[i].y());
            auxPoint.setZ(vertices[i].z());

            add_vertex(currentVertex++, auxPoint);
            add_color(currentVertex-1, 1.0, 0.0, 0.0);
        }

        //Contorno del cilindro antes

        //Luego los centros del cilindro el ambas tapas
        int longuitudDosTapas = currentVertex;

        //Check number of vertices
        assert(currentVertex == num_vertices);

        int currentTriangle = 0;
        int j = (longuitudDosTapas / 2);
        int valor = (longuitudDosTapas / 2);
        for(int i = 0; i < valor; i++){
            if (i < (valor - 1)){
                add_triangle(currentTriangle++, i, j, j + 1);
                add_triangle(currentTriangle++, i, j + 1, i + 1);
                //i++;
                j = j + 1;
            }
            else{//para encerrar la parte lateral
                add_triangle(currentTriangle++, i, j, valor);
                add_triangle(currentTriangle++, i, valor , 0);
                i++;
                j = j + 1;
            }
        }

        add_vertex(currentVertex++, 0.0, 0.0, 0.0);
        add_color(currentVertex-1, 1.0, 0.0, 0.0);

        int indice_tapa_arriba = currentVertex - 1;


        add_vertex(currentVertex++,offset*(mNumPartitions + 1),0.0,0.0);
        //add_vertex(currentVertex++, 0.0, 0.0, 1.0);
        add_color(currentVertex-1, 1.0, 0.0, 0.0);

        int indice_tapa_abajo = currentVertex - 1;

        //Ahora las tapas
        //Primero la de arriba
        for(int i = 0 ; i < mNumSectors; i++){
            if(i < (mNumSectors - 1)){
                add_triangle(currentTriangle++, indice_tapa_arriba, i, i + 1);
            }else{//Es el ultimo
                add_triangle(currentTriangle++, indice_tapa_arriba, i, 0);
            }
        }
        //luego la tapa de abajo
        for(int i = 0 ; i < mNumSectors; i++){
            int indice = i + mNumSectors;
            if(i < (mNumSectors - 1)){
                add_triangle(currentTriangle++, indice_tapa_abajo, indice + 1, indice);
            }else{//Es el ultimo
                add_triangle(currentTriangle++, indice_tapa_abajo, mNumSectors ,indice);
            }
        }
        compute_normals();
    }

