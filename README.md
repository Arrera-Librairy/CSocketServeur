# CSocketServeur

CSocketServeur est un serveur WebSocket simple basé sur Qt, conçu pour gérer les connexions de plusieurs clients et faciliter la communication bidirectionnelle.

## Utilisation

### Instanciation
Pour utiliser le serveur, commencez par créer une instance de `CArreraServeur`.

```cpp
#include "carreraserveur.h"

// Dans votre classe principale ou fonction main
CArreraServeur *monServeur = new CArreraServeur(this);
```

### Démarrage et Arrêt du Serveur

Pour démarrer le serveur et commencer à écouter les connexions entrantes sur un port spécifique :

```cpp
// Démarrer le serveur sur le port 1234
if (monServeur->startServeur(1234)) {
    qDebug() << "Serveur démarré avec succès.";
} else {
    qDebug() << "Erreur au démarrage du serveur.";
}
```

Pour arrêter le serveur :
```cpp
monServeur->stopServeur();
qDebug() << "Serveur arrêté.";
```

### Communication avec les Clients

#### Enregistrement d'un Client
Lorsqu'un client se connecte, il doit s'identifier en envoyant un premier message contenant son nom, préfixé par `namesoft`. Par exemple : `namesoft MonClientApp`. Le serveur enregistrera alors le client avec le nom `monclientapp`.

#### Envoyer un message à un client
Vous pouvez envoyer un message à un client spécifique en utilisant son nom :

```cpp
monServeur->sendMessage("monclientapp", "Bonjour client !");
```

#### Recevoir des messages et gérer les connexions
Pour être notifié des nouveaux messages, des connexions et des déconnexions, vous pouvez connecter les signaux du serveur à vos propres slots.

```cpp
// Connecter le signal de réception de message
connect(monServeur, &CArreraServeur::messageReceived, this, &MaClasse::onMessageRecu);

// Connecter les signaux de connexion/déconnexion
connect(monServeur, &CArreraServeur::connectClient, this, &MaClasse::onNouveauClient);
connect(monServeur, &CArreraServeur::clientDeconected, this, &MaClasse::onClientDeconnecte);

// ...

void MaClasse::onMessageRecu(const QString &nomClient, const QString &message) {
    qDebug() << "Message reçu de" << nomClient << ":" << message;
}

void MaClasse::onNouveauClient() {
    qDebug() << "Un nouveau client s'est connecté.";
}

void MaClasse::onClientDeconnecte() {
    qDebug() << "Un client s'est déconnecté.";
}
```

## Détail des Méthodes et Signaux

### Méthodes Publiques

- **`bool startServeur(quint16 port)`**
  Démarre le serveur sur le port spécifié. La méthode est idempotente : si le serveur est déjà en cours, il sera redémarré sur le nouveau port. Le serveur écoute sur toutes les interfaces réseau (`QHostAddress::Any`). Retourne `true` en cas de succès, `false` sinon.

- **`bool stopServeur()`**
  Arrête le serveur. Toutes les connexions clientes existantes sont fermées et les ressources sont nettoyées.

- **`bool sendMessage(const QString &nameClient, const QString &message)`**
  Envoie une chaîne de caractères (`message`) au client identifié par `nameClient`. Le nom du client est celui qu'il a fourni lors de son enregistrement (insensible à la casse). Retourne `true` si le client est trouvé et le message envoyé, `false` sinon.

### Signaux

- **`void messageReceived(const QString &nameSoft, const QString &message)`**
  Émis chaque fois qu'un message texte est reçu d'un client identifié. `nameSoft` est le nom du client et `message` est le contenu du message.

- **`void connectClient()`**
  Émis lorsqu'un nouveau client établit une connexion avec le serveur, avant même son identification.

- **`void clientDeconected()`**
  Émis lorsqu'un client se déconnecte du serveur.
