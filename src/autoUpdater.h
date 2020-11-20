#ifndef AUTOUPDATER_H
#define AUTOUPDATER_H

#include <Arduino.h>

class OMAutoUpdater {

  private:

    static boolean isUpdateRequested;
    static void updateFromGit();

  public:

    /**
     * This method does not update the chip, it is used for the loop, same as other components
     **/
    static void update();
    static void requestUpdate();
};

#endif 