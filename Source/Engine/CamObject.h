//
//  CamObject.h
//  Olypsum
//
//  Created by Alexander Meißner on 21.02.12.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#include "Ray3.h"
#include "ShaderProgram.h"

#ifndef CamObject_h
#define CamObject_h

enum CollisionMask {
    CollisionMask_Light = 1,
    CollisionMask_Zone = 2,
    CollisionMask_Static = 4,
    CollisionMask_Object = 8
};

//! Camera used to render the scene
class CamObject : public SimpleObject {
    btVector3 prevPos, velocity;
    public:
    Matrix4 viewMat; //!< Projection matrix used for OpenGL
    float fov, //!< Field of view, = 0 : Ortho, < 90 : Perspective, = 90 : Parabolid, = 180 : Sphere
          near, //!< Distance of the near plane if fov < 90
          far, //!< Distance of the far plane if fov < 90 else the radius of the parabolid or sphere
          width, //!< Width of the view used if fov < 90
          height;//!< Height of the view used if fov < 90
    CamObject();
    CamObject(rapidxml::xml_node<xmlUsedCharType>* node, LevelLoader* levelLoader);
    ~CamObject();
    void remove();
    //Returns a temporary Matrix4 which might differ from the transformation in reflection frames
    Matrix4 getCamMatrix();
    /*! Calculates a ray shot from this Cam in local space
     @param x from -1.0 to 1.0
     @param y from -1.0 to 1.0
     @return A new ray
     */
    Ray3 getRelativeRayAt(float x, float y);
    /*! Calculates a ray shot from this Cam in world space
     @param x from -1.0 to 1.0
     @param y from -1.0 to 1.0
     @return A new ray
     */
    Ray3 getRayAt(float x, float y);
    /*! Sets the DisplayObject::inFrustum flag for each DisplayObject which is in the frustum of this Cam
     @return true if the frustum was invalid (in case of a mirror outside the view)
     */
    bool doFrustumCulling();
    /*! Tests if a DisplayObject is behind the near plane of this cam. Used by the engine to cull LightVolumes.
     @param node The btDbvtProxy of the DisplayObject to be tested
     @see LightObject
     */
    bool testInverseNearPlaneHit(btDbvtProxy* node);
    /*! Draws a model of the frustum for debuging.
     @param color The color used for rendering
     @see LightVolume
     */
    void drawDebugFrustum(Color4 color);
    bool gameTick();
    void newScriptInstance();
    //! Sets this Cam as the currentCam
    void use();
    //! Recalculates the viewMat of this Cam
    void updateViewMat();
    rapidxml::xml_node<xmlUsedCharType>* write(rapidxml::xml_document<xmlUsedCharType>& doc, LevelSaver* levelSaver);
};

extern CamObject *mainCam, *guiCam, *currentCam;

#endif