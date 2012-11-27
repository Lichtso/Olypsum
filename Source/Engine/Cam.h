//
//  Cam.h
//  Olypsum
//
//  Created by Alexander Meißner on 21.02.12.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#import "Ray3.h"
#import "Matrix4.h"
#import "Object.h"

#ifndef Cam_h
#define Cam_h

enum CollisionMask {
    CollisionMask_Light = 1,
    CollisionMask_Zone = 2,
    CollisionMask_Static = 4,
    CollisionMask_Object = 8
};

//! Camera used to render the scene
class Cam : public SimpleObject {
    btVector3 prevPos, velocity;
    public:
    Matrix4 viewMat; //!< Projection matrix used for OpenGL
    float fov, //!< Field of view, = 0 : Ortho, < 90 : Perspective, = 90 : Parabolid, = 180 : Sphere
          near, //!< Distance of the near plane used for clipping
          far, //!< Distance of the far plane used for clipping
          width, //!< Width of the view used when fov < 90
          height;//!< Height of the view used when fov < 90
    Cam();
    ~Cam();
    /*! Calculates a ray shot from this Cam
     @param screenPos from (-1, -1) to (1, 1)
     @return A new ray with the relative direction of the screenPos parameter
     */
    Ray3 getRayAt(btVector3 screenPos);
    /*! Sets the DisplayObject::inFrustum flag for each DisplayObject which is in the frustum of this Cam
     @param filterMask Each DisplayObject with a matching CollisionMask will be proceeded
     */
    void doFrustumCulling(short int filterMask);
    /*! Tests if a DisplayObject is behind the near plane of this cam. Used by the engine to cull LightVolumes.
     @param node The btDbvtProxy of the DisplayObject to be tested
     @see LightObject
     */
    bool testInverseNearPlaneHit(btDbvtProxy* node);
    void gameTick();
    //! Sets this cam as the currentCam
    void use();
};

extern Cam *mainCam, *guiCam, *currentCam;

#endif