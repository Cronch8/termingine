#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

typedef struct {
    int width;
    int height;
    char data[];
} TE_layer;

TE_layer* TE_create_layer(uint width, uint height) {
    TE_layer *layer = malloc(sizeof(TE_layer) + width * height * sizeof(char));
    // check for failed malloc
    layer->width = width;
    layer->height = height;
    memset(layer->data, 0, width * height * sizeof(char));
    return layer;
}


int main() {
    TE_layer *layer = TE_create_layer(20, 10);
    layer->data[20*4+3] = 2;
    printf("data 3,4: %d", layer->data[20*4+3]);
    free(layer);
}
