
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/timer.h>

#include "dma_memcpy.h"
#include "framebuffer.h"
#include "system.h"

uint8_t framebuffer [ FBWIDTH * FBHEIGHT ] /*__attribute((aligned (1024)))*/;
//uint8_t offscreen [ FBWIDTH * FBHEIGHT ] /*__attribute((aligned (1024)))*/;
uint8_t *offscreen;

extern volatile unsigned char vblank;

static inline void _spin_until_vblank ( void ) {
  while ( ! vblank ) {
    __asm__("nop");
  }

}

void fb_setup ( void ) {
  offscreen = ccm_memory_base;

  fb_render_rect_filled ( offscreen, 0, 0, FBWIDTH - 1, FBHEIGHT - 1, 0x00 );
  fb_render_rect_filled ( framebuffer, 0, 0, FBWIDTH - 1, FBHEIGHT - 1, 0x00 );

}

void fb_thin_diamond ( uint8_t *fb ) {
  unsigned int x = 0, y = 0;

  for ( y = 0; y < FBHEIGHT; y++ ) {
    for ( x = 0; x < FBWIDTH; x++ ) {
      x++;
    } // x
  } // y

}

void fb_test_pattern ( uint8_t *fb ) {
  unsigned int i;

#if 1 // fill framebuffer with offset squares
  unsigned int x, y;
  unsigned char v;
  for ( y = 0; y < FBHEIGHT; y++ ) {

    //i = 0;
    i = ( y / 10 ) % 5;

    for ( x = 0; x < FBWIDTH; x++ ) {

      if ( x % 10 == 0 ) {
        i++;
      }

      if ( i == 0 ) {
        v = (unsigned char) GPIO0;
      } else if ( i == 1 ) {
        v = (unsigned char) GPIO1;
      } else if ( i == 2 ) {
        v = (unsigned char) GPIO2;
      } else if ( i == 3 ) {
        v = (unsigned char) GPIO3;
      } else if ( i == 4 ) {
        v = (unsigned char) GPIO4;
      } else if ( i == 5 ) {
        v = (unsigned char) GPIO5;
      } else {
        i = 0;
        v = (unsigned char) GPIO0;
      }

      if ( y % 10 == 0 ) {
        v = 0; // black
      }

      *( fb + ( y * FBWIDTH ) + x ) = v;

      //*( framebuffer + ( y * FBWIDTH ) + x ) = (unsigned char) 0;
      //*( framebuffer + ( y * FBWIDTH ) + x ) = (unsigned char)( GPIO3 );
      //*( framebuffer + ( y * FBWIDTH ) + x ) = (unsigned char)( GPIO5 );
      //*( framebuffer + ( y * FBWIDTH ) + x ) = (unsigned char)( GPIO1 | GPIO3 );
      //*( framebuffer + ( y * FBWIDTH ) + x ) = (unsigned char)( GPIO1 | GPIO0 );
      //*( framebuffer + ( y * FBWIDTH ) + x ) = (unsigned char)( GPIO0 | GPIO1 );
      //*( framebuffer + ( y * FBWIDTH ) + x ) = (unsigned char)( GPIO0 | GPIO1 | GPIO2 | GPIO3 );
      //*( framebuffer + ( y * FBWIDTH ) + x ) = (unsigned char)( GPIO2 | GPIO3 );
      //*( framebuffer + ( y * FBWIDTH ) + x ) = (unsigned char)( GPIO4 | GPIO5 );

    } // x

  } // y
#endif

#if 0 // fill framebuffer with vertical stripes of all colours (1px per colour)
  //unsigned char i;
  unsigned int x, y;
  unsigned char v;

  for ( y = 0; y < FBHEIGHT; y++ ) {

    i = 0;
    for ( x = 0; x < FBWIDTH; x++ ) {

      //*( framebuffer + ( y * FBWIDTH ) + x ) = i / 6;
      *( fb + ( y * FBWIDTH ) + x ) = i;
      //*( framebuffer + ( y * FBWIDTH ) + x ) = (unsigned char)( GPIO0 | GPIO1 );

      i++;
    } // x

  } // y
#endif

#if 0 // vertical strip every X pixels
  //unsigned char i;
  unsigned int x, y;

  for ( y = 0; y < FBHEIGHT; y++ ) {

    i = 0;
    for ( x = 0; x < FBWIDTH; x++ ) {

      if ( i >= 9 ) {
        *( fb + ( y * FBWIDTH ) + x ) = GPIO1;
      }

      if ( i == 10 ) {
        i = 0;
      }

      i++;
    } // x
  } // y
#endif

#if 0 // vertical strip 1 pixel wide
  //unsigned char i;
  unsigned int x, y;

  for ( y = 0; y < FBHEIGHT; y++ ) {

    i = 0;
    for ( x = 0; x < FBWIDTH; x++ ) {

      if ( i == 9 ) {
        *( fb + ( y * FBWIDTH ) + x ) = GPIO1;
        i = 0;
      }

      i++;
    } // x
  } // y
#endif

}

void fb_clone ( uint8_t *fbsrc, uint8_t *fbdst ) {

#ifndef RENDER_DMA
  memcpy ( fbdst, fbsrc, FBWIDTH * FBHEIGHT ); // screws up the DMA!?
  return;
#endif

#if 0
  uint16_t i;

  i = FBWIDTH * FBHEIGHT;
  i /= 16;

  while ( i-- ) {
    *fbdst++ = *fbsrc++;
    *fbdst++ = *fbsrc++;
    *fbdst++ = *fbsrc++;
    *fbdst++ = *fbsrc++;
    *fbdst++ = *fbsrc++;
    *fbdst++ = *fbsrc++;
    *fbdst++ = *fbsrc++;
    *fbdst++ = *fbsrc++;

    *fbdst++ = *fbsrc++;
    *fbdst++ = *fbsrc++;
    *fbdst++ = *fbsrc++;
    *fbdst++ = *fbsrc++;
    *fbdst++ = *fbsrc++;
    *fbdst++ = *fbsrc++;
    *fbdst++ = *fbsrc++;
    *fbdst++ = *fbsrc++;
  }
#else
  uint16_t i;
  uint32_t *s = fbsrc, *d = fbdst;

  i = FBWIDTH * FBHEIGHT;
  i /= 16;
  i /= 4;

  while ( i-- ) {
    *d++ = *s++;
    *d++ = *s++;
    *d++ = *s++;
    *d++ = *s++;
    *d++ = *s++;
    *d++ = *s++;
    *d++ = *s++;
    *d++ = *s++;

    *d++ = *s++;
    *d++ = *s++;
    *d++ = *s++;
    *d++ = *s++;
    *d++ = *s++;
    *d++ = *s++;
    *d++ = *s++;
    *d++ = *s++;
  }
#endif

}

inline void fb_render_rect_filled ( uint8_t *fb, uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint8_t rgb ) {
  register uint16_t tx, ty;
  register uint16_t tx2, ty2;
  register uint8_t *t;

  tx2 = x + w;
  ty2 = y + h;

  for ( ty = y; ty < ty2; ty++ ) {
    t = fb + ( ty * FBWIDTH ) + x; 
    for ( tx = 0; tx < w; tx++ ) {
      *t++ = rgb;
      //__asm__("nop");
    } // x
  } // y

}

inline void fb_render_rect8only_filled ( uint8_t *fb, uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint8_t rgb ) {
  register uint16_t ty;
  register uint16_t ty2;
  register uint8_t *t;

  ty2 = y + h;

  for ( ty = y; ty < ty2; ty++ ) {
    t = fb + ( ty * FBWIDTH ) + x; 

    *t++ = rgb;
    *t++ = rgb;
    *t++ = rgb;
    *t++ = rgb;

    *t++ = rgb;
    *t++ = rgb;
    *t++ = rgb;
    *t++ = rgb;

  } // y

}

static uint16_t dx = 10, dy = 65;
static uint8_t drgb = 0;
void fb_lame_demo_animate ( uint8_t *fb ) {

  //_spin_until_vblank();

  dx++;
  dy++;

  if ( dy % 10 == 0 ) {
    drgb++;
  }

  fb_render_rect8only_filled ( fb, dx, dy, 8, 8, drgb );

  if ( dx > 300 ) {
    dx -= 292;
  }

  if ( dy > 160 ) {
    dy = 65;
    dx += 20;
  }

  if ( drgb == 0b00111111 ) {
    drgb = 0;
  }

}



/* bresenham based line draw */
// ripped from old zotlib and adapted to 8bpp
void zl_render_line ( uint8_t *bits, uint8_t c, uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1 ) {
  int16_t dy = y1 - y0;
  int16_t dx = x1 - x0;
  int16_t stepx, stepy;
  int16_t fraction;

  if (dy < 0) {
    dy = -dy;
    stepy = - ( FBWIDTH );
  } else {
    stepy = ( FBWIDTH );
  }

  if (dx < 0) {
    dx = -dx;
    stepx = -1;
  } else {
    stepx = 1;
  }

  dy <<= 1;
  dx <<= 1;

  y0 *= ( FBWIDTH );
  y1 *= ( FBWIDTH );

  bits [ x0 + y0 ] = c;

  if (dx > dy) {
    fraction = dy - (dx >> 1);
    while (x0 != x1) {
      if (fraction >= 0) {
	y0 += stepy;
	fraction -= dx;
      }
      x0 += stepx;
      fraction += dy;
      bits [ x0 + y0 ] = c;
    }
  } else {
    fraction = dx - (dy >> 1);
    while (y0 != y1) {
      if (fraction >= 0) {
	x0 += stepx;
	fraction -= dy;
      }
      y0 += stepy;
      fraction += dx;
      bits [ x0 + y0 ] = c;
    }
  }

  return;
}

void zl_render_blit32 ( uint8_t *from, uint8_t *to,
                        uint16_t x1, uint16_t y1, uint16_t w, uint16_t h,
                        uint16_t to_x, uint16_t to_y )
{
  uint16_t x2 = x1 + w;
  uint16_t y2 = y1 + h;
  uint16_t x, y;
  uint8_t *vfromline = from;
  uint32_t *vfromiter;
  uint8_t *vtoline = to;
  uint32_t *vtoiter;

  vfromline += ( y1 * FBWIDTH );
  vfromline += x1;

  vtoline += ( to_y * FBWIDTH );
  vtoline += to_x;

  x1 >>= 2;
  x2 >>= 2;

  for ( y = y1; y < y2; y++ ) {
    vtoiter = vtoline;
    vfromiter = vfromline;

    for ( x = x1; x < x2; x++ ) {
      *vtoiter++ = *vfromiter++;
    }

    vtoline += FBWIDTH;
    vfromline += FBWIDTH;
  }

  return;
}

void zl_render_blit8 ( uint8_t *from, uint8_t *to,
                       uint16_t x1, uint16_t y1, uint16_t w, uint16_t h,
                       uint16_t to_x, uint16_t to_y )
{
  uint16_t x2 = x1 + w;
  uint16_t y2 = y1 + h;
  uint16_t x, y;
  uint8_t *vfromline = from;
  uint8_t *vfromiter;
  uint8_t *vtoline = to;
  uint8_t *vtoiter;

  vfromline += ( y1 * FBWIDTH );
  vfromline += x1;

  vtoline += ( to_y * FBWIDTH );
  vtoline += to_x;

  for ( y = y1; y < y2; y++ ) {
    vtoiter = vtoline;
    vfromiter = vfromline;

    for ( x = x1; x < x2; x++ ) {
      *vtoiter++ = *vfromiter++;
    }

    vtoline += FBWIDTH;
    vfromline += FBWIDTH;
  }

  return;
}
