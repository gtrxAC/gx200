#ifndef COMMON_H
#define COMMON_H

#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include "raylib.h"
#include "stb_ds.h"

typedef enum MenuType {
	// Menu has no common behavior, all drawing and updating is handled by
	// the menu update function.
	MT_NONE,

	// Menu is a message popup which has a border around it. Dismissing the
	// popup is handled by the update function.
	MT_POPUP,

	// Menu is a list of options that can be scrolled. Selecting is handled
	// by the update function.
	MT_LIST
} MenuType;

typedef struct MenuItem {
	// What icon to show for this menu item. This is optional and can be set to
	// s->textures.none.
	Texture icon;

	// What text to show for this menu item.
	const char *text;
} MenuItem;

// Global state, contains info about the current menu, as well as loaded assets.
typedef struct State {
	// Menu update function, called every frame.
	void (*curMenu)();

	// Common menu UI elements.
	const char *title;
	const char *leftSoft;
	const char *rightSoft;

	MenuType menuType;

	// Dynamic array of menu items. In MT_POPUP, each menu item is one line
	// of text. The first menu item's icon is the popup's icon.
	MenuItem *menuItems;

	// Menu choice from the previous menu, can be used to pass data from one
	// menu to another.
	int lastMenuChoice;

	// Currently chosen menu option, only applies for MT_LIST.
	int menuChoice;

	// MT_LIST menu scroll offset, used when there are more menu items than what
	// can fit on the screen.
	int menuOffset;

	// How many frames the current menu has been open for.
	int menuTimer;

	// Color settings, changed from the phone's settings menu.
	// These are assigned defaults during initialization.
	Color black;
	Color dark;
	Color light;
	Color white;

	// Keypad state. Each value indicates how many frames that button has been
	// pressed for, 0 if not pressed.
	struct {
		int lSoft, rSoft;
		int call, back;
		int up, down, left, right;
		int keypad[10];
		int star, hash;
	} keys;

	// Loaded assets.
	struct {
		Texture none;
		Texture skin;

		Texture statusbar;
		Texture popup;

		Texture bootAnim;
		Texture shutAnim;
	} textures;

	struct {
		Texture nokia;
		Texture success;
	} icons;

	struct {
		Font s;  // small
		Font m;  // medium
		Font l;  // large
	} fonts;

	struct {
		Sound batteryLow;
		Sound beep;
		Sound keypad;
	} sounds;
} State;

#endif // common.h