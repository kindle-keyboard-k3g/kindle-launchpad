#include <stddef.h>

#include "fntdefs.h"
#include "screen.h"
#include "pixop.h"
#include "statusbar.h"

pixmap_t * statusbar_show(fbscreen_t *pscreen)
{
	pixmap_t *psav ;

	psav = pixbuf_alloc(pscreen->surface.width, STATUSBAR_HEIGHT) ;
	screen_get_pixmap(pscreen, 0, pscreen->surface.height - STATUSBAR_HEIGHT, psav) ;

	screen_rect_at(pscreen, 0, pscreen->surface.height - STATUSBAR_HEIGHT,
			pscreen->surface.width, STATUSBAR_HEIGHT, 0) ;
	screen_update_area(pscreen, UMODE_PARTIAL, 0, pscreen->surface.height - STATUSBAR_HEIGHT,
			pscreen->surface.width, STATUSBAR_HEIGHT, NULL) ;

	return psav ;
}

void statusbar_hide(fbscreen_t *pscreen, pixmap_t *psav)
{

	screen_put_pixmap(pscreen, 0, pscreen->surface.height - STATUSBAR_HEIGHT, psav ) ;
	screen_update_area(pscreen, UMODE_FULL, 0, pscreen->surface.height - STATUSBAR_HEIGHT-1,
			pscreen->surface.width, STATUSBAR_HEIGHT, NULL) ;
	pixbuf_free(psav) ;
}

void statusbar_text(fbscreen_t *pscreen, pixmap_t *psb, char *p)
{
	screen_rect_at(pscreen, 0, pscreen->surface.height - STATUSBAR_HEIGHT,
			pscreen->surface.width, STATUSBAR_HEIGHT, 0) ;
	screen_string_at(pscreen, 30, pscreen->surface.height - STATUSBAR_HEIGHT, p ) ;
}

void statusbar_flash(fbscreen_t *pscreen, pixmap_t *psb, int count, int tmo, int waitflag)
{
	screen_flash_area(pscreen, count, 0, pscreen->surface.height - STATUSBAR_HEIGHT,
		pscreen->surface.width, STATUSBAR_HEIGHT, tmo, waitflag) ;
}
