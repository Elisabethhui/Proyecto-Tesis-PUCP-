#ifndef OFFREADER_H
#define OFFREADER_H

#include "SimpleMesh/simplemesh_global.h"
#include "SimpleMesh/mesh.h"
#include <string>
#include <QThread>

using namespace std;

namespace SimpleMesh{

namespace IO {

class SIMPLEMESH_API OFFReader : public QThread
{
    Q_OBJECT

private:
    string filename;
    bool read_ok;
    Mesh* m_readMesh;

public:
    OFFReader(QObject* parent=0);
    ~OFFReader();

    void set_filename(const char* file);
    bool is_ok() {return read_ok;}
    void read_mesh();
    Mesh* get_mesh() {return m_readMesh;}

Q_SIGNALS:
    void progress(int, const QString&);

protected:
    void run();
};

} //namespace IO
} //namespace SimpleMesh
#endif // OFFREADER_H
