#ifndef PIPELINE_H
#define PIPELINE_H

void rgb_to_grayscale(Config * c ,Image *img);
void apply_lap_filter(Config * c,Image *img);
unsigned char laplace(unsigned char *p,Image*img,Config*c);
void apply_binary(Config * c,Image *img);
int rate(Config * c,Image *img);

#endif