#pragma once
#ifdef __cplusplus
extern "C" {
#endif

#include "foxclip/core/export.h"

/* Menu callback function type */
typedef void(FOXCLIP_CALL *FoxclipMenuCallback)(const char *menuId, const char *itemId, void *userData);

/* Menu item properties */
typedef struct FoxclipMenuItem {
	size_t size;                  /* sizeof(FoxclipMenuItem) for version compatibility */
	const char *itemId;           /* Unique identifier for the menu item */
	const char *title;            /* Display text */
	FoxclipMenuCallback callback; /* Called when item is triggered */
	void *userData;               /* User data passed to callback */
	int checkable;                /* 0 = normal item, 1 = checkable item */
	int checked;                  /* Initial checked state (if checkable) */
} FoxclipMenuItem;

/* Menu item initializer macro */
#define FOXCLIP_MENU_ITEM_INIT \
	{ sizeof(FoxclipMenuItem), NULL, NULL, NULL, NULL, 0, 0 }

/* Menu API functions */
FOXCLIP_EXTERN int FOXCLIP_CALL foxclipCreateMenu(const char *menuId, const char *title);
FOXCLIP_EXTERN int FOXCLIP_CALL foxclipAddMenuItem(const char *menuId, const FoxclipMenuItem *item);
FOXCLIP_EXTERN int FOXCLIP_CALL foxclipRemoveMenuItem(const char *menuId, const char *itemId);
FOXCLIP_EXTERN int FOXCLIP_CALL foxclipRemoveMenu(const char *menuId);
FOXCLIP_EXTERN int FOXCLIP_CALL foxclipAddToolsMenuItem(const char *title, FoxclipMenuCallback callback,
							void *userData);

/* Return codes */
#define FOXCLIP_MENU_SUCCESS        0
#define FOXCLIP_MENU_E_INVALID_ARG -1
#define FOXCLIP_MENU_E_NOT_FOUND   -2
#define FOXCLIP_MENU_E_ALREADY_EXISTS -3
#define FOXCLIP_MENU_E_UI_ERROR    -4

#ifdef __cplusplus
} /* extern "C" */
#endif
