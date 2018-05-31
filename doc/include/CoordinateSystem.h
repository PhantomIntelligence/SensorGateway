/*
	Copyright 2014, 2015 Phantom Intelligence Inc.

	Licensed under the Apache License, Version 2.0 (the "License");
	you may not use this file except in compliance with the License.
	You may obtain a copy of the License at

		http://www.apache.org/licenses/LICENSE-2.0

	Unless required by applicable law or agreed to in writing, software
	distributed under the License is distributed on an "AS IS" BASIS,
	WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
	See the License for the specific language governing permissions and
	limitations under the License.
*/
/*
	This module manages 3D coordinate transformations between frames of reference.
	It also manages conversions from the 3D coordinates to the 2D camera pixel space.
*/

#ifndef AWL_COORDINATESYSTEM_H
#define AWL_COORDINATESYSTEM_H


#include "../TypeDefinition.h"
#include <vector>
#include <deque>
#include <queue>
#include <string>
#include <memory>
#include <mutex>
#include <cmath>
#include <chrono>
#include <ctime>
#include <thread>

//#include "DebugPrintf.h"

#ifndef M_PI
#define M_PI       3.14159265358979323846
#endif
#ifndef M_PI_2
#define M_PI_2     1.57079632679489661923
#endif

namespace awl {
#ifndef DEG2MRAD
#define DEG2MRAD(d)           (float)(17.453293*(d))     // 1000*PI/180
#endif

#ifndef MRAD2DEG
#define MRAD2DEG(r)           (float)(0.05729579513*(r)) // 180/(1000*PI)
#endif

#ifndef DEG2RAD
#define DEG2RAD(d)            (float)(0.017453293*(d))     // PI/180
#endif

#ifndef RAD2DEG
#define RAD2DEG(r)           (float)(57.295779513*(r)) // 180/(PI)
#endif

    class SphericalCoord;

    class CartesianCoord;

    class Orientation;

    class RelativePosition;

    class RotationMatrix;

    class TransformationVector;

    class TransformationMatrix;

    class TransformationNode;

    class CameraCalibration;


    typedef enum eCoordLevel {
        eSensorToReceiverCoord = 2,
        eSensorToVehicleCoord = 1,
        eSensorToWorldCoord = 0,

        eReceiverToVehicleCoord = 1,
        eReceiverToWorldCoord = 0,

        eVehicleToWorldCoord = 0,

        eCameraToVehicleCoord = 1,
        eCameraToWorldCoord = 0,

        eWorldToVehicleCoord = 0,
        eWorldToReceiverCoord = 1,
        eWorldToCameraCoord = 1
    }
            eCoordLevel;

/** \brief Structure holding the values of a 4x4 homogeneous 3D transformation matrix.
*/
    typedef float (TransformationArray)[4][4];

/** \brief Structure holding the a vector (row or column) of a 1x4 homogeneous coordinate.
*/
    typedef float (TransformationRow)[4];

/** \brief The CartesianCoord class defines a position, in cartesian coordinates.
  * \Notes uses coordinate conventions as defined in PCL / ROS.
  *  See: http://www.ros.org/reps/rep-0103.html#coordinate-frame-conventions
  *  That is:
  *
  * Chirality
  *
  * All systems are right handed. This means they comply with the right hand rule [4].
  *
  * Axis Orientation
  *
  * In relation to a body the standard is :
  * �x forward
  * �y left
  * �z up
  *
  *  Note that, in the case of cameras, there is often a different frame which uses a slightly
  *  different convention, NOT USED HERE, where:
  * �z backwards
  * �x right
  * �y up
  * Excercice caution when specifying coordinates to make sure you always use the body standard, not the camera style reference.
  * X is always forward.
  * The only exception to this rule is when we will get to convert Coordinates to camera pixels in the CameraCalibration class.
  */


    class CartesianCoord {
    public:
        CartesianCoord();

        CartesianCoord(float inX, float inY, float inZ);

        CartesianCoord(const CartesianCoord& inCartesian);

        CartesianCoord(const SphericalCoord& inSpherical);

        CartesianCoord(const TransformationVector& inVect);

        void Set(float x, float y, float z);

        static SphericalCoord ToSpherical(const CartesianCoord& inCartesian);

        SphericalCoord ToSpherical() const;


        CartesianCoord& operator=(const SphericalCoord& sourceSpherical);

        CartesianCoord& operator=(const TransformationVector& inVector);

//	operator SphericalCoord() const;

    public:
        union {
            // The standard "x,y,z" naming convention.
            struct {
                float x;
                float y;
                float z;
            };

            // The alternate "relative to body" naming convention can be useful for
            // code that wants non-equivocal naming and avoid confusion,
            // for example when when converting to
            // graphics X, Y coordinates.
            struct {
                float forward;
                float left;
                float up;
            };

        };

    };


/** \brief The SpericalCoord class defines a position, in spherical coordinates.
 *  \notes spherical coordinates are using right-handed notation
 *         rho is distance.
 *         theta is angle from z axis, clockwise.
 *         phi is angle from x axis, counterclockwise
 *  We use the notation that is common practice in physics,
 *  As specified by ISO standard 31-11.
 *
 * Conversions from spherical to cartesian are performed using formulas indicated
 * in http://mathworld.wolfram.com/SphericalCoordinates.html
 * But, since we are using physics convention instead of math convention (as Wolfram),
 * swap theta and phi from document.
 *
 * With x looking forward:
 * Theta: -: Up  +: Down
 * Phi:   + Left  -: Right
*/

    class SphericalCoord {
    public:
        SphericalCoord();

        SphericalCoord(float inRho, float inTheta, float inPhi);

        SphericalCoord(const SphericalCoord& inSpherical);

        SphericalCoord(const CartesianCoord& inCartesian);

        SphericalCoord(const TransformationVector& inVect);

        void Set(float inRho, float inTheta, float inPhi);

        static CartesianCoord ToCartesian(const SphericalCoord& inSpherical);

        CartesianCoord ToCartesian() const;

        SphericalCoord& operator=(const CartesianCoord& sourceCartesian);

        SphericalCoord& operator=(const TransformationVector& inVector);

    public:
        float rho;
        float theta;
        float phi;
    };

/** \brief The Orientation class defines the relative orientation of an object in a frame of reference.
	*  \notes
	*  yaw is a counterclockwise rotation of $ \alpha$ about the $ z$-axis (looking right is yaw negative).
	*  pitch is a counterclockwise rotation of $ \beta$ about the $ y$-axis (looking down is pitch positive [since Y axis is oriented leftwards]).
	 * roll is a counterclockwise rotation of $ \gamma$ about the $ x$-axis (rolling rightwards is roll negative).
	*  This meansd that, for a standard reference frame, where X axis is looking forward from the object:
	*  - looking right is yaw negative.
	*  - looking down is pitch positive.
	 * - rolling to the right side is negative.

	 * \reference: http://planning.cs.uiuc.edu/node102.html
*/

    class Orientation {
    public:
        Orientation();

        // We use the roll, pitch, yaw order, to keep consistent with eigen
        Orientation(float inRoll, float inPitch, float inYaw);

        Orientation(const Orientation& inOrientation);

        Orientation(const TransformationVector& inVect);

        Orientation& operator=(const TransformationVector& inVector);

    public:
        float roll;
        float pitch;
        float yaw;
    };

/** \brief The RelativePosition Class defines the relative position AND orientation of an object or frame of reference from another.
 *  \notes The class contains the relative position and orientation of a frame of reference, from the origin frame of reference.
*/

    class RelativePosition {
    public:
        RelativePosition();

        RelativePosition(float inX, float inY, float inZ, float inRoll, float inPitch, float inYaw);

        RelativePosition(const CartesianCoord& inPosition, const Orientation& inOrientation);

        RelativePosition(const SphericalCoord& inPosition, const Orientation& inOrientation);

        RelativePosition(const RelativePosition& inRelativePosition);

    public:
        CartesianCoord position;
        Orientation orientation;
    };

/** \brief The TransformationMatrix class supports the basic operators on a
 *          4x4 affine transformation matrix.
 *  \notes	Constructors allow the easy filling of a transformation matrix
 *          for Position (or translation) and Orientation or (Rotation)
*/
    class TransformationMatrix {
    public:
        TransformationMatrix();

        TransformationMatrix(const CartesianCoord& inCartesian);

        TransformationMatrix(const SphericalCoord& inSpherical);

        TransformationMatrix(const Orientation& inOrientation);

        TransformationMatrix(const CartesianCoord& inCartesian, const Orientation& inOrientation);

        TransformationMatrix(const SphericalCoord& inSpherical, const Orientation& inOrientation);

        TransformationMatrix(const RelativePosition& inRelativePosition);

        TransformationMatrix& operator=(const CartesianCoord& inCartesian);

        TransformationMatrix& operator=(const SphericalCoord& inSpherical);

        TransformationMatrix& operator=(const Orientation& inOrientation);

        TransformationMatrix& operator=(const RelativePosition& inRelativePosition);

        TransformationMatrix Reverse();

    public:
        TransformationArray matrix;
    };

/** \brief The TransformationVector class supports the basic operators on a
 *          1x4 vector.
 *  \notes	The vectors can be used as operands in affine transformation operations
 *          or as convenient placeholders for sub-sections of TransformationMatrix.
*/
    class TransformationVector {
    public:
        TransformationVector();

        TransformationVector(const CartesianCoord& inCartesian);

        TransformationVector(const SphericalCoord& inSpherical);

        TransformationVector(const Orientation& inOrientation);


        TransformationVector& operator=(const CartesianCoord& inCartesian);

        TransformationVector& operator=(const SphericalCoord& inSpherical);

        TransformationVector& operator=(const Orientation& inOrientation);

    public:
        TransformationRow vect;
    };

    TransformationVector operator*(float scalarLeft, const awl::TransformationVector& right);

    TransformationVector operator*(const awl::TransformationVector& left, float scalarRight);

    TransformationMatrix operator*(const TransformationMatrix& left, const TransformationMatrix& right);

    TransformationMatrix operator+(const TransformationMatrix& left, const TransformationMatrix& right);

    TransformationMatrix operator-(const TransformationMatrix& left, const TransformationMatrix& right);

    TransformationMatrix operator*(float scalarLeft, const TransformationMatrix& right);

    TransformationMatrix operator*(const TransformationMatrix& left, float scalarRight);

    TransformationVector operator*(const TransformationMatrix& left, const TransformationVector& right);

    TransformationVector operator*(const TransformationVector& left, const TransformationMatrix& right);

/** \brief TransformationMatrixSteps define a sequence of affine transformations.
 *  \notes	Using a deque (1 sided list) the steps can identify a sequence
 *          of individual affine transformations.
*/
    typedef std::deque <TransformationMatrix> TransformationMatrixSteps;


/** \brief TransformationNodes are coordinate frames that can be organized in
  *         a tree structure to describe the relative position and sequence of transformations
  *		   from a coordinate frame to another.
  *	\Notes For commodity, each node in the tree stores all of the TransformationMatrixSteps to
  *		   convert its coordinates into each of its parent's coordinates.
  *		   The transformationNode can be used to facilitate transformation of coordinates in
  *		   a multiple axis "robot" (using the ToReferenceCoord() method).
  *		   After modification of a node's coordinates, RefreshGlobal() updates all the children's
  *		   TransformationMaxtrixSteps.
*/
    class TransformationNode : public std::enable_shared_from_this<TransformationNode> {
    public:
        typedef std::shared_ptr <TransformationNode> Ptr;
        typedef std::shared_ptr <TransformationNode> ConstPtr;
        typedef std::vector <TransformationNode::Ptr> List;
        typedef TransformationNode::List* ListPtr;

    public:
        TransformationNode(const CartesianCoord& inCartesian, const Orientation& inOrientation);

        TransformationNode(const RelativePosition& inRelativePosition);

        TransformationNode();

        void AddChild(TransformationNode::Ptr inChild);

        void RefreshGlobal();

        CartesianCoord ToReferenceCoord(eCoordLevel inLevel, const CartesianCoord& inCoord);

        CartesianCoord FromReferenceCoord(eCoordLevel inLevel, const CartesianCoord& inCoord);

    public:
        TransformationNode::Ptr parent;
        TransformationNode::List children;


        RelativePosition relativePosition;
        TransformationMatrixSteps transformations;
    };

/** \brief CameraCalibration parameters are used to convert from CameraCoordinates to
  *        pixel coordinates in the image.
  *
  * \reference: http://docs.opencv.org/doc/tutorials/calib3d/camera_calibration/camera_calibration.html

  *	\Notes We use a calibration system that uses parameters similar to the OpenCV
  *        distorsion matrixes and camera calibration matrix.
  *		   This means we can use the results from OpenCV calibration procedures in configuration files.
*/

    class CameraCalibration {
    public:
        CameraCalibration(int inFrameWidthInPixels = 640, int inFrameHeightInPixels = 480,
                          float inFovWidth = 0.0, float inFovHeight = 0.0,
                          float inFocalLengthX = 0.0, float inFocalLengthY = 0.0,
                          float inCenterX = 0.0, float inCenterY = 0,
                          float inRadialK1 = 0, float inRadialK2 = 0, float inRadialK3 = 0,
                          float inTangentialP1 = 0, float inTangentialP2 = 0);

        /** \brief convert camera coordinates to image (XY) coordinates of the camera
          *  \param[out] cameraX Position of the pixel in the X axis.
          *  \param[out] cameraY Position of the pixel in the Y axis with (0, 0) being top left.
          *  \returns Returns true if the point is in front of the camera.  Returns false for points behind the camera.
          *
          */
        bool ToFrameXY(const CartesianCoord& coordInCameraCart, int& cameraX, int& cameraY) const;

        /** \brief Calculate the focalLengths calibration parameters from the FOVs that are already stored.
          */
        void CalculateFocalLengthsFromFOVs();

    public:
        /* \brief Effective FOV Width
         *  \remarks FOV is typically related to focal length.  However, the effective FOV is used here for display purposes,
         *           as FOV may be constrained by mechanical constraints on some devices.
         */
        float fovWidth;

        /* \brief Effective FOV Height
         *  \remarks FOV is typically related to focal length.  However, the effective FOV is used here for display purposes,
         *           as FOV may be constrained by mechanical constraints on some devices
         */
        float fovHeight;
        float frameWidthInPixels; /** \brief Camera frame width in pixels */
        float frameHeightInPixels; /** \brief Camera frame height in pixels */
        float focalLengthX; /** \brief Camera focal length and scaling (accounts for pixel size in x direction) */
        float focalLengthY;/** \brief Camera focal length and scaling (accounts for pixel size in y direction) */
        float centerX; /** \brief sensorCenter offset in x direction*/
        float centerY; /** \brief sensorCenter offset in y direction*/
        float radialK1; /** \brief radial distorsion (barrel and pincushion) parameter 1*/
        float radialK2; /** \brief radial distorsion (barrel and pincushion) parameter 2*/
        float radialK3; /** \brief radial distorsion (barrel and pincushion) parameter 3*/
        float tangentialP1; /** \brief tangential distorsion (keystone) parameter 1*/
        float tangentialP2; /** \brief tangential distorsion (keystone) parameter 2*/
    };

    bool CameraCoordToFrameXY(double cameraFovWidthInRad, double cameraFovHeightInRad, int frameWidthInPixels,
                              int frameHeightInPixels, const CartesianCoord& coordInCameraCart, int& cameraX,
                              int& cameraY, double barrelK1 = 0.0, double barrelK2 = 0.0);

} // namespace awl
#endif // AWL_COORDINATESYSTEM_H
