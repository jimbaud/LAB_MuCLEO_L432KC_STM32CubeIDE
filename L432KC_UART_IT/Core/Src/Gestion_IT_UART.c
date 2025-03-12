#include "Gestion_IT_UART.h"

int resultat_int = 1000; // Valeur par défaut du délai
unsigned char MSG1[MSG_SIZE]; // Buffer de réception

// Variables internes
static uint8_t  index_reception = 0;
static uint8_t trame_en_cours = 0;

/**
 * @brief Traite la trame reçue et met à jour la valeur de `resultat_int`
 */
void traiter_trame() {
    resultat_int = atoi((char*)&MSG1[1]); // Conversion du nombre reçu
    char message[50];
    snprintf(message, sizeof(message), "\n\rVous avez entré la valeur de : %d ms.\r\n", resultat_int);
    HAL_UART_Transmit(&huart2, (uint8_t*)message, strlen(message), HAL_MAX_DELAY);
}

/**
 * @brief Callback déclenché à la réception d'un octet via UART
 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {

    if (!trame_en_cours) {  // Début d'une nouvelle trame
        if (MSG1[0] == '#') {
            trame_en_cours = 1;
            index_reception = 1;
        }
    }
    else {  // Si une trame est en cours de réception
        if (MSG1[index_reception] == '\r') { // Fin de trame détectée
            traiter_trame();
            trame_en_cours = 0;
            index_reception = 0;
        }
        else if (index_reception >= 4) {  // Sécurité pour éviter un dépassement de mémoire
            trame_en_cours = 0;
            index_reception = 0;
        }
        else {
            index_reception++; // On continue la réception
        }
    }

    // Réactive la réception en interruption pour le prochain octet
    HAL_UART_Receive_IT(&huart2, &MSG1[index_reception], 1);
}
