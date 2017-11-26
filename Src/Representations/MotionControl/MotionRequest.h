/**
 * @file Representations/MotionControl/MotionRequest.h
 * This file declares a struct that represents the motions that can be requested from the robot.
 * @author <A href="mailto:Thomas.Roefer@dfki.de">Thomas Röfer</A>
 */

#pragma once

#include "SpecialActionRequest.h"
#include "WalkRequest.h"
#include "KickRequest.h"
#include "DiveLeftRequest.h"
#include "DiveRightRequest.h"
#include "SumoRequest.h"

/**
 * @struct MotionRequest
 * A struct that represents the motions that can be requested from the robot.
 */
STREAMABLE(MotionRequest,
{
  ENUM(Motion,
  {,
    walk,
    kick,
    specialAction,
    stand,
    getUp,
    diveLeft,
    diveRight,
    sumo,	
  });

  /** Draws something*/
  void draw() const,

  (Motion)(specialAction) motion, /**< The selected motion. */
  (SpecialActionRequest) specialActionRequest, /**< The special action request, if it is the selected motion. */
  (WalkRequest) walkRequest, /**< The walk request, if it is the selected motion. */
  (KickRequest) kickRequest, /**< The kick request, if it is the selected motion. */
  (DiveLeftRequest) diveLeftRequest,
  (DiveRightRequest) diveRightRequest,
  (SumoRequest) sumoRequest,
});

struct BehaviorMotionRequest : public MotionRequest {};
