#include "engine.c"
#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <time.h>

void set_random_particle(TE_particle_layer* layer, TE_particle* particle) {
    particle->x = random() % layer->width;
    particle->y = random() % layer->height;
    // 13 is the number of chars in the " .-:=csZ58#M@" brightness string
    particle->brightness = random() % 13;
}

void drip_water(TE_particle_layer* layer, TE_particle* particle) {
    if (random() % 8 > 1) {
        return;
    }
    if (particle->brightness <= 0) {
        particle->x = random() % layer->width;
        particle->y = 0;
        // 13 is the number of chars in the " .-:=csZ58#M@" brightness string
        particle->brightness = 13;
    } else {
        particle->brightness -= random() % 2;
    }
    if (particle->y >= layer->height) {
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
    TE_particle_layer* layer = TE_create_particle_layer(100);
    TE_update_particles(layer, set_random_particle);
    struct timespec ts;
    double dt = 1.0 / 60.0;
    ts.tv_sec = dt;
    ts.tv_nsec = (int) (dt * 1000000000) % 1000000000;
    while (1) {
        TE_update_particles(layer, drip_water);
        TE_display(layer);
        nanosleep(&ts, &ts);
    }
    TE_finish();
}
