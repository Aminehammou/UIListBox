#ifndef UILISTBOX_H
#define UILISTBOX_H

#include "UITextComponent.h"
#include <vector>
#include <functional>

// Style pour la ListBox
struct UIListBoxStyle {
    const uint8_t* font;
    uint16_t itemHeight;
    uint16_t textColor;
    uint16_t bgColor;
    uint16_t selectedTextColor;
    uint16_t selectedBgColor;
    uint16_t borderColor;
    uint16_t scrollBarColor;
};

class UIListBox : public UITextComponent {
public:
    UIListBox(U8g2_for_TFT_eSPI& u8f, const UIRect& rect, const UIListBoxStyle& style);

    // Gestion des éléments
    void setItems(const std::vector<String>& items);
    const String& getItem(int index) const;
    int getItemCount() const;

    // Gestion de la sélection
    void setSelectedIndex(int index, bool triggerCallback = false);
    int getSelectedIndex() const;
    const String& getSelectedText() const;
    void onSelectionChanged(std::function<void(int, String)> callback);

    // Surcharge des méthodes de UIComponent
    void handlePress(TFT_eSPI& tft, int tx, int ty) override;
    void handleRelease(TFT_eSPI& tft, int tx, int ty) override;
    void handleDrag(TFT_eSPI& tft, int tx, int ty) override;

private:
    void drawInternal(TFT_eSPI& tft, bool force) override;

    UIListBoxStyle _style;
    std::vector<String> _items;
    int _selectedIndex = -1;
    int _topItemIndex = 0; // Index du premier item visible (pour le scroll)
    int _visibleItemCount = 0;

    std::function<void(int, String)> _onSelectionChangedCallback;

    // Pour la gestion du scroll au doigt
    bool _isDragging = false;
    int _dragStartY = 0;
    int _dragStartTopIndex = 0;
};

#endif // UILISTBOX_H