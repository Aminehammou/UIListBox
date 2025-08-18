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
#include <vector>
#include <memory>
#include <TFT_eSPI.h>
#include <U8g2_for_TFT_eSPI.h>

// Inclure les composants UI nécessaires
#include "UIComponent.h"
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

    // ListBox
    auto listBox = new UIListBox(u8f, {160, 10, 150, 220}, listBoxStyle);
    
    // Ajout des éléments à la ListBox
    std::vector<ListBoxItem> cities;

    // 1. Définissez vos données (noms et adresses MAC)
    const char* city_names[] = {
        "Paris", "Tokyo", "New York", "London", "Berlin",
        "Sydney", "Cairo", "Moscow", "Beijing", "Toronto",
        "Madrid", "Rome", "Lisbon", "Amsterdam"
    };

    // Adresses MAC d'exemple (6 octets par adresse)
    uint8_t macs[][6] = {
        {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED}, // Paris
        {0x12, 0x34, 0x56, 0x78, 0x9A, 0xBC}, // Tokyo
        {0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF}, // New York
        {0x00, 0x11, 0x22, 0x33, 0x44, 0x55}, // London
        // Laissez les autres vides pour l'exemple
    };

    // 2. Créez les ListBoxItem avec le nom et l'adresse MAC
    size_t num_cities = sizeof(city_names) / sizeof(city_names[0]);
    size_t num_macs = sizeof(macs) / sizeof(macs[0]);

    for (size_t i = 0; i < num_cities; ++i) {
        // Si une adresse MAC existe pour cette ville, on l'ajoute
        uint8_t* current_mac = (i < num_macs) ? macs[i] : nullptr;
        cities.push_back(ListBoxItem(city_names[i], current_mac));
    }

    listBox->setItems(cities);
    listBox->setSelectedIndex(2); // "New York" par défaut

    // 3. Mettez à jour le callback pour afficher l'adresse MAC
    listBox->onSelectionChanged([](int index, const ListBoxItem& item) {
        Serial.printf("ListBox selection: Index=%d, Text=%s\n", index, item.text.c_str());
        
        // Vérifie si l'adresse MAC n'est pas nulle
        bool isMacValid = false;
        for(int i = 0; i < 6; ++i) {
            if (item.macAddress[i] != 0) {
                isMacValid = true;
                break;
            }
        }

        if (isMacValid) {
            Serial.print("  MAC Address: ");
            for(int i = 0; i < 6; ++i) {
                if(item.macAddress[i] < 0x10) Serial.print("0");
                Serial.print(item.macAddress[i], HEX);
                if(i < 5) Serial.print(":");
            }
            Serial.println();
        }
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