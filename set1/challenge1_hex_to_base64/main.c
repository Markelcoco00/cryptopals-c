#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Fuente de informacion: RFC 4648 


// Convierte un caracter de hexadecimal a valor numerico y en caso de introducir caracter invalido devuelve -1
int valor_hex(char c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    return -1;
}

int main(void) {
    // Cadena a convertir dada por el challenge1
    const char *hex = "49276d206b696c6c696e6720796f757220627261696e206c696b65206120706f69736f6e6f7573206d757368726f6f6d"; 
    // Tabla Base64 para poder hacer la conversión 
    const char base64_tabla[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

    // Longitud de la cadena hexadecimal y número de bytes reales representados
    int hex_lon = (int)strlen(hex); 
    int bytes_lon = hex_lon / 2;

    // Reserva de memoria para los bytes necesarios
    unsigned char *bytes = malloc(bytes_lon);

    if (bytes == NULL) return 1; // Comprobacion de que se ha realizado la reserva de memoria correctamente

    // Conversion de hex a bytes teniendo en cuenta que dos caracteres hex conforman un byte
    for (int i = 0; i < bytes_lon; i++) {
        int alto = valor_hex(hex[2 * i]);
        int bajo = valor_hex(hex[2 * i + 1]);

        if (alto < 0 || bajo < 0) {
            free(bytes);
            return 1;
        }

        bytes[i] = (alto << 4) | bajo;
    }
    // Cada 3 bytes debemos sacar 4 caracteres por lo tanto reservamos la memoria necesaria
    int cadenaBase64_lon = 4 * ((bytes_lon + 2) / 3);
    char *cadenaBase64 = malloc(cadenaBase64_lon + 1);

    if (cadenaBase64 == NULL) {
        free(bytes);
        return 1;
    }

    // Procesamos los bloques de 3 bytes
    int i, j = 0;
    for (i = 0; i < bytes_lon; i += 3) {
        // Se lee por bloques (3 bytes) si faltan bytes para terminar de rellenar el bloque agregamos 0s hasta completar
        int b0 = bytes[i];
        int b1 = (i + 1 < bytes_lon) ? bytes[i + 1] : 0;
        int b2 = (i + 2 < bytes_lon) ? bytes[i + 2] : 0;

        // Agrupamos los 3 bytes en un solo bloque 
        int bloque3bytes = (b0 << 16) | (b1 << 8) | b2;

        // Vamos extrayendo por cada bloque 4 grupos de 6 bits y mapeando con la tabla base64
        cadenaBase64[j++] = base64_tabla[(bloque3bytes >> 18) & 0x3F];
        cadenaBase64[j++] = base64_tabla[(bloque3bytes >> 12) & 0x3F];
        // En caso de que no haya bytes utiles agregamos el pad 
        cadenaBase64[j++] = (i + 1 < bytes_lon) ? base64_tabla[(bloque3bytes >> 6) & 0x3F] : '=';
        cadenaBase64[j++] = (i + 2 < bytes_lon) ? base64_tabla[bloque3bytes & 0x3F] : '=';
    }

    cadenaBase64[j] = '\0';
    // Imprimimos el resultado
    printf("%s\n", cadenaBase64);   

    // Liberamos la memoria utilizada
    free(bytes);
    free(cadenaBase64);

    return 0;
}