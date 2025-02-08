#ifndef ENGINE
#define ENGINE

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

typedef struct {
  uint x;
  uint y;
  uint brightness;
} TE_particle;

typedef struct {
  int width;
  int height;
  int particle_count;
  TE_particle *particles;
} TE_particle_layer;

// i'm basically trusting that the compiler will know to optimize out this
// loop here, else this will be slow.
// Returns the ascii character representing it's brightness. By default has 13
// characters.
char TE_get_brightness_char(int n) {
  char *chars = " .-:=csZ58#M@";
  int length = 0;
  for (int i = 0; chars[i] != '\0'; i++) {
    length = i;
  }
  if (n > length)
    return chars[length];
  if (n < 0)
    return chars[0];
  return chars[n];
}

// Makes a particle layer objet, the core object around which all particle
// operations work. Returns NULL enough memory can't be allocated.
TE_particle_layer *TE_create_particle_layer(uint width, uint height,
                                            int particle_count) {
  TE_particle_layer *layer = malloc(sizeof(TE_particle_layer));
  if (!layer)
    return NULL;
  layer->width = width;
  layer->height = height;
  layer->particle_count = particle_count;

  layer->particles = calloc(particle_count, sizeof(TE_particle));
  if (!layer->particles) {
    free(layer);
    return NULL;
  }
  return layer;
}

void TE_destory_particle_layer(TE_particle_layer *layer) {
  free(layer->particles); // is this correct? is it freeing the whole array?
  free(layer);
}

// update is the fn provided by the user that will update each particle once.
// Can be used to initialize the particles in specific postions too.
void TE_update_particles(TE_particle_layer *layer,
                         void (*update)(TE_particle_layer *, TE_particle *)) {
  for (int i = 0; i < layer->particle_count; i++) {
    update(layer, &layer->particles[i]);
  }
}

// return the array of chars for the whole window.
// Chars needs to be at least width*height*sizeof(char) in size
void TE_calculate_chars(TE_particle_layer *layer, char *chars) {
  memset(chars, 0, layer->height * layer->width * sizeof(char));
  for (int i = 0; i < layer->particle_count; i++) {
    chars[layer->particles[i].y * layer->width + layer->particles[i].x] +=
        layer->particles[i].brightness;
  }
  for (int i = 0; i < layer->width * layer->height; i++) {
    chars[i] = TE_get_brightness_char(chars[i]);
  }
}

#endif
