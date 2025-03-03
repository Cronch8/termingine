#include "engine.c"
#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>

#define frand() ((float) random() / ((float) RAND_MAX))

#define max(a, b)                                                                        \
    ({                                                                                   \
        __typeof__(a) _a = (a);                                                          \
        __typeof__(b) _b = (b);                                                          \
        _a > _b ? _a : _b;                                                               \
    })

#define min(a, b)                                                                        \
    ({                                                                                   \
        __typeof__(a) _a = (a);                                                          \
        __typeof__(b) _b = (b);                                                          \
        _a < _b ? _a : _b;                                                               \
    })

void set_zero(TE_particle_effect* effect, TE_particle* particle, void* shared_data) {
    particle->x = 0;
    particle->y = 0;
    particle->vy = 0;
    particle->vx = 0;
    particle->brightness = 0;
}

void loop(TE_particle_effect* effect, TE_particle* particle, void* shared_data) {
    if (random() % 100 == 0) {
        particle->brightness = random() % strlen(effect->brightness_str);
    }
}

void set_random_particle(TE_particle_effect* effect, TE_particle* particle,
                         void* shared_data) {
    particle->x = random() % effect->width;
    particle->y = random() % effect->height;
    particle->vx = ((float) (random() % 10) - 5) / 2;
    particle->vy = ((float) (random() % 10) - 5) / 2;
    particle->brightness = strlen(effect->brightness_str);
}

void explode_particle_center(TE_particle_effect* effect, TE_particle* particle,
                             void* shared_data) {
    particle->brightness = strlen(effect->brightness_str);
    particle->x = effect->width / 2;
    particle->y = effect->height / 2;
    particle->vx = 2.0f - frand() * 4.0f;
    particle->vy = 2.0f - frand() * 4.0f;
}

void bounce(TE_particle_effect* effect, TE_particle* particle, void* shared_data) {
    if (particle->x + particle->vx >= effect->width || particle->x + particle->vx <= 0) {
        particle->vx = -particle->vx;
        particle->x += particle->vx;
    }
    if (particle->y + particle->vy >= effect->height || particle->y + particle->vy <= 0) {
        particle->vy = -particle->vy;
        particle->y += particle->vy;
    }
    if (random() % 200 == 0) {
        particle->brightness = random() % strlen(effect->brightness_str);
    }
}

void bounce_fade(TE_particle_effect* effect, TE_particle* particle, void* shared_data) {
    if (particle->x + particle->vx >= effect->width || particle->x + particle->vx <= 0) {
        particle->vx = -particle->vx;
        particle->x += particle->vx;
    }
    if (particle->y + particle->vy >= effect->height || particle->y + particle->vy <= 0) {
        particle->vy = -particle->vy;
        particle->y += particle->vy;
    }
    if (random() % 10 == 0) {
        particle->brightness--;
    }
    particle->vy *= 0.99f;
    particle->vx *= 0.99f;
}

void snow_falling(TE_particle_effect* effect, TE_particle* particle, void* shared_data) {
    if (particle->brightness <= 0) {
        particle->x = random() % effect->width;
        particle->y = 0;
        particle->vx = 0.1f;
        particle->vy = 0.4f;
        particle->brightness = strlen(effect->brightness_str) + 2;
    } else {
        particle->brightness -= random() % 2;
        if (particle->y >= effect->height) {
            particle->y = 0;
        }
        particle->vy += frand() * 0.6f;
        particle->vx += 0.14f - frand() * 0.1f;
        particle->vy *= 0.8f;
        particle->vx *= 0.8f;
    }
}

int main() {
    TE_init();
    time_t timer = 0;
    time(&timer);
    srand(timer);
    TE_particle_effect effects[3];
    effects[0] = *TE_create_particle_effect(100);
    effects[1] = *TE_create_particle_effect(10);
    effects[2] = *TE_create_particle_effect(200);
    TE_update_particle_effect(&effects[0], explode_particle_center, NULL);
    TE_update_particle_effect(&effects[1], set_random_particle, NULL);
    TE_update_particle_effect(&effects[2], set_random_particle, NULL);
    struct timespec ts;
    double dt = 1.0 / 60.0;
    ts.tv_sec = dt;
    ts.tv_nsec = (int) (dt * 1000000000) % 1000000000;
    int count = 0;
    while (1) {
        if (count > (int) (3.5 / dt)) {
            count = 0;
            TE_update_particle_effect(&effects[0], explode_particle_center, NULL);
        }
        TE_update_particle_effect(&effects[0], bounce_fade, NULL);
        TE_update_particle_effect(&effects[1], loop, NULL);
        TE_update_particle_effect(&effects[2], snow_falling, NULL);
        TE_display_multiple(effects, 3);
        nanosleep(&ts, &ts);
        count++;
    }
    TE_finish();
}
