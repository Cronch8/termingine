#ifndef ENGINE
#define ENGINE

#include <ncurses.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#define MAX_BRIGHT_LEN 69
#define DEFAULT_BRIGHTNESS_STR " .-:=csZ58#M@"

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
    char brightness_str[MAX_BRIGHT_LEN];
    TE_point* prev_locations;
    TE_particle* particles;
} TE_particle_effect;

// i'm basically trusting that the compiler will know to optimize out this
// loop here, else this will be slow.
// Returns the ascii character representing it's brightness. By default has 13 characters.
char TE_get_brightness_char(TE_particle_effect* effect, int n) {
    int length = 0;
    for (int i = 0; effect->brightness_str[i] != '\0'; i++) {
        length = i;
    }
    if (n > length) {
        return effect->brightness_str[length];
    }
    if (n < 0) {
        return effect->brightness_str[0];
    }
    return effect->brightness_str[n];
}

void TE_set_brightness_str(TE_particle_effect* effect, char* str) {
    strncpy(&effect->brightness_str, str, MAX_BRIGHT_LEN);
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
    strcpy(effect->brightness_str, DEFAULT_BRIGHTNESS_STR);
    return effect;
}

// sets the array of characters that will be used as brightness.
// Default is: " .-:=csZ58#M@`
void TE_set_particle_effect_chars(TE_particle_effect* effect, char* str) {
    *effect->brightness_str = '\0';
    strncpy(effect->brightness_str, str, strlen(str));
}

// frees the whole effect
void TE_destory_particle_effect(TE_particle_effect* effect) {
    // is this correct? is it freeing the whole array?
    free(effect->prev_locations);
    free(effect->particles);
    free(effect);
}

// updates all particles and calls the `update` function on each particle in the effect.
void TE_update_particle_effect(TE_particle_effect* effect,
                               void (*update)(TE_particle_effect*, TE_particle*, void*),
                               void* shared_data) {
    // ideally terminal size would be updated not each update but on resize event
    getmaxyx(stdscr, effect->height, effect->width);
    for (int i = 0; i < effect->particle_count; i++) {
        // assumes that velocity will not be bigger than screen size
        effect->particles[i].y += effect->particles[i].vy / 2;
        if (effect->particles[i].y > effect->height) {
            effect->particles[i].y -= effect->height;
        } else if (effect->particles[i].y < 0) {
            effect->particles[i].y += effect->height;
        }
        effect->particles[i].x += effect->particles[i].vx;
        if (effect->particles[i].x > effect->width) {
            effect->particles[i].x -= effect->width;
        } else if (effect->particles[i].x < 0) {
            effect->particles[i].x += effect->width;
        }
        update(effect, &effect->particles[i], shared_data);
    }
}

// Displays the current state of the particles on the screen
void TE_display(TE_particle_effect* effect) {
    char particle;
    for (int i = 0; i < effect->particle_count; i++) {
        mvaddch(effect->prev_locations[i].y, effect->prev_locations[i].x, ' ');
        particle = TE_get_brightness_char(effect, effect->particles[i].brightness);
        mvaddch(effect->particles[i].y, effect->particles[i].x, particle);
        effect->prev_locations[i].x = effect->particles[i].x;
        effect->prev_locations[i].y = effect->particles[i].y;
    }
    move(0, 0);
    refresh();
}

// Displays the current state of all of the provided particle effects on the screen
void TE_display_multiple(TE_particle_effect effect[], int effect_count) {
    char particle;
    for (int i = 0; i < effect_count; i++) {
        for (int j = 0; j < effect[i].particle_count; j++) {
            mvaddch(effect[i].prev_locations[j].y, effect[i].prev_locations[j].x, ' ');
        }
    }
    for (int i = 0; i < effect_count; i++) {
        for (int j = 0; j < effect[i].particle_count; j++) {
            particle = TE_get_brightness_char(effect, effect[i].particles[j].brightness);
            mvaddch(effect[i].particles[j].y, effect[i].particles[j].x, particle);
            effect[i].prev_locations[j].x = effect[i].particles[j].x;
            effect[i].prev_locations[j].y = effect[i].particles[j].y;
        }
    }
    move(0, 0);
    refresh();
}

#endif
