#include <stdio.h>
#include <stdlib.h>
#include <string.h>

unsigned char *pkcs7_padding(const unsigned char *texto, int longitud, int tam_bloque, int *longitud_final) {
    // Comprobamos que el tamano de bloque tenga sentido
    if (tam_bloque <= 0 || tam_bloque > 255) {
        return NULL;
    }
    // Calculamos cuantos bytes faltan para completar el bloque
    int padding = tam_bloque - (longitud % tam_bloque);
    if (padding == 0) {
        padding = tam_bloque;
    }
    // La longitud final sera la original mas los bytes de padding
    *longitud_final = longitud + padding;
    // Reservamos memoria para resultado
    unsigned char *resultado = malloc(*longitud_final);
    if (resultado == NULL) {
        return NULL;
    }
    // Copiamos primero el texto original
    memcpy(resultado, texto, longitud);
    // Rellenamos el final con el valor del padding
    memset(resultado + longitud, padding, padding);

    return resultado;
}

int main() {
    const unsigned char texto[] = "YELLOW SUBMARINE";
    int longitud = strlen((char *)texto); // Calculamos su longitud
    int tam_bloque = 20; // Tamano del bloque 
    int longitud_final;
    // Aplicamos PKCS#7
    unsigned char *resultado = pkcs7_padding(texto, longitud, tam_bloque, &longitud_final);
    // Si hubo error al reservar memoria
    if (resultado == NULL) {
        printf("Error al reservar memoria\n");
        return 1;
    }

    printf("Texto original: %s\n", texto);
    printf("Resultado con padding: ");
    // Recorremos byte a byte el resultado final
    for (int i = 0; i < longitud_final; i++) {
        // Si el byte es un caracter imprimible lo mostramos como letra
        if (resultado[i] >= 32 && resultado[i] <= 126) {
            printf("%c", resultado[i]);
        } else {
            // Si no es imprimible lo mostramos en hexadecimal
            printf("\\x%02x", resultado[i]);
        }
    }

    printf("\n");

    free(resultado);
    return 0;
}