#include "engine.c"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>

uint width = 240;
uint height = 55;

void set_random_particle(TE_particle_layer *layer, TE_particle *particle) {
  particle->x = random() % width;
  particle->y = random() % height;
  particle->brightness =
      random() % 13; // 13 is the number of chars in the " .-:=csZ58#M@" string
}

void decay_particle(TE_particle_layer *layer, TE_particle *particle) {
  if (particle->brightness <= 0) {
    particle->x = random() % width;
    particle->y = 0;
    particle->brightness = random() % 18 + 5;
  } else
    particle->brightness -= 1;
  if (particle->y >= layer->height)
    particle->y = 0;
  else
    particle->y += 1;
}

int main() {
  time_t timer = 0;
  time(&timer);
  printf("testinng ");
  fflush(stdout);
  printf("time: %ld ", timer);
  fflush(stdout);
  srand(timer);
  TE_particle_layer *layer = TE_create_particle_layer(width, height, 100);
  TE_update_particles(layer, set_random_particle);
  int size = width * height * sizeof(char);
  char *window_chars = calloc(size + 1, sizeof(char));
  window_chars[size] = '\0';
  struct timespec ts;
  double dt = 0.033;
  ts.tv_sec = dt;
  ts.tv_nsec = (int)(dt * 1000000000) % 1000000000;
  while (1) {
    TE_update_particles(layer, decay_particle);
    TE_calculate_chars(layer, window_chars);
    window_chars[size] = '\0';
    printf("%s", window_chars);
    fflush(stdout);
    nanosleep(&ts, &ts);
  }
  free(window_chars);
}
