//
//  CamObject.h
//  Olypsum
//
//  Created by Alexander Meißner on 21.02.12.
//  Copyright (c) 2014 Gamefortec. All rights reserved.
//

#ifndef CamObject_h
#define CamObject_h

#include "ShaderProgram.h"

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
          aspect, //! width/height (only used if fov < 90)
          nearPlane, //!< Distance of the near plane if fov < 90
          farPlane; //!< Distance of the far plane if fov < 90 else the radius of the parabolid or sphere
    //! Initialize as GUI cam
    CamObject();
    CamObject(btTransform transformation);
    CamObject(rapidxml::xml_node<xmlUsedCharType>* node, LevelLoader* levelLoader);
    void removeClean();
    //Returns the aspect of this Cam if > 0.0 else the one of the screen
    float getAspect();
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
    /*! Sets the VisualObject::inFrustum flag for each VisualObject which is in the frustum of this Cam
     @return true if the frustum was invalid (in case of a mirror outside the view)
     */
    bool doFrustumCulling();
    /*! Tests if a VisualObject is behind the near plane of this cam. Used by the engine to cull LightVolumes.
     @param node The btDbvtProxy of the VisualObject to be tested
     @see LightObject
     */
    bool testInverseNearPlaneHit(btDbvtProxy* node);
    /*! Draws a model of the frustum for debuging.
     @param color The color used for rendering
     @see LightVolume
     */
    void drawDebugFrustum(Color4 color);
    bool gameTick();
    //! Sets this Cam as the currentCam
    void use();
    //! Recalculates the viewMat of this Cam
    void updateViewMat();
    rapidxml::xml_node<xmlUsedCharType>* write(rapidxml::xml_document<xmlUsedCharType>& doc, LevelSaver* levelSaver);
};

extern CamObject *mainCam, //!< The viewer CamObject delivered by the loaded Game
                 *guiCam, //!< A ortho CamObject to render the GUI
                 *currentCam; //!< The active CamObject

#endif