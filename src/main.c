#define STB_DS_IMPLEMENTATION
#include "common.h"
#include "menu.h"

RenderTexture rt;
RenderTexture canvas;

void loadAssets(State *s) {
	ChangeDirectory("assets");

	s->textures.skin = LoadTexture("skin.png");

	s->textures.statusbar = LoadTexture("statusbar.png");
	s->textures.charging = LoadTexture("charging.png");
	s->textures.popup = LoadTexture("popup.png");

	s->textures.bootAnim = LoadTexture("boot_anim.png");
	s->textures.shutAnim = LoadTexture("shut_anim.png");

	s->icons.batteryLow = LoadTexture("icons/battery_low.png");
	s->icons.nokia = LoadTexture("icons/nokia.png");
	s->icons.raylib = LoadTexture("icons/raylib.png");
	s->icons.success = LoadTexture("icons/success.png");

	s->icons.scrollUp = LoadTexture("icons/scroll_up.png");
	s->icons.scrollDown = LoadTexture("icons/scroll_down.png");

	s->fonts.s = LoadFontEx("small.ttf", 8, NULL, 0);
	s->fonts.m = LoadFontEx("medium.ttf", 10, NULL, 0);
	s->fonts.l = LoadFontEx("large.ttf", 12, NULL, 0);

	s->sounds.batteryLow = LoadSound("audio/battery_low.wav");
	s->sounds.beep = LoadSound("audio/beep.wav");
	s->sounds.keypad = LoadSound("audio/keypad.wav");

	ChangeDirectory("..");
}

void unloadAssets(State *s) {
	UnloadTexture(s->textures.skin);

	UnloadTexture(s->textures.statusbar);
	UnloadTexture(s->textures.charging);
	UnloadTexture(s->textures.popup);

	UnloadTexture(s->textures.bootAnim);
	UnloadTexture(s->textures.shutAnim);

	UnloadTexture(s->icons.nokia);
	UnloadTexture(s->icons.raylib);
	UnloadTexture(s->icons.success);

	UnloadTexture(s->icons.scrollUp);
	UnloadTexture(s->icons.scrollDown);

	UnloadFont(s->fonts.s);
	UnloadFont(s->fonts.m);
	UnloadFont(s->fonts.l);

	UnloadSound(s->sounds.batteryLow);
	UnloadSound(s->sounds.beep);
	UnloadSound(s->sounds.keypad);
}

void updateKeys(State *s) {
	if (IsKeyPressed(KEY_C)) {
		s->charging = !s->charging;
		if (s->charging) {
			SetWindowTitle("GX200 Emulator");
			if (!s->battery) s->battery = 1;
		}
	}
	
	if (IsMouseButtonUp(MOUSE_BUTTON_LEFT)) {
		s->keys.lSoft = 0;
		s->keys.rSoft = 0;
		s->keys.call = 0;
		s->keys.back = 0;
		s->keys.up = 0;
		s->keys.down = 0;
		s->keys.left = 0;
		s->keys.right = 0;
		for (int i = 0; i < 10; i++) s->keys.keypad[i] = 0;
		s->keys.star = 0;
		s->keys.hash = 0;
		return;
	}

	#define CHECKKEY(rect, prop) \
		if (CheckCollisionPointRec((Vector2) {GetTouchX(), GetTouchY()}, rect)) { \
			if (++s->keys.prop == 1) PlaySound(s->sounds.keypad); \
			DrawTextureRec(s->textures.skin, rect, (Vector2) {rect.x, rect.y}, (Color) {224, 224, 224, 255}); \
		} else { \
			s->keys.prop = 0; \
		}

	CHECKKEY(((Rectangle) {34, 335, 154, 39}), lSoft)
	CHECKKEY(((Rectangle) {196, 335, 154, 39}), rSoft)
	CHECKKEY(((Rectangle) {25, 398, 106, 61}), call)
	CHECKKEY(((Rectangle) {253, 398, 106, 61}), back)
	CHECKKEY(((Rectangle) {173, 383, 38, 38}), up)
	CHECKKEY(((Rectangle) {173, 437, 38, 38}), down)
	CHECKKEY(((Rectangle) {146, 410, 38, 38}), left)
	CHECKKEY(((Rectangle) {200, 410, 38, 38}), right)
	CHECKKEY(((Rectangle) {40, 482, 97, 54}), keypad[1])
	CHECKKEY(((Rectangle) {143, 482, 97, 54}), keypad[2])
	CHECKKEY(((Rectangle) {247, 482, 97, 54}), keypad[3])
	CHECKKEY(((Rectangle) {40, 548, 97, 54}), keypad[4])
	CHECKKEY(((Rectangle) {143, 548, 97, 54}), keypad[5])
	CHECKKEY(((Rectangle) {247, 548, 97, 54}), keypad[6])
	CHECKKEY(((Rectangle) {40, 614, 97, 54}), keypad[7])
	CHECKKEY(((Rectangle) {143, 614, 97, 54}), keypad[8])
	CHECKKEY(((Rectangle) {247, 614, 97, 54}), keypad[9])
	CHECKKEY(((Rectangle) {40, 680, 97, 54}), star)
	CHECKKEY(((Rectangle) {143, 680, 97, 54}), keypad[0])
	CHECKKEY(((Rectangle) {247, 680, 97, 54}), hash)
}

void update(State *s) {
	if (s->battery < 1 && s->curMenu != shutAnimMenu)
		setMenu(s, shutAnimMenu);

	if (!s->power) {
		if (s->keys.back == 60) {
			s->power = true;
			setMenu(s, startupMenu);
		}
		return;
	}

	if (--s->batteryTimer < 1) {
		if (s->charging) {
			s->batteryTimer = GetRandomValue(250, 400);
			if (s->battery < 5) s->battery++;
		} else {
			s->batteryTimer = GetRandomValue(900, 1100);
			if (--s->battery == 1) setMenu(s, batteryLowMenu);
		}
	}

	s->menuTimer++;
	if (s->curMenu) s->curMenu(s);

	switch (s->menuType) {
		case MT_LIST:
			if (s->keys.up == 1 && s->menuChoice > 0) {
				if (--s->menuChoice == s->menuOffset - 1) s->menuOffset--;
			}
			if (s->keys.down == 1 && s->menuChoice < arrlen(s->menuItems) - 1) {
				if (++s->menuChoice == s->menuOffset + 4) s->menuOffset++;
			}
			break;

		case MT_TEXT:
			if (s->keys.up == 1 && s->menuChoice > 0) {
				s->menuChoice--;
			}
			if (s->keys.down == 1 && s->menuChoice < arrlen(s->menuItems) - 1) {
				s->menuChoice++;
			}
			break;
	}
}

void drawUI(State *s) {
	// Status bar
	DrawRectangle(0, 0, 96, 8, s->white);

	int srcWidth = 81;

	// Blinking battery bar when battery is low
	if (s->battery > 1 || s->charging || s->batteryTimer % 40 < 20)
		srcWidth += s->battery*3;

	DrawTextureRec(s->textures.statusbar, (Rectangle) {0, 0, srcWidth, 8}, (Vector2) {0, 0}, s->dark);

	if (s->charging) DrawTexture(s->textures.charging, 0, 0, s->dark);

	DrawTextEx(s->fonts.s, s->title, (Vector2) {18, 0}, 8, 0, s->dark);

	// Softkeys
	DrawRectangle(0, 56, 96, 12, s->white);
	DrawTextEx(s->fonts.l, s->leftSoft, (Vector2) {1, 56}, 12, 0, s->black);

	Vector2 rSoftSize = MeasureTextEx(s->fonts.l, s->rightSoft, 12, 0);
	DrawTextEx(s->fonts.l, s->rightSoft, (Vector2) {96 - rSoftSize.x, 56}, 12, 0, s->black);

	// Scroll indicators
	if (s->menuType == MT_LIST || s->menuType == MT_TEXT) {
		if (s->menuChoice != 0) {
			DrawTexture(s->icons.scrollUp, 42, 56, s->light);
		}
		if (s->menuChoice < arrlen(s->menuItems) - 1) {
			DrawTexture(s->icons.scrollDown, 48, 62, s->light);
		}
	}
}

void draw(State *s) {
	if (!s->power) {
		ClearBackground(s->black);
		return;
	}

	ClearBackground(s->white);

	switch (s->menuType) {
		case MT_LIST:
			for (int i = s->menuOffset, y = 8; i < arrlen(s->menuItems); i++, y += 12) {
				int x = 1;
				if (s->menuItems[i].icon.id) x += s->menuItems[i].icon.width + 1;

				if (i == s->menuChoice) {
					DrawRectangle(0, y, 96, 12, s->dark);
					DrawTextEx(s->fonts.m, s->menuItems[i].text, (Vector2) {x, y + 1}, 10, 0, s->white);
				} else {
					DrawTextEx(s->fonts.s, s->menuItems[i].text, (Vector2) {x, y + 2}, 8, 0, s->black);
				}

				if (s->menuItems[i].icon.id) DrawTexture(s->menuItems[i].icon, 1, y + 1, WHITE);
			}
			break;

		case MT_POPUP: {
			DrawTexture(s->textures.popup, 0, 8, WHITE);
			if (!arrlen(s->menuItems)) break;

			int x = 3;
			if (s->menuItems[0].icon.id) {
				x += s->menuItems[0].icon.width + 4;
				DrawTexture(s->menuItems[0].icon, 3, 12, WHITE);
			}

			for (int i = 0; i < arrlen(s->menuItems); i++) {
				DrawTextEx(s->fonts.s, s->menuItems[i].text, (Vector2) {x, 12 + i*10}, 8, 0, s->black);
			}
			break;
		}

		case MT_TEXT: {
			if (!arrlen(s->menuItems)) break;

			int y = 9;
			if (s->menuItems[s->menuChoice].icon.id) {
				DrawTexture(s->menuItems[s->menuChoice].icon, 1, 8, WHITE);
				y += s->menuItems[s->menuChoice].icon.height + 1;
			}

			DrawTextEx(s->fonts.s, s->menuItems[s->menuChoice].text, (Vector2) {1, y}, 8, 0, s->black);
			break;
		}
	}
}

int main() {
	State *s = calloc(1, sizeof(State));

	SetConfigFlags(FLAG_WINDOW_TRANSPARENT);
	InitWindow(384, 768, "GX200 Emulator");
	InitAudioDevice();
	SetTargetFPS(60);
	loadAssets(s);

	#ifdef PLATFORM_DESKTOP
		Image icon = LoadImage("assets/icon.png");
		ImageFormat(&icon, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8);
		SetWindowIcon(icon);
		UnloadImage(icon);
	#endif

	rt = LoadRenderTexture(96, 68);
	setMenu(s, startupMenu);

	s->black = (Color) {18, 16, 14, 255};
	s->dark = (Color) {64, 48, 192, 255};
	s->light = (Color) {128, 144, 255, 255};
	s->white = (Color) {255, 240, 224, 255};

	s->battery = 6;

	while (!WindowShouldClose()) {
		BeginTextureMode(rt);
		update(s);
		draw(s);
		if (s->drawUI) drawUI(s);
		EndTextureMode();

		BeginDrawing();
		ClearBackground(BLANK);
		DrawTexture(s->textures.skin, 0, 0, WHITE);
		updateKeys(s);
		
		DrawTexturePro(
			rt.texture,
			(Rectangle){0, 0, 96, -68}, (Rectangle){48, 113, 96*3, 68*3},
			(Vector2){0, 0}, 0.0f, WHITE
		);

		EndDrawing();
	}

	unloadAssets(s);
	free(s);
	CloseWindow();
	CloseAudioDevice();
}