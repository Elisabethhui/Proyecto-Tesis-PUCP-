#include "objectcontainer.h"
#include "SimpleMesh/vertex.h"
#include <iostream>

ObjectContainer::ObjectContainer()
{
    bbox[0][0] = bbox[0][1] = bbox[0][2] = 1.0E30F;
    bbox[1][0] = bbox[1][1] = bbox[1][2] = -1.0E30F;

    selectedObject = -1;
    scale = 1.0;
    center[0] = center[1] = center[2] = 0.0;

}

ObjectContainer::~ObjectContainer(){
    clearData();
}

void ObjectContainer::clearData(){
    for(int i = 0; i < m_objects.size(); i++){
        delete m_objects[i];
        delete m_bufferedObjects[i];
    }
    m_objects.clear();
    m_bufferedObjects.clear();
    m_visibility.clear();
    m_names.clear();

    bbox[0][0] = bbox[0][1] = bbox[0][2] = 1.0E30F;
    bbox[1][0] = bbox[1][1] = bbox[1][2] = -1.0E30F;

    selectedObject = -1;
}

void ObjectContainer::removeObject(int index){
    if(index < m_objects.size()){
        SimpleMesh::Mesh* auxMesh = m_objects[index];
        SymArchData::BufferedObject3D* auxBO = m_bufferedObjects[index];
        delete auxMesh;
        delete auxBO;

        vector<SimpleMesh::Mesh*>::iterator itObject = m_objects.begin();
        vector<SymArchData::BufferedObject3D*>::iterator itBO = m_bufferedObjects.begin();
        vector<bool>::iterator itVis = m_visibility.begin();
        vector<QString>::iterator itNam = m_names.begin();

        m_objects.erase(itObject + index);
        m_bufferedObjects.erase(itBO + index);
        m_visibility.erase(itVis + index);
        m_names.erase(itNam + index);

        selectedObject = -1; //After removing, none object is selected
    }
}

void ObjectContainer::recomputeScaleAndCenter(){
    bbox[0][0] = bbox[0][1] = bbox[0][2] = 1.0E30F;
    bbox[1][0] = bbox[1][1] = bbox[1][2] = -1.0E30F;

    for(int j = 0; j < m_objects.size(); j++){
        //Update the bounding box
        SimpleMesh::Vertex* verts = m_objects[j]->get_vertices();
        for (int i = 0; i < m_objects[j]->get_number_vertices(); i++) {
            if (verts[i].x() < bbox[0][0]) bbox[0][0] = verts[i].x();
            else if (verts[i].x() > bbox[1][0]) bbox[1][0] = verts[i].x();
            if (verts[i].y() < bbox[0][1]) bbox[0][1] = verts[i].y();
            else if (verts[i].y() > bbox[1][1]) bbox[1][1] = verts[i].y();
            if (verts[i].z() < bbox[0][2]) bbox[0][2] = verts[i].z();
            else if (verts[i].z() > bbox[1][2]) bbox[1][2] = verts[i].z();
        }
    }

    bbox_modificado[0][0] = bbox_modificado[0][1] = bbox_modificado[0][2] = 1.0E30F;
    bbox_modificado[1][0] = bbox_modificado[1][1] = bbox_modificado[1][2] = -1.0E30F;

    // Setup initial viewing scale
    float dx = bbox[1][0] - bbox[0][0];
    float dy = bbox[1][1] - bbox[0][1];
    float dz = bbox[1][2] - bbox[0][2];
    scale = 2.0 / sqrt(dx*dx + dy*dy + dz*dz);

    center[0] = 0.5 * (bbox[1][0] + bbox[0][0]);
    center[1] = 0.5 * (bbox[1][1] + bbox[0][1]);
    center[2] = 0.5 * (bbox[1][2] + bbox[0][2]);

}

void ObjectContainer::replaceObject(SimpleMesh::Mesh *new_mesh, SymArchData::BufferedObject3D *new_buffered_object, QString name, int indice){
    cout << "La longuitud del m_objects antes de la reconstrucción es " << m_objects.size() <<endl;
    //m_objects.insert(m_objects.begin(),new_mesh);
    m_objects.at(0) = new_mesh;
    //m_bufferedObjects.insert(m_bufferedObjects.begin(),new_buffered_object);
    m_bufferedObjects.at(0) = new_buffered_object;
    //m_visibility.insert(m_visibility.begin(), true); //Always visible when first inserted
    m_visibility.at(0) = true;
    //m_names.insert(m_names.begin(), name);
    m_names.at(0) = name;

    cout << "La longuitud del m_objects despues de la reconstrucción es " << m_objects.size() <<endl;
    selectedObject = indice;

    //Update the bounding box
    if(selectedObject<1){
        SimpleMesh::Vertex* verts = new_mesh->get_vertices();
        for (int i = 0; i < new_mesh->get_number_vertices(); i++) {
            if (verts[i].x() < bbox[0][0]) bbox[0][0] = verts[i].x();
            else if (verts[i].x() > bbox[1][0]) bbox[1][0] = verts[i].x();
            if (verts[i].y() < bbox[0][1]) bbox[0][1] = verts[i].y();
            else if (verts[i].y() > bbox[1][1]) bbox[1][1] = verts[i].y();
            if (verts[i].z() < bbox[0][2]) bbox[0][2] = verts[i].z();
            else if (verts[i].z() > bbox[1][2]) bbox[1][2] = verts[i].z();
        }

        //Inicialización de bbox modificado
        for(int i = 0 ; i < 2; i++){
            for(int j = 0; j < 3; j++){
                bbox_modificado[i][j] = bbox[i][j];
            }
        }


        // Setup initial viewing scale
        float dx = bbox[1][0] - bbox[0][0];
        float dy = bbox[1][1] - bbox[0][1];
        float dz = bbox[1][2] - bbox[0][2];
        scale = 2.0 / sqrt(dx*dx + dy*dy + dz*dz);

        center[0] = 0.5 * (bbox[1][0] + bbox[0][0]);
        center[1] = 0.5 * (bbox[1][1] + bbox[0][1]);
        center[2] = 0.5 * (bbox[1][2] + bbox[0][2]);
    }
}

void ObjectContainer::addObject(SimpleMesh::Mesh *new_mesh, SymArchData::BufferedObject3D *new_buffered_object, QString name)
{
    m_objects.push_back(new_mesh);
    m_bufferedObjects.push_back(new_buffered_object);
    m_visibility.push_back(true); //Always visible when first inserted
    m_names.push_back(name);

    selectedObject = m_objects.size()-1;

    //Update the bounding box
    if(selectedObject<1){
        SimpleMesh::Vertex* verts = new_mesh->get_vertices();
        for (int i = 0; i < new_mesh->get_number_vertices(); i++) {
            if (verts[i].x() < bbox[0][0]) bbox[0][0] = verts[i].x();
            else if (verts[i].x() > bbox[1][0]) bbox[1][0] = verts[i].x();
            if (verts[i].y() < bbox[0][1]) bbox[0][1] = verts[i].y();
            else if (verts[i].y() > bbox[1][1]) bbox[1][1] = verts[i].y();
            if (verts[i].z() < bbox[0][2]) bbox[0][2] = verts[i].z();
            else if (verts[i].z() > bbox[1][2]) bbox[1][2] = verts[i].z();
        }

        //Inicialización de bbox modificado
        for(int i = 0 ; i < 2; i++){
            for(int j = 0; j < 3; j++){
                bbox_modificado[i][j] = bbox[i][j];
            }
        }


        // Setup initial viewing scale
        float dx = bbox[1][0] - bbox[0][0];
        float dy = bbox[1][1] - bbox[0][1];
        float dz = bbox[1][2] - bbox[0][2];
        scale = 2.0 / sqrt(dx*dx + dy*dy + dz*dz);

        center[0] = 0.5 * (bbox[1][0] + bbox[0][0]);
        center[1] = 0.5 * (bbox[1][1] + bbox[0][1]);
        center[2] = 0.5 * (bbox[1][2] + bbox[0][2]);
    }
}

void ObjectContainer::drawObjects(QOpenGLShaderProgram* program,
                                  int indiceCara,
                                  int posicionCara,
                                  bool isMoverCara,
                                  bool isResizeCara,
                                  unsigned char* mapcolor_X,
                                  unsigned char* mapcolor_Y,
                                  unsigned char* mapcolor_Z){
    cout << "ObjectContainer::drawObjects" << endl;
    cout << "La loguitud es "<< m_bufferedObjects.size() << endl;
    for(int i = 0; i < (m_bufferedObjects.size()); i++){
        if(m_visibility[i]){
            if(i == 0 || i == 2){ //primer elemento, El objeto a analizar
            //if(i == 0){
                m_bufferedObjects[i]->drawBufferedObject(program,
                                                         bbox_modificado,
                                                         i, indiceCara, posicionCara, isMoverCara, isResizeCara,
                                                         mapcolor_X,
                                                         mapcolor_Y,
                                                         mapcolor_Z);
            }
        }
    }
}

void ObjectContainer::drawObjectsAndColorBar(QOpenGLShaderProgram* programObject, QOpenGLShaderProgram* programColorBar){
    cout << "ObjectContainer::drawObjectsAndColorBar" << endl;
    for(int i = 0; i < (m_bufferedObjects.size()); i++){
        if(m_visibility[i]){
            //m_bufferedObjects[i]->drawBufferedObject(programObject,programColorBar ,bbox,i);
        }
    }
}

void ObjectContainer::drawColorBar(QOpenGLShaderProgram* program,
                                   int indiceCara,
                                   int posicionCara,
                                   bool isMoverCara,
                                   bool isResizeCara,
                                   unsigned char* mapcolor_X,
                                   unsigned char* mapcolor_Y,
                                   unsigned char* mapcolor_Z){
    cout << "ObjectContainer::drawObjects" << endl;
    for(int i = 0; i < (m_bufferedObjects.size()); i++){
        if(m_visibility[i]){
            if(i != 0){ //El segundo elemento son los color bar
                m_bufferedObjects[i]->drawBufferedObject(program,
                                                         bbox_modificado,
                                                         i, indiceCara, posicionCara, isMoverCara, isResizeCara,
                                                         mapcolor_X,
                                                         mapcolor_Y,
                                                         mapcolor_Z);
            }
        }
    }
}

void ObjectContainer::drawBoundingBox(QOpenGLShaderProgram *program){
    QOpenGLFunctions *f = NULL;
    f = QOpenGLContext::currentContext()->functions();

    GLfloat vertices[]={
        bbox[0][0], bbox[0][1], bbox[0][2], 1.0,
        bbox[1][0], bbox[0][1], bbox[0][2], 1.0,
        bbox[1][0], bbox[1][1], bbox[0][2], 1.0,
        bbox[0][0], bbox[1][1], bbox[0][2], 1.0,
        bbox[0][0], bbox[0][1], bbox[1][2], 1.0,
        bbox[1][0], bbox[0][1], bbox[1][2], 1.0,
        bbox[1][0], bbox[1][1], bbox[1][2], 1.0,
        bbox[0][0], bbox[1][1], bbox[1][2], 1.0,
     };

    GLuint vbo_vertices;
    f->glGenBuffers(1, &vbo_vertices);
    f->glBindBuffer(GL_ARRAY_BUFFER, vbo_vertices);
    f->glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    f->glBindBuffer(GL_ARRAY_BUFFER, 0);

    GLushort elements[] = {
        0, 1, 2, 3,
        4, 5, 6, 7,
        0, 4, 1, 5, 2, 6, 3, 7
    };
    GLuint ibo_elements;
    f->glGenBuffers(1, &ibo_elements);
    f->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_elements);
    f->glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);
    f->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    f->glBindBuffer(GL_ARRAY_BUFFER, vbo_vertices);
    int vertexLocation = program->attributeLocation("v_coord");
    program->enableAttributeArray(vertexLocation);
    program->setAttributeBuffer(vertexLocation, GL_FLOAT, 0, 4, sizeof(float) * 4);

    f->glLineWidth(4.0);

    f->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_elements);
    f->glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_SHORT, 0);
    f->glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_SHORT, (GLvoid*)(4*sizeof(GLushort)));
    f->glDrawElements(GL_LINES, 8, GL_UNSIGNED_SHORT, (GLvoid*)(8*sizeof(GLushort)));
    f->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    program->disableAttributeArray(vertexLocation);
    f->glBindBuffer(GL_ARRAY_BUFFER, 0);

    f->glDeleteBuffers(1, &vbo_vertices);
    f->glDeleteBuffers(1, &ibo_elements);
}
