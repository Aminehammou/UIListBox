#ifndef UILISTBOX_H
#define UILISTBOX_H

#include "UITextComponent.h"
#include <vector>
#include <functional>
#include <array> // Pour std::array

/**
 * @brief Structure représentant un élément de la liste avec son texte et son adresse MAC associée.
 */
struct ListBoxItem {
    String text;
    std::array<uint8_t, 6> macAddress; // Adresse MAC (6 octets)

    // Constructeur par défaut
    ListBoxItem() : text("") {
        macAddress.fill(0); // Initialise l'adresse MAC à zéro
    }

    // Constructeur avec texte et adresse MAC
    ListBoxItem(const String& t, const uint8_t* mac) : text(t) {
        if (mac) {
            std::copy(mac, mac + 6, macAddress.begin());
        } else {
            macAddress.fill(0);
        }
    }

    // Constructeur de copie
    ListBoxItem(const ListBoxItem& other) : text(other.text), macAddress(other.macAddress) {}

    // Opérateur d'affectation de copie
    ListBoxItem& operator=(const ListBoxItem& other) {
        if (this != &other) {
            text = other.text;
            macAddress = other.macAddress; // std::array a son propre opérateur d'affectation
        }
        return *this;
    }
};

/**
 * @brief Définit l'apparence visuelle d'un composant UIListBox.
 */
struct UIListBoxStyle {
    const uint8_t* font;        ///< Police de caractères U8g2 pour le texte des éléments.
    uint16_t itemHeight;        ///< Hauteur de chaque élément de la liste en pixels.
    uint16_t textColor;         ///< Couleur du texte pour les éléments non sélectionnés.
    uint16_t bgColor;           ///< Couleur de fond générale de la liste.
    uint16_t selectedTextColor; ///< Couleur du texte pour l'élément sélectionné.
    uint16_t selectedBgColor;   ///< Couleur de fond pour l'élément sélectionné.
    uint16_t borderColor;       ///< Couleur de la bordure autour de la liste.
    uint16_t scrollBarColor;    ///< Couleur de la barre de défilement.
};

/**
 * @brief Composant UI affichant une liste déroulante d'éléments sélectionnables.
 * 
 * Gère l'affichage, la sélection par contact et le défilement tactile.
 */
class UIListBox : public UITextComponent {
public:
    /**
     * @brief Construit un nouvel objet UIListBox.
     * 
     * @param u8f Référence à l'objet U8g2_for_TFT_eSPI pour le rendu du texte.
     * @param rect Rectangle définissant la position et la taille de la liste.
     * @param style Style visuel à appliquer à la liste.
     */
    UIListBox(U8g2_for_TFT_eSPI& u8f, const UIRect& rect, const UIListBoxStyle& style);

    // Gestion des éléments
    /**
     * @brief Définit les éléments à afficher dans la liste.
     * 
     * @param items Un vecteur de structures ListBoxItem représentant les éléments.
     */
    void setItems(const std::vector<ListBoxItem>& items);

    /**
     * @brief Ajoute un élément à la fin de la liste.
     * 
     * @param item La structure ListBoxItem à ajouter.
     */
    void addItem(const ListBoxItem& item);

    /**
     * @brief Ajoute un élément à la fin de la liste avec un texte et une adresse MAC.
     * 
     * @param text Le texte de l'élément.
     * @param mac L'adresse MAC de l'élément (tableau de 6 octets).
     */
    void addItem(const String& text, const uint8_t* mac);

    /**
     * @brief Ajoute une liste d'éléments à la fin de la liste existante.
     * 
     * @param items Un vecteur de structures ListBoxItem à ajouter.
     */
    void addItems(const std::vector<ListBoxItem>& items);

    /**
     * @brief Supprime un élément de la liste par son index.
     * 
     * @param index L'index de l'élément à supprimer.
     * @return true si l'élément a été supprimé avec succès, false sinon.
     */
    bool removeItem(int index);

    /**
     * @brief Récupère un élément par son index.
     * 
     * @param index L'index de l'élément à récupérer.
     * @return const ListBoxItem& L'élément complet. Retourne un ListBoxItem vide si l'index est invalide.
     */
    const ListBoxItem& getItem(int index) const;

    /**
     * @brief Obtient le nombre total d'éléments dans la liste.
     * 
     * @return int Le nombre d'éléments.
     */
    int getItemCount() const;

    // Gestion de la sélection
    /**
     * @brief Définit l'élément actuellement sélectionné.
     * 
     * @param index L'index de l'élément à sélectionner. Mettre -1 pour désélectionner.
     * @param triggerCallback Si vrai, le callback onSelectionChanged sera appelé.
     */
    void setSelectedIndex(int index, bool triggerCallback = false);

    /**
     * @brief Obtient l'index de l'élément actuellement sélectionné.
     * 
     * @return int L'index de la sélection, ou -1 si aucun élément n'est sélectionné.
     */
    int getSelectedIndex() const;

    /**
     * @brief Obtient le texte de l'élément actuellement sélectionné.
     * 
     * @return const String& Le texte de la sélection.
     */
    const String& getSelectedText() const;

    /**
     * @brief Obtient l'adresse MAC de l'élément actuellement sélectionné.
     * 
     * @return const std::array<uint8_t, 6>& L'adresse MAC de la sélection. Retourne une adresse MAC nulle si aucun élément n'est sélectionné.
     */
    const std::array<uint8_t, 6>& getSelectedMacAddress() const;

    /**
     * @brief Définit une fonction de rappel à exécuter lorsque la sélection change.
     * 
     * @param callback La fonction à appeler. Elle prend en paramètre l'index et l'élément complet du nouvel élément sélectionné.
     */
    void onSelectionChanged(std::function<void(int, const ListBoxItem&)> callback);

    // Surcharge des méthodes de UIComponent pour la gestion du tactile
    void handlePress(TFT_eSPI& tft, int tx, int ty) override;
    void handleRelease(TFT_eSPI& tft, int tx, int ty) override;
    void handleDrag(TFT_eSPI& tft, int tx, int ty) override;

private:
    /**
     * @brief Méthode interne pour dessiner le composant sur l'écran.
     * @param tft Référence à l'objet TFT_eSPI.
     * @param force Indique si le redessin doit être forcé.
     */
    void drawInternal(TFT_eSPI& tft, bool force) override;

    UIListBoxStyle _style;                      ///< Style visuel de la liste.
    std::vector<ListBoxItem> _items;            ///< Conteneur pour les éléments de la liste.
    int _selectedIndex = -1;                    ///< Index de l'élément actuellement sélectionné.
    int _topItemIndex = 0;                      ///< Index du premier élément visible (pour le défilement).
    int _visibleItemCount = 0;                  ///< Nombre d'éléments visibles à l'écran.

    std::function<void(int, const ListBoxItem&)> _onSelectionChangedCallback; ///< Pointeur de fonction pour le callback de sélection.

    // Variables pour la gestion du défilement par glissement
    bool _isDragging = false;                   ///< Vrai si un glissement est en cours.
    int _dragStartY = 0;                        ///< Position Y de départ du glissement.
    int _dragStartTopIndex = 0;                 ///< Index de l'élément supérieur au début du glissement.
};

#endif // UILISTBOX_H