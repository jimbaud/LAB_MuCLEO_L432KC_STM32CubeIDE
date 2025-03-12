/*
 * menu.c
 *
 *  Created on: Sep 9, 2023
 *      Author: Joel
 */

#include "menu.h"
#include "testimg.h"

// Textes dans les rectangles
const char *menuItems[] = {
		"Option 1",
		"Option 2",
		"Option 3",
		"Option 4"
};

// Variables pour le suivi de l'état du bouton

volatile uint32_t buttonPressTime = 0;
volatile uint32_t lastButtonReleaseTime = 0;
int selectedMenuItem = 0; // L'élément de menu sélectionné
volatile uint32_t lastClickTime = 0;

// Variable pour suivre le décalage dans le menu

int menuOffset = 0;

// Fonction pour dessiner le menu

void drawMenu(void) {
	ST7735_FillScreenFast(COLOR_BG);

	int visibleItems = 4; // Nombre d'éléments de menu visibles à la fois
	int startItem = selectedMenuItem - 1;
	if (startItem < 0) {
		startItem = 0;
	} else if (startItem > 3) {
		startItem = 3;
	}

	for (int i = startItem; i < startItem + visibleItems; i++) {
		if (i < 0 || i >= 4) {
			continue; // Éviter de dessiner des éléments de menu en dehors de la plage
		}

		uint16_t rectColor = (i == selectedMenuItem) ? COLOR_SELECTED : COLOR_RECT;
		uint16_t textColor = (i == selectedMenuItem) ? COLOR_BG : COLOR_TEXT;

		// Dessine le rectangle
		ST7735_FillRectangleFast(RECT_X, RECT_Y1 + (i - startItem) * (RECT_HEIGHT + RECT_SPACING), RECT_WIDTH, RECT_HEIGHT, rectColor);

		// Affiche le texte au centre du rectangle
		int textX = RECT_X + (RECT_WIDTH - strlen(menuItems[i]) * 6) / 2;
		int textY = RECT_Y1 + (i - startItem) * (RECT_HEIGHT + RECT_SPACING) + (RECT_HEIGHT - 8) / 2;
		ST7735_WriteString(textX, textY, menuItems[i], Font_7x10, textColor, rectColor);
	}
}

// Fonction pour naviguer dans le menu en fonction de la direction (+1 ou -1)

void navigateMenu(int direction) {
	selectedMenuItem += direction;
	if (selectedMenuItem < 0) {
		selectedMenuItem = 3;
	} else if (selectedMenuItem > 3) {
		selectedMenuItem = 0;
	}

	drawMenu();
}

void drawBarGraph(float value) {

	static float previous_value = -1; // Initialiser avec une valeur impossible pour forcer la première mise à jour
	const float tolerance = 1.0; // Définir un seuil de tolérance
	// Limiter la valeur entre 0 et 100
	if (value < 0) {
		value = 0;
	} else if (value > 100) {
		value = 100;
	}
	// Vérifier si la différence entre la nouvelle valeur et la précédente dépasse le seuil de tolérance
	if (fabs(value - previous_value) > tolerance) {
		// Calculer la largeur de la barre en fonction de la valeur
		int barWidth = (int)(BAR_WIDTH * (value / 100));

		// Définir les couleurs
		uint16_t greenColor = ST7735_GREEN;
		uint16_t orangeColor = ST7735_ORANGE;
		uint16_t redColor = ST7735_RED;
		uint16_t backgroundColor = ST7735_BLACK; // Couleur de l'arrière-plan

		// Dessiner le cadre de la barre (bordures)
		ST7735_DrawRectangle(BAR_X - 1, BAR_Y - 1, BAR_WIDTH + 2, BAR_HEIGHT + 2, ST7735_WHITE);

		// Effacer la zone complète avant de redessiner (permet d'éviter les résidus visuels)
		ST7735_FillRectangleFast(BAR_X, BAR_Y, BAR_WIDTH, BAR_HEIGHT, backgroundColor);

		// Diviser la barre en trois parties égales
		int partWidth = BAR_WIDTH / 3;

		// Remplir chaque partie avec la couleur correspondante
		if (barWidth > 0) {
			if (barWidth <= partWidth) {
				// La partie verte
				ST7735_FillRectangleFast(BAR_X, BAR_Y, barWidth, BAR_HEIGHT, greenColor);
			} else if (barWidth <= 2 * partWidth) {
				// La partie verte et orange
				ST7735_FillRectangleFast(BAR_X, BAR_Y, partWidth, BAR_HEIGHT, greenColor);
				ST7735_FillRectangleFast(BAR_X + partWidth, BAR_Y, barWidth - partWidth, BAR_HEIGHT, orangeColor);
			} else {
				// La partie verte, orange et rouge
				ST7735_FillRectangleFast(BAR_X, BAR_Y, partWidth, BAR_HEIGHT, greenColor);
				ST7735_FillRectangleFast(BAR_X + partWidth, BAR_Y, partWidth, BAR_HEIGHT, orangeColor);
				ST7735_FillRectangleFast(BAR_X + 2 * partWidth, BAR_Y, barWidth - 2 * partWidth, BAR_HEIGHT, redColor);
			}
		}

		// Afficher la valeur numérique au-dessus du bargraphe
		char valueStr[5];
		sprintf(valueStr, "%d", (int)value);
		ST7735_FillRectangleFast(BAR_X + (BAR_WIDTH / 2) - 10, BAR_Y - 15, BAR_X + (BAR_WIDTH / 2) - 21 , BAR_Y - 20, backgroundColor);
		ST7735_WriteString(BAR_X + (BAR_WIDTH / 2) - 10, BAR_Y - 15, valueStr, Font_7x10, ST7735_WHITE, backgroundColor);

		previous_value = value; // Mettre à jour la valeur précédente
	}

}



// Function to read ADC value and convert it to a range from 0 to 100

float ReadAndConvertADC(void) {
	ADC_HandleTypeDef hadc;
	hadc.Instance = ADC1;

	// Start ADC conversion
	if (HAL_ADC_Start(&hadc) != HAL_OK) {
		// ADC Start Conversion Error
		Error_Handler();
	}

	// Wait for conversion to complete
	if (HAL_ADC_PollForConversion(&hadc, HAL_MAX_DELAY) != HAL_OK) {
		// ADC Poll for Conversion Error
		Error_Handler();
	}

	// Read ADC value
	uint32_t adcValue = HAL_ADC_GetValue(&hadc);

	// Convert ADC value to a range from 0 to 100
	float convertedValue = (adcValue * 100) / ADC_RESOLUTION;

	return convertedValue;
}

// Fonction pour exécuter une fonction associée à un élément du menu

void executeMenuFunction(int menuItem) {
	// Ajoutez ici le code pour exécuter la fonction associée à l'élément du menu
	// Par exemple, vous pouvez utiliser une instruction switch pour appeler différentes fonctions en fonction de menuItem

	switch (menuItem) {
	case 0:
		// Action pour Option 1
		ST7735_FillScreen(COLOR_BG);
		while(HAL_GPIO_ReadPin(B1_GPIO_Port, B1_Pin)==1){
			drawBarGraph(ReadAndConvertADC());
		}
		break;
	case 1:
		// Action pour Option 2
		ST7735_DrawImage(0, 0, ST7735_WIDTH, ST7735_HEIGHT, (uint16_t*)test_img_160x80);
		break;
	case 2:
		// Action pour Option 3
		ST7735_FillScreenFast(ST7735_RED);
		ST7735_WriteString(0, 0, "RED", Font_11x18, ST7735_BLACK, ST7735_RED);
		break;
	case 3:
		// Action pour Option 4
		ST7735_FillScreenFast(ST7735_GREEN);
		ST7735_WriteString(0, 0, "GREEN", Font_11x18, ST7735_BLACK, ST7735_GREEN);
		break;
	default:
		break;
	}
}



// Fonction d'interruption pour le bouton-poussoir
// Necessite de configurer l'interruption sur front montant ET descendant

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
	if (GPIO_Pin == Push_Btn_Pin) {
		uint32_t currentTime = HAL_GetTick();

		if (HAL_GPIO_ReadPin(Push_Btn_GPIO, Push_Btn_Pin) == GPIO_PIN_RESET) { // Le bouton est enfoncé
			if (currentTime - lastButtonReleaseTime > DEBOUNCE_DELAY) {
				// Appui détecté après le délai de rebond
				buttonPressTime = currentTime;
			}
		} else { // Le bouton est relâché
			if (currentTime - buttonPressTime > DEBOUNCE_DELAY) {
				// Relâchement détecté après le délai de rebond
				uint32_t pressDuration = currentTime - buttonPressTime;
				// Vérifiez la durée de l'appui
				if (pressDuration < 1000) { // Réglez la limite de temps pour un appui court
					// Appui court détecté
					// Changez la sélection dans le menu (par exemple, passez à l'élément suivant)
					navigateMenu(1); // Passez à l'élément suivant du menu
				} else {
					// Appui long détecté
					// Exécutez la fonction associée à l'élément actuel du menu
					executeMenuFunction(selectedMenuItem);
				}
				lastButtonReleaseTime = currentTime;
			}
		}
	}
}


void menu_demo(void) {
	// Check border
	ST7735_FillScreenFast(ST7735_BLACK);

	for(int x = 0; x < ST7735_WIDTH; x++) {
		ST7735_DrawPixel(x, 0, ST7735_RED);
		ST7735_DrawPixel(x, ST7735_HEIGHT-1, ST7735_RED);
	}

	for(int y = 0; y < ST7735_HEIGHT; y++) {
		ST7735_DrawPixel(0, y, ST7735_RED);
		ST7735_DrawPixel(ST7735_WIDTH-1, y, ST7735_RED);
	}

	HAL_Delay(1000);

	//    // Check fonts
	ST7735_FillScreenFast(ST7735_BLACK);
	ST7735_WriteString(0, 0, "Font_7x10, red on black, lorem ipsum dolor sit amet", Font_7x10, ST7735_RED, ST7735_BLACK);
	ST7735_WriteString(0, 3*10, "Font_11x18, green, lorem ipsum", Font_11x18, ST7735_GREEN, ST7735_BLACK);
	ST7735_WriteString(0, 3*10+3*18, "Font_16x26", Font_16x26, ST7735_BLUE, ST7735_BLACK);
	HAL_Delay(2000);

	// Check colors
	ST7735_FillScreenFast(ST7735_BLACK);
	ST7735_WriteString(0, 0, "BLACK", Font_11x18, ST7735_WHITE, ST7735_BLACK);
	HAL_Delay(500);

	ST7735_FillScreenFast(ST7735_BLUE);
	ST7735_WriteString(0, 0, "BLUE", Font_11x18, ST7735_BLACK, ST7735_BLUE);
	HAL_Delay(500);

	ST7735_FillScreenFast(ST7735_RED);
	ST7735_WriteString(0, 0, "RED", Font_11x18, ST7735_BLACK, ST7735_RED);
	HAL_Delay(500);

	ST7735_FillScreenFast(ST7735_GREEN);
	ST7735_WriteString(0, 0, "GREEN", Font_11x18, ST7735_BLACK, ST7735_GREEN);
	HAL_Delay(500);

	ST7735_FillScreenFast(ST7735_CYAN);
	ST7735_WriteString(0, 0, "CYAN", Font_11x18, ST7735_BLACK, ST7735_CYAN);
	HAL_Delay(500);

	ST7735_FillScreenFast(ST7735_MAGENTA);
	ST7735_WriteString(0, 0, "MAGENTA", Font_11x18, ST7735_BLACK, ST7735_MAGENTA);
	HAL_Delay(500);

	ST7735_FillScreenFast(ST7735_YELLOW);
	ST7735_WriteString(0, 0, "YELLOW", Font_11x18, ST7735_BLACK, ST7735_YELLOW);
	HAL_Delay(500);

	ST7735_FillScreenFast(ST7735_WHITE);
	ST7735_WriteString(0, 0, "WHITE", Font_11x18, ST7735_BLACK, ST7735_WHITE);
	HAL_Delay(500);


	// Display test image 128x128
	ST7735_DrawImage(0, 0, ST7735_WIDTH, ST7735_HEIGHT, (uint16_t*)test_img_160x80);

	ST7735_SetGamma(GAMMA_10);
	HAL_Delay(500);
	ST7735_SetGamma(GAMMA_18);
	HAL_Delay(500);
	ST7735_SetGamma(GAMMA_22);
	HAL_Delay(500);
	ST7735_SetGamma(GAMMA_25);
	HAL_Delay(500);

	HAL_Delay(1000);
	// Display test image 128x128 pixel by pixel
	for(int x = 0; x < ST7735_WIDTH; x++) {
		for(int y = 0; y < ST7735_HEIGHT; y++) {
			uint16_t color565 = test_img_160x80[y][x];
			// fix endiness
			color565 = ((color565 & 0xFF00) >> 8) | ((color565 & 0xFF) << 8);
			ST7735_DrawPixel(x, y, color565);
		}
	}

	HAL_Delay(1000);


}
