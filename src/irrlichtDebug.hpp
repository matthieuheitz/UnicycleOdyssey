#include <irrlicht.h>
#include <iostream>

namespace ic = irr::core;
namespace iv = irr::video;
namespace is = irr::scene;

// Serialize an irrlicht vector3df
std::ostream& operator<<(std::ostream& out, const ic::vector3df vec)
{
    out << "(" << vec.X
        << "," << vec.Y
        << "," << vec.Z
        << ")";
    return out;
}

// Serialize an irrlicht vector3df
std::ostream& operator<<(std::ostream& out, const ic::vector3di vec)
{
    out << "(" << vec.X
        << "," << vec.Y
        << "," << vec.Z
        << ")";
    return out;
}

// Serialize an irrlicht vector3df
std::ostream& operator<<(std::ostream& out, const ic::vector2df vec)
{
    out << "(" << vec.X
        << "," << vec.Y
        << ")";
    return out;
}

// Serialize an irrlicht vector3df
std::ostream& operator<<(std::ostream& out, const ic::vector2di vec)
{
    out << "(" << vec.X
        << "," << vec.Y
        << ")";
    return out;
}

// Serialize an irrlicht matrix4
std::ostream& operator<<(std::ostream& out, const ic::matrix4 mat)
{
    for(int i=0 ; i<4 ; ++i)
    {
        for(int j=0 ; j<4 ; ++j)
        {
            out << mat(j,i) << " ";
        }
        out << std::endl;
    }
    return out;
}


// Serialize an irrlicht SColor
std::ostream& operator<<(std::ostream& out, const iv::SColor color)
{
    out << "(" << color.getAlpha()
        << "," << color.getRed()
        << "," << color.getGreen()
        << "," << color.getBlue()
        << ")";
    return out;
}

// Serialize an irrlicht SColorf
std::ostream& operator<<(std::ostream& out, const iv::SColorf color)
{
    out << "(" << color.getAlpha()
        << "," << color.getRed()
        << "," << color.getGreen()
        << "," << color.getBlue()
        << ")";
    return out;
}

// Load a mesh from an obj file, make an X symmetry, and flip the triangles
// This is to avoid the mesh to be misplaced, because of irrlicht left-hand convention.
is::IMesh * loadIMeshFromOBJ(is::ISceneManager * smgr, const char * filepath)
{
    is::IMesh * mesh = smgr->getMesh(filepath);

    iv::S3DVertex* vertexArray = (iv::S3DVertex*)mesh->getMeshBuffer(0)->getVertices();
    for(int i=0 ; i < mesh->getMeshBuffer(0)->getVertexCount() ; i++)
    {
      vertexArray[i].Pos.X = -vertexArray[i].Pos.X;
    }
    smgr->getMeshManipulator()->flipSurfaces(mesh);

    return mesh;
}
