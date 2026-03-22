#ifndef UTILS_H
#define UTILS_H

int valor_hex(char c);
void convertir_hex(unsigned char *datos, int longitud);

int puntuacion_caracter(unsigned char c);
void clasificar_segmento(int longitud_segmento, int *segmentos_razonables, int *segmentos_excesivamente_largos);
int puntuar_texto(const unsigned char *texto, int longitud);

void aplicar_xor_clave_repetida(const unsigned char *entrada, int longitud_entrada,const unsigned char *clave, int longitud_clave, unsigned char *salida);

unsigned char encontrar_mejor_byte_clave(const unsigned char *bloque, int longitud);

int valor_base64(char c);
int leer_archivo_base64(const char *ruta, char *buffer, int max_buffer);
int base64_a_bytes(const char *entrada, unsigned char *salida);

int contar_bits_1(unsigned char byte);
int distancia_hamming(const unsigned char *bloque1, const unsigned char *bloque2, int longitud);


#endif