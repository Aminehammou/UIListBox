/*
 * @file main.cpp
 * @author Gemini
 * @brief 
 * @version 0.1
 * @date 2025-08-10
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#include <Arduino.h>
#include <TFT_eSPI.h>
#include <U8g2_for_TFT_eSPI.h>
#include <vector>
#include <memory>

// Inclure les composants UI nécessaires
#include "UIComponent.h"
#include "UICheckbox.h"
#include "UIListBox.h" // <-- INCLURE LE NOUVEAU COMPOSANT

#define BACKLIGHT_PIN 32

// Objets globaux pour l'écran et la police
TFT_eSPI tft = TFT_eSPI();
U8g2_for_TFT_eSPI u8f;

// Vecteur pour gérer tous nos composants UI
std::vector<std::unique_ptr<UIComponent>> components;

// Variables pour la gestion simple du toucher
static bool wasTouched = false;
uint16_t touchX, touchY;

/**
 * @brief 
 * 
 * @param intensity 
 */
void setBacklight(uint8_t intensity) {
    analogWrite(BACKLIGHT_PIN, intensity);
}

/**
 * @brief 
 * 
 */
void setup() {
    // 1. Initialisation
    Serial.begin(115200);
    Serial.println("Exemple de test pour UICheckbox et UIListBox");

    pinMode(BACKLIGHT_PIN, OUTPUT);
    setBacklight(255); // Allumer le rétroéclairage

    tft.begin();
    tft.setRotation(3); // Ajustez la rotation selon votre écran (0-3)
    tft.fillScreen(TFT_BLACK);

    uint16_t calData[5] = { 255, 3626, 216, 3663, 1 };
    tft.setTouch(calData);

    u8f.begin(tft); // Initialise le helper U8g2

    // 2. Définition des styles
    // Style pour la Checkbox
    UICheckboxStyle checkboxStyle;
    checkboxStyle.labelStyle.font = u8g2_font_profont17_tr;
    checkboxStyle.labelStyle.textColor = TFT_WHITE;
    checkboxStyle.labelStyle.bgColor = TFT_BLACK;
    checkboxStyle.labelStyle.disabledTextColor = TFT_DARKGREY;
    checkboxStyle.boxColor = TFT_CYAN;
    checkboxStyle.checkColor = TFT_GREEN;
    checkboxStyle.disabledBoxColor = TFT_DARKGREY;
    checkboxStyle.boxSize = 20;
    checkboxStyle.spacing = 12;

    // Style pour la ListBox
    UIListBoxStyle listBoxStyle;
    listBoxStyle.font = u8g2_font_profont15_tr; // Police légèrement plus petite
    listBoxStyle.itemHeight = 22;
    listBoxStyle.textColor = TFT_WHITE;
    listBoxStyle.bgColor = 0x2104; // Un gris foncé
    listBoxStyle.selectedTextColor = TFT_BLACK;
    listBoxStyle.selectedBgColor = TFT_ORANGE;
    listBoxStyle.borderColor = TFT_WHITE;
    listBoxStyle.scrollBarColor = TFT_LIGHTGREY;

    // 3. Création des composants
    
    // Checkbox
    auto checkbox1 = new UICheckbox(u8f, {10, 10, 140, 30}, "Logs", checkboxStyle, true);
    components.emplace_back(checkbox1);

    // ListBox
    auto listBox = new UIListBox(u8f, {160, 10, 150, 220}, listBoxStyle);
    
    // Ajout des éléments à la ListBox
    std::vector<String> cities = {
        "Paris", "Tokyo", "New York", "London", "Berlin",
        "Sydney", "Cairo", "Moscow", "Beijing", "Toronto",
        "Madrid", "Rome", "Lisbon", "Amsterdam"
    };
    listBox->setItems(cities);
    listBox->setSelectedIndex(2); // "New York" par défaut

    // Définition du callback pour la ListBox
    listBox->onSelectionChanged([](int index, String text) {
        Serial.printf("ListBox selection: Index=%d, Text=%s\n", index, text.c_str());
    });
    components.emplace_back(listBox);


    // Forcer un premier dessin de tous les composants
    tft.fillScreen(TFT_BLACK);
    for (const auto& comp : components) {
        comp->setDirty(true);
        comp->draw(tft);
    }
}

/**
 * @brief 
 * 
 */
void loop() {
    // Gestion du toucher
    bool isTouched = tft.getTouch(&touchX, &touchY);

    if (isTouched && !wasTouched) { // Press
        for (const auto& comp : components) comp->handlePress(tft, touchX, touchY);
    }
    if (!isTouched && wasTouched) { // Release
        for (const auto& comp : components) comp->handleRelease(tft, touchX, touchY);
    }
    if (isTouched) { // Drag
        for (const auto& comp : components) comp->handleDrag(tft, touchX, touchY);
    }
    wasTouched = isTouched;

    // Redessiner les composants qui ont changé
    for (const auto& comp : components) {
        if (comp->isDirty()) {
            comp->draw(tft);
        }
    }
    delay(20);
}