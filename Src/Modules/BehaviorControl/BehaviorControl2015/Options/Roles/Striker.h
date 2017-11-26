#include "Modules/BehaviorControl/BehaviorControl2015/Options/HeadControl/HeadControl.h"
#include "Modules/BehaviorControl/BehaviorControl2015/Options/HeadControl/ScanLeftRight.h"

/** A test striker option without common decision */
option(Striker)
{
  initial_state(start)
  {
    transition
    {
      if(state_time > 1000)
        goto turnToBall;
    }
    action
    {
      LookForward();
      Stand();
    }
  }

  state(turnToBall)
  {
    transition
    {
      if(libCodeRelease.timeSinceBallWasSeen() > theBehaviorParameters.ballNotSeenTimeOut)
        goto lookForBall;
      if(std::abs(theBallModel.estimate.position.angle()) < 5_deg)
        goto walkToBall;
    }
    action
    {
      LookForward();
      WalkToTarget(Pose2f(50.f, 50.f, 50.f), Pose2f(theBallModel.estimate.position.angle(), 0.f, 0.f));
    }
  }

  state(walkToBall)
  {
    transition
    {
      if(libCodeRelease.timeSinceBallWasSeen() > theBehaviorParameters.ballNotSeenTimeOut)
        goto lookForBall;
      if(theBallModel.estimate.position.norm() < 500.f)
        goto alignToGoal;
    }
    action
    {
      LookForward();
      WalkToTarget(Pose2f(50.f, 50.f, 50.f), theBallModel.estimate.position);
    }
  }

  state(alignToGoal)
  {
    transition
    {
      if(libCodeRelease.timeSinceBallWasSeen() > theBehaviorParameters.ballNotSeenTimeOut)
        goto lookForBall;
      if(std::abs(libCodeRelease.angleToOppGoal) < 10_deg && std::abs(theBallModel.estimate.position.y()) < 100.f)
        goto alignBehindBall;
    }
    action
    {
      LookForward();
      WalkToTarget(Pose2f(100.f, 100.f, 100.f), Pose2f(libCodeRelease.angleToOppGoal, theBallModel.estimate.position.x() - 400.f, theBallModel.estimate.position.y()));
    }
  }

  state(alignBehindBall)
  {
    transition
    {
      if(libCodeRelease.timeSinceBallWasSeen() > theBehaviorParameters.ballNotSeenTimeOut)
        goto lookForBall;
      if(libCodeRelease.between(theBallModel.estimate.position.y(), 20.f, 50.f)
          && libCodeRelease.between(theBallModel.estimate.position.x(), 140.f, 170.f)
          && std::abs(libCodeRelease.angleToOppGoal) < 2_deg)
        goto kick;
    }
    action
    {
      LookForward();
      WalkToTarget(Pose2f(80.f, 80.f, 80.f), Pose2f(libCodeRelease.angleToOppGoal, theBallModel.estimate.position.x() - 150.f, theBallModel.estimate.position.y() - 30.f));
    }
  }

  state(kick)
  {
    transition
    {
      if(state_time > 3000 || (state_time > 10 && action_done))
        goto start;
    }
    action
    {
      LookForward();
      InWalkKick(WalkKickVariant(WalkKicks::forward, Legs::left), Pose2f(libCodeRelease.angleToOppGoal, theBallModel.estimate.position.x() - 160.f, theBallModel.estimate.position.y() - 55.f));
    }
  }
  state(lookForBall)
  {
    transition
    {
      if(libCodeRelease.timeSinceBallWasSeen() < 200)
        goto turnToBall;
	  if(libCodeRelease.timeSinceBallWasSeen() > 20000)
        goto searchForBall;
    }
    action
    {
	  /**ThatHeadControl::Searching;**/
	  /**SetHeadPanTilt(-1.f, .5f, 2.f );**/
		if (libCodeRelease.timeSinceBallWasSeen() < 4000)
			{
		 SetHeadPanTilt(1.f, .5f, .75f );
			}
		if (libCodeRelease.timeSinceBallWasSeen() > 10000 and libCodeRelease.timeSinceBallWasSeen()< 15000)
			{
		 SetHeadPanTilt(-1.f, .5f, .75f );
			}
		if (libCodeRelease.timeSinceBallWasSeen() > 15000 ) 
			{
		 SetHeadPanTilt(1.f, .5f, .75f );  
			}
      WalkAtSpeedPercentage(Pose2f(0.f, 0.f, 0.f));
      /**WalkAtRelativeSpeed(Pose2f(0.f, 0.f, 0.f));**/
    }
  }
  state(searchForBall)
  {
    transition
    {
      if(libCodeRelease.timeSinceBallWasSeen() < 300)
        goto turnToBall;
    }
    action
    {
      LookForward();
      WalkAtSpeedPercentage(Pose2f(0.75f, 0.f, 0.f));
    }
  }
}
