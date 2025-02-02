#include "engine.c"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void set_random_particle(TE_particle_layer *layer, TE_particle *particle) {
    particle->x = random() % 100;
    particle->y = random() % 30;
    particle->brightness = random() % 13;// 13 is the number of chars in the " .-:=csZ58#M@" string
}

int main() {
    TE_particle_layer *layer = TE_create_particle_layer(100, 30, 40);
    TE_update_particles(layer, set_random_particle);
    int size = (100 * 30);
    char window_chars[size+1];
    memset(&window_chars, 0, size);
    TE_calculate_chars(layer, window_chars);
    window_chars[size+1] = '\0';
    printf("%s", window_chars);
}
