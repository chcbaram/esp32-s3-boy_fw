/*
 * resize.c
 *
 *  Created on: 2020. 2. 1.
 *      Author: Baram
 */




#include "resize.h"
#include <math.h>


#define USE_COLOR_DEPTH 16
#define ALGO3



#if (USE_COLOR_DEPTH == 15 || USE_COLOR_DEPTH == 16)
#define COLOR_DEPTH_TYPE uint16_t
#elif (USE_COLOR_DEPTH == 24 || USE_COLOR_DEPTH == 32)
#define COLOR_DEPTH_TYPE uint32_t
#endif

#if (USE_COLOR_DEPTH == 15)
#define GETR(c) (((unsigned short)(c)) >> 10)
#define GETG(c) (((c) & 0x03E0)>>5)
#define GETB(c) ((c) & 0x1F)
#define MAKECOL(r, g, b) ( ((r)<<10) | ((g)<<5) | (b))
#elif (USE_COLOR_DEPTH == 16)
#define GETR(c) (((uint16_t)(c)) >> 11)
#define GETG(c) (((c) & 0x07E0)>>5)
#define GETB(c) ((c) & 0x1F)
#define MAKECOL(r, g, b) ( ((r)<<11) | ((g)<<5) | (b))
#elif (USE_COLOR_DEPTH == 24 || USE_COLOR_DEPTH == 32)
#define GETR(c) ((c & 0xFF0000) >> 16)
#define GETG(c) (((c) & 0xFF00)>>8)
#define GETB(c) ((c) & 0xFF)
#define MAKECOL(r, g, b) ( ((r)<<16) | ((g)<<8) | (b))
#endif

#if (USE_COLOR_DEPTH != 15 && USE_COLOR_DEPTH != 16 && USE_COLOR_DEPTH != 32)
#if (USE_COLOR_DEPTH == 24)
//#warning 'USE_COLOR_DEPTH' = 24: 24bpp is not currently supported. Graphics will be corrupt.

#else
#error USE_COLOR_DEPTH must be either 15, 16 or 32
#endif
#endif





void resizeImage(resize_image_t *src, resize_image_t *dest)
{
  int srcw = src->w;
  int srch = src->h;
  int destw = dest->w;
  int desth = dest->h;
  int i, j;
  float src_dest_w = (float)(src->w-1)/(float)(dest->w-1);
  float src_dest_h = (float)(src->h-1)/(float)(dest->h-1);
  float ci, cj;
  int x1, y1, x2, y2;
  float xoff, yoff;
  unsigned c1_rgb[3], c2_rgb[3], c3_rgb[3], c4_rgb[3];
  unsigned r, g, b;


   COLOR_DEPTH_TYPE *line1, *line2;
   COLOR_DEPTH_TYPE *dest_line;

   cj = 0;
   for (j=0; j<desth; j++)
   {
     y1 = cj;
     y2 = y1 < srch-1 ? y1+1 : srch-1;
     yoff = cj-y1;
     line1 = (COLOR_DEPTH_TYPE *)&src->p_data[y1*src->w];
     line2 = (COLOR_DEPTH_TYPE *)&src->p_data[y2*src->w];
     dest_line = (COLOR_DEPTH_TYPE *)&dest->p_data[j*dest->w];

     ci = 0;
     for(i=0; i<destw; i++)
     {
       x1 = ci;
       x2 = x1<srcw-1 ? x1+1 : srcw-1;
       c1_rgb[0] = GETR(line1[x1]);
       c1_rgb[1] = GETG(line1[x1]);
       c1_rgb[2] = GETB(line1[x1]);
       c2_rgb[0] = GETR(line1[x2]);
       c2_rgb[1] = GETG(line1[x2]);
       c2_rgb[2] = GETB(line1[x2]);
       c3_rgb[0] = GETR(line2[x1]);
       c3_rgb[1] = GETG(line2[x1]);
       c3_rgb[2] = GETB(line2[x1]);
       c4_rgb[0] = GETR(line2[x2]);
       c4_rgb[1] = GETG(line2[x2]);
       c4_rgb[2] = GETB(line2[x2]);

       xoff = ci-x1;
       r = (c1_rgb[0]*(1-xoff)+(c2_rgb[0]*xoff))*(1-yoff)+(c3_rgb[0]*(1-xoff)+(c4_rgb[0]*xoff))*yoff;
       g = (c1_rgb[1]*(1-xoff)+(c2_rgb[1]*xoff))*(1-yoff)+(c3_rgb[1]*(1-xoff)+(c4_rgb[1]*xoff))*yoff;
       b = (c1_rgb[2]*(1-xoff)+(c2_rgb[2]*xoff))*(1-yoff)+(c3_rgb[2]*(1-xoff)+(c4_rgb[2]*xoff))*yoff;
       dest_line[i] = MAKECOL(r, g, b);
       ci+=src_dest_w;
     }
     cj+=src_dest_h;
   }
}


#define AFFINEWARP_ISHIFT  16
#define AFFINEWARP_QSHIFT  0
#define AFFINEWARP_COEFFMASK  0xFFFF

#define round2int32(dbl) ( floor((dbl) +0.5))



__attribute__((optimize("O2"))) void resizeImageFast(resize_image_t *src, resize_image_t *dest)
{
  int destw = dest->w;
  int desth = dest->h;
  int i, j;
  float src_dest_w = (float)(src->w-1)/(float)(dest->w-1);
  float src_dest_h = (float)(src->h-1)/(float)(dest->h-1);
  unsigned r, g, b;


   COLOR_DEPTH_TYPE *line1, *line2;
   COLOR_DEPTH_TYPE *dest_line;

   int_least32_t xstep_c;
   int_least32_t xstep_r;
   int_least32_t ystep_c;
   int_least32_t ystep_r;
   int_least32_t xf, yf;
   uint_least16_t cxf, cyf;
   uint_least16_t oneminuscxf, oneminuscyf;
   int_least32_t xstart_r;
   int_least32_t ystart_r;

   uint_least16_t xi, yi;
   uint_least8_t s0, s1, s2, s3;
   uint_least32_t s01f, s23f, soutf;

   xstep_c = (int_least32_t) round2int32(src_dest_w * (1<<AFFINEWARP_ISHIFT));
   xstep_r = (int_least32_t) round2int32(0 * (1<<AFFINEWARP_ISHIFT));
   ystep_c = (int_least32_t) round2int32(0 * (1<<AFFINEWARP_ISHIFT));
   ystep_r = (int_least32_t) round2int32(src_dest_h * (1<<AFFINEWARP_ISHIFT));


   xstart_r = 0;
   ystart_r = 0;

   if (src->stride == 0)  src->stride  = src->w;
   if (dest->stride == 0) dest->stride = dest->w;

   if ((destw+dest->x) > dest->stride) destw = dest->stride - dest->x;

   for (j=0; j<desth; j++)
   {
     xf = xstart_r;
     yf = ystart_r;

     yi = (uint_least16_t)(yf >> AFFINEWARP_ISHIFT);

     line1 = (COLOR_DEPTH_TYPE *)&src->p_data[(yi)*src->stride];
     line2 = (COLOR_DEPTH_TYPE *)&src->p_data[(yi+1)*src->stride];
     dest_line = (COLOR_DEPTH_TYPE *)&dest->p_data[(j+dest->y)*(dest->stride) + dest->x];

     for(i=0; i<destw; i++)
     {
       xi = (uint_least16_t)(xf >> AFFINEWARP_ISHIFT);
       yi = (uint_least16_t)(yf >> AFFINEWARP_ISHIFT);

       /* extract most significant 16 bits  */
       cxf = (uint_least16_t)( (xf & AFFINEWARP_COEFFMASK) >> AFFINEWARP_QSHIFT);
       cyf = (uint_least16_t)( (yf & AFFINEWARP_COEFFMASK) >> AFFINEWARP_QSHIFT);
       oneminuscxf = 0xFFFF - cxf;
       oneminuscyf = 0xFFFF - cyf;


       s0 = GETR(line1[xi]);
       s1 = GETR(line1[xi+1]);
       s2 = GETR(line2[xi]);
       s3 = GETR(line2[xi+1]);

       // Interpolate
       s01f  = oneminuscxf*s0 + cxf*s1;   // Interpolate points above
       s23f  = oneminuscxf*s2 + cxf*s3;   // Interpolate points below

       s01f >>= 8;
       s23f >>= 8;
       soutf = oneminuscyf*s01f + cyf*s23f; // Final interpolation
       soutf = (soutf >> 24);
       r = (uint_least8_t)soutf;


       s0 = GETG(line1[xi]);
       s1 = GETG(line1[xi+1]);
       s2 = GETG(line2[xi]);
       s3 = GETG(line2[xi+1]);

       // Interpolate
       s01f  = oneminuscxf*s0 + cxf*s1;   // Interpolate points above
       s23f  = oneminuscxf*s2 + cxf*s3;   // Interpolate points below

       s01f >>= 8;
       s23f >>= 8;
       soutf = oneminuscyf*s01f + cyf*s23f; // Final interpolation
       soutf = (soutf >> 24);
       g = (uint_least8_t)soutf;


       s0 = GETB(line1[xi]);
       s1 = GETB(line1[xi+1]);
       s2 = GETB(line2[xi]);
       s3 = GETB(line2[xi+1]);


       // Interpolate
       s01f  = oneminuscxf*s0 + cxf*s1;   // Interpolate points above
       s23f  = oneminuscxf*s2 + cxf*s3;   // Interpolate points below

       s01f >>= 8;
       s23f >>= 8;
       soutf = oneminuscyf*s01f + cyf*s23f; // Final interpolation
       soutf = (soutf >> 24);
       b = (uint_least8_t)soutf;

       dest_line[i] = MAKECOL(r, g, b);
       xf += xstep_c;
       yf += ystep_c;
     }

     xstart_r += xstep_r;
     ystart_r += ystep_r;
   }
}

__attribute__((optimize("O2"))) void resizeImageFastGray(resize_image_t *src, resize_image_t *dest)
{
  int destw = dest->w;
  int desth = dest->h;
  int i, j;
  float src_dest_w = (float)(src->w-1)/(float)(dest->w-1);
  float src_dest_h = (float)(src->h-1)/(float)(dest->h-1);
  unsigned pixel;


   COLOR_DEPTH_TYPE *line1, *line2;
   COLOR_DEPTH_TYPE *dest_line;

   int_least32_t xstep_c;
   int_least32_t xstep_r;
   int_least32_t ystep_c;
   int_least32_t ystep_r;
   int_least32_t xf, yf;
   uint_least16_t cxf, cyf;
   uint_least16_t oneminuscxf, oneminuscyf;
   int_least32_t xstart_r;
   int_least32_t ystart_r;

   uint_least16_t xi, yi;
   uint_least8_t s0, s1, s2, s3;
   uint_least32_t s01f, s23f, soutf;

   xstep_c = (int_least32_t) round2int32(src_dest_w * (1<<AFFINEWARP_ISHIFT));
   xstep_r = (int_least32_t) round2int32(0 * (1<<AFFINEWARP_ISHIFT));
   ystep_c = (int_least32_t) round2int32(0 * (1<<AFFINEWARP_ISHIFT));
   ystep_r = (int_least32_t) round2int32(src_dest_h * (1<<AFFINEWARP_ISHIFT));


   xstart_r = 0;
   ystart_r = 0;

   if (src->stride == 0)  src->stride  = src->w;
   if (dest->stride == 0) dest->stride = dest->w;

   if ((destw+dest->x) > dest->stride) destw = dest->stride - dest->x;

   for (j=0; j<desth; j++)
   {
     xf = xstart_r;
     yf = ystart_r;

     yi = (uint_least16_t)(yf >> AFFINEWARP_ISHIFT);

     line1 = (COLOR_DEPTH_TYPE *)&src->p_data[(yi)*src->stride];
     line2 = (COLOR_DEPTH_TYPE *)&src->p_data[(yi+1)*src->stride];
     dest_line = (COLOR_DEPTH_TYPE *)&dest->p_data[(j+dest->y)*(dest->stride) + dest->x];

     for(i=0; i<destw; i++)
     {
       xi = (uint_least16_t)(xf >> AFFINEWARP_ISHIFT);
       yi = (uint_least16_t)(yf >> AFFINEWARP_ISHIFT);

       /* extract most significant 16 bits  */
       cxf = (uint_least16_t)( (xf & AFFINEWARP_COEFFMASK) >> AFFINEWARP_QSHIFT);
       cyf = (uint_least16_t)( (yf & AFFINEWARP_COEFFMASK) >> AFFINEWARP_QSHIFT);
       oneminuscxf = 0xFFFF - cxf;
       oneminuscyf = 0xFFFF - cyf;


       s0 = line1[xi];
       s1 = line1[xi+1];
       s2 = line2[xi];
       s3 = line2[xi+1];

       // Interpolate
       s01f  = oneminuscxf*s0 + cxf*s1;   // Interpolate points above
       s23f  = oneminuscxf*s2 + cxf*s3;   // Interpolate points below

       s01f >>= 8;
       s23f >>= 8;
       soutf = oneminuscyf*s01f + cyf*s23f; // Final interpolation
       soutf = (soutf >> 24);
       pixel = (uint_least8_t)soutf;

       dest_line[i] = pixel;
       xf += xstep_c;
       yf += ystep_c;
     }

     xstart_r += xstep_r;
     ystart_r += ystep_r;
   }
}


__attribute__((optimize("O2"))) void resizeImageNearest(resize_image_t *src, resize_image_t *dest)
{
  int x_ratio = (int)((src->w<<16)/dest->w) +1;
  int y_ratio = (int)((src->h<<16)/dest->h) +1;
  int x2, y2;
  int h2, w2;
  int w1;
  int stride_src;
  int stride_dst;

  w1 = src->w;
  w2 = dest->w;
  h2 = dest->h;

  if (dest->stride > 0)
  {
    stride_dst = dest->stride;
  }
  else
  {
    stride_dst = w2;
  }
  if (src->stride > 0)
  {
    stride_src = src->stride;
  }
  else
  {
    stride_src = w1;
  }


  for (int i=0;i<h2;i++)
  {
    for (int j=0;j<w2;j++)
    {
      x2 = ((j*x_ratio)>>16) ;
      y2 = ((i*y_ratio)>>16) ;
      dest->p_data[((i+dest->y)*stride_dst)+j+dest->x] = src->p_data[(y2*stride_src)+x2];
    }
  }
}


