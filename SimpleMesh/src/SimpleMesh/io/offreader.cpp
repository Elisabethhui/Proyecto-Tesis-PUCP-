#include <iostream>
#include <fstream>

#include "SimpleMesh/io/offreader.h"
#include <boost/foreach.hpp>
#include <boost/tokenizer.hpp>

using namespace std;
using namespace boost;

namespace SimpleMesh{

namespace IO {

OFFReader::OFFReader(QObject* parent) : QThread(parent)
{

}

OFFReader::~OFFReader(){
    wait();
}

void OFFReader::set_filename(const char *file){
    read_ok = false;
    ifstream in(file);

    if(!in.is_open()){
        return;
    }

    cout << "File " << file << " - opened!" << endl;

    string str;

    getline(in, str);
    if(str.find("OFF") != string::npos){
        read_ok = true;
        filename = file;
    }
    else{
        cout << "File " << file << " format! - Bad" << endl;
    }

    cout << "File " << file << " - " << read_ok << endl;
    in.close();
}

void OFFReader::run(){
    bool flagColor=false, flagNormal=false, flagTexture=false;

    ifstream in(filename.c_str());
    string str;

    //Read header
    getline(in, str);
    #ifdef DEBUG
        cout << str << endl;
    #endif // DEBUG

    if(str.find("ST")!=string::npos)
        flagTexture = true;

    if(str.find("C")!=string::npos)
        flagColor = true;

    if(str.find("N")!=string::npos)
        flagNormal = true;

    //Read header - info mesh
    unsigned int nVertices, nTriangles, nEdges;
    getline(in, str);
    #ifdef DEBUG
        cout << str << endl;
    #endif // DEBUG

    vector<string> tokIni;
    char_separator<char> sepIni(" ");
    tokenizer< char_separator<char> > tokensIni(str, sepIni);
    BOOST_FOREACH(const string& t, tokensIni){
        tokIni.push_back(t);
    }

    assert(tokIni.size() == 3);
    nVertices = (unsigned int)atoi(tokIni[0].c_str());
    nTriangles = (unsigned int)atoi(tokIni[1].c_str());
    nEdges = (unsigned int)atoi(tokIni[2].c_str());

    unsigned int numUnits = nVertices + nTriangles;

    #ifdef DEBUG
        cout << nVertices << " - " << nTriangles << " - " << nEdges << endl;
    #endif // DEBUG

    //Allocate storage for the mesh
    m_readMesh = new Mesh;
    m_readMesh->set_number_vertices(nVertices);
    m_readMesh->set_number_triangles(nTriangles);

    #ifdef DEBUG
    cout << "#vertex:" << m_readMesh->get_number_vertices() << " - set!" << endl;
    cout << "#triangle:" << m_readMesh->get_number_triangles() << " - set!" << endl;
    #endif // DEBUG

    //Actually reading the mesh structure
    for(unsigned int i = 0; i < m_readMesh->get_number_vertices(); i++){
        getline(in, str);
        vector<string> tok;
        char_separator<char> sep(" ");
        tokenizer< char_separator<char> > tokens(str, sep);
        BOOST_FOREACH(const string& t, tokens){
            tok.push_back(t);
        }

        m_readMesh->add_vertex(i, atof(tok[0].c_str()), atof(tok[1].c_str()), atof(tok[2].c_str()));

        if(flagNormal)
            m_readMesh->add_normal(i, atof(tok[3].c_str()), atof(tok[4].c_str()), atof(tok[5].c_str()));
        if(flagColor){
            if(!flagNormal){
                m_readMesh->add_color(i, atof(tok[3].c_str())/255.0, atof(tok[4].c_str())/255.0, atof(tok[5].c_str())/255.0);
            }
            if(flagNormal){
                m_readMesh->add_color(i, atof(tok[6].c_str())/255.0, atof(tok[7].c_str())/255.0, atof(tok[8].c_str())/255.0);
            }
        }
        if(i%100==0){
            float ni = (float)i;
            float ti = (float)numUnits;

            int val = (int)((ni/ti)*100);
            cout << "Thread:" << val << endl;
            emit progress(val, QString("Reading vertices"));
        }
    }

    for(unsigned int i = 0; i < m_readMesh->get_number_triangles(); i++){
        getline(in, str);
        vector<string> tok;
        char_separator<char> sep(" ");
        tokenizer< char_separator<char> > tokens(str, sep);
        BOOST_FOREACH(const string& t, tokens){
            tok.push_back(t);
        }

        assert(atoi(tok[0].c_str()) == 3);
        unsigned int p1, p2, p3;
        p1 = atoi(tok[1].c_str());
        p2 = atoi(tok[2].c_str());
        p3 = atoi(tok[3].c_str());
        m_readMesh->add_triangle(i, p1, p2, p3);
        if(i%100==0){
            float ni = (float)(i + nVertices);
            float ti = (float)numUnits;
            int val = (int)((ni/ti)*100);
            emit progress(val, QString("Reading triangles"));
        }
    }

    if(flagNormal)
       m_readMesh->enable_normals();
    if(flagColor)
        m_readMesh->enable_color();

    in.close();
}

void OFFReader::read_mesh()
{

    start();
}


} //namespace IO
}//namespace SimpleMesh
