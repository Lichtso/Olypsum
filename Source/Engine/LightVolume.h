//
//  LightVolume.h
//  Olypsum
//
//  Created by Alexander Meißner on 23.02.12.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#ifndef LightVolume_h
#define LightVolume_h

#include "ParticlesObject.h"

//! A abstract volume to be rendered for deferred shading or debuging purpose
class LightVolume {
    protected:
    VertexArrayObject vao; //!< The geometry VAO
    public:
    //! Initializes the VAO
    void init();
    //! Returns a array of its verices and stores the count of verices in the parameter
    virtual std::unique_ptr<float[]> getVertices(unsigned int& verticesCount) = 0;
    //! Returns a array of its indecies and stores the length of the array in the parameter
    virtual std::unique_ptr<unsigned int[]> getIndecies(unsigned int& elementsCount) = 0;
    //! Renders the VAO using the monochromeSP
    void drawDebug(Color4 color);
    //! Renders the VAO using the active ShaderProgram
    void draw();
};

//! A LightVolume with the shape of a box (used by DirectionalLight)
class LightBoxVolume : public LightVolume {
    public:
    btVector3 halfSize; //!< The half extends of the box
    LightBoxVolume(btVector3 halfSize);
    std::unique_ptr<float[]> getVertices(unsigned int& verticesCount);
    std::unique_ptr<unsigned int[]> getIndecies(unsigned int& elementsCount);
};

//! A LightVolume with the shape of a frustum (used by CamObject)
class FrustumVolume : public LightVolume {
    public:
    Ray3 bounds[4]; //!< 4 rays defining a frustum
    float length; //!< The length of each ray
    FrustumVolume(Ray3 bounds[4], float length);
    std::unique_ptr<float[]> getVertices(unsigned int& verticesCount);
    std::unique_ptr<unsigned int[]> getIndecies(unsigned int& elementsCount);
};

//! A LightVolume with the shape of a sphere (used by PositionalLight)
class LightSphereVolume : public LightVolume {
    public:
    btScalar radius; //!< The radius of the sphere
    unsigned int accuracyX, //!< Count of the vertices for each ring
                 accuracyY; //!< Count of the rings defining the sphere
    LightSphereVolume(btScalar radius, unsigned int accuracyX, unsigned int accuracyY);
    std::unique_ptr<float[]> getVertices(unsigned int& verticesCount);
    std::unique_ptr<unsigned int[]> getIndecies(unsigned int& elementsCount);
};

//! A LightVolume with the shape of a cone or parabolid (used by SpotLight and PositionalLight)
class LightParabolidVolume : public LightVolume {
    public:
    btScalar radius; //!< The radius of the parabolid
    unsigned int accuracyX, //!< Count of the vertices for each ring
                 accuracyY; //!< Count of the rings defining the parabolid without its base
    LightParabolidVolume(btScalar radius, unsigned int accuracyX, unsigned int accuracyY);
    std::unique_ptr<float[]> getVertices(unsigned int& verticesCount);
    std::unique_ptr<unsigned int[]> getIndecies(unsigned int& elementsCount);
};

#endif
