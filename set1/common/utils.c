#include "utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>
int valor_hex(char c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    return -1;
}

// Imprime un array de bytes en formato hexadecimal
void convertir_hex(unsigned char *datos, int longitud) {
    for (int i = 0; i < longitud; i++) {
        printf("%02x", datos[i]);
    }
    printf("\n");
}

// Da una puntuacion al caracter segun su frecuencia en ingles
int puntuacion_caracter(unsigned char c) {
    c = tolower((unsigned char)c);

    switch (c) {
        case ' ': return 20;
        case 'e': return 12;
        case 't': return 9;
        case 'a': return 8;
        case 'o': return 8;
        case 'i': return 8;
        case 'n': return 7;
        case 's': return 7;
        case 'r': return 6;
        case 'h': return 5;
        case 'l': return 4;
        case 'd': return 4;
        case 'c': return 3;
        case 'u': return 3;
        case 'm': return 3;
        case 'f': return 2;
        case 'p': return 2;
        case 'g': return 2;
        case 'w': return 2;
        case 'y': return 2;
        case 'b': return 1;
        case 'v': return 1;
        case 'k': return 1;
        case '\'': case '.': case ',': case '!': case '?': case '-': return 1;

        default:
            if (isalpha(c)) return 3;
            if (isprint(c)) return 0;
            return -20;
    }
}

// Clasifica la longitud de las palabras
void clasificar_segmento(int longitud_segmento, int *segmentos_razonables, int *segmentos_excesivamente_largos) {
    if (longitud_segmento >= 2 && longitud_segmento <= 12) {
        (*segmentos_razonables)++;
    } else if (longitud_segmento > 12) {
        (*segmentos_excesivamente_largos)++;
    }
}

// Puntua un texto completo
int puntuar_texto(const unsigned char *texto, int longitud) {
    int puntuacion = 0;
    int num_espacios = 0;
    int espacios_dobles = 0;

    int longitud_segmento = 0;
    int segmentos_razonables = 0;
    int segmentos_excesivamente_largos = 0;

    for (int i = 0; i < longitud; i++) {
        unsigned char c = texto[i];
        puntuacion += puntuacion_caracter(c);

        if (c == ' ') {
            num_espacios++;

            if (i > 0 && texto[i - 1] == ' ') {
                espacios_dobles++;
            }

            if (longitud_segmento > 0) {
                clasificar_segmento(longitud_segmento, &segmentos_razonables, &segmentos_excesivamente_largos);
                longitud_segmento = 0;
            }
        } else if (isalpha(c)) {
            longitud_segmento++;
        } else {
            if (longitud_segmento > 0) {
                clasificar_segmento(longitud_segmento, &segmentos_razonables, &segmentos_excesivamente_largos);
                longitud_segmento = 0;
            }
        }
    }

    if (longitud_segmento > 0) {
        clasificar_segmento(longitud_segmento, &segmentos_razonables, &segmentos_excesivamente_largos);
    }

    if (num_espacios == 0) puntuacion -= 30;
    puntuacion += segmentos_razonables * 4;
    puntuacion -= segmentos_excesivamente_largos * 6;
    puntuacion -= espacios_dobles * 10;

    return puntuacion;
}

// Aplica XOR con clave repetida a datos binarios
void aplicar_xor_clave_repetida(const unsigned char *entrada, int longitud_entrada,
                                const unsigned char *clave, int longitud_clave,
                                unsigned char *salida) {
    for (int i = 0; i < longitud_entrada; i++) {
        salida[i] = entrada[i] ^ clave[i % longitud_clave];
    }

    salida[longitud_entrada] = '\0';
}

// Reutiliza la logica del challenge 3 para encontrar el mejor byte de clave
unsigned char encontrar_mejor_byte_clave(const unsigned char *bloque, int longitud) {
    int mejor_puntuacion = INT_MIN;
    unsigned char mejor_clave = 0;
    unsigned char texto_candidato[20000];

    for (int clave = 0; clave < 256; clave++) {
        for (int i = 0; i < longitud; i++) {
            texto_candidato[i] = bloque[i] ^ (unsigned char)clave;
        }

        int puntuacion_actual = puntuar_texto(texto_candidato, longitud);

        if (puntuacion_actual > mejor_puntuacion) {
            mejor_puntuacion = puntuacion_actual;
            mejor_clave = (unsigned char)clave;
        }
    }

    return mejor_clave;
}

// Convierte un caracter base64 a valor numerico
int valor_base64(char c) {
    if (c >= 'A' && c <= 'Z') return c - 'A';
    if (c >= 'a' && c <= 'z') return c - 'a' + 26;
    if (c >= '0' && c <= '9') return c - '0' + 52;
    if (c == '+') return 62;
    if (c == '/') return 63;
    return -1;
}

// Lee el archivo base64 ignorando saltos de linea
int leer_archivo_base64(const char *ruta, char *buffer, int max_buffer) {
    FILE *archivo = fopen(ruta, "r");

    if (archivo == NULL) {
        return -1;
    }

    int longitud = 0;
    int c;

    while ((c = fgetc(archivo)) != EOF) {
        if (c == '\n' || c == '\r') {
            continue;
        }

        if (longitud >= max_buffer - 1) {
            fclose(archivo);
            return -1;
        }

        buffer[longitud++] = (char)c;
    }

    buffer[longitud] = '\0';
    fclose(archivo);

    return longitud;
}

// Convierte una cadena base64 en bytes
int base64_a_bytes(const char *entrada, unsigned char *salida) {
    int longitud = strlen(entrada);
    int i = 0;
    int j = 0;

    while (i < longitud) {
        int a = valor_base64(entrada[i++]);
        int b = valor_base64(entrada[i++]);
        int c = (entrada[i] == '=') ? -1 : valor_base64(entrada[i]);
        i++;
        int d = (entrada[i] == '=') ? -1 : valor_base64(entrada[i]);
        i++;

        if (a < 0 || b < 0) {
            return -1;
        }

        salida[j++] = (a << 2) | (b >> 4);

        if (c >= 0) {
            salida[j++] = ((b & 0x0F) << 4) | (c >> 2);

            if (d >= 0) {
                salida[j++] = ((c & 0x03) << 6) | d;
            }
        }
    }

    return j;
}

// Cuenta cuantos bits a 1 tiene un byte
int contar_bits_1(unsigned char byte) {
    int contador = 0;

    while (byte != 0) {
        contador += byte & 1;
        byte >>= 1;
    }

    return contador;
}

// Calcula la distancia de Hamming entre dos bloques
int distancia_hamming(const unsigned char *bloque1, const unsigned char *bloque2, int longitud) {
    int distancia = 0;

    for (int i = 0; i < longitud; i++) {
        distancia += contar_bits_1(bloque1[i] ^ bloque2[i]);
    }

    return distancia;
}