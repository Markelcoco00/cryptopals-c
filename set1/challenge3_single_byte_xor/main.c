#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>

// Convierte un caracter hexadecimal a valor numerico y devuelve -1 si es invalido
int valor_hex(char c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    return -1;
}

// Da una puntuacion al caracter segun su frecuencia en ingles basado en Norvig
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
        case 'x': return 0;
        case 'j': return 0;
        case 'q': return 0;
        case 'z': return 0;

        case '\'': case '.': case ',': case '!': case '?': case '-': return 1;

        default:
            if (isalpha(c)) return 3; // Si es una letra no recogida arriba nos aseguramos
            if (isprint(c)) return 0; // Si es imprimible y no letra le damos valor neutro
            return -20; // Si no es letra ni imprimible penalizamos
    }
}
// Evaluo cada segmento  
void clasificar_segmento(int longitud_segmento, int *segmentos_razonables, int *segmentos_excesivamente_largos) {
    if (longitud_segmento >= 2 && longitud_segmento <= 12) {
        (*segmentos_razonables)++;
    } else if (longitud_segmento > 12) {
        (*segmentos_excesivamente_largos)++;
    }
}
// Puntua el texto para poder discriminar si es un texto coherente o no
int puntuar_texto(const unsigned char *texto, int longitud) {
    int puntuacion = 0;
    int num_espacios = 0;
    int espacios_dobles = 0;

    int longitud_segmento = 0;
    int segmentos_razonables = 0;
    int segmentos_excesivamente_largos = 0;

    for (int i = 0; i < longitud; i++) {
        unsigned char c = texto[i];
        // Primero sumamos la puntuacion individual
        puntuacion += puntuacion_caracter(c);

        if (c == ' ') {
            num_espacios++;
            
            // Tenemos en cuenta que dos espacios seguidos no suele ser bueno 
            if (i > 0 && texto[i - 1] == ' ') {
                espacios_dobles++;
            }
            // Si veniamos recogiendo letras aqui acabaria el segmento entonces valoramos
            if (longitud_segmento > 0) {
                clasificar_segmento(longitud_segmento, &segmentos_razonables, &segmentos_excesivamente_largos);
                longitud_segmento = 0;
            }
        } else if (isalpha(c)) {
            // Mientras siga leyendo letras seguimos contando
            longitud_segmento++;
        } else {
            // Cualquier otra cosa ya sea simbolo raro damos por concluido el segmento y valoramos
            if (longitud_segmento > 0) {
                clasificar_segmento(longitud_segmento, &segmentos_razonables, &segmentos_excesivamente_largos);
                longitud_segmento = 0;
            }
        }
    }

    // Procesar el ultimo segmento si el texto acaba en letra y no en espacio 
    if (longitud_segmento > 0) {
        clasificar_segmento(longitud_segmento, &segmentos_razonables, &segmentos_excesivamente_largos);
    }

    // Hacemos un balance para puntuar la coherencia estructural del texto 
    if (num_espacios == 0) puntuacion -= 30;
    puntuacion += segmentos_razonables * 4;
    puntuacion -= segmentos_excesivamente_largos * 6;
    puntuacion -= espacios_dobles * 10;

    return puntuacion;
}

int main() {
    // Cadena hexadecimal del reto
    char *texto_hex = "1b37373331363f78151b7f2b783431333d78397828372d363c78373e783a393b3736";

    int longitud_hex = strlen(texto_hex);
    int num_bytes = longitud_hex / 2;
    // Resergo memoria para cada texto candidatop y para almacenar el mejor de ellos 
    unsigned char *texto_candidato = malloc(num_bytes + 1);
    unsigned char *mejor_texto = malloc(num_bytes + 1);

    if (texto_candidato == NULL || mejor_texto == NULL) {
        printf("Error al reservar memoria.\n");
        free(texto_candidato);
        free(mejor_texto);
        return 1;
    }

    int mejor_puntuacion = INT_MIN;
    int mejor_clave = 0;

    // Probar todas las claves posibles de un byte de 0 a 255
    for (int clave = 0; clave < 256; clave++) {
        int posicion_texto = 0;

        // Vamos recorriendo la cadena hexadecimal de dos en dos ya que en un byte entran dos caracteres
        for (int i = 0; i < longitud_hex; i += 2) {
            int valor_alto = valor_hex(texto_hex[i]);
            int valor_bajo = valor_hex(texto_hex[i + 1]);

            if (valor_alto < 0 || valor_bajo < 0) {
                printf("Error: caracter hexadecimal invalido.\n");
                free(texto_candidato);
                free(mejor_texto);
                return 1;
            }
            // Reconstruyo el byte 
            int byte_cifrado = valor_alto * 16 + valor_bajo;
            texto_candidato[posicion_texto] = byte_cifrado ^ clave;
            posicion_texto++;
        }

        texto_candidato[num_bytes] = '\0';
        // Calculo la puntuacion del texto a evaluar 
        int puntuacion_actual = puntuar_texto(texto_candidato, num_bytes);
        // Si es mejor que las anteriores la guarda como posible clave ganadora 
        if (puntuacion_actual > mejor_puntuacion) {
            mejor_puntuacion = puntuacion_actual;
            mejor_clave = clave;
            strcpy((char *)mejor_texto, (char *)texto_candidato);
        }
    }

    printf("Mejor clave: 0x%02x '%c'", mejor_clave, mejor_clave);
    printf("\n");


    printf("Texto descifrado: %s\n", mejor_texto);

    free(texto_candidato);
    free(mejor_texto);

    return 0;
}