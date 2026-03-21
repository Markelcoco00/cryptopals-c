#include <stdio.h>
#include <string.h>

// Imprimimos los datos en formato hexadecimal
void convertir_hex(unsigned char *datos, int longitud) {
    for (int i = 0; i < longitud; i++) {
        printf("%02x", datos[i]);
    }
    printf("\n");
}


void xor_clave_repetida(unsigned char *texto, unsigned char *clave, unsigned char *resultado) {
    int longitud_texto = strlen((char *)texto);
    int longitud_clave = strlen((char *)clave);

    for (int i = 0; i < longitud_texto; i++) {
        resultado[i] = texto[i] ^ clave[i % longitud_clave];
    }
}

int main() {
    // Texto que vamos cifrar
    unsigned char texto[] = "Burning 'em, if you ain't quick and nimble\nI go crazy when I hear a cymbal";
    // Clave para cifrar
    unsigned char clave[] = "ICE";

    // Tamano del texto para guardar el resultado
    int longitud = strlen((char *)texto);
    unsigned char resultado[longitud];
    // Hacemos la XOR con la clave repetida
    xor_clave_repetida(texto, clave, resultado);

    // Ressultado en formato hexadecimal
    convertir_hex(resultado, longitud);

    return 0;
}