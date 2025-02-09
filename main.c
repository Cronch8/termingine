#include "engine.c"
#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>

void set_random_particle(TE_particle_effect* effect, TE_particle* particle) {
    particle->x = random() % effect->width;
    particle->y = random() % effect->height;
    particle->brightness = strlen(effect->brightness_str);
}

void explosion(TE_particle_effect* effect, TE_particle* particle) {
    if (particle->brightness <= 0) {
        particle->brightness = strlen(effect->brightness_str);
        particle->x = effect->width / 2;
        particle->y = effect->height / 2;
        particle->vx = 4 - random() % 8;
        particle->vy = 4 - random() % 8;
    }
    particle->brightness--;
}

void drip_water(TE_particle_effect* effect, TE_particle* particle) {
    if (random() % 8 > 1) {
        return;
    }
    if (particle->brightness <= 0) {
        particle->x = random() % effect->width;
        particle->y = 0;
        particle->vx = 0;
        particle->vy = 0;
        particle->brightness = strlen(effect->brightness_str) + 1;
    } else {
        particle->brightness -= random() % 2;
    }
    if (particle->y >= effect->height) {
        particle->y = 0;
    } else {
        particle->y += random() % 2;
    }
}

int main() {
    TE_init();
    time_t timer = 0;
    time(&timer);
    srand(timer);
    TE_particle_effect* effect = TE_create_particle_effect(100);
    TE_update_particle_effect(effect, set_random_particle);
    struct timespec ts;
    double dt = 1.0 / 60.0;
    ts.tv_sec = dt;
    ts.tv_nsec = (int) (dt * 1000000000) % 1000000000;
    while (1) {
        TE_update_particle_effect(effect, explosion);
        TE_display(effect);
        nanosleep(&ts, &ts);
    }
    TE_finish();
}
