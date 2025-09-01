/*
**  PARTICLE.H
**  particles system effect functions header.
**  ZJian,2000.11.30.
*/
#ifndef PARTICLE_H_INCLUDE
#define PARTICLE_H_INCLUDE
#include "xgrafx.h"

extern  int     init_particle(int width, int height);
extern  void    free_particle(void);
extern  void    active_particle(int active);
extern  void    explosion_particle(int cx,int cy);
extern  void    render_particle(BMP *bitmap);

#endif//PARTICLE_H_INCLUDE
