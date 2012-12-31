//
//  Cam.h
//  Olypsum
//
//  Created by Alexander Meißner on 21.02.12.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#import "Ray3.h"
#import "ShaderProgram.h"

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
          near, //!< Distance of the near plane if fov < 90
          far, //!< Distance of the far plane if fov < 90 else the radius of the parabolid or sphere
          width, //!< Width of the view used if fov < 90
          height;//!< Height of the view used if fov < 90
    Cam();
    Cam(rapidxml::xml_node<xmlUsedCharType>* node, LevelLoader* levelLoader);
    ~Cam();
    void remove();
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
    /*! Draws a wireframe model of the frustum for debuging.
     @param color The color used for rendering
     @see LightVolume
     */
    void drawWireframeFrustum(Color4 color);
    bool gameTick();
    //! Sets this Cam as the currentCam
    void use();
    //! Updates the OpenAL listener
    void updateAudioListener();
    rapidxml::xml_node<xmlUsedCharType>* write(rapidxml::xml_document<xmlUsedCharType>& doc, LevelSaver* levelSaver);
};

extern Cam *mainCam, *guiCam, *currentCam;

#endif