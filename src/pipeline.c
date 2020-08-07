#include <stdio.h>
#include <stdlib.h>
#include "../include/image.h"
#include "../include/utils.h"

#include "../include/pipeline.h"


/**
 * @brief Función que devuelve el valor de la función a 
 *        aplicar en la Mascara de filtro laplaciano.
 *        
 * 
 * @param p Pixel escogido
 * @param img Estructura imagen
 * @param c Configuracion
 * @return unsigned char 
 */
unsigned char laplace(unsigned char *p,Image*img,Config*c){
    // Se inicializa el valor inicial, en esta variable se irá guardando
    // El resultado de la función
    unsigned char pixel = (uint8_t)0;
    unsigned char* pivot; // Pivote para recorrer la imagen
    int useSamePixel = 0;  // Bandera para reemplazar el pixel actual en los casos de borde

    /**
     * @brief Primero es importante saber como la librería STBI para el manejo
     * de imágenes funciona. Al abrir una imagen, esta queda guardada como un
     * arreglo de dimensión 1 de largo: largo*ancho, donde cada pixel tiene el largo en base
     * a la cantidad de canales (en este caso 1 ya que es escala de grises).
     * Para poder recorrer y simular como si fuera una matriz, basta con ir sumando y restando
     * valores al puntero hasta llegar al pixel deseado.
     * 
     * Se necesita buscar los pixeles adyacentes, es decir:
     *   |---|---|---|
     *   | 0 | 1 | 2 |
     *   |---|---|---|
     *   | 3 | 4 | 5 |
     *   |---|---|---|
     *   | 6 | 7 | 8 |
     *   |---|---|---|
     * 
     * Donde el pixel 4 es el pixel seleccionado (p).
     * Luego se recorre utilizando el pivote para obtener los pixeles e ir multiplicando en
     * base a la matriz laplaciana(c->lap_mak)
     * 
     * Como las dimensiones son conocidas, no es necesario hacer un ciclo,
     * El orden de lo que está abajo es buscar los pixeles: 0-1-2 3-4-5 6-7-8
     * 
     * Para cada pixel encontrado, se pregunta si es nulo(Caso de borde), si no lo es
     * se le multiplica por su valor correspondiente en la matriz laplaciana y luego se
     * suma a la variable pixel. Si la bandera useSamePixel está activada se utilizará (p) en
     * los casos de borde. Y si no está activada esta no se toma en cuenta(o en otro modo "se multiplica por 0")
     * 
     * 
     */



    // PARA PIXELES 0 1 2

    //PIXEL 0: largo+1 pixeles atrás
    if((pivot = p-img->channels*(img->width+1))!= NULL){
        pixel = pixel + (uint8_t)*pivot*c->lap_mask[0]; // PIXEL NO ES NULO SE MULTIPLICA POR EL VALOR DE LA MATRIZ
    }
    else if(useSamePixel){
        pixel = pixel + (uint8_t)*p*c->lap_mask[0]; // SI ES VERDADERO SE MULTIPLICA USANDO EL PIXEL ORIGINAL
    }

    //PIXEL 1: largo pixeles atrás
    if((pivot = p-img->channels*(img->width))!= NULL){
        pixel = pixel + (uint8_t)*pivot*c->lap_mask[1];
    }
    else if(useSamePixel){
        pixel = pixel + (uint8_t)*p*c->lap_mask[1];
    }

    //PIXEL 2: largo - 1 pixeles atrás
    if((pivot = p-img->channels*(img->width-1))!= NULL){
        pixel = pixel + (uint8_t)*pivot*c->lap_mask[2];
    }
    else if(useSamePixel){
        pixel = pixel + (uint8_t)*p*c->lap_mask[2];
    }


    //PIXEL 3: 1 pixel atras
    if((pivot = p-img->channels)!= NULL){
        pixel = pixel + (uint8_t)*pivot*c->lap_mask[3];
    }
    else if(useSamePixel){
        pixel = pixel + (uint8_t)*p*c->lap_mask[3];
    }
    //PIXEL 4: El mismo pixel
    if((pivot = p)!= NULL){
        pixel = pixel + (uint8_t)*pivot*c->lap_mask[4];
    }
    else if(useSamePixel){
        pixel = pixel + (uint8_t)*p*c->lap_mask[4];
    }

    //PIXEL 4: 1 pixel adelante
    if((pivot = p+img->channels) != NULL){
        pixel = pixel + (uint8_t)*pivot*c->lap_mask[5];
    }
    else if(useSamePixel){
        pixel = pixel + (uint8_t)*p*c->lap_mask[5];
    }



    //PIXEL 5: largo-1 pixeles adelante
    if((pivot = p+img->channels*(img->width-1))!= NULL){
        pixel = pixel + (uint8_t)*pivot*c->lap_mask[6];
    }
    else if(useSamePixel){
        pixel = pixel + (uint8_t)*p*c->lap_mask[6];
    }
    //PIXEL 5: largo pixeles adelante
    if((pivot = p+img->channels*(img->width))!= NULL){
        pixel = pixel + (uint8_t)*pivot*c->lap_mask[7];
    }
    else if(useSamePixel){
        pixel = pixel + (uint8_t)*p*c->lap_mask[7];
    }
    //PIXEL 5: largo+1 pixeles adelante
    if((pivot = p+img->channels*(img->width+1))!= NULL){
        pixel = pixel + (uint8_t)*pivot*c->lap_mask[8];
    }
    else if(useSamePixel){
        pixel = pixel + (uint8_t)*p*c->lap_mask[8];
    }

    return pixel;

}

/**
 * @brief Función que transforma la imagen a escala de grises
 * 
 * 
 * @param c Configuracion
 * @param img Estructura imagen
 */
void rgb_to_grayscale(Config * c,Image *img){
    // Inicializa las variables
    size_t img_size = img->width*img->height*img->channels; //Tamaño de la imagen
    int gray_channels = img->channels == 4 ? 2 : 1; // Nuevos canales
    size_t gray_img_size = img->width*img->height*gray_channels; //Tamaño de la imagen usando los nuevos canales
    
    unsigned char *gray_img = malloc(gray_img_size); //Asignar la memoria para la nueva imagen
    //Recorriendo la imagen
    //Idea extraida del video: https://solarianprogrammer.com/2019/06/10/c-programming-reading-writing-images-stb_image-libraries/
    for(unsigned char *p = img->data, *pg = gray_img; p != img->data + img_size; p+= img->channels, pg+= gray_channels){
        *pg = (uint8_t)((*p)*0.3 + (*p + 1)*0.59 + (*p+2)*0.11);
    }
    stbi_image_free(img->data); // Liberación de memoria
    img->data = gray_img; // Asignación de la nueva imagen
    img->channels = gray_channels;// Asignación de los nuevos canales
}


/**
 * @brief Función que aplica la máscara de filtro laplaciano a la imagen
 * 
 * @param c Estructura configuracion
 * @param img Estructura imagen
 */
void apply_lap_filter(Config * c,Image *img){
    size_t img_size = img->width*img->height*img->channels; //Tamaño de la imagen
    unsigned char *lap_img = malloc(img_size); //Asignar la memoria para la nueva imagen
    //Recorriendo la imagen
    //Idea extraida del video: https://solarianprogrammer.com/2019/06/10/c-programming-reading-writing-images-stb_image-libraries/
    for(unsigned char *p = img->data, *pg = lap_img; p != img->data + img_size; p+= img->channels, pg+= img->channels){
        *pg = (uint8_t)laplace(p,img,c); //LLAMADA A LA FUNCION, PARA OBTENER EL NUEVO VALOR DEL PIXEL
    }
    stbi_image_free(img->data);  // Liberación de memoria
    img->data = lap_img; // Asignación de la nueva imagen
}

/**
 * @brief Función que Binariza la imagen en base a un umbral
 * 
 * @param c Estructura configuración
 * @param img Estructura imagen
 */
void apply_binary(Config * c,Image *img){
    size_t img_size = img->width*img->height*img->channels; //Tamaño de la imagen
    unsigned char *binary_img = malloc(img_size); //Asignar la memoria para la nueva imagen
    //Recorriendo la imagen
    //Idea extraida del video: https://solarianprogrammer.com/2019/06/10/c-programming-reading-writing-images-stb_image-libraries/
    for(unsigned char *p = img->data, *pg = binary_img; p != img->data + img_size; p+= img->channels, pg+= img->channels){
        //Si el valor del pixel es mayor al umbral asignarlo a 255 sino asignarlo a 0
        if(*p > c->bin_threshold){
            *pg = (uint8_t)255; 
        }
        else{
            *pg = (uint8_t)0;
        }
    }
    stbi_image_free(img->data); // Liberación de memoria
    img->data = binary_img; // Asignación de la nueva imagen
}

/**
 * @brief Función que devuelve si una imagen es lo suficentemente
 * negra en base a un umbral
 * 
 * @param c Estructura de configuracion
 * @param img Estructura Imagen
 * @return int Si es suficientemente negra devuelve 1 de lo contrario 0
 */
int rate(Config * c,Image *img){
    size_t img_size = img->width*img->height*img->channels; //Tamaño de la imagen
    int black_count = 0; // Contador
    for(unsigned char *p = img->data; p != img->data + img_size; p+= img->channels){
        //Si es negro suma al contador
        if(*p == 0){
            black_count+=1; 
        }
    }
    // Si el porcentaje de pixeles negros es mayor o igual al umbral devuelve 1
    if((black_count*100.0/img->width*img->height) >= c->rating_threshold){
        return 1;
    }
    else{
        return 0;
    }

}

