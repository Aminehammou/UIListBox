# UIListBox

`UIListBox` est une bibliothèque C++ pour Arduino et les environnements similaires, conçue pour créer facilement des listes déroulantes interactives sur des écrans TFT. Elle est bâtie sur la populaire bibliothèque `TFT_eSPI` et utilise `U8g2_for_TFT_eSPI` pour un rendu de police de haute qualité.

Ce composant est idéal pour les interfaces utilisateur qui nécessitent la sélection d'un élément dans une liste, comme des menus de configuration, des sélecteurs de fichiers, etc. Il gère le défilement tactile, la personnalisation de l'apparence et les callbacks sur sélection.

## Fonctionnalités

*   **Défilement Tactile :** Défilement fluide et naturel en glissant le doigt sur la liste.
*   **Personnalisation Complète :** Modifiez les polices, les couleurs (texte, fond, sélection, bordure) et les dimensions pour correspondre à votre interface.
*   **Gestion par Callback :** Attachez une fonction à l'événement `onSelectionChanged` pour réagir instantanément à la sélection de l'utilisateur.
*   **Optimisation du Rendu :** Le composant ne se redessine que lorsque c'est nécessaire pour des performances optimales.
*   **Intégration Facile :** Conçu comme un `UIComponent` pour s'intégrer dans une architecture d'interface utilisateur plus large.

## Dépendances

Pour utiliser cette bibliothèque, vous aurez besoin des dépendances suivantes :

*   [TFT_eSPI](https://github.com/Bodmer/TFT_eSPI)
*   [U8g2_for_TFT_eSPI](https://github.com/Bodmer/U8g2_for_TFT_eSPI)
*   Un framework UI qui fournit la classe de base `UIComponent` (ou vous pouvez l'adapter).

## Comment Utiliser

Voici un exemple simple pour commencer :

### 1. Inclure les bibliothèques et les composants

```cpp
#include <TFT_eSPI.h>
#include <U8g2_for_TFT_eSPI.h>
#include "UIListBox.h"
```

### 2. Initialiser l'écran et U8g2

```cpp
TFT_eSPI tft = TFT_eSPI();
U8g2_for_TFT_eSPI u8f;

void setup() {
    tft.begin();
    u8f.begin(tft);
    // ... autres initialisations
}
```

### 3. Définir un style pour la ListBox

```cpp
UIListBoxStyle listBoxStyle;
listBoxStyle.font = u8g2_font_profont15_tr;
listBoxStyle.itemHeight = 22;
listBoxStyle.textColor = TFT_WHITE;
listBoxStyle.bgColor = 0x2104; // Gris foncé
listBoxStyle.selectedTextColor = TFT_BLACK;
listBoxStyle.selectedBgColor = TFT_ORANGE;
listBoxStyle.borderColor = TFT_WHITE;
listBoxStyle.scrollBarColor = TFT_LIGHTGREY;
```

### 4. Créer une instance de UIListBox

```cpp
// Créer la ListBox dans une zone rectangulaire {x, y, largeur, hauteur}
auto listBox = new UIListBox(u8f, {160, 10, 150, 220}, listBoxStyle);
```

### 5. Remplir la liste et définir un callback

```cpp
// Ajouter des éléments
std::vector<String> cities = {
    "Paris", "Tokyo", "New York", "London", "Berlin",
    "Sydney", "Cairo", "Moscow", "Beijing", "Toronto"
};
listBox->setItems(cities);

// (Optionnel) Pré-sélectionner un élément
listBox->setSelectedIndex(2); // "New York"

// (Optionnel) Définir un callback pour la sélection
listBox->onSelectionChanged([](int index, String text) {
    Serial.printf("Sélection : Index=%d, Texte=%s\n", index, text.c_str());
});
```

### 6. Gérer les interactions et le dessin dans la boucle principale

```cpp
void loop() {
    // Gestion du toucher (exemple simplifié)
    uint16_t tx, ty;
    bool isTouched = tft.getTouch(&tx, &ty);

    if (isTouched) {
        listBox->handlePress(tft, tx, ty);
        listBox->handleDrag(tft, tx, ty);
    } else {
        listBox->handleRelease(tft, tx, ty);
    }

    // Dessiner le composant s'il a changé
    if (listBox->isDirty()) {
        listBox->draw(tft);
    }
}
```

## Personnalisation du Style

La structure `UIListBoxStyle` vous permet de contrôler l'apparence de votre liste :

| Propriété           | Type                | Description                                               |
| ------------------- | ------------------- | --------------------------------------------------------- |
| `font`              | `const uint8_t*`    | Pointeur vers la police U8g2 à utiliser.                  |
| `itemHeight`        | `uint16_t`          | Hauteur en pixels de chaque élément de la liste.          |
| `textColor`         | `uint16_t`          | Couleur du texte pour les éléments non sélectionnés.      |
| `bgColor`           | `uint16_t`          | Couleur de fond de la liste.                              |
| `selectedTextColor` | `uint16_t`          | Couleur du texte pour l'élément sélectionné.              |
| `selectedBgColor`   | `uint16_t`          | Couleur de fond pour l'élément sélectionné.               |
| `borderColor`       | `uint16_t`          | Couleur de la bordure autour de la liste.                 |
| `scrollBarColor`    | `uint16_t`          | Couleur de la barre de défilement.                        |

## Référence de l'API

### Méthodes Principales

*   `void setItems(const std::vector<String>& items)`
    *   Définit ou remplace tous les éléments de la liste.

*   `void setSelectedIndex(int index, bool triggerCallback = false)`
    *   Sélectionne un élément par son index. Si `triggerCallback` est `true`, le callback `onSelectionChanged` sera appelé.

*   `int getSelectedIndex() const`
    *   Retourne l'index de l'élément actuellement sélectionné, ou -1 si aucun n'est sélectionné.

*   `const String& getSelectedText() const`
    *   Retourne le texte de l'élément sélectionné.

*   `void onSelectionChanged(std::function<void(int, String)> callback)`
    *   Définit la fonction à appeler lorsqu'un nouvel élément est sélectionné. La fonction reçoit l'index et le texte de l'élément.

*   `int getItemCount() const`
    *   Retourne le nombre total d'éléments dans la liste.

*   `const String& getItem(int index) const`
    *   Retourne le texte de l'élément à un index donné.

### Méthodes de Gestion des Événements

Ces méthodes doivent être appelées depuis votre gestionnaire d'événements principal.

*   `void handlePress(TFT_eSPI& tft, int tx, int ty)`
*   `void handleRelease(TFT_eSPI& tft, int tx, int ty)`
*   `void handleDrag(TFT_eSPI& tft, int tx, int ty)`

### Méthodes de Rendu

*   `void draw(TFT_eSPI& tft, bool force = false)`
    *   Dessine le composant. Le dessin n'a lieu que si le composant est "dirty" ou si `force` est `true`.
*   `void setDirty(bool dirty = true)`
    *   Marque le composant comme nécessitant un redessin.
*   `bool isDirty() const`
    *   Vérifie si le composant a besoin d'être redessiné.
