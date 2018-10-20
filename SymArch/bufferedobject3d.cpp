#include "bufferedobject3d.h"
#include "glwidget.h"
#include <QOpenGLShaderProgram>
#include <iostream>
#include <QCoreApplication>

using namespace std;

namespace SymArchData{

BufferedObject3D::BufferedObject3D()
    :m_vertexBuffer(QOpenGLBuffer::VertexBuffer),
      m_normalBuffer(QOpenGLBuffer::VertexBuffer),
      m_colorBuffer(QOpenGLBuffer::VertexBuffer),
      m_indexBuffer(QOpenGLBuffer::IndexBuffer)
{
    m_vertices = NULL;
    m_normals = NULL;
    m_indices = NULL;
    m_colors = NULL;

}

BufferedObject3D::~BufferedObject3D(){
    if(m_vertices){delete[] m_vertices; m_vertices = NULL;}
    if(m_normals){delete[] m_normals; m_normals = NULL;}
    if(m_indices){delete[] m_indices; m_indices = NULL;}
    if(m_colors){delete[] m_colors; m_colors = NULL;}

    if(m_vertexBuffer.isCreated())
        m_vertexBuffer.destroy();
    if(m_normalBuffer.isCreated())
        m_normalBuffer.destroy();
    if(m_colorBuffer.isCreated())
        m_colorBuffer.destroy();
    if(m_indexBuffer.isCreated())
        m_indexBuffer.destroy();
}

int BufferedObject3D::getVertices(){
    if(num_vertices == NULL){
        return 0;
    }else{
        return num_vertices;
    }
}

int BufferedObject3D::getTriangles(){
    if(num_triangles == NULL){
        return 0;
    }else{
        return num_triangles;
    }
}

void BufferedObject3D::createBuffersFromObject(SimpleMesh::Mesh *mesh){

    glArea->makeCurrent();
    initializeOpenGLFunctions();
    num_vertices = mesh->get_number_vertices();
    SimpleMesh::Vertex* vertices = mesh->get_vertices();

    //Creating the vertex buffer
    m_vertices = new float[num_vertices * 3];
    for(int i = 0; i < num_vertices; i++){
        m_vertices[3 * i]       = vertices[i].x();
        m_vertices[3 * i + 1]   = vertices[i].y();
        m_vertices[3 * i + 2]   = vertices[i].z();
    }

    //Creating the normal buffer
    m_normals = new float[num_vertices * 3];
    for(int i = 0; i < num_vertices; i++){
        m_normals[3 * i]        = vertices[i].nx();
        m_normals[3 * i + 1]    = vertices[i].ny();
        m_normals[3 * i + 2]    = vertices[i].nz();
    }

    //Creating the index buffer
    num_triangles = mesh->get_number_triangles();
    SimpleMesh::Triangle* triangles = mesh->get_triangles();

    m_indices = new unsigned int[num_triangles * 3];
    for(int i = 0; i < num_triangles; i++){
        vector<unsigned int> vert = triangles[i].get_vertices();
        m_indices[3 * i]        = vert[0];
        m_indices[3 * i + 1]    = vert[1];
        m_indices[3 * i + 2]    = vert[2];
    }

    m_vao.create();
    QOpenGLVertexArrayObject::Binder vao_binder(&m_vao);

        m_vertexBuffer.create();
        m_vertexBuffer.bind();
        m_vertexBuffer.allocate(m_vertices, num_vertices * 3 * sizeof(GLfloat));

        m_normalBuffer.create();
        m_normalBuffer.bind();
        m_normalBuffer.allocate(m_normals, num_vertices * 3 * sizeof(GLfloat));

        m_indexBuffer.create();
        m_indexBuffer.bind();
        m_indexBuffer.allocate(m_indices, num_triangles * 3 * sizeof(GLuint));
        glArea->doneCurrent();
}

void BufferedObject3D::drawBufferedBoundingObject(QOpenGLShaderProgram* program,
                                                  float bbox[][3],int vertexLocation){
    //QOpenGLFunctions *f = NULL;
    //f = QOpenGLContext::currentContext()->functions();
    QOpenGLFunctions_4_1_Core *f = NULL;
    f = QOpenGLContext::currentContext()->versionFunctions<QOpenGLFunctions_4_1_Core>();

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
    //int vertexLocation = program->attributeLocation("v_coord");
    program->enableAttributeArray(vertexLocation);
    program->setAttributeBuffer(vertexLocation, GL_FLOAT, 0, 4, sizeof(float) * 4);

    f->glLineWidth(5.0);

    f->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_elements);
    f->glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_SHORT, 0);
    f->glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_SHORT, (GLvoid*)(4*sizeof(GLushort)));
    f->glDrawElements(GL_LINES, 8, GL_UNSIGNED_SHORT, (GLvoid*)(8*sizeof(GLushort)));
    f->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    program->disableAttributeArray(vertexLocation);
    f->glBindBuffer(GL_ARRAY_BUFFER, 0);

    //Dibujar cuadrado
    /*GLfloat* triangle_vertices = new GLfloat[9];
    int j = 0;
    for(int i = 0; i < 12;i++){
        if(!(i == 3 || i == 7 || i == 11)){
            triangle_vertices[j] = vertices[i];
            j++;
        }
    }

    GLuint vbo_triangle_vertices;
    GLuint vbo_triangle_texcoords;
    GLuint ibo_triangle_elements;
    GLint attribute_coord3d, attribute_texcoord;
    GLuint texture_id;
    GLint uniform_mvp, uniform_mytexture;

    f->glBindBuffer(GL_ARRAY_BUFFER, vbo_triangle_vertices);
    f->glBufferData(GL_ARRAY_BUFFER, 9*sizeof(GLfloat), triangle_vertices, GL_STATIC_DRAW);
    f->glBindBuffer(GL_ARRAY_BUFFER, 0);

    GLfloat* triangle_texcoords = new GLfloat[6];

    for(int i = 0; i < 2; i++){
        triangle_texcoords[3*i] = 0.0;
        triangle_texcoords[3*i + 1] = 1.0;
        triangle_texcoords[3*i + 2] = 1.0;
    }

    f->glGenBuffers(1, &vbo_triangle_texcoords);
    f->glBindBuffer(GL_ARRAY_BUFFER, vbo_triangle_texcoords);
    f->glBufferData(GL_ARRAY_BUFFER, 6*sizeof(GLfloat), triangle_texcoords, GL_STATIC_DRAW);

    GLushort* triangle_elements = new GLushort[6];

    for(int i = 0; i < 2*3; i++){
        triangle_elements[i] = i;
    }

    f->glGenBuffers(1, &ibo_triangle_elements);
    f->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_triangle_elements);
    f->glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6*sizeof(GLushort), triangle_elements, GL_STATIC_DRAW);

    f->glGenTextures(1, &texture_id);
    assert(glGetError() == GL_NO_ERROR);

    f->glBindTexture(GL_TEXTURE_1D, texture_id);
    assert(glGetError() == GL_NO_ERROR);

    f->glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    f->glTexImage1D(GL_TEXTURE_1D,
                    0,
                    GL_RGB,
                    4,
                    0,
                    GL_RGB,
                    GL_UNSIGNED_BYTE,
                    tapas);

    // texture sampling/filtering operation.
    f->glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    f->glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    f->glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    f->glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    //Definiendo las estructuras para al colorbar con sus atributos usando glsl.
    //const char* attribute_name;
    const char* attribute_name = "coord3d";
    attribute_coord3d = program->attributeLocation(attribute_name);
    bool dontDraw = true;
    if(attribute_coord3d == -1){
        dontDraw = false;
        fprintf(stderr, "Couldn't bind attribute %s\n", attribute_name);
    }

    if(dontDraw){
        cout << "llega" <<endl;
        attribute_name = "texcoord";
        attribute_texcoord = program->attributeLocation(attribute_name);
        if(attribute_texcoord == -1){
            fprintf(stderr, "Couldn't bind attribute %s\n", attribute_name);
        }

        //const char* uniform_name;
        const char* uniform_name = "mvp";
        uniform_mvp = program->uniformLocation(uniform_name);
        if(uniform_mvp == -1){
            fprintf(stderr, "Couldn't bind uniform %s\n", uniform_name);
        }

        uniform_name = "mytexture";
        uniform_mytexture = program->uniformLocation(uniform_name);
        if(uniform_mytexture == -1){
            fprintf(stderr, "Couldn't bind uniform %s\n", uniform_name);
        }
        f->glBindTexture(GL_TEXTURE_1D,0);

        f->glActiveTexture(GL_TEXTURE0);

        f->glBindTexture(GL_TEXTURE_1D, texture_id);
        f->glUniform1i(uniform_mytexture, 0);

        f->glEnableVertexAttribArray(attribute_coord3d);
        f->glBindBuffer(GL_ARRAY_BUFFER, vbo_triangle_vertices);

        f->glVertexAttribPointer(
                    attribute_coord3d,
                    3,
                    GL_FLOAT,
                    GL_FALSE,
                    0,0
                    );

        f->glEnableVertexAttribArray(attribute_texcoord);
        f->glBindBuffer(GL_ARRAY_BUFFER, vbo_triangle_texcoords);

        f->glVertexAttribPointer(
                    attribute_texcoord,
                    1,
                    GL_FLOAT,
                    GL_FALSE,
                    0,0
                    );


        f->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_triangle_elements);
        int size;
        f->glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
        f->glDrawElements(GL_TRIANGLES, size/sizeof(GLushort), GL_UNSIGNED_SHORT,0);

        f->glDisableVertexAttribArray(attribute_coord3d);
        f->glDisableVertexAttribArray(attribute_texcoord);
    }*/

    f->glDeleteBuffers(1, &vbo_vertices);
    f->glDeleteBuffers(1, &ibo_elements);

    GLfloat colors[36];

    // Initialize the random seed from the system time
    // srand(time(NULL));

    // Fill colors with random numbers from 0 to 1, use continuous polynomials for r,g,b:
    int k = 0;
    for(int i = 0; i < sizeof(colors)/sizeof(float)/3; ++i) {
        float t = (float)rand()/(float)RAND_MAX;
        colors[k] = 9*(1-t)*t*t*t;
        k++;
        colors[k] = 15*(1-t)*(1-t)*t*t;
        k++;
        colors[k] = 8.5*(1-t)*(1-t)*(1-t)*t;
        k++;
    }

    GLfloat vertices_position[8] = {
     -20.5, -20.5,
      20.5, -20.5,
      20.5, 20.5,
      -20.5, 20.5,
    };

    GLuint indices[6] = {
     0, 1, 2,
     2, 3, 0
    };


    // Create a Vector Buffer Object that will store the vertices on video memory
        GLuint vbo;
        f->glGenBuffers(1, &vbo);

        // Allocate space for vertex positions and colors
        f->glBindBuffer(GL_ARRAY_BUFFER, vbo);
        f->glBufferData(GL_ARRAY_BUFFER, sizeof(vertices_position) + sizeof(colors), NULL, GL_STATIC_DRAW);

        // Transfer the vertex positions:
        f->glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices_position), vertices_position);

        // Transfer the vertex colors:
        f->glBufferSubData(GL_ARRAY_BUFFER, sizeof(vertices_position), sizeof(colors), colors);

        // Create an Element Array Buffer that will store the indices array:
        GLuint eab;
        f->glGenBuffers(1, &eab);

        // Transfer the data from indices to eab
        f->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eab);
        f->glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    f->glDrawElements( GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void BufferedObject3D::drawBufferedObject(QOpenGLShaderProgram* program, float bbox[][3],
                                            int indice, int indiceCara, int posicionCara,
                                            bool isMoverCara, bool isResizeCara,
                                            unsigned char* mapcolor_X,
                                            unsigned char* mapcolor_Y,
                                            unsigned char* mapcolor_Z){
    QOpenGLVertexArrayObject::Binder vao_binder(&m_vao);

    cout << "BufferedObject3D::drawBufferedObject" << endl;
    cout << "Indice de cara " << indiceCara << endl;
    int vertexLocation;
    if(indice <= 0 || indice >= 2){
    //if(indice <= 0){
        int material_color = program->uniformLocation("material_color");
        program->setUniformValue(material_color, color);
        //int vertexLocation;

        m_vertexBuffer.bind();
            vertexLocation = program->attributeLocation("v_coord");
            program->enableAttributeArray(vertexLocation);
            program->setAttributeBuffer(vertexLocation, GL_FLOAT, 0, 3, sizeof(float) * 3);
        m_vertexBuffer.release();

        m_normalBuffer.bind();
            int normalLocation = program->attributeLocation("v_normal");
            program->enableAttributeArray(normalLocation);
            program->setAttributeBuffer(normalLocation, GL_FLOAT, 0, 3, sizeof(float) * 3);
        m_normalBuffer.release();

        m_indexBuffer.bind();
            glDrawElements(GL_TRIANGLES, num_triangles * 3, GL_UNSIGNED_INT, 0 );
        m_indexBuffer.release();
        drawBufferedBoundingObject(program,bbox,vertexLocation);
    }else{
        drawBufferedColorBar(program,bbox,indice,mapcolor_X,mapcolor_Y,mapcolor_Z);
        drawWall(program,bbox, mapcolor_Y, indiceCara);
        drawWallInverse(program,bbox, mapcolor_Y, indiceCara);
        cout << "ENTRA BOUNDIN BOX" << endl;
        drawBufferedBoundingObject(program,bbox,vertexLocation);
    }

}

void BufferedObject3D::drawBufferedColorBarAndPaint(QOpenGLShaderProgram* program, float bbox[][3], int indice){
    QOpenGLFunctions_4_1_Core *f = NULL;
    cout << "El indice es de " << indice << endl;
    f = QOpenGLContext::currentContext()->versionFunctions<QOpenGLFunctions_4_1_Core>();
    GLuint vbo_triangle_vertices;
    GLuint vbo_triangle_texcoords;
    GLuint ibo_triangle_elements;
    GLint attribute_coord3d, attribute_texcoord;
    GLuint texture_id;
    GLint uniform_mvp, uniform_mytexture;

    int valor = num_vertices * 3;
    int resta = num_vertices - (2 * 3);
    int numSector = resta / 6; //Numero de sectores en el color bar

    GLfloat* triangle_vertices = new GLfloat[valor];

    for(int i = 0; i < (num_vertices * 3); i++){
        triangle_vertices[i] = m_vertices[i];
    }

    /*GLfloat triangle_vertices[]={
        0.0, 0.0, 1.0,
        1.0, 0.0, 1.0,
        1.0, 1.0, 1.0,
        0.0, 0.0, 1.0,
        1.0, 1.0, 1.0,
        0.0, 1.0, 1.0
    };*/


    f->glGenBuffers(1, &vbo_triangle_vertices);
    f->glBindBuffer(GL_ARRAY_BUFFER, vbo_triangle_vertices);
    f->glBufferData(GL_ARRAY_BUFFER, 3*num_vertices*sizeof(GLfloat), triangle_vertices, GL_STATIC_DRAW);
    //f->glBufferData(GL_ARRAY_BUFFER, sizeof(triangle_vertices), triangle_vertices, GL_STATIC_DRAW);
    f->glBindBuffer(GL_ARRAY_BUFFER, 0);

    //Definicion de textcoords
    /*GLfloat triangle_texcoords[]={
        0.0, 1.0, 1.0,
        0.0, 1.0, 0.0
    };*/

    /*
    int i = 0;
    GLfloat* triangle_texcoords = new GLfloat[6];
    triangle_texcoords[0] = 0.0;
    triangle_texcoords[1] = 1.0;
    triangle_texcoords[2] = 1.0;
    triangle_texcoords[3] = 0.0;
    triangle_texcoords[4] = 1.0;
    triangle_texcoords[5] = 0.0;
    */
    GLfloat* triangle_texcoords = new GLfloat[numSector*6];

    // GLfloat* triangle_texcoords = new GLfloat[6];
    for(int i = 0; i < numSector; i++){
        //Para X
        triangle_texcoords[6*i] = 0.0;
        triangle_texcoords[6*i + 1] = 1.0;
        triangle_texcoords[6*i + 2] = 1.0;
        triangle_texcoords[6*i + 3] = 0.0;
        triangle_texcoords[6*i + 4] = 1.0;
        triangle_texcoords[6*i + 5] = 0.0;
    }
    //for(int i = 0; i < )

    f->glGenBuffers(1, &vbo_triangle_texcoords);
    f->glBindBuffer(GL_ARRAY_BUFFER, vbo_triangle_texcoords);
    //f->glBufferData(GL_ARRAY_BUFFER, sizeof(triangle_texcoords), triangle_texcoords, GL_STATIC_DRAW);
    //f->glBufferData(GL_ARRAY_BUFFER, 6*numSector*sizeof(GLfloat), triangle_texcoords, GL_STATIC_DRAW);
    f->glBufferData(GL_ARRAY_BUFFER, 6*sizeof(GLfloat), triangle_texcoords, GL_STATIC_DRAW);

    int triangles_numeros = num_triangles * 3;
    GLushort* triangle_elements = new GLushort[triangles_numeros];

    for(int i = 0; i < num_triangles*3; i++){
        triangle_elements[i] = m_indices[i];
    }
    /*GLushort triangle_elements[] = {
        0, 1, 2,
        3, 4, 5
    };*/

    f->glGenBuffers(1, &ibo_triangle_elements);
    f->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_triangle_elements);
    f->glBufferData(GL_ELEMENT_ARRAY_BUFFER, 3*num_triangles*sizeof(GLushort), triangle_elements, GL_STATIC_DRAW);
    //f->glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(triangle_elements), triangle_elements, GL_STATIC_DRAW);
    //f->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    f->glGenTextures(1, &texture_id);
    assert(glGetError() == GL_NO_ERROR);

    f->glBindTexture(GL_TEXTURE_1D, texture_id);
    assert(glGetError() == GL_NO_ERROR);

    //glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    //assert(glGetError() == GL_NO_ERROR);
    f->glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    f->glTexImage1D(GL_TEXTURE_1D,
                    0,
                    GL_RGB,
                    64,
                    0,
                    GL_RGB,
                    GL_UNSIGNED_BYTE,
                    pixel_data_x);

    assert(glGetError() == GL_NO_ERROR);

    // texture sampling/filtering operation.
    /*f->glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    cout <<"El valor de 1 es " <<glGetError() << endl;
    assert(glGetError() == GL_NO_ERROR);
    f->glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    cout <<"El valor de 2 es " <<glGetError() << endl;
    assert(glGetError() == GL_NO_ERROR);
    f->glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    cout <<"El valor de 3 es " <<glGetError() << endl;
    assert(glGetError() == GL_NO_ERROR);
    f->glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    cout <<"El valor de 4 es " <<glGetError() << endl;
    assert(glGetError() == GL_NO_ERROR);*/

    //f->glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    //Definiendo las estructuras para al colorbar con sus atributos usando glsl.
    const char* attribute_name;
    attribute_name = "coord3d";
    attribute_coord3d = program->attributeLocation(attribute_name);
    if(attribute_coord3d == -1){
        fprintf(stderr, "Couldn't bind attribute %s\n", attribute_name);
        qApp->exit(-1);
    }

    attribute_name = "texcoord";
    attribute_texcoord = program->attributeLocation(attribute_name);
    if(attribute_texcoord == -1){
        fprintf(stderr, "Couldn't bind attribute %s\n", attribute_name);
        qApp->exit(-1);
    }

    const char* uniform_name;
    uniform_name = "mvp";
    uniform_mvp = program->uniformLocation(uniform_name);
    if(uniform_mvp == -1){
        fprintf(stderr, "Couldn't bind uniform %s\n", uniform_name);
        qApp->exit(-1);
    }

    uniform_name = "mytexture";
    uniform_mytexture = program->uniformLocation(uniform_name);
    if(uniform_mytexture == -1){
        fprintf(stderr, "Couldn't bind uniform %s\n", uniform_name);
        qApp->exit(-1);
    }
    f->glBindTexture(GL_TEXTURE_1D,0);
    //f->glClearColor(1.0, 1.0, 1.0, 1.0);

    f->glActiveTexture(GL_TEXTURE0);

    f->glBindTexture(GL_TEXTURE_1D, texture_id);
    f->glUniform1i(uniform_mytexture, 0);

    f->glEnableVertexAttribArray(attribute_coord3d);
    f->glBindBuffer(GL_ARRAY_BUFFER, vbo_triangle_vertices);

    f->glVertexAttribPointer(
                attribute_coord3d,
                3,
                GL_FLOAT,
                GL_FALSE,
                0,0
                );

    f->glEnableVertexAttribArray(attribute_texcoord);
    f->glBindBuffer(GL_ARRAY_BUFFER, vbo_triangle_texcoords);

    f->glVertexAttribPointer(
                attribute_texcoord,
                1,
                GL_FLOAT,
                GL_FALSE,
                0,0
                );


    f->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_triangle_elements);
    int size;
    f->glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
    f->glDrawElements(GL_TRIANGLES, size/sizeof(GLushort), GL_UNSIGNED_SHORT, 0);

    f->glDisableVertexAttribArray(attribute_coord3d);
    f->glDisableVertexAttribArray(attribute_texcoord);
    cout << "Fin de BufferedObject3D::drawBufferedColorBarAndPaint" << endl;
}

void BufferedObject3D::drawWall(QOpenGLShaderProgram* program, float bbox[][3], unsigned char* mapcolor, int indiceCara) {
    QOpenGLFunctions_4_1_Core *f = NULL;
    f = QOpenGLContext::currentContext()->versionFunctions<QOpenGLFunctions_4_1_Core>();
    GLuint vbo_triangle_vertices;
    GLuint vbo_triangle_texcoords;
    GLuint ibo_triangle_elements;
    GLint attribute_coord3d, attribute_texcoord;
    GLuint texture_id;
    GLint uniform_mvp, uniform_mytexture;

    int numero_vertices = 6;
    int valor = numero_vertices * 3;
    GLfloat* triangle_vertices = new GLfloat[valor];

    if(indiceCara == 0) { //Izquierda
        // Primer triangulo
        triangle_vertices[0] = bbox[0][0];
        triangle_vertices[1] = bbox[0][1];
        triangle_vertices[2] = bbox[1][2];
        triangle_vertices[3] = bbox[0][0];
        triangle_vertices[4] = bbox[0][1];
        triangle_vertices[5] = bbox[0][2];
        triangle_vertices[6] = bbox[0][0];
        triangle_vertices[7] = bbox[1][1];
        triangle_vertices[8] = bbox[0][2];

        // Segundo triangulo
        triangle_vertices[9] = bbox[0][0];
        triangle_vertices[10] = bbox[0][1];
        triangle_vertices[11] = bbox[1][2];
        triangle_vertices[12] = bbox[0][0];
        triangle_vertices[13] = bbox[1][1];
        triangle_vertices[14] = bbox[0][2];
        triangle_vertices[15] = bbox[0][0];
        triangle_vertices[16] = bbox[1][1];
        triangle_vertices[17] = bbox[1][2];

    } else if(indiceCara == 1) {
        // Primer triangulo
        triangle_vertices[0] = bbox[1][0];
        triangle_vertices[1] = bbox[1][1];
        triangle_vertices[2] = bbox[1][2];
        triangle_vertices[3] = bbox[1][0];
        triangle_vertices[4] = bbox[1][1];
        triangle_vertices[5] = bbox[0][2];
        triangle_vertices[6] = bbox[1][0];
        triangle_vertices[7] = bbox[0][1];
        triangle_vertices[8] = bbox[0][2];


        // Segundo triangulo
        triangle_vertices[9] = bbox[1][0];
        triangle_vertices[10] = bbox[1][1];
        triangle_vertices[11] = bbox[1][2];
        triangle_vertices[12] = bbox[1][0];
        triangle_vertices[13] = bbox[0][1];
        triangle_vertices[14] = bbox[0][2];
        triangle_vertices[15] = bbox[1][0];
        triangle_vertices[16] = bbox[0][1];
        triangle_vertices[17] = bbox[1][2];

    } else if(indiceCara == 2) {
        // Primer triangulo
        triangle_vertices[3] = bbox[1][0];
        triangle_vertices[4] = bbox[0][1];
        triangle_vertices[5] = bbox[0][2];
        triangle_vertices[6] = bbox[0][0];
        triangle_vertices[7] = bbox[0][1];
        triangle_vertices[8] = bbox[0][2];
        triangle_vertices[0] = bbox[0][0];
        triangle_vertices[1] = bbox[0][1];
        triangle_vertices[2] = bbox[1][2];


        // Segundo triangulo
        triangle_vertices[15] = bbox[0][0];
        triangle_vertices[16] = bbox[0][1];
        triangle_vertices[17] = bbox[1][2];
        triangle_vertices[9] = bbox[1][0];
        triangle_vertices[10] = bbox[0][1];
        triangle_vertices[11] = bbox[1][2];
        triangle_vertices[12] = bbox[1][0];
        triangle_vertices[13] = bbox[0][1];
        triangle_vertices[14] = bbox[0][2];
    } else if(indiceCara == 3) {
        // Primer triangulo
        triangle_vertices[0] = bbox[0][0];
        triangle_vertices[1] = bbox[1][1];
        triangle_vertices[2] = bbox[0][2];
        triangle_vertices[3] = bbox[0][0];
        triangle_vertices[4] = bbox[1][1];
        triangle_vertices[5] = bbox[1][2];
        triangle_vertices[6] = bbox[1][0];
        triangle_vertices[7] = bbox[1][1];
        triangle_vertices[8] = bbox[1][2];

        // Segundo triangulo
        triangle_vertices[9] = bbox[0][0];
        triangle_vertices[10] = bbox[1][1];
        triangle_vertices[11] = bbox[0][2];
        triangle_vertices[12] = bbox[1][0];
        triangle_vertices[13] = bbox[1][1];
        triangle_vertices[14] = bbox[1][2];
        triangle_vertices[15] = bbox[1][0];
        triangle_vertices[16] = bbox[1][1];
        triangle_vertices[17] = bbox[0][2];
    } else if(indiceCara == 4) {
        // Primer triangulo
        triangle_vertices[0] = bbox[0][0];
        triangle_vertices[1] = bbox[0][1];
        triangle_vertices[2] = bbox[1][2];
        triangle_vertices[3] = bbox[0][0];
        triangle_vertices[4] = bbox[1][1];
        triangle_vertices[5] = bbox[1][2];
        triangle_vertices[6] = bbox[1][0];
        triangle_vertices[7] = bbox[1][1];
        triangle_vertices[8] = bbox[1][2];


        // Segundo triangulo
        triangle_vertices[9] = bbox[0][0];
        triangle_vertices[10] = bbox[0][1];
        triangle_vertices[11] = bbox[1][2];
        triangle_vertices[12] = bbox[1][0];
        triangle_vertices[13] = bbox[1][1];
        triangle_vertices[14] = bbox[1][2];
        triangle_vertices[15] = bbox[1][0];
        triangle_vertices[16] = bbox[0][1];
        triangle_vertices[17] = bbox[1][2];
    } else if(indiceCara == 5) {
        // Primer triangulo
        triangle_vertices[0] = bbox[0][0];
        triangle_vertices[1] = bbox[0][1];
        triangle_vertices[2] = bbox[0][2];
        triangle_vertices[3] = bbox[1][0];
        triangle_vertices[4] = bbox[0][1];
        triangle_vertices[5] = bbox[0][2];
        triangle_vertices[6] = bbox[1][0];
        triangle_vertices[7] = bbox[1][1];
        triangle_vertices[8] = bbox[0][2];

        // Segundo triangulo
        triangle_vertices[9] = bbox[0][0];
        triangle_vertices[10] = bbox[0][1];
        triangle_vertices[11] = bbox[0][2];
        triangle_vertices[12]  = bbox[1][0];
        triangle_vertices[13] = bbox[1][1];
        triangle_vertices[14] = bbox[0][2];
        triangle_vertices[15] = bbox[0][0];
        triangle_vertices[16] = bbox[1][1];
        triangle_vertices[17] = bbox[0][2];
    } else if(indiceCara == 0) {
        // Primer triangulo
        triangle_vertices[6] = bbox[0][0];
        triangle_vertices[7] = bbox[1][1];
        triangle_vertices[8] = bbox[0][2];
        triangle_vertices[0] = bbox[0][0];
        triangle_vertices[1] = bbox[0][1];
        triangle_vertices[2] = bbox[1][2];
        triangle_vertices[3] = bbox[0][0];
        triangle_vertices[4] = bbox[0][1];
        triangle_vertices[5] = bbox[0][2];

        // Segundo triangulo
        triangle_vertices[9] = bbox[0][0];
        triangle_vertices[10] = bbox[0][1];
        triangle_vertices[11] = bbox[1][2];
        triangle_vertices[12] = bbox[0][0];
        triangle_vertices[13] = bbox[1][1];
        triangle_vertices[14] = bbox[0][2];
        triangle_vertices[15] = bbox[0][0];
        triangle_vertices[16] = bbox[1][1];
        triangle_vertices[17] = bbox[1][2];
    }

    f->glGenBuffers(1, &vbo_triangle_vertices);
    f->glBindBuffer(GL_ARRAY_BUFFER, vbo_triangle_vertices);

    f->glBufferData(GL_ARRAY_BUFFER, valor*sizeof(GLfloat), triangle_vertices, GL_STATIC_DRAW);

    f->glBindBuffer(GL_ARRAY_BUFFER, 0);

    GLfloat* triangle_texcoords = new GLfloat[numero_vertices];

    triangle_texcoords[0] = 0.0;
    triangle_texcoords[1] = 1.0;
    triangle_texcoords[2] = 1.0;
    triangle_texcoords[3] = 0.0;
    triangle_texcoords[4] = 1.0;
    triangle_texcoords[5] = 0.0;

    f->glGenBuffers(1, &vbo_triangle_texcoords);
    f->glBindBuffer(GL_ARRAY_BUFFER, vbo_triangle_texcoords);
    f->glBufferData(GL_ARRAY_BUFFER, numero_vertices*sizeof(GLfloat), triangle_texcoords, GL_STATIC_DRAW);

    GLushort* triangle_elements = new GLushort[numero_vertices];
    for(int i = 0; i < numero_vertices; i++){
        triangle_elements[i] = i;
    }

    f->glGenBuffers(1, &ibo_triangle_elements);
    f->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_triangle_elements);
    f->glBufferData(GL_ELEMENT_ARRAY_BUFFER, numero_vertices*sizeof(GLushort), triangle_elements, GL_STATIC_DRAW);

    f->glGenTextures(1, &texture_id);
    assert(glGetError() == GL_NO_ERROR);

    f->glBindTexture(GL_TEXTURE_1D, texture_id);
    assert(glGetError() == GL_NO_ERROR);

    f->glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    f->glTexImage1D(GL_TEXTURE_1D,
                    0,
                    GL_RGB,
                    64,
                    0,
                    GL_RGB,
                    GL_UNSIGNED_BYTE,
                    mapcolor);

    assert(glGetError() == GL_NO_ERROR);

    // texture sampling/filtering operation.
    f->glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    assert(glGetError() == GL_NO_ERROR);
    f->glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    assert(glGetError() == GL_NO_ERROR);
    f->glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    assert(glGetError() == GL_NO_ERROR);
    f->glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    assert(glGetError() == GL_NO_ERROR);

    //Definiendo las estructuras para al colorbar con sus atributos usando glsl.
    const char* attribute_name;
    attribute_name = "coord3d";
    attribute_coord3d = program->attributeLocation(attribute_name);
    if(attribute_coord3d == -1){
        fprintf(stderr, "Couldn't bind attribute %s\n", attribute_name);
        qApp->exit(-1);
    }

    attribute_name = "texcoord";
    attribute_texcoord = program->attributeLocation(attribute_name);
    if(attribute_texcoord == -1){
        fprintf(stderr, "Couldn't bind attribute %s\n", attribute_name);
        qApp->exit(-1);
    }

    const char* uniform_name;
    uniform_name = "mvp";
    uniform_mvp = program->uniformLocation(uniform_name);
    if(uniform_mvp == -1){
        fprintf(stderr, "Couldn't bind uniform %s\n", uniform_name);
        qApp->exit(-1);
    }

    uniform_name = "mytexture";
    uniform_mytexture = program->uniformLocation(uniform_name);
    if(uniform_mytexture == -1){
        fprintf(stderr, "Couldn't bind uniform %s\n", uniform_name);
        qApp->exit(-1);
    }
    f->glBindTexture(GL_TEXTURE_1D,0);
    //f->glClearColor(1.0, 1.0, 1.0, 1.0);

    f->glActiveTexture(GL_TEXTURE0);

    f->glBindTexture(GL_TEXTURE_1D, texture_id);
    f->glUniform1i(uniform_mytexture, 0);

    f->glEnableVertexAttribArray(attribute_coord3d);
    f->glBindBuffer(GL_ARRAY_BUFFER, vbo_triangle_vertices);

    f->glVertexAttribPointer(
                attribute_coord3d,
                3,
                GL_FLOAT,
                GL_FALSE,
                0,0
                );

    f->glEnableVertexAttribArray(attribute_texcoord);
    f->glBindBuffer(GL_ARRAY_BUFFER, vbo_triangle_vertices);

    f->glVertexAttribPointer(
                attribute_coord3d,
                3,
                GL_FLOAT,
                GL_FALSE,
                0,0
                );

    f->glEnableVertexAttribArray(attribute_texcoord);
    f->glBindBuffer(GL_ARRAY_BUFFER, vbo_triangle_texcoords);

    f->glVertexAttribPointer(
                attribute_texcoord,
                1,
                GL_FLOAT,
                GL_FALSE,
                0,0
                );


    f->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_triangle_elements);
    int size;
    f->glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
    f->glDrawElements(GL_TRIANGLES, size/sizeof(GLushort), GL_UNSIGNED_SHORT, 0);

    f->glDisableVertexAttribArray(attribute_coord3d);
    f->glDisableVertexAttribArray(attribute_texcoord);
}


void BufferedObject3D::drawWallInverse(QOpenGLShaderProgram* program, float bbox[][3], unsigned char* mapcolor, int indiceCara) {
    QOpenGLFunctions_4_1_Core *f = NULL;
    f = QOpenGLContext::currentContext()->versionFunctions<QOpenGLFunctions_4_1_Core>();
    GLuint vbo_triangle_vertices;
    GLuint vbo_triangle_texcoords;
    GLuint ibo_triangle_elements;
    GLint attribute_coord3d, attribute_texcoord;
    GLuint texture_id;
    GLint uniform_mvp, uniform_mytexture;

    int numero_vertices = 6;
    int valor = numero_vertices * 3;
    GLfloat* triangle_vertices = new GLfloat[valor];

    if(indiceCara == 0) { //Izquierda
        // Primer triangulo
        triangle_vertices[3] = bbox[0][0];
        triangle_vertices[4] = bbox[1][1];
        triangle_vertices[5] = bbox[0][2];
        triangle_vertices[6] = bbox[0][0];
        triangle_vertices[7] = bbox[0][1];
        triangle_vertices[8] = bbox[0][2];
        triangle_vertices[0] = bbox[0][0];
        triangle_vertices[1] = bbox[0][1];
        triangle_vertices[2] = bbox[1][2];

        // Segundo triangulo
        triangle_vertices[9] = bbox[0][0];
        triangle_vertices[10] = bbox[1][1];
        triangle_vertices[11] = bbox[1][2];
        triangle_vertices[12] = bbox[0][0];
        triangle_vertices[13] = bbox[1][1];
        triangle_vertices[14] = bbox[0][2];
        triangle_vertices[15] = bbox[0][0];
        triangle_vertices[16] = bbox[0][1];
        triangle_vertices[17] = bbox[1][2];


    } else if(indiceCara == 1) {
        // Primer triangulo
        triangle_vertices[0] = bbox[1][0];
        triangle_vertices[1] = bbox[1][1];
        triangle_vertices[2] = bbox[1][2];
        triangle_vertices[3] = bbox[1][0];
        triangle_vertices[4] = bbox[0][1];
        triangle_vertices[5] = bbox[0][2];
        triangle_vertices[6] = bbox[1][0];
        triangle_vertices[7] = bbox[1][1];
        triangle_vertices[8] = bbox[0][2];

        // Segundo triangulo
        triangle_vertices[15] = bbox[1][0];
        triangle_vertices[16] = bbox[1][1];
        triangle_vertices[17] = bbox[1][2];
        triangle_vertices[12] = bbox[1][0];
        triangle_vertices[13] = bbox[0][1];
        triangle_vertices[14] = bbox[0][2];
        triangle_vertices[9] = bbox[1][0];
        triangle_vertices[10] = bbox[0][1];
        triangle_vertices[11] = bbox[1][2];

    } else if(indiceCara == 2) {
        // Primer triangulo
        triangle_vertices[3] = bbox[0][0];
        triangle_vertices[4] = bbox[0][1];
        triangle_vertices[5] = bbox[0][2];
        triangle_vertices[6] = bbox[1][0];
        triangle_vertices[7] = bbox[0][1];
        triangle_vertices[8] = bbox[0][2];
        triangle_vertices[0] = bbox[0][0];
        triangle_vertices[1] = bbox[0][1];
        triangle_vertices[2] = bbox[1][2];

        // Segundo triangulo
        triangle_vertices[15] = bbox[1][0];
        triangle_vertices[16] = bbox[0][1];
        triangle_vertices[17] = bbox[1][2];
        triangle_vertices[12] = bbox[1][0];
        triangle_vertices[13] = bbox[0][1];
        triangle_vertices[14] = bbox[0][2];
        triangle_vertices[9] = bbox[0][0];
        triangle_vertices[10] = bbox[0][1];
        triangle_vertices[11] = bbox[1][2];
    } else if(indiceCara == 3) {
        // Primer triangulo
        triangle_vertices[3] = bbox[1][0];
        triangle_vertices[4] = bbox[1][1];
        triangle_vertices[5] = bbox[1][2];
        triangle_vertices[6] = bbox[0][0];
        triangle_vertices[7] = bbox[1][1];
        triangle_vertices[8] = bbox[1][2];
        triangle_vertices[0] = bbox[0][0];
        triangle_vertices[1] = bbox[1][1];
        triangle_vertices[2] = bbox[0][2];

        // Segundo triangulo
        triangle_vertices[9] = bbox[1][0];
        triangle_vertices[10] = bbox[1][1];
        triangle_vertices[11] = bbox[0][2];
        triangle_vertices[12] = bbox[1][0];
        triangle_vertices[13] = bbox[1][1];
        triangle_vertices[14] = bbox[1][2];
        triangle_vertices[15] = bbox[0][0];
        triangle_vertices[16] = bbox[1][1];
        triangle_vertices[17] = bbox[0][2];

    } else if(indiceCara == 4) {
        // Primer triangulo
        triangle_vertices[3] = bbox[1][0];
        triangle_vertices[4] = bbox[1][1];
        triangle_vertices[5] = bbox[1][2];
        triangle_vertices[6] = bbox[0][0];
        triangle_vertices[7] = bbox[1][1];
        triangle_vertices[8] = bbox[1][2];
        triangle_vertices[0] = bbox[0][0];
        triangle_vertices[1] = bbox[0][1];
        triangle_vertices[2] = bbox[1][2];

        // Segundo triangulo
        triangle_vertices[15] = bbox[0][0];
        triangle_vertices[16] = bbox[0][1];
        triangle_vertices[17] = bbox[1][2];
        triangle_vertices[12] = bbox[1][0];
        triangle_vertices[13] = bbox[1][1];
        triangle_vertices[14] = bbox[1][2];
        triangle_vertices[9] = bbox[1][0];
        triangle_vertices[10] = bbox[0][1];
        triangle_vertices[11] = bbox[1][2];
    } else if(indiceCara == 5) {
        // Primer triangulo
        triangle_vertices[3] = bbox[1][0];
        triangle_vertices[4] = bbox[1][1];
        triangle_vertices[5] = bbox[0][2];
        triangle_vertices[6] = bbox[1][0];
        triangle_vertices[7] = bbox[0][1];
        triangle_vertices[8] = bbox[0][2];
        triangle_vertices[0] = bbox[0][0];
        triangle_vertices[1] = bbox[0][1];
        triangle_vertices[2] = bbox[0][2];

        // Segundo triangulo
        triangle_vertices[15] = bbox[0][0];
        triangle_vertices[16] = bbox[0][1];
        triangle_vertices[17] = bbox[0][2];
        triangle_vertices[12]  = bbox[1][0];
        triangle_vertices[13] = bbox[1][1];
        triangle_vertices[14] = bbox[0][2];
        triangle_vertices[9] = bbox[0][0];
        triangle_vertices[10] = bbox[1][1];
        triangle_vertices[11] = bbox[0][2];
    } else if(indiceCara == 0) {
        // Primer triangulo
        triangle_vertices[6] = bbox[0][0];
        triangle_vertices[7] = bbox[0][1];
        triangle_vertices[8] = bbox[1][2];
        triangle_vertices[3] = bbox[0][0];
        triangle_vertices[4] = bbox[0][1];
        triangle_vertices[5] = bbox[0][2];
        triangle_vertices[0] = bbox[0][0];
        triangle_vertices[1] = bbox[1][1];
        triangle_vertices[2] = bbox[0][2];

        // Segundo triangulo
        triangle_vertices[15] = bbox[0][0];
        triangle_vertices[16] = bbox[0][1];
        triangle_vertices[17] = bbox[1][2];
        triangle_vertices[12] = bbox[0][0];
        triangle_vertices[13] = bbox[1][1];
        triangle_vertices[14] = bbox[0][2];
        triangle_vertices[9] = bbox[0][0];
        triangle_vertices[10] = bbox[1][1];
        triangle_vertices[11] = bbox[1][2];
    }

    f->glGenBuffers(1, &vbo_triangle_vertices);
    f->glBindBuffer(GL_ARRAY_BUFFER, vbo_triangle_vertices);

    f->glBufferData(GL_ARRAY_BUFFER, valor*sizeof(GLfloat), triangle_vertices, GL_STATIC_DRAW);

    f->glBindBuffer(GL_ARRAY_BUFFER, 0);

    GLfloat* triangle_texcoords = new GLfloat[numero_vertices];

    triangle_texcoords[0] = 0.0;
    triangle_texcoords[1] = 1.0;
    triangle_texcoords[2] = 1.0;
    triangle_texcoords[3] = 0.0;
    triangle_texcoords[4] = 1.0;
    triangle_texcoords[5] = 0.0;

    f->glGenBuffers(1, &vbo_triangle_texcoords);
    f->glBindBuffer(GL_ARRAY_BUFFER, vbo_triangle_texcoords);
    f->glBufferData(GL_ARRAY_BUFFER, numero_vertices*sizeof(GLfloat), triangle_texcoords, GL_STATIC_DRAW);

    GLushort* triangle_elements = new GLushort[numero_vertices];
    for(int i = 0; i < numero_vertices; i++){
        triangle_elements[i] = i;
    }

    f->glGenBuffers(1, &ibo_triangle_elements);
    f->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_triangle_elements);
    f->glBufferData(GL_ELEMENT_ARRAY_BUFFER, numero_vertices*sizeof(GLushort), triangle_elements, GL_STATIC_DRAW);

    f->glGenTextures(1, &texture_id);
    assert(glGetError() == GL_NO_ERROR);

    f->glBindTexture(GL_TEXTURE_1D, texture_id);
    assert(glGetError() == GL_NO_ERROR);

    f->glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    f->glTexImage1D(GL_TEXTURE_1D,
                    0,
                    GL_RGB,
                    64,
                    0,
                    GL_RGB,
                    GL_UNSIGNED_BYTE,
                    mapcolor);

    assert(glGetError() == GL_NO_ERROR);

    // texture sampling/filtering operation.
    f->glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    assert(glGetError() == GL_NO_ERROR);
    f->glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    assert(glGetError() == GL_NO_ERROR);
    f->glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    assert(glGetError() == GL_NO_ERROR);
    f->glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    assert(glGetError() == GL_NO_ERROR);

    //Definiendo las estructuras para al colorbar con sus atributos usando glsl.
    const char* attribute_name;
    attribute_name = "coord3d";
    attribute_coord3d = program->attributeLocation(attribute_name);
    if(attribute_coord3d == -1){
        fprintf(stderr, "Couldn't bind attribute %s\n", attribute_name);
        qApp->exit(-1);
    }

    attribute_name = "texcoord";
    attribute_texcoord = program->attributeLocation(attribute_name);
    if(attribute_texcoord == -1){
        fprintf(stderr, "Couldn't bind attribute %s\n", attribute_name);
        qApp->exit(-1);
    }

    const char* uniform_name;
    uniform_name = "mvp";
    uniform_mvp = program->uniformLocation(uniform_name);
    if(uniform_mvp == -1){
        fprintf(stderr, "Couldn't bind uniform %s\n", uniform_name);
        qApp->exit(-1);
    }

    uniform_name = "mytexture";
    uniform_mytexture = program->uniformLocation(uniform_name);
    if(uniform_mytexture == -1){
        fprintf(stderr, "Couldn't bind uniform %s\n", uniform_name);
        qApp->exit(-1);
    }
    f->glBindTexture(GL_TEXTURE_1D,0);
    //f->glClearColor(1.0, 1.0, 1.0, 1.0);

    f->glActiveTexture(GL_TEXTURE0);

    f->glBindTexture(GL_TEXTURE_1D, texture_id);
    f->glUniform1i(uniform_mytexture, 0);

    f->glEnableVertexAttribArray(attribute_coord3d);
    f->glBindBuffer(GL_ARRAY_BUFFER, vbo_triangle_vertices);

    f->glVertexAttribPointer(
                attribute_coord3d,
                3,
                GL_FLOAT,
                GL_FALSE,
                0,0
                );

    f->glEnableVertexAttribArray(attribute_texcoord);
    f->glBindBuffer(GL_ARRAY_BUFFER, vbo_triangle_vertices);

    f->glVertexAttribPointer(
                attribute_coord3d,
                3,
                GL_FLOAT,
                GL_FALSE,
                0,0
                );

    f->glEnableVertexAttribArray(attribute_texcoord);
    f->glBindBuffer(GL_ARRAY_BUFFER, vbo_triangle_texcoords);

    f->glVertexAttribPointer(
                attribute_texcoord,
                1,
                GL_FLOAT,
                GL_FALSE,
                0,0
                );


    f->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_triangle_elements);
    int size;
    f->glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
    f->glDrawElements(GL_TRIANGLES, size/sizeof(GLushort), GL_UNSIGNED_SHORT, 0);

    f->glDisableVertexAttribArray(attribute_coord3d);
    f->glDisableVertexAttribArray(attribute_texcoord);
}

void BufferedObject3D::drawBufferedColorBar(QOpenGLShaderProgram* program, float bbox[][3], int indice, unsigned char* mapcolor_X,
                                            unsigned char* mapcolor_Y, unsigned char* mapcolor_Z){
    QOpenGLFunctions_4_1_Core *f = NULL;
    f = QOpenGLContext::currentContext()->versionFunctions<QOpenGLFunctions_4_1_Core>();
    GLuint vbo_triangle_vertices;
    GLuint vbo_triangle_texcoords;
    GLuint ibo_triangle_elements;
    GLint attribute_coord3d, attribute_texcoord;
    GLuint texture_id;
    GLint uniform_mvp, uniform_mytexture;

    int valor = num_vertices * 3;
    int resta = num_vertices - (2 * 3);
    int numSector = resta / 6; //Numero de sectores en el color bar

    GLfloat* triangle_vertices = new GLfloat[numSector*2*3*3];

    for(int i = 0; i < numSector; i++){
        if(i < (numSector - 1)){
            //Primer triangulo
            triangle_vertices[18*i] = m_vertices[3*i];
            triangle_vertices[(18*i) + 1] = m_vertices[(3*i) + 1];
            triangle_vertices[(18*i) + 2] = m_vertices[(3*i) + 2];
            triangle_vertices[(18*i) + 3] = m_vertices[3*(i + numSector)];
            triangle_vertices[(18*i) + 4] = m_vertices[3*(i + numSector) + 1];
            triangle_vertices[(18*i) + 5] = m_vertices[3*(i + numSector) + 2];
            triangle_vertices[(18*i) + 6] = m_vertices[3*(i + numSector + 1)];
            triangle_vertices[(18*i) + 7] = m_vertices[3*(i + numSector + 1) + 1];
            triangle_vertices[(18*i) + 8] = m_vertices[3*(i + numSector + 1) + 2];

            //Segundo Triangulo
            triangle_vertices[(18*i) + 9] = m_vertices[3*i];
            triangle_vertices[(18*i) + 10] = m_vertices[(3*i) + 1];
            triangle_vertices[(18*i) + 11] = m_vertices[(3*i) + 2];
            triangle_vertices[(18*i) + 12] = m_vertices[3*(i + numSector + 1)];
            triangle_vertices[(18*i) + 13] = m_vertices[3*(i + numSector + 1) + 1];
            triangle_vertices[(18*i) + 14] = m_vertices[3*(i + numSector + 1) + 2];
            triangle_vertices[(18*i) + 15] = m_vertices[3*(i + 1)];
            triangle_vertices[(18*i) + 16] = m_vertices[3*(i + 1) + 1];
            triangle_vertices[(18*i) + 17] = m_vertices[3*(i + 1) + 2];
        }else{
            //Primer triangulo extremo
            triangle_vertices[18*i] = m_vertices[3*i];
            triangle_vertices[(18*i) + 1] = m_vertices[(3*i) + 1];
            triangle_vertices[(18*i) + 2] = m_vertices[(3*i) + 2];
            triangle_vertices[(18*i) + 3] = m_vertices[3*(i + numSector)];
            triangle_vertices[(18*i) + 4] = m_vertices[3*(i + numSector) + 1];
            triangle_vertices[(18*i) + 5] = m_vertices[3*(i + numSector) + 2];
            triangle_vertices[(18*i) + 6] = m_vertices[3*(numSector)];
            triangle_vertices[(18*i) + 7] = m_vertices[3*(numSector) + 1];
            triangle_vertices[(18*i) + 8] = m_vertices[3*(numSector) + 2];

            //Segundo Triangulo extremo
            triangle_vertices[(18*i) + 9] = m_vertices[3*i];
            triangle_vertices[(18*i) + 10] = m_vertices[(3*i) + 1];
            triangle_vertices[(18*i) + 11] = m_vertices[(3*i) + 2];
            triangle_vertices[(18*i) + 12] = m_vertices[3*(numSector)];
            triangle_vertices[(18*i) + 13] = m_vertices[3*(numSector) + 1];
            triangle_vertices[(18*i) + 14] = m_vertices[3*(numSector) + 2];
            triangle_vertices[(18*i) + 15] = m_vertices[3*(0)];
            triangle_vertices[(18*i) + 16] = m_vertices[3*(0) + 1];
            triangle_vertices[(18*i) + 17] = m_vertices[3*(0) + 2];
        }
    }

    f->glGenBuffers(1, &vbo_triangle_vertices);
    f->glBindBuffer(GL_ARRAY_BUFFER, vbo_triangle_vertices);
    f->glBufferData(GL_ARRAY_BUFFER, numSector*2*3*3*sizeof(GLfloat), triangle_vertices, GL_STATIC_DRAW);
    f->glBindBuffer(GL_ARRAY_BUFFER, 0);

    GLfloat* triangle_texcoords = new GLfloat[numSector*2*3];

    for(int i = 0; i < numSector; i++){
        triangle_texcoords[6*i] = 0.0;
        triangle_texcoords[6*i + 1] = 1.0;
        triangle_texcoords[6*i + 2] = 1.0;
        triangle_texcoords[6*i + 3] = 0.0;
        triangle_texcoords[6*i + 4] = 1.0;
        triangle_texcoords[6*i + 5] = 0.0;
    }


    f->glGenBuffers(1, &vbo_triangle_texcoords);
    f->glBindBuffer(GL_ARRAY_BUFFER, vbo_triangle_texcoords);
    f->glBufferData(GL_ARRAY_BUFFER, numSector*2*3*sizeof(GLfloat), triangle_texcoords, GL_STATIC_DRAW);

    GLushort* triangle_elements = new GLushort[numSector*2*3];

    for(int i = 0; i < numSector*2*3; i++){
        triangle_elements[i] = i;
    }

    f->glGenBuffers(1, &ibo_triangle_elements);
    f->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_triangle_elements);
    f->glBufferData(GL_ELEMENT_ARRAY_BUFFER, numSector*2*3*sizeof(GLushort), triangle_elements, GL_STATIC_DRAW);

    f->glGenTextures(1, &texture_id);
    assert(glGetError() == GL_NO_ERROR);

    f->glBindTexture(GL_TEXTURE_1D, texture_id);
    assert(glGetError() == GL_NO_ERROR);

    f->glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    f->glTexImage1D(GL_TEXTURE_1D,
                    0,
                    GL_RGB,
                    64,
                    0,
                    GL_RGB,
                    GL_UNSIGNED_BYTE,
                    mapcolor_Y);

    assert(glGetError() == GL_NO_ERROR);

    // texture sampling/filtering operation.
    f->glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    assert(glGetError() == GL_NO_ERROR);
    f->glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    assert(glGetError() == GL_NO_ERROR);
    f->glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    assert(glGetError() == GL_NO_ERROR);
    f->glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    assert(glGetError() == GL_NO_ERROR);

    //Definiendo las estructuras para al colorbar con sus atributos usando glsl.
    const char* attribute_name;
    attribute_name = "coord3d";
    attribute_coord3d = program->attributeLocation(attribute_name);
    if(attribute_coord3d == -1){
        fprintf(stderr, "Couldn't bind attribute %s\n", attribute_name);
        qApp->exit(-1);
    }

    attribute_name = "texcoord";
    attribute_texcoord = program->attributeLocation(attribute_name);
    if(attribute_texcoord == -1){
        fprintf(stderr, "Couldn't bind attribute %s\n", attribute_name);
        qApp->exit(-1);
    }

    const char* uniform_name;
    uniform_name = "mvp";
    uniform_mvp = program->uniformLocation(uniform_name);
    if(uniform_mvp == -1){
        fprintf(stderr, "Couldn't bind uniform %s\n", uniform_name);
        qApp->exit(-1);
    }

    uniform_name = "mytexture";
    uniform_mytexture = program->uniformLocation(uniform_name);
    if(uniform_mytexture == -1){
        fprintf(stderr, "Couldn't bind uniform %s\n", uniform_name);
        qApp->exit(-1);
    }
    f->glBindTexture(GL_TEXTURE_1D,0);
    //f->glClearColor(1.0, 1.0, 1.0, 1.0);

    f->glActiveTexture(GL_TEXTURE0);

    f->glBindTexture(GL_TEXTURE_1D, texture_id);
    f->glUniform1i(uniform_mytexture, 0);

    f->glEnableVertexAttribArray(attribute_coord3d);
    f->glBindBuffer(GL_ARRAY_BUFFER, vbo_triangle_vertices);

    f->glVertexAttribPointer(
                attribute_coord3d,
                3,
                GL_FLOAT,
                GL_FALSE,
                0,0
                );

    f->glEnableVertexAttribArray(attribute_texcoord);
    f->glBindBuffer(GL_ARRAY_BUFFER, vbo_triangle_texcoords);

    f->glVertexAttribPointer(
                attribute_texcoord,
                1,
                GL_FLOAT,
                GL_FALSE,
                0,0
                );


    f->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_triangle_elements);
    int size;
    f->glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
    f->glDrawElements(GL_TRIANGLES, size/sizeof(GLushort), GL_UNSIGNED_SHORT, 0);

    f->glDisableVertexAttribArray(attribute_coord3d);
    f->glDisableVertexAttribArray(attribute_texcoord);

    //Tapa superior
    GLfloat* triangle_vertices_tapaSuperior = new GLfloat[numSector*3*3];
    for(int i = 0; i < numSector; i++){
        if(i < (numSector - 1)){
            triangle_vertices_tapaSuperior[9*i] = m_vertices[numSector*2*3];
            triangle_vertices_tapaSuperior[(9*i) + 1] = m_vertices[numSector*2*3 + 1];
            triangle_vertices_tapaSuperior[(9*i) + 2] = m_vertices[numSector*2*3 + 2];
            triangle_vertices_tapaSuperior[(9*i) + 3] = m_vertices[3*i];
            triangle_vertices_tapaSuperior[(9*i) + 4] = m_vertices[(3*i) + 1];
            triangle_vertices_tapaSuperior[(9*i) + 5] = m_vertices[(3*i) + 2];
            triangle_vertices_tapaSuperior[(9*i) + 6] = m_vertices[3*(i + 1)];
            triangle_vertices_tapaSuperior[(9*i) + 7] = m_vertices[(3*(i + 1)) + 1];
            triangle_vertices_tapaSuperior[(9*i) + 8] = m_vertices[(3*(i + 1)) + 2];
        }else{
            triangle_vertices_tapaSuperior[9*i] = m_vertices[numSector*2*3];
            triangle_vertices_tapaSuperior[(9*i) + 1] = m_vertices[numSector*2*3 + 1];
            triangle_vertices_tapaSuperior[(9*i) + 2] = m_vertices[numSector*2*3 + 2];
            triangle_vertices_tapaSuperior[(9*i) + 3] = m_vertices[3*i];
            triangle_vertices_tapaSuperior[(9*i) + 4] = m_vertices[(3*i) + 1];
            triangle_vertices_tapaSuperior[(9*i) + 5] = m_vertices[(3*i) + 2];
            triangle_vertices_tapaSuperior[(9*i) + 6] = m_vertices[3*(0)];
            triangle_vertices_tapaSuperior[(9*i) + 7] = m_vertices[(3*(0)) + 1];
            triangle_vertices_tapaSuperior[(9*i) + 8] = m_vertices[(3*(0)) + 2];
        }
    }

    f->glGenBuffers(1, &vbo_triangle_vertices);
    f->glBindBuffer(GL_ARRAY_BUFFER, vbo_triangle_vertices);
    f->glBufferData(GL_ARRAY_BUFFER, numSector*3*3*sizeof(GLfloat), triangle_vertices_tapaSuperior, GL_STATIC_DRAW);
    f->glBindBuffer(GL_ARRAY_BUFFER, 0);

    triangle_texcoords = new GLfloat[numSector*3];

    for(int i = 0; i < numSector; i++){
        triangle_texcoords[3*i] = 0.0;
        triangle_texcoords[3*i + 1] = 1.0;
        triangle_texcoords[3*i + 2] = 1.0;
    }

    f->glGenBuffers(1, &vbo_triangle_texcoords);
    f->glBindBuffer(GL_ARRAY_BUFFER, vbo_triangle_texcoords);
    f->glBufferData(GL_ARRAY_BUFFER, numSector*3*sizeof(GLfloat), triangle_texcoords, GL_STATIC_DRAW);

    triangle_elements = new GLushort[numSector*3];

    for(int i = 0; i < numSector*3; i++){
        triangle_elements[i] = i;
    }

    f->glGenBuffers(1, &ibo_triangle_elements);
    f->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_triangle_elements);
    f->glBufferData(GL_ELEMENT_ARRAY_BUFFER, numSector*3*sizeof(GLushort), triangle_elements, GL_STATIC_DRAW);

    f->glGenTextures(1, &texture_id);
    assert(glGetError() == GL_NO_ERROR);

    f->glBindTexture(GL_TEXTURE_1D, texture_id);
    assert(glGetError() == GL_NO_ERROR);

    f->glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    f->glTexImage1D(GL_TEXTURE_1D,
                    0,
                    GL_RGB,
                    4,
                    0,
                    GL_RGB,
                    GL_UNSIGNED_BYTE,
                    tapas);

    assert(glGetError() == GL_NO_ERROR);

    // texture sampling/filtering operation.
    f->glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    assert(glGetError() == GL_NO_ERROR);
    f->glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    assert(glGetError() == GL_NO_ERROR);
    f->glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    assert(glGetError() == GL_NO_ERROR);
    f->glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    assert(glGetError() == GL_NO_ERROR);

    //Definiendo las estructuras para al colorbar con sus atributos usando glsl.
    //const char* attribute_name;
    attribute_name = "coord3d";
    attribute_coord3d = program->attributeLocation(attribute_name);
    if(attribute_coord3d == -1){
        fprintf(stderr, "Couldn't bind attribute %s\n", attribute_name);
        qApp->exit(-1);
    }

    attribute_name = "texcoord";
    attribute_texcoord = program->attributeLocation(attribute_name);
    if(attribute_texcoord == -1){
        fprintf(stderr, "Couldn't bind attribute %s\n", attribute_name);
        qApp->exit(-1);
    }

    //const char* uniform_name;
    uniform_name = "mvp";
    uniform_mvp = program->uniformLocation(uniform_name);
    if(uniform_mvp == -1){
        fprintf(stderr, "Couldn't bind uniform %s\n", uniform_name);
        qApp->exit(-1);
    }

    uniform_name = "mytexture";
    uniform_mytexture = program->uniformLocation(uniform_name);
    if(uniform_mytexture == -1){
        fprintf(stderr, "Couldn't bind uniform %s\n", uniform_name);
        qApp->exit(-1);
    }
    f->glBindTexture(GL_TEXTURE_1D,0);
    //f->glClearColor(1.0, 1.0, 1.0, 1.0);

    f->glActiveTexture(GL_TEXTURE0);

    f->glBindTexture(GL_TEXTURE_1D, texture_id);
    f->glUniform1i(uniform_mytexture, 0);

    f->glEnableVertexAttribArray(attribute_coord3d);
    f->glBindBuffer(GL_ARRAY_BUFFER, vbo_triangle_vertices);

    f->glVertexAttribPointer(
                attribute_coord3d,
                3,
                GL_FLOAT,
                GL_FALSE,
                0,0
                );

    f->glEnableVertexAttribArray(attribute_texcoord);
    f->glBindBuffer(GL_ARRAY_BUFFER, vbo_triangle_texcoords);

    f->glVertexAttribPointer(
                attribute_texcoord,
                1,
                GL_FLOAT,
                GL_FALSE,
                0,0
                );


    f->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_triangle_elements);
    //int size;
    f->glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
    f->glDrawElements(GL_TRIANGLES, size/sizeof(GLushort), GL_UNSIGNED_SHORT, 0);

    f->glDisableVertexAttribArray(attribute_coord3d);
    f->glDisableVertexAttribArray(attribute_texcoord);

    //Tapa inferior
    GLfloat* triangle_vertices_tapaInferior = new GLfloat[numSector*3*3];
    for(int i = 0; i < numSector; i++){
        if(i < (numSector - 1)){
            triangle_vertices_tapaInferior[9*i] = m_vertices[numSector*2*3 + 3];
            triangle_vertices_tapaInferior[(9*i) + 1] = m_vertices[(numSector*2*3 + 3) + 1];
            triangle_vertices_tapaInferior[(9*i) + 2] = m_vertices[(numSector*2*3 + 3) + 2];
            triangle_vertices_tapaInferior[(9*i) + 3] = m_vertices[3*(i + numSector + 1)];
            triangle_vertices_tapaInferior[(9*i) + 4] = m_vertices[3*(i + numSector + 1) + 1];
            triangle_vertices_tapaInferior[(9*i) + 5] = m_vertices[3*(i + numSector + 1) + 2];
            triangle_vertices_tapaInferior[(9*i) + 6] = m_vertices[3*(i + numSector)];
            triangle_vertices_tapaInferior[(9*i) + 7] = m_vertices[(3*(i + numSector)) + 1];
            triangle_vertices_tapaInferior[(9*i) + 8] = m_vertices[(3*(i + numSector)) + 2];
        }else{
            triangle_vertices_tapaInferior[9*i] = m_vertices[numSector*2*3 + 3];
            triangle_vertices_tapaInferior[(9*i) + 1] = m_vertices[(numSector*2*3 + 3) + 1];
            triangle_vertices_tapaInferior[(9*i) + 2] = m_vertices[(numSector*2*3 + 3) + 2];
            triangle_vertices_tapaInferior[(9*i) + 3] = m_vertices[(3*numSector)];
            triangle_vertices_tapaInferior[(9*i) + 4] = m_vertices[(3*numSector) + 1];
            triangle_vertices_tapaInferior[(9*i) + 5] = m_vertices[(3*numSector) + 2];
            triangle_vertices_tapaInferior[(9*i) + 6] = m_vertices[3*(i + numSector)];
            triangle_vertices_tapaInferior[(9*i) + 7] = m_vertices[(3*(i + numSector)) + 1];
            triangle_vertices_tapaInferior[(9*i) + 8] = m_vertices[(3*(i + numSector)) + 2];
        }
    }

    f->glGenBuffers(1, &vbo_triangle_vertices);
    f->glBindBuffer(GL_ARRAY_BUFFER, vbo_triangle_vertices);
    f->glBufferData(GL_ARRAY_BUFFER, numSector*3*3*sizeof(GLfloat), triangle_vertices_tapaInferior, GL_STATIC_DRAW);
    f->glBindBuffer(GL_ARRAY_BUFFER, 0);

    triangle_texcoords = new GLfloat[numSector*3];

    for(int i = 0; i < numSector; i++){
        triangle_texcoords[3*i] = 0.0;
        triangle_texcoords[3*i + 1] = 1.0;
        triangle_texcoords[3*i + 2] = 1.0;
    }

    f->glGenBuffers(1, &vbo_triangle_texcoords);
    f->glBindBuffer(GL_ARRAY_BUFFER, vbo_triangle_texcoords);
    f->glBufferData(GL_ARRAY_BUFFER, numSector*3*sizeof(GLfloat), triangle_texcoords, GL_STATIC_DRAW);

    triangle_elements = new GLushort[numSector*3];

    for(int i = 0; i < numSector*3; i++){
        triangle_elements[i] = i;
    }

    f->glGenBuffers(1, &ibo_triangle_elements);
    f->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_triangle_elements);
    f->glBufferData(GL_ELEMENT_ARRAY_BUFFER, numSector*3*sizeof(GLushort), triangle_elements, GL_STATIC_DRAW);

    f->glGenTextures(1, &texture_id);
    assert(glGetError() == GL_NO_ERROR);

    f->glBindTexture(GL_TEXTURE_1D, texture_id);
    assert(glGetError() == GL_NO_ERROR);

    f->glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    f->glTexImage1D(GL_TEXTURE_1D,
                    0,
                    GL_RGB,
                    4,
                    0,
                    GL_RGB,
                    GL_UNSIGNED_BYTE,
                    tapas);

    assert(glGetError() == GL_NO_ERROR);

    // texture sampling/filtering operation.
    f->glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    assert(glGetError() == GL_NO_ERROR);
    f->glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    assert(glGetError() == GL_NO_ERROR);
    f->glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    assert(glGetError() == GL_NO_ERROR);
    f->glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    assert(glGetError() == GL_NO_ERROR);

    //Definiendo las estructuras para al colorbar con sus atributos usando glsl.
    //const char* attribute_name;
    attribute_name = "coord3d";
    attribute_coord3d = program->attributeLocation(attribute_name);
    if(attribute_coord3d == -1){
        fprintf(stderr, "Couldn't bind attribute %s\n", attribute_name);
        qApp->exit(-1);
    }

    attribute_name = "texcoord";
    attribute_texcoord = program->attributeLocation(attribute_name);
    if(attribute_texcoord == -1){
        fprintf(stderr, "Couldn't bind attribute %s\n", attribute_name);
        qApp->exit(-1);
    }

    uniform_name = "mvp";
    uniform_mvp = program->uniformLocation(uniform_name);
    if(uniform_mvp == -1){
        fprintf(stderr, "Couldn't bind uniform %s\n", uniform_name);
        qApp->exit(-1);
    }

    uniform_name = "mytexture";
    uniform_mytexture = program->uniformLocation(uniform_name);
    if(uniform_mytexture == -1){
        fprintf(stderr, "Couldn't bind uniform %s\n", uniform_name);
        qApp->exit(-1);
    }
    f->glBindTexture(GL_TEXTURE_1D,0);

    f->glActiveTexture(GL_TEXTURE0);

    f->glBindTexture(GL_TEXTURE_1D, texture_id);
    f->glUniform1i(uniform_mytexture, 0);

    f->glEnableVertexAttribArray(attribute_coord3d);
    f->glBindBuffer(GL_ARRAY_BUFFER, vbo_triangle_vertices);

    f->glVertexAttribPointer(
                attribute_coord3d,
                3,
                GL_FLOAT,
                GL_FALSE,
                0,0
                );

    f->glEnableVertexAttribArray(attribute_texcoord);
    f->glBindBuffer(GL_ARRAY_BUFFER, vbo_triangle_texcoords);

    f->glVertexAttribPointer(
                attribute_texcoord,
                1,
                GL_FLOAT,
                GL_FALSE,
                0,0
                );


    f->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_triangle_elements);
    f->glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
    f->glDrawElements(GL_TRIANGLES, size/sizeof(GLushort), GL_UNSIGNED_SHORT, 0);

    f->glDisableVertexAttribArray(attribute_coord3d);
    f->glDisableVertexAttribArray(attribute_texcoord);

    //Segundo Cilindro
    triangle_vertices = new GLfloat[numSector*2*3*3];
    int valorInicialSegundoCilindro = 2*numSector*3 + 2*3;
    for(int i = 0; i < numSector; i++){
        if(i < (numSector - 1)){
            //Primer triangulo
            triangle_vertices[18*i] = m_vertices[3*i + valorInicialSegundoCilindro];
            triangle_vertices[(18*i) + 1] = m_vertices[(3*i) + valorInicialSegundoCilindro + 1];
            triangle_vertices[(18*i) + 2] = m_vertices[(3*i) + valorInicialSegundoCilindro + 2];
            triangle_vertices[(18*i) + 3] = m_vertices[3*(i + numSector + 1) + valorInicialSegundoCilindro];
            triangle_vertices[(18*i) + 4] = m_vertices[3*(i + numSector + 1) + valorInicialSegundoCilindro + 1];
            triangle_vertices[(18*i) + 5] = m_vertices[3*(i + numSector + 1) + valorInicialSegundoCilindro + 2];
            triangle_vertices[(18*i) + 6] = m_vertices[3*(i + numSector) + valorInicialSegundoCilindro];
            triangle_vertices[(18*i) + 7] = m_vertices[3*(i + numSector) + valorInicialSegundoCilindro + 1];
            triangle_vertices[(18*i) + 8] = m_vertices[3*(i + numSector) + valorInicialSegundoCilindro + 2];


            //Segundo Triangulo
            triangle_vertices[(18*i) + 9] = m_vertices[3*i + valorInicialSegundoCilindro];
            triangle_vertices[(18*i) + 10] = m_vertices[(3*i) + valorInicialSegundoCilindro + 1];
            triangle_vertices[(18*i) + 11] = m_vertices[(3*i) + valorInicialSegundoCilindro + 2];
            triangle_vertices[(18*i) + 12] = m_vertices[3*(i + 1) + valorInicialSegundoCilindro];
            triangle_vertices[(18*i) + 13] = m_vertices[3*(i + 1) + valorInicialSegundoCilindro + 1];
            triangle_vertices[(18*i) + 14] = m_vertices[3*(i + 1) + valorInicialSegundoCilindro + 2];
            triangle_vertices[(18*i) + 15] = m_vertices[3*(i + numSector + 1) + valorInicialSegundoCilindro];
            triangle_vertices[(18*i) + 16] = m_vertices[3*(i + numSector + 1) + valorInicialSegundoCilindro + 1];
            triangle_vertices[(18*i) + 17] = m_vertices[3*(i + numSector + 1) + valorInicialSegundoCilindro + 2];

        }else{
            //Primer triangulo extremo
            triangle_vertices[18*i] = m_vertices[3*i];
            triangle_vertices[(18*i) + 1] = m_vertices[(3*i) + valorInicialSegundoCilindro + 1];
            triangle_vertices[(18*i) + 2] = m_vertices[(3*i) + valorInicialSegundoCilindro + 2];
            triangle_vertices[(18*i) + 3] = m_vertices[3*(numSector) + valorInicialSegundoCilindro];
            triangle_vertices[(18*i) + 4] = m_vertices[3*(numSector) + valorInicialSegundoCilindro + 1];
            triangle_vertices[(18*i) + 5] = m_vertices[3*(numSector) + valorInicialSegundoCilindro + 2];
            triangle_vertices[(18*i) + 6] = m_vertices[3*(i + numSector) + valorInicialSegundoCilindro];
            triangle_vertices[(18*i) + 7] = m_vertices[3*(i + numSector) + valorInicialSegundoCilindro + 1];
            triangle_vertices[(18*i) + 8] = m_vertices[3*(i + numSector) + valorInicialSegundoCilindro + 2];


            //Segundo Triangulo extremo
            triangle_vertices[(18*i) + 9] = m_vertices[3*i + valorInicialSegundoCilindro];
            triangle_vertices[(18*i) + 10] = m_vertices[(3*i) + valorInicialSegundoCilindro + 1];
            triangle_vertices[(18*i) + 11] = m_vertices[(3*i) + valorInicialSegundoCilindro + 2];
            triangle_vertices[(18*i) + 12] = m_vertices[3*(0) + valorInicialSegundoCilindro];
            triangle_vertices[(18*i) + 13] = m_vertices[3*(0) + valorInicialSegundoCilindro + 1];
            triangle_vertices[(18*i) + 14] = m_vertices[3*(0) + valorInicialSegundoCilindro + 2];
            triangle_vertices[(18*i) + 15] = m_vertices[3*(numSector) + valorInicialSegundoCilindro];
            triangle_vertices[(18*i) + 16] = m_vertices[3*(numSector) + valorInicialSegundoCilindro + 1];
            triangle_vertices[(18*i) + 17] = m_vertices[3*(numSector) + valorInicialSegundoCilindro + 2];
        }
    }

    f->glGenBuffers(1, &vbo_triangle_vertices);
    f->glBindBuffer(GL_ARRAY_BUFFER, vbo_triangle_vertices);
    f->glBufferData(GL_ARRAY_BUFFER, numSector*2*3*3*sizeof(GLfloat), triangle_vertices, GL_STATIC_DRAW);
    f->glBindBuffer(GL_ARRAY_BUFFER, 0);

    triangle_texcoords = new GLfloat[numSector*2*3];

    for(int i = 0; i < numSector; i++){
        triangle_texcoords[6*i] = 0.0;
        triangle_texcoords[6*i + 1] = 1.0;
        triangle_texcoords[6*i + 2] = 1.0;
        triangle_texcoords[6*i + 3] = 0.0;
        triangle_texcoords[6*i + 4] = 0.0;
        triangle_texcoords[6*i + 5] = 1.0;
    }


    f->glGenBuffers(1, &vbo_triangle_texcoords);
    f->glBindBuffer(GL_ARRAY_BUFFER, vbo_triangle_texcoords);
    f->glBufferData(GL_ARRAY_BUFFER, numSector*2*3*sizeof(GLfloat), triangle_texcoords, GL_STATIC_DRAW);

    triangle_elements = new GLushort[numSector*2*3];

    for(int i = 0; i < numSector*2*3; i++){
        triangle_elements[i] = i;
    }

    f->glGenBuffers(1, &ibo_triangle_elements);
    f->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_triangle_elements);
    f->glBufferData(GL_ELEMENT_ARRAY_BUFFER, numSector*2*3*sizeof(GLushort), triangle_elements, GL_STATIC_DRAW);

    f->glGenTextures(1, &texture_id);
    assert(glGetError() == GL_NO_ERROR);

    f->glBindTexture(GL_TEXTURE_1D, texture_id);
    assert(glGetError() == GL_NO_ERROR);

    f->glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    f->glTexImage1D(GL_TEXTURE_1D,
                    0,
                    GL_RGB,
                    64,
                    0,
                    GL_RGB,
                    GL_UNSIGNED_BYTE,
                    mapcolor_Z);

    assert(glGetError() == GL_NO_ERROR);

    // texture sampling/filtering operation.
    f->glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    assert(glGetError() == GL_NO_ERROR);
    f->glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    assert(glGetError() == GL_NO_ERROR);
    f->glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    assert(glGetError() == GL_NO_ERROR);
    f->glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    assert(glGetError() == GL_NO_ERROR);

    //Definiendo las estructuras para al colorbar con sus atributos usando glsl.
    //const char* attribute_name;
    attribute_name = "coord3d";
    attribute_coord3d = program->attributeLocation(attribute_name);
    if(attribute_coord3d == -1){
        fprintf(stderr, "Couldn't bind attribute %s\n", attribute_name);
        qApp->exit(-1);
    }

    attribute_name = "texcoord";
    attribute_texcoord = program->attributeLocation(attribute_name);
    if(attribute_texcoord == -1){
        fprintf(stderr, "Couldn't bind attribute %s\n", attribute_name);
        qApp->exit(-1);
    }

    //const char* uniform_name;
    uniform_name = "mvp";
    uniform_mvp = program->uniformLocation(uniform_name);
    if(uniform_mvp == -1){
        fprintf(stderr, "Couldn't bind uniform %s\n", uniform_name);
        qApp->exit(-1);
    }

    uniform_name = "mytexture";
    uniform_mytexture = program->uniformLocation(uniform_name);
    if(uniform_mytexture == -1){
        fprintf(stderr, "Couldn't bind uniform %s\n", uniform_name);
        qApp->exit(-1);
    }
    f->glBindTexture(GL_TEXTURE_1D,0);
    //f->glClearColor(1.0, 1.0, 1.0, 1.0);

    f->glActiveTexture(GL_TEXTURE0);

    f->glBindTexture(GL_TEXTURE_1D, texture_id);
    f->glUniform1i(uniform_mytexture, 0);

    f->glEnableVertexAttribArray(attribute_coord3d);
    f->glBindBuffer(GL_ARRAY_BUFFER, vbo_triangle_vertices);

    f->glVertexAttribPointer(
                attribute_coord3d,
                3,
                GL_FLOAT,
                GL_FALSE,
                0,0
                );

    f->glEnableVertexAttribArray(attribute_texcoord);
    f->glBindBuffer(GL_ARRAY_BUFFER, vbo_triangle_texcoords);

    f->glVertexAttribPointer(
                attribute_texcoord,
                1,
                GL_FLOAT,
                GL_FALSE,
                0,0
                );


    f->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_triangle_elements);
    //int size;
    f->glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
    f->glDrawElements(GL_TRIANGLES, size/sizeof(GLushort), GL_UNSIGNED_SHORT, 0);

    f->glDisableVertexAttribArray(attribute_coord3d);
    f->glDisableVertexAttribArray(attribute_texcoord);

    //Tapas Superior
    triangle_vertices_tapaSuperior = new GLfloat[numSector*3*3];
    for(int i = 0; i < numSector; i++){
        if(i < (numSector - 1)){
            triangle_vertices_tapaSuperior[9*i] = m_vertices[numSector*2*3 + valorInicialSegundoCilindro];
            triangle_vertices_tapaSuperior[(9*i) + 1] = m_vertices[numSector*2*3 + valorInicialSegundoCilindro + 1];
            triangle_vertices_tapaSuperior[(9*i) + 2] = m_vertices[numSector*2*3  + valorInicialSegundoCilindro + 2];
            triangle_vertices_tapaSuperior[(9*i) + 3] = m_vertices[3*(i + 1) + valorInicialSegundoCilindro];
            triangle_vertices_tapaSuperior[(9*i) + 4] = m_vertices[(3*(i + 1)) + valorInicialSegundoCilindro + 1];
            triangle_vertices_tapaSuperior[(9*i) + 5] = m_vertices[(3*(i + 1)) + valorInicialSegundoCilindro + 2];
            triangle_vertices_tapaSuperior[(9*i) + 6] = m_vertices[3*i + valorInicialSegundoCilindro];
            triangle_vertices_tapaSuperior[(9*i) + 7] = m_vertices[(3*i) + valorInicialSegundoCilindro + 1];
            triangle_vertices_tapaSuperior[(9*i) + 8] = m_vertices[(3*i) + valorInicialSegundoCilindro + 2];
        }else{
            triangle_vertices_tapaSuperior[9*i] = m_vertices[numSector*2*3 + valorInicialSegundoCilindro];
            triangle_vertices_tapaSuperior[(9*i) + 1] = m_vertices[numSector*2*3 + valorInicialSegundoCilindro + 1];
            triangle_vertices_tapaSuperior[(9*i) + 2] = m_vertices[numSector*2*3 + valorInicialSegundoCilindro + 2];
            triangle_vertices_tapaSuperior[(9*i) + 3] = m_vertices[3*(0) + valorInicialSegundoCilindro];
            triangle_vertices_tapaSuperior[(9*i) + 4] = m_vertices[(3*(0)) + valorInicialSegundoCilindro + 1];
            triangle_vertices_tapaSuperior[(9*i) + 5] = m_vertices[(3*(0)) + valorInicialSegundoCilindro + 2];
            triangle_vertices_tapaSuperior[(9*i) + 6] = m_vertices[3*i + valorInicialSegundoCilindro];
            triangle_vertices_tapaSuperior[(9*i) + 7] = m_vertices[(3*i) + valorInicialSegundoCilindro + 1];
            triangle_vertices_tapaSuperior[(9*i) + 8] = m_vertices[(3*i) + valorInicialSegundoCilindro + 2];

        }
    }

    f->glGenBuffers(1, &vbo_triangle_vertices);
    f->glBindBuffer(GL_ARRAY_BUFFER, vbo_triangle_vertices);
    f->glBufferData(GL_ARRAY_BUFFER, numSector*3*3*sizeof(GLfloat), triangle_vertices_tapaSuperior, GL_STATIC_DRAW);
    f->glBindBuffer(GL_ARRAY_BUFFER, 0);

    triangle_texcoords = new GLfloat[numSector*3];

    for(int i = 0; i < numSector; i++){
        triangle_texcoords[3*i] = 0.0;
        triangle_texcoords[3*i + 1] = 1.0;
        triangle_texcoords[3*i + 2] = 1.0;
    }

    f->glGenBuffers(1, &vbo_triangle_texcoords);
    f->glBindBuffer(GL_ARRAY_BUFFER, vbo_triangle_texcoords);
    f->glBufferData(GL_ARRAY_BUFFER, numSector*3*sizeof(GLfloat), triangle_texcoords, GL_STATIC_DRAW);

    triangle_elements = new GLushort[numSector*3];

    for(int i = 0; i < numSector*3; i++){
        triangle_elements[i] = i;
    }

    f->glGenBuffers(1, &ibo_triangle_elements);
    f->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_triangle_elements);
    f->glBufferData(GL_ELEMENT_ARRAY_BUFFER, numSector*3*sizeof(GLushort), triangle_elements, GL_STATIC_DRAW);

    f->glGenTextures(1, &texture_id);
    assert(glGetError() == GL_NO_ERROR);

    f->glBindTexture(GL_TEXTURE_1D, texture_id);
    assert(glGetError() == GL_NO_ERROR);

    f->glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    f->glTexImage1D(GL_TEXTURE_1D,
                    0,
                    GL_RGB,
                    4,
                    0,
                    GL_RGB,
                    GL_UNSIGNED_BYTE,
                    tapas);

    assert(glGetError() == GL_NO_ERROR);

    // texture sampling/filtering operation.
    f->glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    assert(glGetError() == GL_NO_ERROR);
    f->glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    assert(glGetError() == GL_NO_ERROR);
    f->glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    assert(glGetError() == GL_NO_ERROR);
    f->glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    assert(glGetError() == GL_NO_ERROR);

    //Definiendo las estructuras para al colorbar con sus atributos usando glsl.
    //const char* attribute_name;
    attribute_name = "coord3d";
    attribute_coord3d = program->attributeLocation(attribute_name);
    if(attribute_coord3d == -1){
        fprintf(stderr, "Couldn't bind attribute %s\n", attribute_name);
        qApp->exit(-1);
    }

    attribute_name = "texcoord";
    attribute_texcoord = program->attributeLocation(attribute_name);
    if(attribute_texcoord == -1){
        fprintf(stderr, "Couldn't bind attribute %s\n", attribute_name);
        qApp->exit(-1);
    }

    //const char* uniform_name;
    uniform_name = "mvp";
    uniform_mvp = program->uniformLocation(uniform_name);
    if(uniform_mvp == -1){
        fprintf(stderr, "Couldn't bind uniform %s\n", uniform_name);
        qApp->exit(-1);
    }

    uniform_name = "mytexture";
    uniform_mytexture = program->uniformLocation(uniform_name);
    if(uniform_mytexture == -1){
        fprintf(stderr, "Couldn't bind uniform %s\n", uniform_name);
        qApp->exit(-1);
    }
    f->glBindTexture(GL_TEXTURE_1D,0);
    //f->glClearColor(1.0, 1.0, 1.0, 1.0);

    f->glActiveTexture(GL_TEXTURE0);

    f->glBindTexture(GL_TEXTURE_1D, texture_id);
    f->glUniform1i(uniform_mytexture, 0);

    f->glEnableVertexAttribArray(attribute_coord3d);
    f->glBindBuffer(GL_ARRAY_BUFFER, vbo_triangle_vertices);

    f->glVertexAttribPointer(
                attribute_coord3d,
                3,
                GL_FLOAT,
                GL_FALSE,
                0,0
                );

    f->glEnableVertexAttribArray(attribute_texcoord);
    f->glBindBuffer(GL_ARRAY_BUFFER, vbo_triangle_texcoords);

    f->glVertexAttribPointer(
                attribute_texcoord,
                1,
                GL_FLOAT,
                GL_FALSE,
                0,0
                );


    f->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_triangle_elements);
    //int size;
    f->glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
    f->glDrawElements(GL_TRIANGLES, size/sizeof(GLushort), GL_UNSIGNED_SHORT, 0);

    f->glDisableVertexAttribArray(attribute_coord3d);
    f->glDisableVertexAttribArray(attribute_texcoord);


    //Tapa inferior
    triangle_vertices_tapaInferior = new GLfloat[numSector*3*3];
    for(int i = 0; i < numSector; i++){
        if(i < (numSector - 1)){
            triangle_vertices_tapaInferior[9*i] = m_vertices[numSector*2*3 + valorInicialSegundoCilindro + 3];
            triangle_vertices_tapaInferior[(9*i) + 1] = m_vertices[(numSector*2*3 + 3) + valorInicialSegundoCilindro + 1];
            triangle_vertices_tapaInferior[(9*i) + 2] = m_vertices[(numSector*2*3 + 3) + valorInicialSegundoCilindro + 2];
            triangle_vertices_tapaInferior[(9*i) + 3] = m_vertices[3*(i + numSector) + valorInicialSegundoCilindro];
            triangle_vertices_tapaInferior[(9*i) + 4] = m_vertices[(3*(i + numSector)) + valorInicialSegundoCilindro + 1];
            triangle_vertices_tapaInferior[(9*i) + 5] = m_vertices[(3*(i + numSector)) + valorInicialSegundoCilindro + 2];
            triangle_vertices_tapaInferior[(9*i) + 6] = m_vertices[3*(i + numSector + 1) + valorInicialSegundoCilindro];
            triangle_vertices_tapaInferior[(9*i) + 7] = m_vertices[3*(i + numSector + 1) + valorInicialSegundoCilindro + 1];
            triangle_vertices_tapaInferior[(9*i) + 8] = m_vertices[3*(i + numSector + 1) + valorInicialSegundoCilindro + 2];

        }else{
            triangle_vertices_tapaInferior[9*i] = m_vertices[numSector*2*3 + valorInicialSegundoCilindro + 3];
            triangle_vertices_tapaInferior[(9*i) + 1] = m_vertices[(numSector*2*3 + 3) + valorInicialSegundoCilindro + 1];
            triangle_vertices_tapaInferior[(9*i) + 2] = m_vertices[(numSector*2*3 + 3) + valorInicialSegundoCilindro + 2];
            triangle_vertices_tapaInferior[(9*i) + 3] = m_vertices[3*(i + numSector) + valorInicialSegundoCilindro];
            triangle_vertices_tapaInferior[(9*i) + 4] = m_vertices[(3*(i + numSector)) + valorInicialSegundoCilindro + 1];
            triangle_vertices_tapaInferior[(9*i) + 5] = m_vertices[(3*(i + numSector)) + valorInicialSegundoCilindro + 2];
            triangle_vertices_tapaInferior[(9*i) + 6] = m_vertices[(3*numSector) + valorInicialSegundoCilindro];
            triangle_vertices_tapaInferior[(9*i) + 7] = m_vertices[(3*numSector) + valorInicialSegundoCilindro + 1];
            triangle_vertices_tapaInferior[(9*i) + 8] = m_vertices[(3*numSector) + valorInicialSegundoCilindro + 2];
        }
    }

    f->glGenBuffers(1, &vbo_triangle_vertices);
    f->glBindBuffer(GL_ARRAY_BUFFER, vbo_triangle_vertices);
    f->glBufferData(GL_ARRAY_BUFFER, numSector*3*3*sizeof(GLfloat), triangle_vertices_tapaInferior, GL_STATIC_DRAW);
    f->glBindBuffer(GL_ARRAY_BUFFER, 0);

    triangle_texcoords = new GLfloat[numSector*3];

    for(int i = 0; i < numSector; i++){
        triangle_texcoords[3*i] = 0.0;
        triangle_texcoords[3*i + 1] = 1.0;
        triangle_texcoords[3*i + 2] = 1.0;
    }

    f->glGenBuffers(1, &vbo_triangle_texcoords);
    f->glBindBuffer(GL_ARRAY_BUFFER, vbo_triangle_texcoords);
    f->glBufferData(GL_ARRAY_BUFFER, numSector*3*sizeof(GLfloat), triangle_texcoords, GL_STATIC_DRAW);

    triangle_elements = new GLushort[numSector*3];

    for(int i = 0; i < numSector*3; i++){
        triangle_elements[i] = i;
    }

    f->glGenBuffers(1, &ibo_triangle_elements);
    f->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_triangle_elements);
    f->glBufferData(GL_ELEMENT_ARRAY_BUFFER, numSector*3*sizeof(GLushort), triangle_elements, GL_STATIC_DRAW);

    f->glGenTextures(1, &texture_id);
    assert(glGetError() == GL_NO_ERROR);

    f->glBindTexture(GL_TEXTURE_1D, texture_id);
    assert(glGetError() == GL_NO_ERROR);

    f->glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    f->glTexImage1D(GL_TEXTURE_1D,
                    0,
                    GL_RGB,
                    4,
                    0,
                    GL_RGB,
                    GL_UNSIGNED_BYTE,
                    tapas);

    assert(glGetError() == GL_NO_ERROR);

    // texture sampling/filtering operation.
    f->glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    assert(glGetError() == GL_NO_ERROR);
    f->glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    assert(glGetError() == GL_NO_ERROR);
    f->glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    assert(glGetError() == GL_NO_ERROR);
    f->glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    assert(glGetError() == GL_NO_ERROR);

    //Definiendo las estructuras para al colorbar con sus atributos usando glsl.
    //const char* attribute_name;
    attribute_name = "coord3d";
    attribute_coord3d = program->attributeLocation(attribute_name);
    if(attribute_coord3d == -1){
        fprintf(stderr, "Couldn't bind attribute %s\n", attribute_name);
        qApp->exit(-1);
    }

    attribute_name = "texcoord";
    attribute_texcoord = program->attributeLocation(attribute_name);
    if(attribute_texcoord == -1){
        fprintf(stderr, "Couldn't bind attribute %s\n", attribute_name);
        qApp->exit(-1);
    }

    uniform_name = "mvp";
    uniform_mvp = program->uniformLocation(uniform_name);
    if(uniform_mvp == -1){
        fprintf(stderr, "Couldn't bind uniform %s\n", uniform_name);
        qApp->exit(-1);
    }

    uniform_name = "mytexture";
    uniform_mytexture = program->uniformLocation(uniform_name);
    if(uniform_mytexture == -1){
        fprintf(stderr, "Couldn't bind uniform %s\n", uniform_name);
        qApp->exit(-1);
    }
    f->glBindTexture(GL_TEXTURE_1D,0);

    f->glActiveTexture(GL_TEXTURE0);

    f->glBindTexture(GL_TEXTURE_1D, texture_id);
    f->glUniform1i(uniform_mytexture, 0);

    f->glEnableVertexAttribArray(attribute_coord3d);
    f->glBindBuffer(GL_ARRAY_BUFFER, vbo_triangle_vertices);

    f->glVertexAttribPointer(
                attribute_coord3d,
                3,
                GL_FLOAT,
                GL_FALSE,
                0,0
                );

    f->glEnableVertexAttribArray(attribute_texcoord);
    f->glBindBuffer(GL_ARRAY_BUFFER, vbo_triangle_texcoords);

    f->glVertexAttribPointer(
                attribute_texcoord,
                1,
                GL_FLOAT,
                GL_FALSE,
                0,0
                );


    f->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_triangle_elements);
    f->glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
    f->glDrawElements(GL_TRIANGLES, size/sizeof(GLushort), GL_UNSIGNED_SHORT, 0);

    f->glDisableVertexAttribArray(attribute_coord3d);
    f->glDisableVertexAttribArray(attribute_texcoord);

    //Tercer Cilindro
    triangle_vertices = new GLfloat[numSector*2*3*3];
    int valorInicialTercerCilindro = 4*numSector*3 + 4*3;
    for(int i = 0; i < numSector; i++){
        if(i < (numSector - 1)){
            //Primer triangulo
            triangle_vertices[18*i] = m_vertices[3*i + valorInicialTercerCilindro];
            triangle_vertices[(18*i) + 1] = m_vertices[(3*i) + valorInicialTercerCilindro + 1];
            triangle_vertices[(18*i) + 2] = m_vertices[(3*i) + valorInicialTercerCilindro + 2];
            triangle_vertices[(18*i) + 3] = m_vertices[3*(i + numSector) + valorInicialTercerCilindro];
            triangle_vertices[(18*i) + 4] = m_vertices[3*(i + numSector) + valorInicialTercerCilindro + 1];
            triangle_vertices[(18*i) + 5] = m_vertices[3*(i + numSector) + valorInicialTercerCilindro + 2];
            triangle_vertices[(18*i) + 6] = m_vertices[3*(i + numSector + 1) + valorInicialTercerCilindro];
            triangle_vertices[(18*i) + 7] = m_vertices[3*(i + numSector + 1) + valorInicialTercerCilindro + 1];
            triangle_vertices[(18*i) + 8] = m_vertices[3*(i + numSector + 1) + valorInicialTercerCilindro + 2];

            //Segundo Triangulo
            triangle_vertices[(18*i) + 9] = m_vertices[3*i + valorInicialTercerCilindro];
            triangle_vertices[(18*i) + 10] = m_vertices[(3*i) + valorInicialTercerCilindro + 1];
            triangle_vertices[(18*i) + 11] = m_vertices[(3*i) + valorInicialTercerCilindro + 2];
            triangle_vertices[(18*i) + 12] = m_vertices[3*(i + numSector + 1) + valorInicialTercerCilindro];
            triangle_vertices[(18*i) + 13] = m_vertices[3*(i + numSector + 1) + valorInicialTercerCilindro + 1];
            triangle_vertices[(18*i) + 14] = m_vertices[3*(i + numSector + 1) + valorInicialTercerCilindro + 2];
            triangle_vertices[(18*i) + 15] = m_vertices[3*(i + 1) + valorInicialTercerCilindro];
            triangle_vertices[(18*i) + 16] = m_vertices[3*(i + 1) + valorInicialTercerCilindro + 1];
            triangle_vertices[(18*i) + 17] = m_vertices[3*(i + 1) + valorInicialTercerCilindro + 2];
        }else{
            //Primer triangulo extremo
            triangle_vertices[18*i] = m_vertices[3*i + valorInicialTercerCilindro];
            triangle_vertices[(18*i) + 1] = m_vertices[(3*i) + valorInicialTercerCilindro + 1];
            triangle_vertices[(18*i) + 2] = m_vertices[(3*i) + valorInicialTercerCilindro + 2];
            triangle_vertices[(18*i) + 3] = m_vertices[3*(i + numSector) + valorInicialTercerCilindro];
            triangle_vertices[(18*i) + 4] = m_vertices[3*(i + numSector) + valorInicialTercerCilindro + 1];
            triangle_vertices[(18*i) + 5] = m_vertices[3*(i + numSector) + valorInicialTercerCilindro + 2];
            triangle_vertices[(18*i) + 6] = m_vertices[3*(numSector) + valorInicialTercerCilindro];
            triangle_vertices[(18*i) + 7] = m_vertices[3*(numSector) + valorInicialTercerCilindro + 1];
            triangle_vertices[(18*i) + 8] = m_vertices[3*(numSector) + valorInicialTercerCilindro + 2];

            //Segundo Triangulo extremo
            triangle_vertices[(18*i) + 9] = m_vertices[3*i + valorInicialTercerCilindro];
            triangle_vertices[(18*i) + 10] = m_vertices[(3*i) + valorInicialTercerCilindro + 1];
            triangle_vertices[(18*i) + 11] = m_vertices[(3*i) + valorInicialTercerCilindro + 2];
            triangle_vertices[(18*i) + 12] = m_vertices[3*(numSector) + valorInicialTercerCilindro];
            triangle_vertices[(18*i) + 13] = m_vertices[3*(numSector) + valorInicialTercerCilindro + 1];
            triangle_vertices[(18*i) + 14] = m_vertices[3*(numSector) + valorInicialTercerCilindro + 2];
            triangle_vertices[(18*i) + 15] = m_vertices[3*(0) + valorInicialTercerCilindro];
            triangle_vertices[(18*i) + 16] = m_vertices[3*(0) + valorInicialTercerCilindro + 1];
            triangle_vertices[(18*i) + 17] = m_vertices[3*(0) + valorInicialTercerCilindro + 2];
        }
    }

    f->glGenBuffers(1, &vbo_triangle_vertices);
    f->glBindBuffer(GL_ARRAY_BUFFER, vbo_triangle_vertices);
    f->glBufferData(GL_ARRAY_BUFFER, numSector*2*3*3*sizeof(GLfloat), triangle_vertices, GL_STATIC_DRAW);
    f->glBindBuffer(GL_ARRAY_BUFFER, 0);

    triangle_texcoords = new GLfloat[numSector*2*3];

    for(int i = 0; i < numSector; i++){
        triangle_texcoords[6*i] = 0.0;
        triangle_texcoords[6*i + 1] = 1.0;
        triangle_texcoords[6*i + 2] = 1.0;
        triangle_texcoords[6*i + 3] = 0.0;
        triangle_texcoords[6*i + 4] = 1.0;
        triangle_texcoords[6*i + 5] = 0.0;
    }


    f->glGenBuffers(1, &vbo_triangle_texcoords);
    f->glBindBuffer(GL_ARRAY_BUFFER, vbo_triangle_texcoords);
    f->glBufferData(GL_ARRAY_BUFFER, numSector*2*3*sizeof(GLfloat), triangle_texcoords, GL_STATIC_DRAW);

    triangle_elements = new GLushort[numSector*2*3];

    for(int i = 0; i < numSector*2*3; i++){
        triangle_elements[i] = i;
    }

    f->glGenBuffers(1, &ibo_triangle_elements);
    f->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_triangle_elements);
    f->glBufferData(GL_ELEMENT_ARRAY_BUFFER, numSector*2*3*sizeof(GLushort), triangle_elements, GL_STATIC_DRAW);

    f->glGenTextures(1, &texture_id);
    assert(glGetError() == GL_NO_ERROR);

    f->glBindTexture(GL_TEXTURE_1D, texture_id);
    assert(glGetError() == GL_NO_ERROR);

    f->glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    f->glTexImage1D(GL_TEXTURE_1D,
                    0,
                    GL_RGB,
                    64,
                    0,
                    GL_RGB,
                    GL_UNSIGNED_BYTE,
                    mapcolor_X);

    assert(glGetError() == GL_NO_ERROR);

    // texture sampling/filtering operation.
    f->glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    assert(glGetError() == GL_NO_ERROR);
    f->glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    assert(glGetError() == GL_NO_ERROR);
    f->glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    assert(glGetError() == GL_NO_ERROR);
    f->glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    assert(glGetError() == GL_NO_ERROR);

    //Definiendo las estructuras para al colorbar con sus atributos usando glsl.
    //const char* attribute_name;
    attribute_name = "coord3d";
    attribute_coord3d = program->attributeLocation(attribute_name);
    if(attribute_coord3d == -1){
        fprintf(stderr, "Couldn't bind attribute %s\n", attribute_name);
        qApp->exit(-1);
    }

    attribute_name = "texcoord";
    attribute_texcoord = program->attributeLocation(attribute_name);
    if(attribute_texcoord == -1){
        fprintf(stderr, "Couldn't bind attribute %s\n", attribute_name);
        qApp->exit(-1);
    }

    //const char* uniform_name;
    uniform_name = "mvp";
    uniform_mvp = program->uniformLocation(uniform_name);
    if(uniform_mvp == -1){
        fprintf(stderr, "Couldn't bind uniform %s\n", uniform_name);
        qApp->exit(-1);
    }

    uniform_name = "mytexture";
    uniform_mytexture = program->uniformLocation(uniform_name);
    if(uniform_mytexture == -1){
        fprintf(stderr, "Couldn't bind uniform %s\n", uniform_name);
        qApp->exit(-1);
    }
    f->glBindTexture(GL_TEXTURE_1D,0);
    //f->glClearColor(1.0, 1.0, 1.0, 1.0);

    f->glActiveTexture(GL_TEXTURE0);

    f->glBindTexture(GL_TEXTURE_1D, texture_id);
    f->glUniform1i(uniform_mytexture, 0);

    f->glEnableVertexAttribArray(attribute_coord3d);
    f->glBindBuffer(GL_ARRAY_BUFFER, vbo_triangle_vertices);

    f->glVertexAttribPointer(
                attribute_coord3d,
                3,
                GL_FLOAT,
                GL_FALSE,
                0,0
                );

    f->glEnableVertexAttribArray(attribute_texcoord);
    f->glBindBuffer(GL_ARRAY_BUFFER, vbo_triangle_texcoords);

    f->glVertexAttribPointer(
                attribute_texcoord,
                1,
                GL_FLOAT,
                GL_FALSE,
                0,0
                );


    f->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_triangle_elements);
    //int size;
    f->glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
    f->glDrawElements(GL_TRIANGLES, size/sizeof(GLushort), GL_UNSIGNED_SHORT, 0);

    f->glDisableVertexAttribArray(attribute_coord3d);
    f->glDisableVertexAttribArray(attribute_texcoord);

    //Tapa superior
    triangle_vertices_tapaSuperior = new GLfloat[numSector*3*3];
    for(int i = 0; i < numSector; i++){
        if(i < (numSector - 1)){
            triangle_vertices_tapaSuperior[9*i] = m_vertices[numSector*2*3 + valorInicialTercerCilindro];
            triangle_vertices_tapaSuperior[(9*i) + 1] = m_vertices[numSector*2*3 + valorInicialTercerCilindro + 1];
            triangle_vertices_tapaSuperior[(9*i) + 2] = m_vertices[numSector*2*3 + valorInicialTercerCilindro + 2];
            triangle_vertices_tapaSuperior[(9*i) + 3] = m_vertices[3*i + valorInicialTercerCilindro];
            triangle_vertices_tapaSuperior[(9*i) + 4] = m_vertices[(3*i) + valorInicialTercerCilindro + 1];
            triangle_vertices_tapaSuperior[(9*i) + 5] = m_vertices[(3*i) + valorInicialTercerCilindro + 2];
            triangle_vertices_tapaSuperior[(9*i) + 6] = m_vertices[3*(i + 1) + valorInicialTercerCilindro];
            triangle_vertices_tapaSuperior[(9*i) + 7] = m_vertices[(3*(i + 1)) + valorInicialTercerCilindro + 1];
            triangle_vertices_tapaSuperior[(9*i) + 8] = m_vertices[(3*(i + 1)) + valorInicialTercerCilindro + 2];
        }else{
            triangle_vertices_tapaSuperior[9*i] = m_vertices[numSector*2*3 + valorInicialTercerCilindro];
            triangle_vertices_tapaSuperior[(9*i) + 1] = m_vertices[numSector*2*3 + valorInicialTercerCilindro + 1];
            triangle_vertices_tapaSuperior[(9*i) + 2] = m_vertices[numSector*2*3 + valorInicialTercerCilindro + 2];
            triangle_vertices_tapaSuperior[(9*i) + 3] = m_vertices[3*i + valorInicialTercerCilindro];
            triangle_vertices_tapaSuperior[(9*i) + 4] = m_vertices[(3*i) + valorInicialTercerCilindro + 1];
            triangle_vertices_tapaSuperior[(9*i) + 5] = m_vertices[(3*i) + valorInicialTercerCilindro + 2];
            triangle_vertices_tapaSuperior[(9*i) + 6] = m_vertices[3*(0) + valorInicialTercerCilindro];
            triangle_vertices_tapaSuperior[(9*i) + 7] = m_vertices[(3*(0)) + valorInicialTercerCilindro + 1];
            triangle_vertices_tapaSuperior[(9*i) + 8] = m_vertices[(3*(0)) + valorInicialTercerCilindro + 2];
        }
    }

    f->glGenBuffers(1, &vbo_triangle_vertices);
    f->glBindBuffer(GL_ARRAY_BUFFER, vbo_triangle_vertices);
    f->glBufferData(GL_ARRAY_BUFFER, numSector*3*3*sizeof(GLfloat), triangle_vertices_tapaSuperior, GL_STATIC_DRAW);
    f->glBindBuffer(GL_ARRAY_BUFFER, 0);

    triangle_texcoords = new GLfloat[numSector*3];

    for(int i = 0; i < numSector; i++){
        triangle_texcoords[3*i] = 0.0;
        triangle_texcoords[3*i + 1] = 1.0;
        triangle_texcoords[3*i + 2] = 1.0;
    }

    f->glGenBuffers(1, &vbo_triangle_texcoords);
    f->glBindBuffer(GL_ARRAY_BUFFER, vbo_triangle_texcoords);
    f->glBufferData(GL_ARRAY_BUFFER, numSector*3*sizeof(GLfloat), triangle_texcoords, GL_STATIC_DRAW);

    triangle_elements = new GLushort[numSector*3];

    for(int i = 0; i < numSector*3; i++){
        triangle_elements[i] = i;
    }

    f->glGenBuffers(1, &ibo_triangle_elements);
    f->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_triangle_elements);
    f->glBufferData(GL_ELEMENT_ARRAY_BUFFER, numSector*3*sizeof(GLushort), triangle_elements, GL_STATIC_DRAW);

    f->glGenTextures(1, &texture_id);
    assert(glGetError() == GL_NO_ERROR);

    f->glBindTexture(GL_TEXTURE_1D, texture_id);
    assert(glGetError() == GL_NO_ERROR);

    f->glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    f->glTexImage1D(GL_TEXTURE_1D,
                    0,
                    GL_RGB,
                    4,
                    0,
                    GL_RGB,
                    GL_UNSIGNED_BYTE,
                    tapas);

    assert(glGetError() == GL_NO_ERROR);

    // texture sampling/filtering operation.
    f->glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    assert(glGetError() == GL_NO_ERROR);
    f->glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    assert(glGetError() == GL_NO_ERROR);
    f->glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    assert(glGetError() == GL_NO_ERROR);
    f->glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    assert(glGetError() == GL_NO_ERROR);

    //Definiendo las estructuras para al colorbar con sus atributos usando glsl.
    //const char* attribute_name;
    attribute_name = "coord3d";
    attribute_coord3d = program->attributeLocation(attribute_name);
    if(attribute_coord3d == -1){
        fprintf(stderr, "Couldn't bind attribute %s\n", attribute_name);
        qApp->exit(-1);
    }

    attribute_name = "texcoord";
    attribute_texcoord = program->attributeLocation(attribute_name);
    if(attribute_texcoord == -1){
        fprintf(stderr, "Couldn't bind attribute %s\n", attribute_name);
        qApp->exit(-1);
    }

    //const char* uniform_name;
    uniform_name = "mvp";
    uniform_mvp = program->uniformLocation(uniform_name);
    if(uniform_mvp == -1){
        fprintf(stderr, "Couldn't bind uniform %s\n", uniform_name);
        qApp->exit(-1);
    }

    uniform_name = "mytexture";
    uniform_mytexture = program->uniformLocation(uniform_name);
    if(uniform_mytexture == -1){
        fprintf(stderr, "Couldn't bind uniform %s\n", uniform_name);
        qApp->exit(-1);
    }
    f->glBindTexture(GL_TEXTURE_1D,0);
    //f->glClearColor(1.0, 1.0, 1.0, 1.0);

    f->glActiveTexture(GL_TEXTURE0);

    f->glBindTexture(GL_TEXTURE_1D, texture_id);
    f->glUniform1i(uniform_mytexture, 0);

    f->glEnableVertexAttribArray(attribute_coord3d);
    f->glBindBuffer(GL_ARRAY_BUFFER, vbo_triangle_vertices);

    f->glVertexAttribPointer(
                attribute_coord3d,
                3,
                GL_FLOAT,
                GL_FALSE,
                0,0
                );

    f->glEnableVertexAttribArray(attribute_texcoord);
    f->glBindBuffer(GL_ARRAY_BUFFER, vbo_triangle_texcoords);

    f->glVertexAttribPointer(
                attribute_texcoord,
                1,
                GL_FLOAT,
                GL_FALSE,
                0,0
                );


    f->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_triangle_elements);
    //int size;
    f->glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
    f->glDrawElements(GL_TRIANGLES, size/sizeof(GLushort), GL_UNSIGNED_SHORT, 0);

    f->glDisableVertexAttribArray(attribute_coord3d);
    f->glDisableVertexAttribArray(attribute_texcoord);

    //Tapa inferior
    triangle_vertices_tapaInferior = new GLfloat[numSector*3*3];
    for(int i = 0; i < numSector; i++){
        if(i < (numSector - 1)){
            triangle_vertices_tapaInferior[9*i] = m_vertices[numSector*2*3 + valorInicialTercerCilindro + 3];
            triangle_vertices_tapaInferior[(9*i) + 1] = m_vertices[(numSector*2*3 + 3) + valorInicialTercerCilindro + 1];
            triangle_vertices_tapaInferior[(9*i) + 2] = m_vertices[(numSector*2*3 + 3) + valorInicialTercerCilindro + 2];
            triangle_vertices_tapaInferior[(9*i) + 3] = m_vertices[3*(i + numSector + 1) + valorInicialTercerCilindro];
            triangle_vertices_tapaInferior[(9*i) + 4] = m_vertices[3*(i + numSector + 1) + valorInicialTercerCilindro + 1];
            triangle_vertices_tapaInferior[(9*i) + 5] = m_vertices[3*(i + numSector + 1) + valorInicialTercerCilindro + 2];
            triangle_vertices_tapaInferior[(9*i) + 6] = m_vertices[3*(i + numSector) + valorInicialTercerCilindro];
            triangle_vertices_tapaInferior[(9*i) + 7] = m_vertices[(3*(i + numSector)) + valorInicialTercerCilindro + 1];
            triangle_vertices_tapaInferior[(9*i) + 8] = m_vertices[(3*(i + numSector)) + valorInicialTercerCilindro + 2];
        }else{
            triangle_vertices_tapaInferior[9*i] = m_vertices[numSector*2*3 + valorInicialTercerCilindro + 3];
            triangle_vertices_tapaInferior[(9*i) + 1] = m_vertices[(numSector*2*3 + 3) + valorInicialTercerCilindro + 1];
            triangle_vertices_tapaInferior[(9*i) + 2] = m_vertices[(numSector*2*3 + 3) + valorInicialTercerCilindro + 2];
            triangle_vertices_tapaInferior[(9*i) + 3] = m_vertices[(3*numSector) + valorInicialTercerCilindro];
            triangle_vertices_tapaInferior[(9*i) + 4] = m_vertices[(3*numSector) + valorInicialTercerCilindro + 1];
            triangle_vertices_tapaInferior[(9*i) + 5] = m_vertices[(3*numSector) + valorInicialTercerCilindro + 2];
            triangle_vertices_tapaInferior[(9*i) + 6] = m_vertices[3*(i + numSector) + valorInicialTercerCilindro];
            triangle_vertices_tapaInferior[(9*i) + 7] = m_vertices[(3*(i + numSector)) + valorInicialTercerCilindro + 1];
            triangle_vertices_tapaInferior[(9*i) + 8] = m_vertices[(3*(i + numSector)) + valorInicialTercerCilindro + 2];
        }
    }

    f->glGenBuffers(1, &vbo_triangle_vertices);
    f->glBindBuffer(GL_ARRAY_BUFFER, vbo_triangle_vertices);
    f->glBufferData(GL_ARRAY_BUFFER, numSector*3*3*sizeof(GLfloat), triangle_vertices_tapaInferior, GL_STATIC_DRAW);
    f->glBindBuffer(GL_ARRAY_BUFFER, 0);

    triangle_texcoords = new GLfloat[numSector*3];

    for(int i = 0; i < numSector; i++){
        triangle_texcoords[3*i] = 0.0;
        triangle_texcoords[3*i + 1] = 1.0;
        triangle_texcoords[3*i + 2] = 1.0;
    }

    f->glGenBuffers(1, &vbo_triangle_texcoords);
    f->glBindBuffer(GL_ARRAY_BUFFER, vbo_triangle_texcoords);
    f->glBufferData(GL_ARRAY_BUFFER, numSector*3*sizeof(GLfloat), triangle_texcoords, GL_STATIC_DRAW);

    triangle_elements = new GLushort[numSector*3];

    for(int i = 0; i < numSector*3; i++){
        triangle_elements[i] = i;
    }

    f->glGenBuffers(1, &ibo_triangle_elements);
    f->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_triangle_elements);
    f->glBufferData(GL_ELEMENT_ARRAY_BUFFER, numSector*3*sizeof(GLushort), triangle_elements, GL_STATIC_DRAW);

    f->glGenTextures(1, &texture_id);
    assert(glGetError() == GL_NO_ERROR);

    f->glBindTexture(GL_TEXTURE_1D, texture_id);
    assert(glGetError() == GL_NO_ERROR);

    f->glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    f->glTexImage1D(GL_TEXTURE_1D,
                    0,
                    GL_RGB,
                    4,
                    0,
                    GL_RGB,
                    GL_UNSIGNED_BYTE,
                    tapas);

    assert(glGetError() == GL_NO_ERROR);

    // texture sampling/filtering operation.
    f->glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    assert(glGetError() == GL_NO_ERROR);
    f->glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    assert(glGetError() == GL_NO_ERROR);
    f->glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    assert(glGetError() == GL_NO_ERROR);
    f->glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    assert(glGetError() == GL_NO_ERROR);

    //Definiendo las estructuras para al colorbar con sus atributos usando glsl.
    //const char* attribute_name;
    attribute_name = "coord3d";
    attribute_coord3d = program->attributeLocation(attribute_name);
    if(attribute_coord3d == -1){
        fprintf(stderr, "Couldn't bind attribute %s\n", attribute_name);
        qApp->exit(-1);
    }

    attribute_name = "texcoord";
    attribute_texcoord = program->attributeLocation(attribute_name);
    if(attribute_texcoord == -1){
        fprintf(stderr, "Couldn't bind attribute %s\n", attribute_name);
        qApp->exit(-1);
    }

    uniform_name = "mvp";
    uniform_mvp = program->uniformLocation(uniform_name);
    if(uniform_mvp == -1){
        fprintf(stderr, "Couldn't bind uniform %s\n", uniform_name);
        qApp->exit(-1);
    }

    uniform_name = "mytexture";
    uniform_mytexture = program->uniformLocation(uniform_name);
    if(uniform_mytexture == -1){
        fprintf(stderr, "Couldn't bind uniform %s\n", uniform_name);
        qApp->exit(-1);
    }
    f->glBindTexture(GL_TEXTURE_1D,0);

    f->glActiveTexture(GL_TEXTURE0);

    f->glBindTexture(GL_TEXTURE_1D, texture_id);
    f->glUniform1i(uniform_mytexture, 0);

    f->glEnableVertexAttribArray(attribute_coord3d);
    f->glBindBuffer(GL_ARRAY_BUFFER, vbo_triangle_vertices);

    f->glVertexAttribPointer(
                attribute_coord3d,
                3,
                GL_FLOAT,
                GL_FALSE,
                0,0
                );

    f->glEnableVertexAttribArray(attribute_texcoord);
    f->glBindBuffer(GL_ARRAY_BUFFER, vbo_triangle_texcoords);

    f->glVertexAttribPointer(
                attribute_texcoord,
                1,
                GL_FLOAT,
                GL_FALSE,
                0,0
                );

    f->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_triangle_elements);
    f->glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
    f->glDrawElements(GL_TRIANGLES, size/sizeof(GLushort), GL_UNSIGNED_SHORT, 0);

    f->glDisableVertexAttribArray(attribute_coord3d);
    f->glDisableVertexAttribArray(attribute_texcoord);

    cout << "Fin de BufferedObject3D::drawBufferedColorBarAndPaint" << endl;
}

} //namespace
