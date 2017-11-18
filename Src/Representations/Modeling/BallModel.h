/**
 * @file BallModel.h
 *
 * Declaration of struct BallModel
 *
 * @author <A href="mailto:timlaue@informatik.uni-bremen.de">Tim Laue</A>
 */

#pragma once

#include "Tools/Streams/AutoStreamable.h"
#include "Tools/Math/Eigen.h"

/**
 * @struct BallState
 *
 * Base struct for ball position and velocity.
 */
STREAMABLE(BallState,
{,
  (Vector2f)(Vector2f::Zero()) position,      /**< The position of the ball relative to the robot (in mm)*/
  (Vector2f)(Vector2f::Zero()) velocity,      /**< The velocity of the ball relative to the robot (in mm/s)*/
  (float)(0) rotation,                        /**< The rotation of the ball (in rad/s)*/
  (float)(30) radius,                         /**< The assumed radius of the ball (in mm)*/
  (Matrix2f)(Matrix2f::Identity()) covariance, /**< The covariance matrix of the ball*/
});

/**
 * @struct BallModel
 *
 * Contains all current knowledge about the ball.
 */
STREAMABLE(BallModel,
{
  /** Verifies that the ball model contains valid values. */
  void verify() const;
  /** Draws the estimate on the field */
  void draw() const,

  (Vector2f)(Vector2f::Zero()) lastPerception, /**< The last seen position of the ball */
  (BallState) estimate, /**< The state of the ball estimated from own observations; it is propagated even if the ball is not seen */
  (unsigned)(0) timeWhenLastSeen, /**< Time stamp, indicating what its name says */
  (unsigned)(0) timeWhenDisappeared, /**< The time when the ball was not seen in the image altough it should have been there */
  (unsigned char)(0) seenPercentage, /**< How often was the ball seen in the recent past (0%...100%). */
});

/**
 * @struct GroundTruthBallModel
 * The same as the BallModel, but - in general - provided by an external
 * source that has ground truth quality
 */
struct GroundTruthBallModel : public BallModel
{
  /** Draws something*/
  void draw() const;
};

/**
 * @struct BallModelCompressed
 * A compressed version of BallModel used in team communication
 */
STREAMABLE(BallModelCompressed,
{
  BallModelCompressed() = default;
  BallModelCompressed(const BallModel& ballModel);
  operator BallModel() const,

  (Vector2s) lastPerception, /**< The last seen position of the ball */
  (Vector2f) position,
  (Vector2f) velocity,
  (unsigned) timeWhenLastSeen, /**< Time stamp, indicating what its name says */
  (unsigned) timeWhenDisappeared, /**< The time when the ball was not seen in the image altough it should have been there */
  (std::array<float, 3>) covariance,
});

/**
 * @struct BallState
 *
 * Base struct for ball position and velocity.
 */
STREAMABLE(BallState3D,
{,
  (Vector3f)(Vector3f::Zero()) position, /**< The position of the ball relative to the robot (in mm)*/
  (Vector3f)(Vector3f::Zero()) velocity, /**< The velocity of the ball relative to the robot (in mm/s)*/
  (float)(35) radius,                    /**< The assumed radius of the ball (in mm)*/
});

/**
 * @struct BallModel
 *
 * Contains all current knowledge about the ball.
 */
STREAMABLE(BallModel3D,
{
  /** Draws the estimate on the field */
  void draw() const,

  (Vector3f)(Vector3f::Zero()) lastPerception, /**< The last seen position of the ball */
  (BallState3D) estimate, /**< The state of the ball estimated from own observations; it is propagated even if the ball is not seen */
  (unsigned)(0) timeWhenLastSeen, /**< Time stamp, indicating what its name says */
  (unsigned)(0) timeWhenDisappeared, /**< The time when the ball was not seen in the image altough it should have been there */
  (unsigned char)(0) seenPercentage, /**< How often was the ball seen in the recent past (0%...100%). */
});
