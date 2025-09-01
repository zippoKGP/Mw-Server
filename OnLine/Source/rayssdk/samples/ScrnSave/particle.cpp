/*
**  PARTICLE.CPP
**  particle system functions.
**  ZJian,2000.11.30.
*/
#include "rays.h"
#include "xtimer.h"
#include "xdraw.h"
#include "xgrafx.h"
#include "xsystem.h"
#include "xfont.h"
#include "xvga.h"
#include "vpcx.h"
#include "ripple.h"
#include "xtimer.h"


// D E F I N E S /////////////////////////////////////////////////////////////////////////
#define NUM_PARTICLES       500//3000
#define ENERGY              7//5

// S T R U C T S /////////////////////////////////////////////////////////////////////////
typedef struct  tagPARTICLE
{
    float   x;
    float   y;
    float   dirx;
    float   diry;
    long    color;
} PARTICLE,*LPPARTICLE;

typedef struct  tagCOLORTBL
{
    unsigned char   f;
    unsigned char   r;
    unsigned char   g;
    unsigned char   b;
    unsigned long   c;
} COLORTBL;


// M A C R O S ///////////////////////////////////////////////////////////////////////////
#define RANDOM(n)           (((float)rand() / (float)RAND_MAX)*(n))


// G L O B A L S /////////////////////////////////////////////////////////////////////////
static  PARTICLE        *particles=NULL;
static  COLORTBL        *palette=NULL;
static  unsigned char   *particle_buffer=NULL;
static  long            *ytable=NULL;
static  float           gravity = (float)0.1;
static  long            thepitch = -1;
static  long            screen_width;
static  long            screen_height;


// P R O T O T Y P E S ///////////////////////////////////////////////////////////////////
static  void    create_fire_palette(COLORTBL *p);
static  void    move_particles(void);
static  void    draw_particles(BMP *bitmap);
static  void    create_ytable(long *yt);
static  void    blur_particle_buffer(void);
static  void    clear_particle_buffer(void);

static  void    draw_shape_particles(BMP *bitmap);


// F U N C T I O N S /////////////////////////////////////////////////////////////////////
int     init_particle(int width, int height)
{
    screen_width = width;
    screen_height = height;

    // Alloc enough memory for all particles
    if(NULL == (particles = (PARTICLE *)GlobalAlloc(GPTR, sizeof(PARTICLE)*NUM_PARTICLES)) )
        return -1;

    // Create fire palette colors
    if(NULL == (palette = (COLORTBL *)GlobalAlloc(GPTR, sizeof(COLORTBL)*256)) )
        return -2;
    create_fire_palette(palette);

    // Create particle buffer
    if(NULL == (particle_buffer=(unsigned char *)GlobalAlloc(GPTR, screen_width * screen_height)) )
        return -3;
    memset(particle_buffer,0,screen_width*screen_height);

    // Create y table
    ytable = (long*)GlobalAlloc(GPTR, sizeof(long)*screen_height);
    create_ytable(ytable);

    // OK !
    return 0;
}

void    free_particle(void)
{
    if(particles) { GlobalFree(particles); particles=NULL; }
    if(palette) {GlobalFree(palette); palette=NULL; }
    if(particle_buffer) {GlobalFree(particle_buffer); particle_buffer=NULL; }
    if(ytable) {GlobalFree(ytable); ytable=NULL; }
}

void    active_particle(int active)
{
    if(active)
    {
    }
}


void    explosion_particle(int cx,int cy)
{
    long i;
    float lx, ly, dist;
    double len;
    
    for (i=0; i<NUM_PARTICLES; i++)
    {
        particles[i].x = (float)cx;
        particles[i].y = (float)cy;
        
        //particles[i].dirx = (float)(RANDOM(2) - 1);
        //particles[i].diry = (float)(-RANDOM(5));
        particles[i].dirx = (float)(RANDOM(3) - 1.5);
        particles[i].diry = (float)(RANDOM(3) - 1.5);
        
        // Set the energy (power) of the explosion
        dist = RANDOM(ENERGY);
        
        // Store x,y directions in temp variables
        lx = particles[i].dirx;
        ly = particles[i].diry;
        
        // Use distance formula
        len = sqrt(lx*lx + ly*ly);
        
        // We need to normalize the vector to give us a direction
        // vector, then find a random length of the vector, to give
        // a nice, non-uniform distribution
        if (len == 0.0)
            len = 0.0;
        else
            len = 1.0 / len;
        
        // Assign vector
        particles[i].dirx = (float)(particles[i].dirx*len*dist);
        particles[i].diry = (float)(particles[i].diry*len*dist);
        
        // Set particle to brightest color
        particles[i].color = 255;
    }
}


void    render_particle(BMP *bitmap)
{
    move_particles();
    draw_particles(bitmap);
//  draw_shape_particles(bitmap);
    blur_particle_buffer();
    //clear_particle_buffer();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  STATIC FUNCTIONS
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void    create_fire_palette(COLORTBL *p)
{
    int i;

    // Here, we make a nice palette, that fades from red->yellow->white,
    // giving the usual spread of colors seen in an explosion.
    // Set first 64 colors to shades of red
    for (i=0; i<64; i++)
    {
        palette[i].f = 0x4;//PC_NOCOLLAPSE;
        palette[i].r = (unsigned char)(i << 2);
        palette[i].g = 0;
        palette[i].b = 0;
    }

    // Set next 64 colors to shades of yellow and orange
    for (i=64; i<128; i++)
    {
        palette[i].f = 0x4;//PC_NOCOLLAPSE;
        palette[i].r = 255;
        palette[i].g = (unsigned char)((i - 64) << 2);
        palette[i].b = 0;
    }

    // Set last 128 colors to yellowish-white
    for (i=128; i<256; i++)
    {
        palette[i].f = 0x4;//PC_NOCOLLAPSE;
        palette[i].r = 255;
        palette[i].g = 255;
        palette[i].b = (unsigned char)((i - 128) << 1);
    }

    // Make colors matched for now screen settings
    for(i=0; i<256; i++)
    {
        palette[i].c = rgb2hi(palette[i].r, palette[i].g, palette[i].b );
    }
}



static  void    move_particles(void)
{
    long i;
    PARTICLE *part;
    
    part = particles;
    for (i=0; i<NUM_PARTICLES; i++)
    {
        part->x += part->dirx;
        part->y += part->diry;
        
        // Check if the particle has collided with any of the screen
        // top or bottom edges. If so, reflect the direction, and
        // lose energy. If not, then just add gravity
        if (part->y >= screen_height-1)
        {
            part->y = (float)(screen_height-1);
            part->dirx /= 4;
            part->diry = -part->diry / 2;
        }
        else if (part->y < 1)
        {
            part->y = 1;
            part->dirx /= 4;
            part->diry = -part->diry / 2;
        }
        else
        {
            part->diry += gravity;
        }
            
        if (part->x < 0)
        {
            part->x = 1;
            part->dirx = -part->dirx / 2;
            part->diry /= 4;
        }
        else if (part->x > screen_width-1)
        {
            part->x = (float)(screen_width-1);
            part->dirx = -part->dirx / 2;
            part->diry /= 4;
        }
        
        // If the particle is near the bottom of the screen, we
        // make its color random, to simulate the effect of the
        // particle dying, things catching fire, that kind of stuff
        if (part->y >= screen_height-1)
            part->color = (rand() & 127) + 128;
        
        part++;
    }
}


static  void    create_ytable(long *yt)
{
    int y;
    for(y=0;y<screen_height;y++)
    {
        ytable[y] = screen_width*y;
    }
}


static  void    draw_particles(BMP *bitmap)
{
    PARTICLE *part;
    long i,x,y;
    PIXEL color;
    unsigned char *double_buffer;
    
    part = particles;
    double_buffer = particle_buffer;
    for (i=0; i<NUM_PARTICLES; i++)
    {
        x = (long)(part->x + 0.5);
        y = (long)(part->y + 0.5);
        if(x>=0 && x<screen_width && y>=0 && y<screen_height)
            double_buffer[ytable[y]+x] = (unsigned char)part->color;
        part++;
    }

    for(y=0;y<screen_height;y++)
    {
        for(x=0;x<screen_width;x++)
        {
            color = (PIXEL) palette[ double_buffer[ytable[y]+x] ].c;

            // we assert this bitmap to screen size
            bitmap->line[y][x] = color;
        }
    }
}


static  void    draw_shape_particles(BMP *bitmap)
{
    SLONG   mx,my;
    PIXEL   color;
    SLONG   x,y;
    unsigned char *double_buffer;
    SLONG   wide;

    extern  void    get_mouse_position(SLONG *xpos, SLONG *ypos);

    double_buffer = particle_buffer;
    color = SYSTEM_WHITE;
    get_mouse_position(&mx, &my);

    wide = 20 + rand()%50;
    for(x=mx-wide; x<=mx+wide; x++)
    {
        for(y=my-wide; y<=my+wide; y++)
        {
            if(x==mx-wide || x==mx+wide || y==my-wide || y==my+wide)
            {
                if(x>=0 && x<screen_width && y>=0 && y<screen_height)
                    double_buffer[ytable[y]+x] = (unsigned char)255;
            }
        }
    }

    for(y=0;y<screen_height;y++)
    {
        for(x=0;x<screen_width;x++)
        {
            color = (PIXEL) palette[ double_buffer[ytable[y]+x] ].c;

            // we assert this bitmap to screen size
            bitmap->line[y][x] = color;
        }
    }
}


static  void    blur_particle_buffer(void)
{
    unsigned char *row1, *row2, *row3;
    long x, y, color;
    unsigned char *double_buffer;

    double_buffer = particle_buffer;

    for (y=1; y<screen_height-1; y++)
    {
        // Get values of pixel above and below the particle's row
        row1 = &double_buffer[ytable[y - 1]];
        row2 = &double_buffer[ytable[y]];
        row3 = &double_buffer[ytable[y + 1]];
        
        // Set value of next row's pixel to 0 to prevent sides of screen
        // from staying ablaze
        *row1++ = 0;
        *row2++ = 0;
        *row3++ = 0;
        
        // Take the average of the 4 pixels on each side of the current pixel
        // and darken it a bit too
        
        for (x=1; x<screen_width-1; x++)
        {
            color = (*row1 + *row3 + *(row2 - 1) + *(row2 + 1)) / 4;
            color -= 2;
            
            if (color < 0)
                color = 0;
            
            // Set current pixel to calculated color
            *row2 = (unsigned char)color;
            
            row1++;
            row2++;
            row3++;
        }
        
        // Set value of next row's pixel to 0 to prevent sides of screen
        // from staying ablaze
        *row1++ = 0;
        *row2++ = 0;
        *row3++ = 0;
    }
    memset(double_buffer,0,screen_width);
    memset(&double_buffer[ytable[screen_height-1]],0,screen_width);
}


static  void    clear_particle_buffer(void)
{
    memset(particle_buffer,0,screen_width*screen_height);
}


// T E S T   F U N C T I O N S ///////////////////////////////////////////////////////////////////////
#if     0 //-----------------------
#include "xsystem.h"
#include "xkiss.h"
#include "xinput.h"
#include "xgrafx.h"
#include "xfont.h"

SLONG   test_particle(void)
{
    SLONG main_pass;
    SLONG mx,my;
    UCHR ch;

    init_particle(screen_width, screen_height);
    explosion_particle(screen_width/2,screen_height/2);

    main_pass=0;
    reset_data_key();
    reset_mouse_key();
    while(main_pass==0)
    {
        idle_loop();
        clear_bitmap(screen_buffer);
        render_particle(screen_buffer);
        sprintf((char *)print_rec,"~C0~O3FPS = %d~C0~O0",get_fps());
        print16(4,4,(USTR*)print_rec,PEST_PUT,screen_buffer);
        update_screen(screen_buffer);

        ch=read_data_key();
        if(ch) reset_data_key();
        else ch=read_mouse_key();
        get_mouse_position(&mx,&my);
        reset_mouse_key();

        switch(ch)
        {
        case S_Esc:
            main_pass=1;
            break;
        case ' ':case S_Enter:case MS_LDn:
            explosion_particle(mx,my);
            break;
        }
    }

    free_particle();

    return 0;
}

#endif//0
