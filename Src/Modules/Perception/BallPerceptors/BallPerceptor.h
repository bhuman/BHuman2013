/**
 * @file BallPerceptor.h
 * This file declares a module that detects balls in CNS images.
 * @author Thomas Röfer
 * @author Udo Frese
 * @author <a href="mailto:jesse@tzi.de">Jesse Richter-Klug</a>
 */

#pragma once

#include "Representations/BehaviorControl/Role.h"
#include "Representations/Configuration/BallSpecification.h"
#include "Representations/Configuration/FieldDimensions.h"
#include "Representations/Infrastructure/CameraInfo.h"
#include "Representations/Infrastructure/CameraIntrinsics.h"
#include "Representations/Infrastructure/DebuggingOutput.h"
#include "Representations/Modeling/RobotPose.h"
#include "Representations/Modeling/WorldModelPrediction.h"
#include "Representations/MotionControl/MotionInfo.h"
#include "Representations/Perception/BallPercepts/BallPercept.h"
#include "Representations/Perception/BallPercepts/BallSpots.h"
#include "Representations/Perception/ImagePreprocessing/BodyContour.h"
#include "Representations/Perception/ImagePreprocessing/CameraMatrix.h"
#include "Representations/Perception/ImagePreprocessing/CNSImage.h"
#include "Representations/Perception/ImagePreprocessing/ECImage.h"
#include "Representations/Perception/ImagePreprocessing/ImageCoordinateSystem.h"
#include "Representations/Perception/ImagePreprocessing/ImageRegions.h"
#include "Tools/ImageProcessing/CNS/ObjectCNSStereoDetector.h"
#include "Tools/Math/Eigen.h"
#include "Tools/Module/Module.h"
#include "Tools/Streams/OutStreams.h"
#include <unordered_set>

MODULE(BallPerceptor,
{,
  USES(Role),
  USES(RobotPose),
  REQUIRES(MotionInfo),
  REQUIRES(BallRegions),
  REQUIRES(BallSpecification),
  REQUIRES(BallSpots),
  REQUIRES(BodyContour),
  REQUIRES(CameraInfo),
  REQUIRES(CameraIntrinsics),
  REQUIRES(CameraMatrix),
  REQUIRES(CNSImage),
  REQUIRES(ECImage),
  REQUIRES(FieldDimensions),
  REQUIRES(ImageCoordinateSystem),
  REQUIRES(WorldModelPrediction),
  REQUIRES(BallPercept),
  PROVIDES(BallPercept),
  PROVIDES(DebuggingOutput),
  LOADS_PARAMETERS(
  {,
    (int) contourSubDivisions, /**< The number of times triangles of a cube are split to form a sphere as ball model for contour computation. */
    (float) maxTableRadius, /**< The maximum distance from the robot covered by the precomputed ball contour table. */
    (float) minTableHeight, /**< The minimum height of the camera covered by the precomputed ball contour table. */
    (float) maxTableHeight, /**< The maximum height of the camera covered by the precomputed ball contour table. */
    (float) spacing, /**< The spatial descretization of the precomputed ball contour table. */
    (int) sampleSubDivisions, /**< The number of times triangles of a cube are split to form a sphere as ball model for determining sample points. */
    (float) sampleDepthRatio, /**< Depth clipping of sample points. Larger means less points. */
    (float) sampleHeightRatio, /**< Height clipping of sample points. Larger means more points. */
    (Angle) sampleRange, /**< Rotations are sampled in the range of [-sampleRange .. sampleRange[ when computing the ball patterns. */
    (Angle) sampleStep, /**< The sampling is done in these step width when computing the ball patterns. */
    (bool) usePrediction, /**< Include the ball prediction as a starting point for the search? */
    (int) refineIterations, /**< The number of refinements performed after the global search. */
    (float) refineStepSize, /**< The step size during refinement (in pixels). */
    (float) minResponse, /**< The minimum response returned by the contour detector required for a ball candidate. */
    (float) maxResponse, /**< The response required if other criteria are not satisfied by a ball candidate. */
    (float) minResponseWithinGreen, /**< Minimum response required to accept a ball just because it is surrounded by green. */
    (float) maxRadiusDeviation, /**< The maximum deviation between measured and expected ball radius allowed. */
    (std::vector<float>) greenCheckRadiusRatios, /**< The additional radius (to the ball radius) to define the search rectangle for the "all green around" - check */
    (int) numberOfGreenChecks, /**< The number of pixels checked per scan around the ball. */
    (float) minAroundGreenRatio, /**< The minimum ratio of pixels that must be green when scanning around the ball. */
    (float) minAroundNonWhiteRatio, /**< The minimum ratio of pixels that must be green when scanning around the ball. */
    (float) minAroundDistance, /**< The minimum distance from which balls are accepted just because they are surrounded by green. */
    (float) maxAroundDistance, /**< The maximum distance up to which balls are accepted just because they are surrounded by green. */
    (float) brightnessBonus, /**< Increase brightness based on the height of a point on the ball surface. */
    (float) minContrast, /**< Minimum contrast between black and white parts of the ball. */
    (bool) skipMostChecks, /**< Accept the object with the highest response that the response of which was suffcient and the radius within tolerance. */
  }),
});

class BallPerceptor : public BallPerceptorBase
{
private:
  ObjectCNSStereoDetector detector; /**< The detector. */
  ObjectCNSStereoDetector::IsometryWithResponses objects; /**< The poses of the detected objects in the coordinate system of the camera and the responses. */
  SearchSpecification spec; /**< The current search specification. */
  mutable OutTextRawMemory* tip = nullptr; /**< Used to collect text for the tool tips of debug drawings. */
  mutable std::vector<std::string> tips; /**< Collecting all tips for debugging. */
  bool recordTips = false; /**< Flag to record tips as debugging output. */
  std::vector<Vector3d> samplePoints; /**< The 3-D points of a ball candidate sampled for the surface pattern. */

  static const int ballTextureWidth = 300; /**< Width of the texture the ball patterns are generated from (in pixels). */
  static const int ballTextureHeight = 150; /**< Height of the texture the ball patterns are generated from (in pixels). */

  using BallPattern = unsigned; /**< The type of a ball pattern. */
  std::unordered_set<BallPattern> ballPatterns; /**< Is the bit pattern a valid ball pattern? */

  /**
   * The main method of this module.
   * @param ballPercept The percept that is filled by this module.
   */
  void update(BallPercept& ballPercept);

  /**
   * Helper to fill loggable debugging output.
   * @param debuggingOutput The debugging output text,
   */
  void update(DebuggingOutput& debuggingOutput) {recordTips = true; debuggingOutput.text = tips;}

  /**
   * The method updates the search space relative to the pose of the camera.
   */
  void updateSearchSpace();

  /**
   * Performs several tests to determine whether a ball candidate
   * can be an actual ball.
   * @param object The object found to check.
   * @return Could it be the ball?
   */
  BallPercept::Status checkBall(const IsometryWithResponse& object) const;

  /**
   * The method counts how much the ball is surrounded by enough green.
   * @param center The assumed center of the ball candidate.
   * @param radius The expected ball radius at that point in the image.
   * @param greenCount The number of green pixels found is returned here.
   * @param nonWhiteCount The number of non-white pixels found is returned here.
   */
  void countAround(const Vector2i& center, float radius,
                   int& greenCount, int& nonWhiteCount) const;

  /**
   * The method counts how much the ball is surrounded by green
   * for a single radius.
   * @param center The assumed center of the ball candidate.
   * @param radius The radius that is scanned around the center.
   * @param greenCount The number of green pixels found is added here.
   * @param nonWhiteCount The number of non-white pixels found is added here.
   */
  void countAtRadius(const Vector2i& center, float radius,
                     int& greenCount, int& nonWhiteCount) const;

  /**
   * Check whether an object has the correct black and white pattern to
   * be the official ball.
   * @param object The object to check.
   * @param accepted If drawn, draw as accepted pattern.
   * @param brightnesses The brightesses sampled are returned here.
   * @param threshold The threshold between dark and bright is returned here.
   * @param pattern The pattern determined is returned here.
   * @return Could it be the ball?
   */
  bool checkSamplePoints(const IsometryWithResponse& object, bool accepted, std::vector<unsigned char>& brightnesses, int& threshold, BallPattern& pattern) const;

  /**
   * Determine the threshold separating the brightnesses into two classes
   * using Otsu's method. There are a few optimizations for the histogram
   * being very sparse.
   * @param brigtnesses The brightnesses measured.
   * @return The best threshold to split them into two classes.
   */
  static int calcThreshold(const std::vector<unsigned char>& brightnesses);

  /**
   * Checks whether there is enough contrast between the dark samples and the
   * bright samples.
   * @param brightnesses The brighnesses sampled.
   * @param threshold the threshold between dark and bright.
   * @return Is there enough contrast?
   */
  bool checkContrast(const std::vector<unsigned char>& brightnesses, int threshold) const;

  /**
   * Determines whether the first parameter is less or equal to the
   * second parameter during the ball check.
   * The method can also output data for a debug drawing if requested.
   * @param op1 The first argument of the comparison.
   * @param op2 The second argument of the comparison.
   * @param label A label that is used for the debug output.
   * @return Is the first argument less or equal to the second argument?
   */
  bool leq(float op1, float op2, const char* label) const;

  /**
   * Adds this result to the debug output during the ball check if
   * requested.
   * @param result The result of the check.
   * @param label A label that is used for the debug output.
   * @return The result.
   */
  bool print(bool result, const char* label) const;

  /**
   * Fills in all the fields of the ball percept.
   * @param ballPercept The representation that is filled.
   */
  void fillBallPercept(BallPercept& ballPercept) const;

  /**
   * Create or load the ball pattern table from the ball texture.
   */
  void createOrLoadPatternTable();

  /**
   * Determine a ball pattern from the sample points and the ball texture for a
   * rotation.
   * @param ballTexture The texture to get the pattern from.
   * @param x The rotation around the x axis.
   * @param y The rotation around the y axis.
   * @param z The rotation around the z axis.
   * @return The pattern. 0 bits mean black and 1 bits mean white.
   */
  BallPattern getPattern(unsigned char ballTexture[ballTextureHeight][ballTextureWidth], float x, float y, float z) const;

  /**
   * Determine the brightness of the ball at a point on its surface.
   * @param ballTexture The texture to get the pattern from.
   * @param v The vector from the center of the ball to the surface point
   *          that is sampled.
   * @return The brightness at the surface point.
   */
  static unsigned char getBrightness(unsigned char ballTexture[ballTextureHeight][ballTextureWidth], const Vector3d& v);

  // Drawing methods for debugging
  void draw();
  void drawContourViaLutRasterizer(const LutRasterizer& lr, const Eigen::Isometry3d& object2World, const CameraModelOpenCV& camera, const ColorRGBA& color) const;
  void drawRasteredContour(const Contour& contour, const ColorRGBA& color) const;
  void drawCylinderRing(const CylinderRing& cylinderRing, const CameraModelOpenCV& camera) const;

public:
  BallPerceptor();
};
