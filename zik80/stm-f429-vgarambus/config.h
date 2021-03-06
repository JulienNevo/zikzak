#ifndef h_config_h
#define h_config_h

//#define RUNMODE_COMMAND_SERIAL /* listen to GPU Serial console for commands */
#define RUNMODE_FRAMEBUFFER_FOREVER /* 60fps, grab framebuffer from ext ram and copy to local, blitting to VGA */

#define INVERT_VBLANK /* inverted means vbl active is LOW on VBL line; otherwise vbl active is HIGH */
//#define VGA_DMA /* if defined, use DMA for VGA emit; otherwise, brute force */

#define BUS_FRAMEBUFFER /* if defined, will try to use the BUS to find RAM; if not defined, bus is not touched */

#endif
