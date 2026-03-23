#include <stdio.h>
#include <string.h>
#include <limits.h>
#include "../common/utils.h"

#define MAX_DATOS 20000 // Tamano maximo de datos
#define MAX_CLAVE 40 // Tamano maximo de clave  
#define NUM_CANDIDATOS 3 // Numero de candidatos de tamano de clave que guardamos

// Estructura para guardar un tamano de clave y su distancia asociada
typedef struct {
    int tamano_clave;
    double distancia;
} CandidatoClave;

// Calcula la distancia media normalizada entre bloques consecutivos
double distancia_media_normalizada(const unsigned char *datos, int longitud, int tamano_clave) {
    // Calculamos cuantos bloques completos caben con este tamano
    int num_bloques = longitud / tamano_clave;

    // Si hay muy pocos bloques, este tamano no nos sirve
    if (num_bloques < 4) {
        return 1e20;
    }
    // Usamos como mucho 8 bloques para no comparar demasiado
    int bloques_a_usar = num_bloques;
    if (bloques_a_usar > 8) {
        bloques_a_usar = 8;
    }
    // Aqui vamos acumulando las distancias
    double suma = 0.0;
    int comparaciones = 0;
    // Comparamos cada bloque con el siguiente
    for (int i = 0; i < bloques_a_usar - 1; i++) {
         // Distancia de Hamming y normalizamos dividiendo entre el tamano de clave
        double distancia = (double)distancia_hamming(
            datos + i * tamano_clave,
            datos + (i + 1) * tamano_clave,
            tamano_clave
        ) / tamano_clave;

        suma += distancia;
        comparaciones++;
    } 
    // Devolvemos la media de todas las comparaciones
    return suma / comparaciones;
}
// Busca los mejores tamanos de clave segun la distancia media
void buscar_mejores_tamanos_clave(const unsigned char *datos, int longitud, CandidatoClave *candidatos) {
    // Inicializamos los candidatos
    for (int i = 0; i < NUM_CANDIDATOS; i++) {
        candidatos[i].tamano_clave = 0;
        candidatos[i].distancia = 1e20;
    }
    // Probamos todos los tamanos posibles 2-40
    for (int tamano = 2; tamano <= MAX_CLAVE; tamano++) {
        double distancia = distancia_media_normalizada(datos, longitud, tamano);
        // Si encontramos un candidato mejor lo metemos en la lista
        for (int i = 0; i < NUM_CANDIDATOS; i++) {
            if (distancia < candidatos[i].distancia) {
                // Desplazamos hacia abajo los peores candidatos
                for (int j = NUM_CANDIDATOS - 1; j > i; j--) {
                    candidatos[j] = candidatos[j - 1];
                }
                // Guardamos el nuevo candidato en su posicion   
                candidatos[i].tamano_clave = tamano;
                candidatos[i].distancia = distancia;
                break;
            }
        }
    }
}
// Reconstruimos la clave suponiendo que ya sabemos su tamano
void obtener_clave(const unsigned char *datos, int longitud, unsigned char *clave, int tamano_clave) {
    // Vamos columna por columna
    for (int columna = 0; columna < tamano_clave; columna++) {
        unsigned char bloque[MAX_DATOS];
        int longitud_bloque = 0;
        // Cogemos todos los bytes que han sido cifrados con el mismo byte de clave
        for (int i = columna; i < longitud; i += tamano_clave) {
            bloque[longitud_bloque++] = datos[i];
        }
        // Resolvemos esta columna como si fuera un single-byte XOR
        clave[columna] = encontrar_mejor_byte_clave(bloque, longitud_bloque);
    }
    // Cerramos la clave como cadena para poder imprimirla
    clave[tamano_clave] = '\0';
}

int main() {
    char texto_base64[MAX_DATOS]; // Buffer para guardar el texto del archivo
    unsigned char datos_cifrados[MAX_DATOS]; // Buffer para guardar los bytes ya decodificados
    // Leemos el archivo
    if (leer_archivo_base64("set1/challenge6_Break_repeating_key_xor/text5.txt", texto_base64, MAX_DATOS) < 0) {
        printf("Error: no se pudo leer el archivo.\n");
        return 1;
    }
    // Pasamos el contenido base64 a bytes reales
    int longitud_datos = base64_a_bytes(texto_base64, datos_cifrados);
    // Por si falla la conversion
    if (longitud_datos < 0) {
        printf("Error: no se pudo decodificar el base64.\n");
        return 1;
    }
    // Guardaremos aqui los mejores tamanos de clave encontrados
    CandidatoClave candidatos[NUM_CANDIDATOS];
    // Buscamos los mejores candidatos
    buscar_mejores_tamanos_clave(datos_cifrados, longitud_datos, candidatos);
    // Variables para quedarnos con la mejor solucion final
    int mejor_puntuacion = INT_MIN;
    int mejor_tamano_clave = 0;
    unsigned char mejor_clave[MAX_CLAVE + 1];
    unsigned char mejor_texto[MAX_DATOS];
    // Probamos cada uno de los candidatos encontrados
    for (int i = 0; i < NUM_CANDIDATOS; i++) {
        int tamano_clave = candidatos[i].tamano_clave;
        unsigned char clave_actual[MAX_CLAVE + 1];
        unsigned char texto_actual[MAX_DATOS];
        // Obtenemos la clave para este tamano
        obtener_clave(datos_cifrados, longitud_datos, clave_actual, tamano_clave);
        // Desciframos el texto usando esa clave
        aplicar_xor_clave_repetida(datos_cifrados, longitud_datos, clave_actual, tamano_clave, texto_actual);
        // Puntuar el resultado
        int puntuacion_actual = puntuar_texto(texto_actual, longitud_datos);
        // Si este resultado es mejor que los anteriores nos lo quedamos
        if (puntuacion_actual > mejor_puntuacion) {
            mejor_puntuacion = puntuacion_actual;
            mejor_tamano_clave = tamano_clave;
            strcpy((char *)mejor_clave, (char *)clave_actual);
            strcpy((char *)mejor_texto, (char *)texto_actual);
        }
    }
    // Mostramos el mejor resultado encontrado
    printf("Tamano de clave: %d\n", mejor_tamano_clave);
    printf("Clave encontrada: '%s'\n\n", mejor_clave);
    printf("Texto descifrado:\n%s\n", mejor_texto);
    return 0;
}