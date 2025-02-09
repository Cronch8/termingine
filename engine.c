#ifndef ENGINE
#define ENGINE

#include <ncurses.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

typedef struct {
    float x;
    float y;
    float vx;
    float vy;
    uint brightness;
} TE_particle;

typedef struct {
    float x;
    float y;
} TE_point;

typedef struct {
    int width;
    int height;
    int particle_count;
    char brightness_str[60];
    TE_point* prev_locations;
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

// Makes a particle effect objet, the core object around which all particle
// operations work. Returns NULL enough memory can't be allocated.
TE_particle_effect* TE_create_particle_effect(int particle_count) {
    TE_particle_effect* effect = malloc(sizeof(TE_particle_effect));
    if (!effect) {
        return NULL;
    }
    effect->prev_locations = calloc(particle_count, sizeof(TE_point));
    if (!effect->prev_locations) {
        free(effect);
        return NULL;
    }
    effect->particles = calloc(particle_count, sizeof(TE_particle));
    if (!effect->particles) {
        free(effect->prev_locations);
        free(effect);
        return NULL;
    }
    getmaxyx(stdscr, effect->height, effect->width);
    effect->particle_count = particle_count;
    strcpy(effect->brightness_str, " .-:=csZ58#M@");
    return effect;
}

void TE_set_particle_effect_chars(TE_particle_effect* effect, char* str) {
    *effect->brightness_str = '\0';
    strncpy(effect->brightness_str, str, strlen(str));
}

void TE_destory_particle_effect(TE_particle_effect* effect) {
    // is this correct? is it freeing the whole array?
    free(effect->prev_locations);
    free(effect->particles);
    free(effect);
}

// update is the fn provided by the user that will update each particle
// once. Can be used to initialize the particles in specific postions too.
void TE_update_particle_effect(TE_particle_effect* effect,
                               void (*update)(TE_particle_effect*,
                                              TE_particle*)) {
    // ideally terminal size would be updated not each update but on resize event
    getmaxyx(stdscr, effect->height, effect->width);
    for (int i = 0; i < effect->particle_count; i++) {
        // assumes that velocity will not be bigger than screen size
        effect->particles[i].y += effect->particles[i].vy / 2;
        if (effect->particles[i].y > effect->width) {
            effect->particles[i].y -= effect->width;
        } else if (effect->particles[i].y < 0) {
            effect->particles[i].y += effect->width;
        }
        effect->particles[i].x += effect->particles[i].vx;
        if (effect->particles[i].x > effect->width) {
            effect->particles[i].x -= effect->width;
        } else if (effect->particles[i].x < 0) {
            effect->particles[i].x += effect->width;
        }
        update(effect, &effect->particles[i]);
    }
}

void TE_display(TE_particle_effect* effect) {
    getmaxyx(stdscr, effect->height, effect->width);
    char particle;
    for (int i = 0; i < effect->particle_count; i++) {
        mvaddch(effect->prev_locations[i].y, effect->prev_locations[i].x, ' ');
        particle = TE_get_brightness_char(effect->particles[i].brightness);
        mvaddch(effect->particles[i].y, effect->particles[i].x, particle);
        effect->prev_locations[i].x = effect->particles[i].x;
        effect->prev_locations[i].y = effect->particles[i].y;
    }
    move(0, 0);
    refresh();
}

#endif
