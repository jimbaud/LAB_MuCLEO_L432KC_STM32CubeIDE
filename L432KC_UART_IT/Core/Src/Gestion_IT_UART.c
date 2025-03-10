/*
 * Gestion_IT_UART.c
 *
 *  Created on: Nov 15, 2023
 *      Author: joel.imbaud
 */

#include "Gestion_IT_UART.h"

int resultat_int = 1000; // Variable globale contenant le résultat
unsigned char MSG1[32]; // Variable globale du buffer de réception

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
	static int i = 0; // Variable locale utilisée uniquement dans la fonction, static permet de garder la valeur
	static int trame_en_cours = 0; // Idem

	if (trame_en_cours == 1) { // La réception est déjà commencée
		if (MSG1[i] != 0x0D) { // Le caractère reçu est différent du retour chariot (CR) 0X0D ou retour à la ligne (LF) 0X0A
			if (i > 4) { // On se limite à 3 caractères en plus du #
				i = 0; // La trame est plus longue que prévue, on réinitialise pour attendre une nouvelle trame
				trame_en_cours = 0;
			} else {
				i++; // La trame est en cours d'acquisition
			}
		} else { // La trame est totalement reçue, elle est envoyée dans la variable resultat_int
			resultat_int = atoi((char*)&MSG1[1]); // Fonction de conversion de la bibliothèque math.h www.cplusplus.com/reference/cstdlib/atoi/
			char resp[50];
			snprintf(resp, sizeof(resp), "\n\rVous avez entrez la valeur de : %d ms.\r\n", resultat_int);// Créer le message à transmettre
			HAL_UART_Transmit(&huart2, (uint8_t*)resp, strlen(resp), HAL_MAX_DELAY); // Transmettre le message via UART
			i = 0;
			trame_en_cours = 0;
		}
	} else { // La réception n'est pas commencée
		if (MSG1[0] == '#') { // On attend le premier caractère de la chaîne de réception ici #
			i = 1; // On initialise le compteur de caractères
			trame_en_cours = 1; // On indique que la trame est en cours de traitement
		} else { // Le premier caractère n'a pas été reçu
			i = 0;
			trame_en_cours = 0; // Pas de trame en cours de traitement
		}
	}
	HAL_UART_Receive_IT(&huart2, &MSG1[i], 1); // On réinitialise l'interruption sur l'UART
}
