#include "common.h"
#include "menu.h"

char dialerStr[15] = {0};

void setMenu(State *s, void (*func)()) {
	s->curMenu = func;

	arrfree(s->menuItems);
	s->lastMenuChoice = s->menuChoice;
	s->menuChoice = 0;
	s->menuOffset = 0;
	s->menuItems = 0;
	s->menuTimer = -1;  // gets incremented to 0 next frame
}

void homeScreenMenu(State *s) {
	if (!s->menuTimer) {
		s->title = "Operator";
		s->leftSoft = "Menu";
		s->rightSoft = "Contacts";
		s->menuType = MT_NONE;
	}

	if (s->keys.lSoft == 1) {
		setMenu(s, mainMenu);
		return;
	}

	DrawRectangle(0, 8, 96, 16, s->dark);

	char dateStr[16];
	char timeStr[8];

	struct tm tm = *localtime(&(time_t){time(NULL)});
	strftime(dateStr, 16, "%a %d %b", &tm);
	strftime(timeStr, 8, "%H:%M", &tm);

	DrawTextEx(s->fonts.l, timeStr, (Vector2) {2, 11}, 12, 0, s->white);
	DrawTextEx(s->fonts.s, dateStr, (Vector2) {36, 12}, 8, 0, s->white);

	#define HOMEKEYPAD(key, chr) \
		if (s->keys.key == 1) { \
			dialerStr[0] = chr; \
			for (int i = 1; i < 15; i++) dialerStr[i] = 0; \
			setMenu(s, dialerMenu); \
			return; \
		}

	HOMEKEYPAD(keypad[1], '1')
	HOMEKEYPAD(keypad[2], '2')
	HOMEKEYPAD(keypad[3], '3')
	HOMEKEYPAD(keypad[4], '4')
	HOMEKEYPAD(keypad[5], '5')
	HOMEKEYPAD(keypad[6], '6')
	HOMEKEYPAD(keypad[7], '7')
	HOMEKEYPAD(keypad[8], '8')
	HOMEKEYPAD(keypad[9], '9')
	HOMEKEYPAD(star, '*')
	HOMEKEYPAD(keypad[0], '0')
	HOMEKEYPAD(hash, '#')
}

void mainMenu(State *s) {
	if (!s->menuTimer) {
		s->title = "Menu";
		s->leftSoft = "Select";
		s->rightSoft = "";
		s->menuType = MT_LIST;

		arrput(s->menuItems, ((MenuItem) {s->textures.none, "Settings"}));
		arrput(s->menuItems, ((MenuItem) {s->textures.none, "Test"}));
	}

	if (s->keys.back == 1) {
		setMenu(s, homeScreenMenu);
		return;
	}

	if (s->keys.lSoft == 1) {
		switch (s->menuChoice) {
			case 0: setMenu(s, settingsMenu); break;
			case 1: setMenu(s, testMenu); break;
		}
		return;
	}
}

void dialerMenu(State *s) {
	if (!s->menuTimer) {
		s->title = "Dialer";
		s->leftSoft = "Message";
		s->rightSoft = "Save";
		s->menuType = MT_POPUP;

		arrput(s->menuItems, ((MenuItem) {s->textures.none, dialerStr}));
	}

	DrawTextEx(s->fonts.l, dialerStr, (Vector2) {3, 11}, 12, 0, s->black);

	#define DIALERKEYPAD(key, chr) \
		if (s->keys.key == 1) { \
			for (int i = 1; i < 15; i++) { \
				if (!dialerStr[i]) { \
					dialerStr[i] = chr; \
					break; \
				} \
			} \
			return; \
		}

	DIALERKEYPAD(keypad[1], '1')
	DIALERKEYPAD(keypad[2], '2')
	DIALERKEYPAD(keypad[3], '3')
	DIALERKEYPAD(keypad[4], '4')
	DIALERKEYPAD(keypad[5], '5')
	DIALERKEYPAD(keypad[6], '6')
	DIALERKEYPAD(keypad[7], '7')
	DIALERKEYPAD(keypad[8], '8')
	DIALERKEYPAD(keypad[9], '9')
	DIALERKEYPAD(star, '*')
	DIALERKEYPAD(keypad[0], '0')
	DIALERKEYPAD(hash, '#')

	if (s->keys.back == 1) {
		if (strlen(dialerStr)) {
			dialerStr[strlen(dialerStr) - 1] = 0;
		}
		if (!strlen(dialerStr)) {
			setMenu(s, homeScreenMenu);
			return;
		}
	}

	if (s->keys.back == 30) {
		setMenu(s, homeScreenMenu);
		return;
	}

	if (!strcmp(dialerStr, "*#0000#")) {
		setMenu(s, versionMenu);
		return;
	}

	if (!strcmp(dialerStr, "*#06#")) {
		setMenu(s, imeiMenu);
		return;
	}
}

void settingsMenu(State *s) {
	if (!s->menuTimer) {
		s->title = "Settings";
		s->leftSoft = "Select";
		s->rightSoft = "";
		s->menuType = MT_LIST;

		arrput(s->menuItems, ((MenuItem) {s->textures.none, "Colors"}));
	}

	if (s->keys.back == 1) {
		setMenu(s, mainMenu);
		return;
	}

	if (s->keys.lSoft == 1) {
		switch (s->menuChoice) {
			case 0: setMenu(s, colorsMenu); break;
		}
		return;
	}
}

void colorsMenu(State *s) {
	if (!s->menuTimer) {
		s->title = "Color setting";
		s->leftSoft = "Select";
		s->rightSoft = "";
		s->menuType = MT_LIST;

		arrput(s->menuItems, ((MenuItem) {s->textures.none, "Color 1"}));
		arrput(s->menuItems, ((MenuItem) {s->textures.none, "Color 2"}));
	}

	if (s->keys.back == 1) {
		setMenu(s, settingsMenu);
		return;
	}

	if (s->keys.lSoft == 1) {
		setMenu(s, setColorMenu);
		return;
	}
}

void setColorMenu(State *s) {
	if (!s->menuTimer) {
		s->title = "Set color";
		s->leftSoft = "";
		s->rightSoft = "";
		s->menuType = MT_LIST;

		arrput(s->menuItems, ((MenuItem) {s->textures.none, ""}));
		arrput(s->menuItems, ((MenuItem) {s->textures.none, ""}));
		arrput(s->menuItems, ((MenuItem) {s->textures.none, ""}));
	}

	Color color = s->lastMenuChoice ? s->light : s->dark;
	s->menuItems[0].text = TextFormat("Red: %d", color.r);
	s->menuItems[1].text = TextFormat("Green: %d", color.g);
	s->menuItems[2].text = TextFormat("Blue: %d", color.b);

	DrawTextEx(s->fonts.s, "left/right to modify", (Vector2) {1, 46}, 8, 0, color);

	if (s->keys.left) {
		switch (s->menuChoice) {
			case 0: if (color.r) color.r--; break;
			case 1: if (color.g) color.g--; break;
			case 2: if (color.b) color.b--; break;
		}
	}

	if (s->keys.right) {
		switch (s->menuChoice) {
			case 0: if (color.r < 255) color.r++; break;
			case 1: if (color.g < 255) color.g++; break;
			case 2: if (color.b < 255) color.b++; break;
		}
	}

	if (s->keys.back == 1) {
		setMenu(s, colorsMenu);
		return;
	}

	if (s->lastMenuChoice) s->light = color;
	else s->dark = color;
}

void testMenu(State *s) {
	if (!s->menuTimer) {
		s->title = "Test menu";
		s->leftSoft = "Select";
		s->rightSoft = "";
		s->menuType = MT_LIST;

		arrput(s->menuItems, ((MenuItem) {s->textures.none, "Keypad test"}));
		arrput(s->menuItems, ((MenuItem) {s->textures.none, "Menu test"}));
	}

	if (s->keys.back == 1) {
		setMenu(s, mainMenu);
		return;
	}

	if (s->keys.lSoft == 1) {
		switch (s->menuChoice) {
			case 0:
				setMenu(s, keyTestMenu);
				break;

			case 1:
				setMenu(s, menuTestMenu);
				break;
		}
		return;
	}
}

void keyTestMenu(State *s) {
	s->title = "Keypad test";
	s->leftSoft = "";
	s->rightSoft = "";
	s->menuType = MT_NONE;

	if (s->keys.back == 1) {
		setMenu(s, testMenu);
		return;
	}

	DrawTextEx(s->fonts.s, "LS RS Call  Up Dn Lt Rt", (Vector2) {0, 8}, 8, 0, s->black);
	DrawTextEx(
		s->fonts.s,
		TextFormat(
			"%d  %d   %d     %d  %d  %d  %d",
			s->keys.lSoft, s->keys.rSoft, s->keys.call,
			s->keys.up, s->keys.down, s->keys.left, s->keys.right
		),
		(Vector2) {0, 16}, 8, 0, s->black
	);

	DrawTextEx(s->fonts.s, "1 2 3  4 5 6  7 8 9  * 0 #", (Vector2) {0, 32}, 8, 0, s->black);
	DrawTextEx(
		s->fonts.s,
		TextFormat(
			"%d %d %d  %d %d %d  %d %d %d  %d %d %d",
			s->keys.keypad[1], s->keys.keypad[2], s->keys.keypad[3],
			s->keys.keypad[4], s->keys.keypad[5], s->keys.keypad[6],
			s->keys.keypad[7], s->keys.keypad[8], s->keys.keypad[9],
			s->keys.star, s->keys.keypad[0], s->keys.hash
		),
		(Vector2) {0, 40}, 8, 0, s->black
	);
}

void menuTestMenu(State *s) {
	if (!s->menuTimer) {
		s->title = "Menu test";
		s->leftSoft = "Select";
		s->rightSoft = "";
		s->menuType = MT_LIST;

		arrput(s->menuItems, ((MenuItem) {s->textures.none, "First"}));
		arrput(s->menuItems, ((MenuItem) {s->textures.none, "Second"}));
		arrput(s->menuItems, ((MenuItem) {s->textures.none, "Third"}));
		arrput(s->menuItems, ((MenuItem) {s->textures.none, "Fourth"}));
		arrput(s->menuItems, ((MenuItem) {s->textures.none, "Fifth"}));
		arrput(s->menuItems, ((MenuItem) {s->textures.none, "Sixth"}));
		arrput(s->menuItems, ((MenuItem) {s->textures.none, "Seventh"}));
	}

	if (s->keys.back == 1) {
		setMenu(s, testMenu);
		return;
	}

	if (s->keys.lSoft == 1) {
		setMenu(s, menuTestDoneMenu);
		return;
	}
}

void menuTestDoneMenu(State *s) {
	if (!s->menuTimer) {
		s->title = "";
		s->leftSoft = "";
		s->rightSoft = "";
		s->menuType = MT_POPUP;

		PlaySound(s->sounds.beep);
		arrput(s->menuItems, ((MenuItem) {s->icons.success, TextFormat("Option %d chosen", s->lastMenuChoice + 1)}));
	}

	if (s->keys.lSoft == 1 || s->keys.rSoft == 1 || s->keys.back == 1 || s->menuTimer > 120) {
		setMenu(s, mainMenu);
		return;
	}
}

void versionMenu(State *s) {
	if (!s->menuTimer) {
		s->title = "";
		s->leftSoft = "";
		s->rightSoft = "";
		s->menuType = MT_POPUP;

		PlaySound(s->sounds.beep);
		arrput(s->menuItems, ((MenuItem) {s->icons.nokia, "This isn't a"}));
		arrput(s->menuItems, ((MenuItem) {s->textures.none, "Nokia!"}));
	}

	if (s->keys.lSoft == 1 || s->keys.rSoft == 1 || s->keys.back == 1 || s->menuTimer > 120) {
		setMenu(s, homeScreenMenu);
		return;
	}
}

void imeiMenu(State *s) {
	if (!s->menuTimer) {
		s->title = "";
		s->leftSoft = "";
		s->rightSoft = "";
		s->menuType = MT_POPUP;

		PlaySound(s->sounds.beep);
		arrput(s->menuItems, ((MenuItem) {s->textures.none, "Modem Failure"}));
		arrput(s->menuItems, ((MenuItem) {s->textures.none, "Contact Service"}));
	}

	if (s->keys.lSoft == 1 || s->keys.rSoft == 1 || s->keys.back == 1 || s->menuTimer > 120) {
		setMenu(s, imei2Menu);
		return;
	}
}

void imei2Menu(State *s) {
	s->title = "Dialer";
	s->leftSoft = "Modem Failure";
	s->rightSoft = "Contact Service";
	s->menuType = MT_NONE;
}