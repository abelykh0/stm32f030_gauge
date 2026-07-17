#ifndef __SCREEN_H__
#define __SCREEN_H__

#include "EPD_2in66.h"

#ifdef __cplusplus
extern "C" {
#endif


extern UBYTE ScreenData[EPD_2IN66_WIDTH * EPD_2IN66_HEIGHT / 8];

#ifdef __cplusplus
}
#endif

#endif
