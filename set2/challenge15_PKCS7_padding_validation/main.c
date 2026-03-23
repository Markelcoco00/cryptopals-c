#include <stdio.h>
#include <string.h>

// Devuelve 1 si el padding es valido y 0 si no lo es
int validar_pkcs7(const unsigned char *datos, int longitud) {
    // No puede ser buffer vacío
    if (longitud <= 0) {
        return 0;
    }

    // ultimo byte indica cuantos bytes de padding hay
    unsigned char padding = datos[longitud - 1];

    // El padding no puede ser 0 ni mayor que la longitud total
    if (padding == 0 || padding > longitud) {
        return 0;
    }

    // Comprobamos que los últimos "padding" bytes tengan todos mismo valor
    for (int i = 0; i < padding; i++) {
        if (datos[longitud - 1 - i] != padding) {
            return 0;
        }
    }

    return 1;
}

int main() {
    unsigned char valido[]   = "ICE ICE BABY\x04\x04\x04\x04";
    unsigned char invalido1[] = "ICE ICE BABY\x05\x05\x05\x05";
    unsigned char invalido2[] = "ICE ICE BABY\x01\x02\x03\x04";

    int longitud_valido = sizeof(valido) - 1;
    int longitud_invalido1 = sizeof(invalido1) - 1;
    int longitud_invalido2 = sizeof(invalido2) - 1;

    printf("Caso 1: %s", validar_pkcs7(valido, longitud_valido) ? "Padding valido" : "Padding invalido");
    printf("\t\tLongitud de cadena: %d\n",longitud_valido);
    for (int i = 0; i < longitud_valido; i++) {
        // Si el byte es un caracter imprimible lo mostramos como letra
        if (valido[i] >= 32 && valido[i] <= 126) {
            printf("%c", valido[i]);
        } else {
            // Si no es imprimible lo mostramos en hexadecimal
            printf("\\x%02x", valido[i]);
        }
    }
    printf("\n");
    printf("Caso 2: %s", validar_pkcs7(invalido1, longitud_invalido1) ? "Padding valido" : "Padding invalido");
    printf("\t\tLongitud de cadena: %d\n",longitud_invalido1);
    for (int i = 0; i < longitud_invalido1; i++) {
        // Si el byte es un caracter imprimible lo mostramos como letra
        if (invalido1[i] >= 32 && invalido1[i] <= 126) {
            printf("%c", invalido1[i]);
        } else {
            // Si no es imprimible lo mostramos en hexadecimal
            printf("\\x%02x", invalido1[i]);
        }
    }
    printf("\n");
    printf("Caso 3: %s", validar_pkcs7(invalido2, longitud_invalido2) ? "Padding valido" : "Padding invalido");
    printf("\t\tLongitud de cadena: %d\n",longitud_invalido2);
    for (int i = 0; i < longitud_invalido2; i++) {
        // Si el byte es un caracter imprimible lo mostramos como letra
        if (invalido2[i] >= 32 && invalido2[i] <= 126) {
            printf("%c", invalido2[i]);
        } else {
            // Si no es imprimible lo mostramos en hexadecimal
            printf("\\x%02x", invalido2[i]);
        }
    }
    printf("\n");

    return 0;
}