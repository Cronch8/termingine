#ifndef ENGINE
#define ENGINE

#include <ncurses.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

typedef struct {
    uint x;
    uint y;
    uint brightness;
} TE_particle;

typedef struct {
    uint x;
    uint y;
} TE_point;

typedef struct {
    int width;
    int height;
    int particle_count;
    TE_particle* prev_locations;
    TE_particle* particles;
} TE_particle_effect;

// i'm basically trusting that the compiler will know to optimize out this
// loop here, else this will be slow.
// Returns the ascii character representing it's brightness. By default has 13 characters.
char TE_get_brightness_char(int n) {
    char* chars = " .-:=csZ58#M@";
    int length = 0;
    for (int i = 0; chars[i] != '\0'; i++) {
        length = i;
    }
    if (n > length) {
        return chars[length];
    }
    if (n < 0) {
        return chars[0];
    }
    return chars[n];
}

void TE_init() { initscr(); }

void TE_finish() { endwin(); }

// Makes a particle layer objet, the core object around which all particle
// operations work. Returns NULL enough memory can't be allocated.
TE_particle_effect* TE_create_particle_layer(int particle_count) {
    TE_particle_effect* layer = malloc(sizeof(TE_particle_effect));
    if (!layer) {
        return NULL;
    }
    layer->prev_locations = calloc(particle_count, sizeof(TE_point));
    if (!layer->prev_locations) {
        free(layer);
        return NULL;
    }

    getmaxyx(stdscr, layer->height, layer->width);
    layer->particle_count = particle_count;

    layer->particles = calloc(particle_count, sizeof(TE_particle));
    if (!layer->particles) {
        // is this correct? is it freeing the whole layer?
        free(layer->prev_locations);
        free(layer);
        return NULL;
    }
    return layer;
}

void TE_destory_particle_layer(TE_particle_effect* layer) {
    // is this correct? is it freeing the whole array?
    free(layer->prev_locations);
    free(layer->particles);
    free(layer);
}

// update is the fn provided by the user that will update each particle once.
// Can be used to initialize the particles in specific postions too.
void TE_update_particles(TE_particle_effect* layer,
                         void (*update)(TE_particle_effect*, TE_particle*)) {
    // ideally terminal size would be updated not each update but on resize event
    getmaxyx(stdscr, layer->height, layer->width);
    for (int i = 0; i < layer->particle_count; i++) {
        update(layer, &layer->particles[i]);
    }
}

// return the array of chars for the whole window.
// Chars needs to be at least width*height*sizeof(char) in size
void TE_display(TE_particle_effect* layer) {
    getmaxyx(stdscr, layer->height, layer->width);
    char particle;
    for (int i = 0; i < layer->particle_count; i++) {
        mvaddch(layer->prev_locations[i].y, layer->prev_locations[i].x, ' ');
        particle = TE_get_brightness_char(layer->particles[i].brightness);
        mvaddch(layer->particles[i].y, layer->particles[i].x, particle);
        layer->prev_locations[i].x = layer->particles[i].x;
        layer->prev_locations[i].y = layer->particles[i].y;
    }
    move(0, 0);
    refresh();
}

#endif
