#include "UIListBox.h"

UIListBox::UIListBox(U8g2_for_TFT_eSPI& u8f, const UIRect& rect, const UIListBoxStyle& style)
    : UITextComponent(u8f, rect, ""), _style(style) {
    _visibleItemCount = rect.h / _style.itemHeight;
}

void UIListBox::setItems(const std::vector<String>& items) {
    _items = items;
    _selectedIndex = -1;
    _topItemIndex = 0;
    setDirty(true);
}

void UIListBox::addItem(const String& item) {
    _items.push_back(item);
    setDirty(true);
}

void UIListBox::addItems(const std::vector<String>& items) {
    _items.insert(_items.end(), items.begin(), items.end());
    setDirty(true);
}

bool UIListBox::removeItem(int index) {
    if (index < 0 || index >= _items.size()) {
        return false; // Index invalide
    }

    _items.erase(_items.begin() + index);

    // Ajuster l'index sélectionné si l'élément supprimé l'affecte
    if (_selectedIndex == index) {
        _selectedIndex = -1; // L'élément sélectionné a été supprimé
    } else if (_selectedIndex > index) {
        _selectedIndex--; // L'index sélectionné se décale vers le haut
    }

    // Ajuster l'index de l'élément supérieur visible si nécessaire
    int maxTopIndex = _items.size() - _visibleItemCount;
    if (maxTopIndex < 0) {
        maxTopIndex = 0;
    }
    if (_topItemIndex > maxTopIndex) {
        _topItemIndex = maxTopIndex;
    }

    setDirty(true);
    return true;
}

const String& UIListBox::getItem(int index) const {
    static const String empty = "";
    if (index >= 0 && index < _items.size()) {
        return _items[index];
    }
    return empty;
}

int UIListBox::getItemCount() const {
    return _items.size();
}

void UIListBox::setSelectedIndex(int index, bool triggerCallback) {
    if (index >= -1 && index < _items.size() && _selectedIndex != index) {
        _selectedIndex = index;
        if (triggerCallback && _onSelectionChangedCallback) {
            _onSelectionChangedCallback(_selectedIndex, getItem(_selectedIndex));
        }
        setDirty(true);
    }
}

int UIListBox::getSelectedIndex() const {
    return _selectedIndex;
}

const String& UIListBox::getSelectedText() const {
    return getItem(_selectedIndex);
}

void UIListBox::onSelectionChanged(std::function<void(int, String)> callback) {
    _onSelectionChangedCallback = callback;
}

void UIListBox::drawInternal(TFT_eSPI& tft, bool force) {
    // 1. Dessiner la bordure extérieure
    tft.drawRect(rect.x, rect.y, rect.w, rect.h, _style.borderColor);

    // 2. Dessiner le fond général (à l'intérieur de la bordure pour ne pas l'écraser)
    tft.fillRect(rect.x + 1, rect.y + 1, rect.w - 2, rect.h - 2, _style.bgColor);

    // 3. Configurer la police pour être transparente
    _u8f.setFontMode(1);
    _u8f.setFont(_style.font);

    // 4. Dessiner les éléments visibles
    for (int i = 0; i < _visibleItemCount; ++i) {
        int itemIndex = _topItemIndex + i;
        if (itemIndex >= _items.size()) break;

        // Calculer la position Y de l'item, en tenant compte de la bordure de 1px
        int itemY = rect.y + 1 + i * _style.itemHeight;

        // Mettre en surbrillance l'élément sélectionné
        if (itemIndex == _selectedIndex) {
            tft.fillRect(rect.x + 1, itemY, rect.w - 2, _style.itemHeight, _style.selectedBgColor);
            _u8f.setForegroundColor(_style.selectedTextColor);
        } else {
            _u8f.setForegroundColor(_style.textColor);
        }

        // Dessiner le texte
        int16_t textH = _u8f.getFontAscent() - _u8f.getFontDescent();
        int textY_baseline = itemY + (_style.itemHeight + textH) / 2;
        _u8f.setCursor(rect.x + 5, textY_baseline); // Marge de 5px à gauche
        _u8f.print(_items[itemIndex]);
    }

    // 5. Dessiner la barre de défilement si nécessaire
    if (_items.size() > _visibleItemCount) {
        int scrollBarX = rect.x + rect.w - 8;
        // Le fond de la barre de scroll doit aussi être dans la bordure
        tft.fillRect(scrollBarX, rect.y + 1, 7, rect.h - 2, _style.bgColor);

        float thumbHeight = (float)_visibleItemCount / _items.size() * (rect.h - 2);
        float thumbY = rect.y + 1 + ((float)_topItemIndex / _items.size() * (rect.h - 2));
        
        tft.fillRect(scrollBarX, (int)thumbY, 7, (int)thumbHeight, _style.scrollBarColor);
    }
}

void UIListBox::handlePress(TFT_eSPI& tft, int tx, int ty) {
    if (enabled && contains(tx, ty)) {
        _isDragging = true;
        _dragStartY = ty;
        _dragStartTopIndex = _topItemIndex;
    }
}

void UIListBox::handleRelease(TFT_eSPI& tft, int tx, int ty) {
    if (enabled && _isDragging) {
        // Si ce n'était pas un drag, c'est un clic pour sélectionner
        if (abs(ty - _dragStartY) < _style.itemHeight / 2) {
            int clickedIndex = _topItemIndex + (ty - rect.y) / _style.itemHeight;
            setSelectedIndex(clickedIndex, true);
        }
    }
    _isDragging = false;
}

void UIListBox::handleDrag(TFT_eSPI& tft, int tx, int ty) {
    if (enabled && _isDragging) {
        // Calculer la distance de glissement depuis le point de départ
        int dragDistance = ty - _dragStartY;

        // Calculer le décalage en nombre d'items.
        // Le signe est inversé pour un défilement "naturel" (glisser vers le bas fait monter le contenu)
        int itemScrolled = -dragDistance / (int)_style.itemHeight;

        int newTopIndex = _dragStartTopIndex + itemScrolled;

        // Brider l'index pour qu'il reste dans les limites valides
        int maxTopIndex = _items.size() - _visibleItemCount;
        if (maxTopIndex < 0) {
            maxTopIndex = 0;
        }

        if (newTopIndex < 0) {
            newTopIndex = 0;
        } else if (newTopIndex > maxTopIndex) {
            newTopIndex = maxTopIndex;
        }

        // Si l'index a changé, marquer pour redessiner
        if (_topItemIndex != newTopIndex) {
            _topItemIndex = newTopIndex;
            setDirty(true);
        }
    }
}