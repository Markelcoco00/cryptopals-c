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
    FILE *archivo = fopen("set1/challenge4_single_character_xor/text4.txt", "r");
    if (archivo == NULL) {
        printf("Error: no se pudo abrir el archivo.\n");
        return 1;
    }

    char linea[256];
    int mejor_puntuacion_global = INT_MIN;
    int mejor_clave_global = 0;
    int mejor_numero_linea = 0;
    char mejor_linea_hex[256] = "";

    unsigned char mejor_texto_global[256];

    int numero_linea = 0;

    while (fgets(linea, sizeof(linea), archivo) != NULL) {
        numero_linea++;

        int longitud_hex = strlen(linea);

        // Eliminar salto de linea si existe
        if (longitud_hex > 0 && linea[longitud_hex - 1] == '\n') {
            linea[longitud_hex - 1] = '\0';
            longitud_hex--;
        }
        if (longitud_hex > 0 && linea[longitud_hex - 1] == '\r') {
            linea[longitud_hex - 1] = '\0';
            longitud_hex--;
        }

        int num_bytes = longitud_hex / 2;

        unsigned char *texto_candidato = malloc(num_bytes + 1);
        if (texto_candidato == NULL) {
            printf("Error al reservar memoria.\n");
            fclose(archivo);
            return 1;
        }

        // Probar todas las claves posibles de un byte de 0 a 255
        for (int clave = 0; clave < 256; clave++) {
            int posicion_texto = 0;
            int linea_valida = 1;

            // Vamos recorriendo la cadena hexadecimal de dos en dos ya que en un byte entran dos caracteres
            for (int i = 0; i < longitud_hex; i += 2) {
                int valor_alto = valor_hex(linea[i]);
                int valor_bajo = valor_hex(linea[i + 1]);

                if (valor_alto < 0 || valor_bajo < 0) {
                    linea_valida = 0;
                    break;
                }

                // Reconstruyo el byte
                int byte_cifrado = valor_alto * 16 + valor_bajo;
                texto_candidato[posicion_texto] = byte_cifrado ^ clave;
                posicion_texto++;
            }

            if (!linea_valida) {
                continue;
            }

            texto_candidato[num_bytes] = '\0';

            // Calculo la puntuacion del texto a evaluar
            int puntuacion_actual = puntuar_texto(texto_candidato, num_bytes);

            // Si es mejor que todas las anteriores la guardo como mejor global
            if (puntuacion_actual > mejor_puntuacion_global) {
                mejor_puntuacion_global = puntuacion_actual;
                mejor_clave_global = clave;
                mejor_numero_linea = numero_linea;
                strcpy(mejor_linea_hex, linea);
                strcpy((char *)mejor_texto_global, (char *)texto_candidato);
            }
        }

        free(texto_candidato);
    }

    fclose(archivo);

    printf("Mejor clave: 0x%02x '%c'  ||  Puntuacion: %d  ||  En la linea: %d", mejor_clave_global, mejor_clave_global,mejor_puntuacion_global,mejor_numero_linea);
    printf("\n");
    printf("Linea cifrada: %s\n",mejor_linea_hex);
    printf("Texto descifrado: %s \r", mejor_texto_global);

    return 0;
}