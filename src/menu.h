#ifndef MENU_H
#define MENU_H

#include "common.h"

void setMenu(State *s, void (*func)());

void startupMenu(State *s);
void bootAnimMenu(State *s);
void shutAnimMenu(State *s);
void batteryLowMenu(State *s);

void homeScreenMenu(State *s);
void mainMenu(State *s);
void dialerMenu(State *s);

void testMenu(State *s);
void keyTestMenu(State *s);
void menuTestMenu(State *s);
void menuTestDoneMenu(State *s);

void settingsMenu(State *s);
void colorsMenu(State *s);
void setColorMenu(State *s);

void versionMenu(State *s);
void version2Menu(State *s);
void imeiMenu(State *s);
void imei2Menu(State *s);

#endif // menu.h