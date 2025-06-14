#ifndef DFPLAYER_H
#define DFPLAYER_H

#include "pins.h"
#include "lcd.h"
#include "gsm.h"
#include <DFRobotDFPlayerMini.h>

void initDFPlayer();
void tryToConnectDFPlayer(int maxAttempts = 5);
void playNextTrack();

#endif