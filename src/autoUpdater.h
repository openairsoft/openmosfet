#ifndef AUTOUPDATER_H
#define AUTOUPDATER_H

#include <Arduino.h>

class OMAutoUpdater {

  public:
    enum AutoUpdaterState
    {
      stateIdle,
      stateUpdateRequested,
      stateUpdating,
      stateSucceeded,
      stateFailed
    };

  private:
    static OMAutoUpdater::AutoUpdaterState _state;
    static void updateFromGit();

  public:
    /**
     * This method does not update the chip, it is used for the loop, same as other components
     **/
    static void update();
    static void requestUpdate();
    static OMAutoUpdater::AutoUpdaterState getState(void){ return OMAutoUpdater::_state; }
};

#endif 