# Guide Détaillé sur la Structure `ListBoxItem`

Ce document explique en détail le fonctionnement et l'utilité de la structure `ListBoxItem` au sein de la bibliothèque `UIListBox`.

## 1. Pourquoi une Structure `ListBoxItem` ?

Dans une interface utilisateur, une liste ne contient souvent pas que du simple texte. Chaque ligne peut représenter un objet plus complexe avec des propriétés associées. Par exemple :
- Une liste de contacts : chaque ligne a un nom (visible) et un numéro de téléphone (donnée associée).
- Une liste de réseaux Wi-Fi : chaque ligne a un SSID (visible) et une adresse MAC ou un niveau de sécurité (données associées).

La structure `ListBoxItem` a été créée précisément pour cela : **associer des données supplémentaires à un texte affichable**. Elle encapsule le texte visible et les métadonnées (ici, une adresse MAC) dans un seul et même objet.

---

## 2. Anatomie de `ListBoxItem`

Voici le code de la structure tel que défini dans `UIListBox.h`, avec des explications ligne par ligne.

```cpp
/**
 * @brief Structure représentant un élément de la liste avec son texte et son adresse MAC associée.
 */
struct ListBoxItem {
    // --- MEMBRES DE DONNÉES ---

    String text; // Le texte qui sera affiché dans la liste.
    std::array<uint8_t, 6> macAddress; // Un tableau de 6 octets pour l'adresse MAC.

    // --- CONSTRUCTEURS ---

    // Constructeur par défaut
    ListBoxItem() : text("") {
        macAddress.fill(0); // Initialise l'adresse MAC à zéro.
    }

    // Constructeur principal avec texte et adresse MAC
    ListBoxItem(const String& t, const uint8_t* mac) : text(t) {
        if (mac) {
            // Si un pointeur MAC est fourni, copie les 6 octets.
            std::copy(mac, mac + 6, macAddress.begin());
        } else {
            // Sinon, remplit l'adresse de zéros.
            macAddress.fill(0);
        }
    }

    // --- GESTION DE LA COPIE ---

    // Constructeur de copie
    ListBoxItem(const ListBoxItem& other) : text(other.text), macAddress(other.macAddress) {}

    // Opérateur d'affectation de copie
    ListBoxItem& operator=(const ListBoxItem& other) {
        if (this != &other) {
            text = other.text;
            macAddress = other.macAddress;
        }
        return *this;
    }
};
```

### Analyse des Membres

- `String text;`
  - C'est un objet `String` Arduino standard. Il contient la chaîne de caractères qui sera dessinée à l'écran pour cet item.

- `std::array<uint8_t, 6> macAddress;`
  - C'est un tableau C++ moderne de taille fixe. Il est conçu pour contenir les 6 octets (un `uint8_t` est un octet) d'une adresse MAC.
  - Utiliser `std::array` est plus sûr qu'un tableau C `uint8_t[6]` car il prévient certaines erreurs courantes de manipulation de pointeurs.

### Analyse des Constructeurs

Un constructeur est une fonction spéciale qui initialise un objet. `ListBoxItem` en a deux principaux.

1.  **`ListBoxItem()`**
    - **Usage** : `ListBoxItem myItem;`
    - **Résultat** : Crée un objet `myItem` où `myItem.text` est `""` (vide) et `myItem.macAddress` est `[0, 0, 0, 0, 0, 0]`.

2.  **`ListBoxItem(const String& t, const uint8_t* mac)`**
    - C'est le constructeur le plus utile.
    - **Usage 1 : Avec une adresse MAC**
      ```cpp
      uint8_t myMac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
      ListBoxItem itemWithMac("Mon ESP32", myMac);
      ```
      - **Résultat** : `itemWithMac.text` sera `"Mon ESP32"` et `itemWithMac.macAddress` contiendra les valeurs de `myMac`.

    - **Usage 2 : Sans adresse MAC**
      ```cpp
      ListBoxItem itemWithoutMac("Juste du texte", nullptr);
      ```
      - **Résultat** : `itemWithoutMac.text` sera `"Juste du texte"` et `itemWithoutMac.macAddress` sera rempli de zéros, car nous avons passé `nullptr`.

---

## 3. Utilisation Pratique avec `UIListBox`

La bibliothèque `UIListBox` est conçue pour fonctionner avec un `std::vector` de ces objets `ListBoxItem`.

### Remplir la liste

La méthode `listBox->setItems()` attend un `std::vector<ListBoxItem>`. Voici comment vous le construisez :

```cpp
// 1. Créez un vecteur vide pour stocker vos items.
std::vector<ListBoxItem> myItems;

// 2. Définissez vos données.
const char* names[] = {"Device A", "Device B"};
uint8_t macs[][6] = {
    {0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA},
    {0xBB, 0xBB, 0xBB, 0xBB, 0xBB, 0xBB}
};

// 3. Remplissez le vecteur en créant des ListBoxItem.
for (int i = 0; i < 2; ++i) {
    myItems.push_back(ListBoxItem(names[i], macs[i]));
}

// Vous pouvez aussi ajouter un item sans MAC.
myItems.push_back(ListBoxItem("Item de log", nullptr));

// 4. Passez le vecteur complet à la ListBox.
listBox->setItems(myItems);
```

### Récupérer les données

La vraie puissance se révèle lorsque l'utilisateur sélectionne un élément. Le *callback* (la fonction de rappel) `onSelectionChanged` vous donne accès à l'objet `ListBoxItem` complet.

```cpp
listBox->onSelectionChanged([](int index, const ListBoxItem& item) {
    // 'item' est une référence constante vers l'objet ListBoxItem sélectionné.

    // Vous pouvez accéder à son texte.
    Serial.printf("Texte sélectionné : %s
", item.text.c_str());

    // Et vous pouvez accéder à son adresse MAC !
    const std::array<uint8_t, 6>& mac = item.macAddress;

    // Affichez l'adresse MAC
    Serial.print("  Adresse MAC : ");
    for(int i = 0; i < 6; ++i) {
        if(mac[i] < 0x10) Serial.print("0");
        Serial.print(mac[i], HEX);
        if(i < 5) Serial.print(":");
    }
    Serial.println();
});
```

Grâce à cette approche, le `UIListBox` ne se contente pas d'afficher une information, il vous permet de lier cette information à des données concrètes et de réagir en conséquence dans votre code.
