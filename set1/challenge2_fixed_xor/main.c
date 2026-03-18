#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Convierte un caracter de hexadecimal a valor numerico y en caso de introducir caracter invalido devuelve -1
int valor_hex(char c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    return -1;
}

int main() {
    // Cadena a combinar dada por el challenge2
    const char *cadenaA = "1c0111001f010100061a024b53535009181c";
    const char *cadenaB = "686974207468652062756c6c277320657965";

    // Obtenemos la longitud de ambas cadenas
    int longitudA = strlen(cadenaA);
    int longitudB = strlen(cadenaB);

    // Comprobamos que las cadenas coinciden en longitud 
    if (longitudA != longitudB) return 1;
 

    // Reservamos la memoria para almacenar el resultado final (tenemos en cuenta el '\0')
    char *cadenaCombinada = malloc(longitudA + 1);

    if (cadenaCombinada == NULL) return 1; // Comprobacion de que se ha realizado la reserva de memoria correctamente

    // Recorremos de dos en dos porque en cada byte hay dos caracteres representados 
    for (int i = 0; i < longitudA; i += 2) {
        // COmprobamos que sean hexadecimales validos 
        if ((valor_hex(cadenaA[i]) == -1) || (valor_hex(cadenaA[i + 1]) == -1) || (valor_hex(cadenaB[i]) == -1) || (valor_hex(cadenaB[i + 1]) == -1)){
            free(cadenaCombinada);
            return 1;
        }
        // Convertimos dos caracteres hexadecimales en un byte
        int byte1 = valor_hex(cadenaA[i]) * 16 + valor_hex(cadenaA[i + 1]);
        int byte2 = valor_hex(cadenaB[i]) * 16 + valor_hex(cadenaB[i + 1]);
        // Realizamos la operacion XOR y la almacenamos en cadenaCombinada
        sprintf(cadenaCombinada + i, "%02x", byte1 ^ byte2);
    }
    // Agregamos el terminador al final de la cadena
    cadenaCombinada[longitudA] = '\0';

    // Imprimimos el resultado 
    printf("%s\n", cadenaCombinada);

    // Liberamos la memoria utilizada
    free(cadenaCombinada);
    return 0;
}