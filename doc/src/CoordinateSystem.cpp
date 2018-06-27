/* CoordinateSystem.cpp */
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

#include "../../include/SensorCommunication/CoordinateSystem.h"

using awl::SphericalCoord;
using awl::CartesianCoord;
using awl::Orientation;
using awl::RelativePosition;
using awl::RotationMatrix;
using awl::TransformationVector;
using awl::TransformationMatrix;
using awl::TransformationNode;
using awl::CameraCalibration;

#ifndef M_PI
#define M_PI       3.14159265358979323846
#endif
#ifndef M_PI_2
#define M_PI_2     1.57079632679489661923
#endif

CartesianCoord::CartesianCoord() :
        x(0.0),
        y(0.0),
        z(0.0) {
}

CartesianCoord::CartesianCoord(float inX, float inY, float inZ) :
        x(inX),
        y(inY),
        z(inZ) {
}

CartesianCoord::CartesianCoord(const CartesianCoord& inCartesian) {
    x = inCartesian.x;
    y = inCartesian.y;
    z = inCartesian.z;
}

CartesianCoord::CartesianCoord(const SphericalCoord& inSpherical) {
    *this = inSpherical.ToCartesian();
}

CartesianCoord::CartesianCoord(const TransformationVector& inVect) {
    x = inVect.vect[0];
    y = inVect.vect[1];
    z = inVect.vect[2];
}

void CartesianCoord::Set(float inX, float inY, float inZ) {
    x = inX;
    y = inY;
    z = inZ;
}

// Convert from cartesian to spherical
// Using formaulas indicated in http://mathworld.wolfram.com/SphericalCoordinates.html
// But, since we are using physics convention instead of math convention (as Wolfram), swap theta and phi.

SphericalCoord CartesianCoord::ToSpherical(const CartesianCoord& inCartesian) {
    SphericalCoord spherical;

    spherical.rho = sqrt(
            (inCartesian.x * inCartesian.x) + (inCartesian.y * inCartesian.y) + (inCartesian.z * inCartesian.z));

    if (spherical.rho != 0.0) {
        spherical.theta = acos(inCartesian.z / spherical.rho);
    }

    if (inCartesian.x != 0.0) {
        spherical.phi = atan(inCartesian.y / inCartesian.x);
    }

    return (spherical);
}

CartesianCoord& CartesianCoord::operator=(const SphericalCoord& sourceSpherical) {
    *this = sourceSpherical.ToCartesian();
    return (*this);
}

CartesianCoord& CartesianCoord::operator=(const TransformationVector& inVector) {
    x = inVector.vect[0];
    y = inVector.vect[1];
    z = inVector.vect[2];
    return (*this);
}

SphericalCoord CartesianCoord::ToSpherical() const {
    return (ToSpherical(*this));
}


SphericalCoord::SphericalCoord() :
        rho(0.0),
        theta(0.0),
        phi(0.0) {
}

SphericalCoord::SphericalCoord(float inRho, float inTheta, float inPhi) :
        rho(inRho),
        theta(inTheta),
        phi(inPhi) {
}

SphericalCoord::SphericalCoord(const SphericalCoord& inSpherical) {
    rho = inSpherical.rho;
    theta = inSpherical.theta;
    phi = inSpherical.phi;
}

SphericalCoord::SphericalCoord(const CartesianCoord& inCartesian) {
    *this = inCartesian.ToSpherical();
}

SphericalCoord::SphericalCoord(const TransformationVector& inVect) {
    *this = inVect;
}


void SphericalCoord::Set(float inRho, float inTheta, float inPhi) {
    rho = inRho;
    theta = inTheta;
    phi = inPhi;
}

// Convert from spherical to cartesian
// Using formaulas indicated in http://mathworld.wolfram.com/SphericalCoordinates.html
// But, since we are using physics convention instead of math convention (as Wolfram), swap theta and phi.

CartesianCoord SphericalCoord::ToCartesian(const SphericalCoord& inSpherical) {
    CartesianCoord cartesian;
    float sinTheta = sin(inSpherical.theta);
    cartesian.x = inSpherical.rho * cos(inSpherical.phi) * sinTheta;
    cartesian.y = inSpherical.rho * sin(inSpherical.phi) * sinTheta;
    cartesian.z = inSpherical.rho * cos(inSpherical.theta);
    return (cartesian);
}

CartesianCoord SphericalCoord::ToCartesian() const {
    return (ToCartesian(*this));
}

SphericalCoord& SphericalCoord::operator=(const TransformationVector& inVector) {
    CartesianCoord cartesian = inVector;
    *this = cartesian.ToSpherical();
    return (*this);
}

SphericalCoord& SphericalCoord::operator=(const CartesianCoord& sourceCartesian) {
    *this = sourceCartesian.ToSpherical();
    return (*this);
}

Orientation::Orientation() :
        roll(0.0),
        pitch(0.0),
        yaw(0.0) {
}

Orientation::Orientation(float inRoll, float inPitch, float inYaw) :
        roll(inRoll),
        pitch(inPitch),
        yaw(inYaw) {
    Orientation orient;
}

Orientation::Orientation(const Orientation& inOrientation) :
        roll(inOrientation.roll),
        pitch(inOrientation.pitch),
        yaw(inOrientation.yaw) {
}

Orientation::Orientation(const TransformationVector& inVect) :
        roll(inVect.vect[0]),
        pitch(inVect.vect[1]),
        yaw(inVect.vect[2]) {
}

Orientation& Orientation::operator=(const TransformationVector& inVector) {
    roll = inVector.vect[0];
    pitch = inVector.vect[1];
    yaw = inVector.vect[2];
    return (*this);
}

RelativePosition::RelativePosition() :
        position(0.0, 0.0, 0.0),
        orientation(0.0, 0.0, 0.0) {
}

RelativePosition::RelativePosition(float inX, float inY, float inZ, float inRoll, float inPitch, float inYaw) :
        position(inX, inY, inZ),
        orientation(inRoll, inPitch, inYaw) {
}

RelativePosition::RelativePosition(const CartesianCoord& inPosition, const Orientation& inOrientation) :
        position(inPosition),
        orientation(inOrientation) {
}

RelativePosition::RelativePosition(const SphericalCoord& inPosition, const Orientation& inOrientation) :
        position(inPosition),
        orientation(inOrientation) {
}

RelativePosition::RelativePosition(const RelativePosition& inRelativePosition) :
        position(inRelativePosition.position),
        orientation(inRelativePosition.orientation) {
}

TransformationMatrix::TransformationMatrix() {
    matrix[0][0] = 1;
    matrix[0][1] = 0;
    matrix[0][2] = 0;
    matrix[0][3] = 0;
    matrix[1][0] = 0;
    matrix[1][1] = 1;
    matrix[1][2] = 0;
    matrix[1][3] = 0;
    matrix[2][0] = 0;
    matrix[2][1] = 0;
    matrix[2][2] = 1;
    matrix[2][3] = 0;
    matrix[3][0] = 0.0;
    matrix[3][1] = 0.0;
    matrix[3][2] = 0.0;
    matrix[3][3] = 1.0;

}

TransformationMatrix::TransformationMatrix(const CartesianCoord& inCartesian) {
    *this = inCartesian;

}

TransformationMatrix::TransformationMatrix(const SphericalCoord& inSpherical) {
    *this = inSpherical;
}

TransformationMatrix::TransformationMatrix(const Orientation& inOrientation) {
    *this = inOrientation;
}

TransformationMatrix::TransformationMatrix(const SphericalCoord& inSpherical, const Orientation& inOrientation) {
    *this = TransformationMatrix(CartesianCoord(inSpherical), inOrientation);
}

TransformationMatrix::TransformationMatrix(const CartesianCoord& inCartesian, const Orientation& inOrientation) {
    float cosAlpha = cos(inOrientation.yaw);
    float sinAlpha = sin(inOrientation.yaw);
    float cosBeta = cos(inOrientation.pitch);
    float sinBeta = sin(inOrientation.pitch);
    float cosGamma = cos(inOrientation.roll);
    float sinGamma = sin(inOrientation.roll);

    matrix[0][0] = cosAlpha * cosBeta;
    matrix[0][1] = (cosAlpha * sinBeta * sinGamma) - (sinAlpha * cosGamma);
    matrix[0][2] = (cosAlpha * sinBeta * cosGamma) + (sinAlpha * sinGamma);
    matrix[0][3] = inCartesian.x;
    matrix[1][0] = sinAlpha * cosBeta;
    matrix[1][1] = (sinAlpha * sinBeta * sinGamma) + (cosAlpha * cosGamma);
    matrix[1][2] = (sinAlpha * sinBeta * cosGamma) - (cosAlpha * sinGamma);
    matrix[1][3] = inCartesian.y;
    matrix[2][0] = -sinBeta;
    matrix[2][1] = cosBeta * sinGamma;
    matrix[2][2] = cosBeta * cosGamma;
    matrix[2][3] = inCartesian.z;
    matrix[3][0] = 0.0;
    matrix[3][1] = 0.0;
    matrix[3][2] = 0.0;
    matrix[3][3] = 1.0;
}

TransformationMatrix::TransformationMatrix(const RelativePosition& inRelativePosition) {
    *this = inRelativePosition;
}

TransformationMatrix& TransformationMatrix::operator=(const CartesianCoord& inCartesian) {
    matrix[0][0] = 1;
    matrix[0][1] = 0;
    matrix[0][2] = 0;
    matrix[0][3] = inCartesian.x;
    matrix[1][0] = 0;
    matrix[1][1] = 1;
    matrix[1][2] = 0;
    matrix[1][3] = inCartesian.y;
    matrix[2][0] = 0;
    matrix[2][1] = 0;
    matrix[2][2] = 1;
    matrix[2][3] = inCartesian.z;
    matrix[3][0] = 0.0;
    matrix[3][1] = 0.0;
    matrix[3][2] = 0.0;
    matrix[3][3] = 1.0;

    return (*this);
}


TransformationMatrix& TransformationMatrix::operator=(const SphericalCoord& inSpherical) {
    CartesianCoord cartesian(inSpherical);
    matrix[0][0] = 1;
    matrix[0][1] = 0;
    matrix[0][2] = 0;
    matrix[0][3] = cartesian.x;
    matrix[1][0] = 0;
    matrix[1][1] = 1;
    matrix[1][2] = 0;
    matrix[1][3] = cartesian.y;
    matrix[2][0] = 0;
    matrix[2][1] = 0;
    matrix[2][2] = 1;
    matrix[2][3] = cartesian.z;
    matrix[3][0] = 0.0;
    matrix[3][1] = 0.0;
    matrix[3][2] = 0.0;
    matrix[3][3] = 1.0;

    return (*this);
}

TransformationMatrix& TransformationMatrix::operator=(const Orientation& inOrientation) {
    float cosAlpha = cos(inOrientation.yaw);
    float sinAlpha = sin(inOrientation.yaw);
    float cosBeta = cos(inOrientation.pitch);
    float sinBeta = sin(inOrientation.pitch);
    float cosGamma = cos(inOrientation.roll);
    float sinGamma = sin(inOrientation.roll);

    matrix[0][0] = cosAlpha * cosBeta;
    matrix[0][1] = (cosAlpha * sinBeta * sinGamma) - (sinAlpha * cosGamma);
    matrix[0][2] = (cosAlpha * sinBeta * cosGamma) + (sinAlpha * sinGamma);
    matrix[0][3] = 0.0;
    matrix[1][0] = sinAlpha * cosBeta;
    matrix[1][1] = (sinAlpha * sinBeta * sinGamma) + (cosAlpha * cosGamma);
    matrix[1][2] = (sinAlpha * sinBeta * cosGamma) - (cosAlpha * sinGamma);
    matrix[1][3] = 0.0;
    matrix[2][0] = -sinBeta;
    matrix[2][1] = cosBeta * sinGamma;
    matrix[2][2] = cosBeta * cosGamma;
    matrix[2][3] = 0.0;
    matrix[3][0] = 0.0;
    matrix[3][1] = 0.0;
    matrix[3][2] = 0.0;
    matrix[3][3] = 1.0;

    return (*this);
}

TransformationMatrix& TransformationMatrix::operator=(const RelativePosition& inRelativePosition) {
    float cosAlpha = cos(inRelativePosition.orientation.yaw);
    float sinAlpha = sin(inRelativePosition.orientation.yaw);
    float cosBeta = cos(inRelativePosition.orientation.pitch);
    float sinBeta = sin(inRelativePosition.orientation.pitch);
    float cosGamma = cos(inRelativePosition.orientation.roll);
    float sinGamma = sin(inRelativePosition.orientation.roll);

    matrix[0][0] = cosAlpha * cosBeta;
    matrix[0][1] = (cosAlpha * sinBeta * sinGamma) - (sinAlpha * cosGamma);
    matrix[0][2] = (cosAlpha * sinBeta * cosGamma) + (sinAlpha * sinGamma);
    matrix[0][3] = inRelativePosition.position.x;
    matrix[1][0] = sinAlpha * cosBeta;
    matrix[1][1] = (sinAlpha * sinBeta * sinGamma) + (cosAlpha * cosGamma);
    matrix[1][2] = (sinAlpha * sinBeta * cosGamma) - (cosAlpha * sinGamma);
    matrix[1][3] = inRelativePosition.position.y;
    matrix[2][0] = -sinBeta;
    matrix[2][1] = cosBeta * sinGamma;
    matrix[2][2] = cosBeta * cosGamma;
    matrix[2][3] = inRelativePosition.position.z;
    matrix[3][0] = 0.0;
    matrix[3][1] = 0.0;
    matrix[3][2] = 0.0;
    matrix[3][3] = 1.0;

    return (*this);
}

TransformationMatrix TransformationMatrix::Reverse() {
    TransformationMatrix destinationMatrix;

    // Ref: http://www.cse.psu.edu/~rcollins/CSE486/lecture12.pdf

    // Inverse the rotation sub-matrix
    destinationMatrix.matrix[0][0] = matrix[0][0] ? 1 / matrix[0][0] : 0;
    destinationMatrix.matrix[0][1] = matrix[1][0] ? 1 / matrix[1][0] : 0;
    destinationMatrix.matrix[0][2] = matrix[2][0] ? 1 / matrix[2][0] : 0;
    destinationMatrix.matrix[1][0] = matrix[0][1] ? 1 / matrix[0][1] : 0;
    destinationMatrix.matrix[1][1] = matrix[1][1] ? 1 / matrix[1][1] : 0;
    destinationMatrix.matrix[1][2] = matrix[2][1] ? 1 / matrix[2][1] : 0;
    destinationMatrix.matrix[2][0] = matrix[0][2] ? 1 / matrix[0][2] : 0;
    destinationMatrix.matrix[2][1] = matrix[1][2] ? 1 / matrix[1][2] : 0;
    destinationMatrix.matrix[2][2] = matrix[2][2] ? 1 / matrix[2][2] : 0;

    // Inverse the transformation matrix and apply the row rotation
    destinationMatrix.matrix[0][3] = -((destinationMatrix.matrix[0][0] * matrix[0][3]) +
                                       (destinationMatrix.matrix[0][1] * matrix[1][3]) +
                                       (destinationMatrix.matrix[0][2] * matrix[2][3]));

    destinationMatrix.matrix[1][3] = -((destinationMatrix.matrix[1][0] * matrix[0][3]) +
                                       (destinationMatrix.matrix[1][1] * matrix[1][3]) +
                                       (destinationMatrix.matrix[1][2] * matrix[2][3]));

    destinationMatrix.matrix[2][3] = -((destinationMatrix.matrix[2][0] * matrix[0][3]) +
                                       (destinationMatrix.matrix[2][1] * matrix[1][3]) +
                                       (destinationMatrix.matrix[2][2] * matrix[2][3]));
    // Carry over the other matrix items
    destinationMatrix.matrix[3][0] = matrix[3][0];
    destinationMatrix.matrix[3][1] = matrix[3][1];
    destinationMatrix.matrix[3][2] = matrix[3][2];
    destinationMatrix.matrix[3][3] = matrix[3][3];

    return (destinationMatrix);
}

TransformationVector::TransformationVector() {
    vect[0] = 0.0;
    vect[1] = 0.0;
    vect[2] = 0.0;
    vect[3] = 1.0;
}

TransformationVector::TransformationVector(const CartesianCoord& inCartesian) {
    vect[0] = inCartesian.x;
    vect[1] = inCartesian.y;
    vect[2] = inCartesian.z;
    vect[3] = 1.0;
}

TransformationVector::TransformationVector(const SphericalCoord& inSpherical) {
    *this = inSpherical;
}

TransformationVector::TransformationVector(const Orientation& inOrientation) {
    *this = inOrientation;
}

TransformationVector& TransformationVector::operator=(const CartesianCoord& inCartesian) {
    vect[0] = inCartesian.x;
    vect[1] = inCartesian.y;
    vect[2] = inCartesian.z;
    vect[3] = 1.0;

    return (*this);
}


TransformationVector& TransformationVector::operator=(const SphericalCoord& inSpherical) {
    CartesianCoord cartesian(inSpherical);

    vect[0] = cartesian.x;
    vect[1] = cartesian.y;
    vect[2] = cartesian.z;
    vect[3] = 1.0;

    return (*this);
}

TransformationVector& TransformationVector::operator=(const Orientation& inOrientation) {
    vect[0] = inOrientation.roll;
    vect[1] = inOrientation.pitch;
    vect[2] = inOrientation.yaw;
    vect[3] = 1.0;

    return (*this);
}

awl::TransformationVector awl::operator*(float scalarLeft, const awl::TransformationVector& right) {
    TransformationVector destinationVector;

    destinationVector.vect[0] = scalarLeft * right.vect[0];
    destinationVector.vect[1] = scalarLeft * right.vect[1];
    destinationVector.vect[2] = scalarLeft * right.vect[2];
    destinationVector.vect[3] = scalarLeft * right.vect[3];

    return (destinationVector);
}


awl::TransformationVector awl::operator*(const awl::TransformationVector& left, float scalarRight) {
    TransformationVector destinationVector;

    destinationVector.vect[0] = scalarRight * left.vect[0];
    destinationVector.vect[1] = scalarRight * left.vect[1];
    destinationVector.vect[2] = scalarRight * left.vect[2];
    destinationVector.vect[3] = scalarRight * left.vect[3];

    return (destinationVector);
}

awl::TransformationMatrix
awl::operator*(const awl::TransformationMatrix& left, const awl::TransformationMatrix& right) {
    TransformationMatrix destinationMatrix;

    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            destinationMatrix.matrix[i][j] = 0;
            for (int k = 0; k < 4; k++) {
                destinationMatrix.matrix[i][j] += left.matrix[i][k] * right.matrix[k][j];
            }
        }
    }

    return (destinationMatrix);
}

awl::TransformationMatrix
awl::operator+(const awl::TransformationMatrix& left, const awl::TransformationMatrix& right) {
    TransformationMatrix destinationMatrix;

    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            destinationMatrix.matrix[i][j] = left.matrix[i][j] + right.matrix[i][j];
        }
    }

    return (destinationMatrix);
}

awl::TransformationMatrix
awl::operator-(const awl::TransformationMatrix& left, const awl::TransformationMatrix& right) {
    TransformationMatrix destinationMatrix;

    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            destinationMatrix.matrix[i][j] = left.matrix[i][j] - right.matrix[i][j];
        }
    }

    return (destinationMatrix);
}

awl::TransformationMatrix awl::operator*(float scalarLeft, const awl::TransformationMatrix& right) {
    TransformationMatrix destinationMatrix;

    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            destinationMatrix.matrix[i][j] = scalarLeft * right.matrix[i][j];
        }
    }

    return (destinationMatrix);
}

awl::TransformationMatrix awl::operator*(const awl::TransformationMatrix& left, float scalarRight) {
    TransformationMatrix destinationMatrix;

    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            destinationMatrix.matrix[i][j] = scalarRight * left.matrix[i][j];
        }
    }

    return (destinationMatrix);
}

awl::TransformationVector
awl::operator*(const awl::TransformationMatrix& left, const awl::TransformationVector& right) {
    TransformationVector destinationVector;

    for (int i = 0; i < 4; i++) {
        destinationVector.vect[i] = 0;
        for (int j = 0; j < 4; j++) {
            destinationVector.vect[i] += left.matrix[i][j] * right.vect[j];
        }
    }

    return (destinationVector);
}

awl::TransformationVector
awl::operator*(const awl::TransformationVector& left, const awl::TransformationMatrix& right) {
    TransformationVector destinationVector;

    for (int i = 0; i < 4; i++) {
        destinationVector.vect[i] = 0;
        for (int j = 0; j < 4; j++) {
            destinationVector.vect[i] += left.vect[j] * right.matrix[j][i];
        }
    }

    return (destinationVector);
}


TransformationNode::TransformationNode(const CartesianCoord& inCartesian, const Orientation& inOrientation) :
        parent(),
        children(),
        relativePosition(inCartesian, inOrientation),
        transformations() {

    RefreshGlobal();
}


TransformationNode::TransformationNode(const RelativePosition& inRelativePosition) :
        parent(),
        children(),
        relativePosition(inRelativePosition),
        transformations() {
    RefreshGlobal();
}

TransformationNode::TransformationNode() :
        parent(),
        children(),
        relativePosition(),
        transformations() {
    RefreshGlobal();
}

void TransformationNode::AddChild(TransformationNode::Ptr inChild) {
    inChild->parent = shared_from_this();
    children.push_back(inChild);
    inChild->RefreshGlobal();
}


void TransformationNode::RefreshGlobal() {
    // Clear the current iteration of transformations
    transformations.clear();

    TransformationMatrix currentTransformation(relativePosition);
    transformations.push_front(currentTransformation);

    //iterate through the parents to create transformations at each of the steps.
    TransformationNode::Ptr ancesterNode = parent;
    while (ancesterNode) {
        currentTransformation =
                ancesterNode->transformations.at(ancesterNode->transformations.size() - 1) * currentTransformation;
        transformations.push_front(currentTransformation);
        ancesterNode = ancesterNode->parent;
    }

    // Tell all the children to refresh as well
    for (auto childNode: children) {
        childNode->RefreshGlobal();
    }
}

CartesianCoord TransformationNode::ToReferenceCoord(eCoordLevel inLevel, const CartesianCoord& inCoord) {
    TransformationVector coordVect(inCoord);
    CartesianCoord retCoord = transformations[inLevel] * coordVect;

    return (retCoord);
}

CartesianCoord TransformationNode::FromReferenceCoord(eCoordLevel inLevel, const CartesianCoord& inCoord) {
    TransformationVector coordVect(inCoord);
    TransformationMatrix reverseMatrix = transformations[inLevel].Reverse();
    CartesianCoord retCoord = reverseMatrix * coordVect;
    return (retCoord);
}


bool awl::CameraCoordToFrameXY(double cameraFovWidthInRad, double cameraFovHeightInRad, int frameWidthInPixels,
                               int frameHeightInPixels, const CartesianCoord& coordInCameraCart, int& cameraX,
                               int& cameraY, double barrelK1, double barrelK2) {
    bool bInFront = false;

    if (coordInCameraCart.x < 0) {
        bInFront = false;
    } else {
        bInFront = true;
    }


    // Insert all of the known camera parameters
    double focal = 1 / (tan(cameraFovWidthInRad / 2.0));
    double principalX = 0; // Ignored. We assume sensor is centered perfectly in FOV
    double principalY = 0; // Ignored.  We assume sensor is centered perfectly in FOV
    double aspectRatioX = 1; // Ignored. We assume square pixels
    double aspectRatioY = 1; // Ignored.  We assume square pixels.
    double skew = 0.0;    // Ignored.  We assume perpendicular plane always
    double far = -(coordInCameraCart.x * 1.5);
    double near = -0.01;

    // Make the Camera perpective transformation matrix
    TransformationMatrix cameraMatrix;
    cameraMatrix.matrix[0][0] = focal / aspectRatioX;
    cameraMatrix.matrix[0][1] = skew;
    cameraMatrix.matrix[0][2] = 0;
    cameraMatrix.matrix[0][3] = principalX;
    cameraMatrix.matrix[1][0] = 0;
    cameraMatrix.matrix[1][1] = focal / aspectRatioY;
    cameraMatrix.matrix[1][2] = 0;
    cameraMatrix.matrix[1][3] = principalY;
    cameraMatrix.matrix[2][0] = 0;
    cameraMatrix.matrix[2][1] = 0;
    cameraMatrix.matrix[2][2] = -far / (far - near);
    cameraMatrix.matrix[2][3] = -1;
    cameraMatrix.matrix[3][0] = 0;
    cameraMatrix.matrix[3][1] = 0;
    cameraMatrix.matrix[3][2] = -(far * near) / (far - near);
    cameraMatrix.matrix[3][3] = 0;

    // Here we have to change the coordinates order.
    // Most of our camera projection equations areright handed, with Z facing behind camera.
    // But here is the case were we use the
    TransformationVector position;
    position.vect[0] = -coordInCameraCart.left;
    position.vect[1] = coordInCameraCart.up;
    position.vect[2] = -coordInCameraCart.forward;
    position.vect[3] = 1;

    //	Normalize
    TransformationVector result = position * cameraMatrix;
    if (result.vect[3] != 1.0) result = (1 / result.vect[3]) * result;

    //	Calculate the barrel parameters
    double xCalc = result.vect[0];
    double yCalc = result.vect[1];

    double r2 = (xCalc * xCalc) + (yCalc * yCalc);

    double xBarrelled = xCalc * (1 + (barrelK1 * r2) + (barrelK2 * r2 * r2));
    double yBarrelled = yCalc * (1 + (barrelK1 * r2) + (barrelK2 * r2 * r2));

    // Convert to camera pixels

    cameraX = (xBarrelled + 1) * 0.5 * frameWidthInPixels;
    cameraY = (1 - (yBarrelled)) * 0.5 * frameHeightInPixels;

    return (bInFront);
}


CameraCalibration::CameraCalibration(int inFrameWidthInPixels, int inFrameHeightInPixels,
                                     float inFovWidth, float inFovHeight,
                                     float inFocalLengthX, float inFocalLengthY,
                                     float inCenterX, float inCenterY,
                                     float inRadialK1, float inRadialK2, float inRadialK3,
                                     float inTangentialP1, float inTangentialP2) :
        frameWidthInPixels(inFrameWidthInPixels),
        frameHeightInPixels(inFrameHeightInPixels),
        focalLengthX(inFocalLengthX),
        focalLengthY(inFocalLengthY),
        centerX(inCenterX),
        centerY(inCenterY),
        radialK1(inRadialK1),
        radialK2(inRadialK2),
        radialK3(inRadialK3),
        tangentialP1(inTangentialP1),
        tangentialP2(inTangentialP2) {
}

void CameraCalibration::CalculateFocalLengthsFromFOVs() {
    focalLengthX = 1 / (tan(fovWidth / 2.0));  // Right now, assume square pixels
    focalLengthY = 1 / (tan(fovWidth /
                            2.0)); // Right now, assume square pixels. And symmetrical lens/sensor. This is why we use width on Y axis.
}

bool CameraCalibration::ToFrameXY(const CartesianCoord& coordInCameraCart, int& cameraX, int& cameraY) const {
    bool bInFront = false;

    if (coordInCameraCart.x < 0) {
        bInFront = false;
    } else {
        bInFront = true;
    }


    // Insert all of the known camera parameters
    float far = -(coordInCameraCart.x * 1.5);
    float near = -0.01;

    // Make the Camera perpective transformation matrix
    TransformationMatrix cameraMatrix;
    cameraMatrix.matrix[0][0] = focalLengthX;
    cameraMatrix.matrix[0][1] = 0;
    cameraMatrix.matrix[0][2] = centerX;
    cameraMatrix.matrix[0][3] = 0;
    cameraMatrix.matrix[1][0] = 0;
    cameraMatrix.matrix[1][1] = focalLengthY;
    cameraMatrix.matrix[1][2] = centerY;
    cameraMatrix.matrix[1][3] = 0;
    cameraMatrix.matrix[2][0] = 0;
    cameraMatrix.matrix[2][1] = 0;
    cameraMatrix.matrix[2][2] = -far / (far - near);
    cameraMatrix.matrix[2][3] = -1;
    cameraMatrix.matrix[3][0] = 0;
    cameraMatrix.matrix[3][1] = 0;
    cameraMatrix.matrix[3][2] = -(far * near) / (far - near);
    cameraMatrix.matrix[3][3] = 0;

    // Here we have to change the coordinates order.
    // Most of our camera projection equations are left handed, with X facing outside camera.
    // But our local equations have
    TransformationVector position;
    position.vect[0] = -coordInCameraCart.left;
    position.vect[1] = coordInCameraCart.up;
    position.vect[2] = -coordInCameraCart.forward;
    position.vect[3] = 1;

    //	Normalize
    TransformationVector result = position * cameraMatrix;
    if (result.vect[3] != 1.0) result = (1 / result.vect[3]) * result;
    float xCalc = result.vect[0];
    float yCalc = result.vect[1];

    //	Calculate the radial (barrel/pincushion) distorsion

    float r2 = (xCalc * xCalc) + (yCalc * yCalc);
    float r2Squared = r2 * r2;
    float r2Cubed = r2Squared * r2;

    float xRadial = xCalc * (1 + (radialK1 * r2) + (radialK2 * r2Squared) + (radialK3 * r2Cubed));
    float yRadial = yCalc * (1 + (radialK1 * r2) + (radialK2 * r2Squared) + (radialK3 * r2Cubed));

    // Add the tangential distorsion (keystone)
    float xTangential =
            xRadial + ((2 * tangentialP1 * xRadial * yRadial) + tangentialP2 * (r2 + (2 * xRadial * xRadial)));
    float yTangential =
            yRadial + (tangentialP1 * (r2 + (2 * yRadial * yRadial)) + (2 * tangentialP2 * xRadial * yRadial));

    // Convert to camera pixel

    cameraX = (xTangential + 1) * 0.5 * frameWidthInPixels;
    cameraY = (frameHeightInPixels / 2.0) - (yTangential * frameWidthInPixels /
                                             2.0);  // Remember: we assume square pixels. So we use witdh here also.

    return (bInFront);
}
