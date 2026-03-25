#include "carreraserveur.h"

CArreraServeur::CArreraServeur(QObject *parent)
    : QObject{parent}
    , serverWebSocket(nullptr)
{
}

CArreraServeur::~CArreraServeur()
{
    // Sans danger même si le serveur n'a jamais démarré
    stopServeur();
}

bool CArreraServeur::startServeur(quint16 port)
{
    // Crée le serveur une seule fois
    if (!serverWebSocket) {
        serverWebSocket = new QWebSocketServer(
            QStringLiteral("WebSocket Server"),
            QWebSocketServer::NonSecureMode,
            this // parent = this -> pas de delete manuel
            );

        connect(serverWebSocket, &QWebSocketServer::newConnection,
                this, &CArreraServeur::onNewConnectionOfClient);
    }

    // Si déjà à l'écoute, on relance proprement sur le nouveau port
    if (serverWebSocket->isListening())
        serverWebSocket->close();

    // Ecoute sur toutes les interfaces
    if (!serverWebSocket->listen(QHostAddress::Any, port)) {
        // Echec de listen -> le pointeur reste valable, on pourra retenter plus tard
        return false;
    }
    return true;
}

bool CArreraServeur::stopServeur()
{
    // 1) Stopper l'acceptation et fermer le serveur (si présent)
    if (serverWebSocket) {
#if QT_VERSION >= QT_VERSION_CHECK(6, 5, 0)
        serverWebSocket->pauseAccepting();
#endif
        if (serverWebSocket->isListening())
            serverWebSocket->close();
        // Pas de delete: parent = this
    }

    // 2) Fermer toutes les connexions clientes
    // Utiliser une copie des clés pour éviter modification pendant l'itération
    const auto clients = socketToName.keys();
    for (QWebSocket *client : clients) {
        if (!client) continue;
        disconnect(client, nullptr, this, nullptr);
        // Code "GoingAway" + raison informative
        client->close(QWebSocketProtocol::CloseCodeGoingAway,
                      QStringLiteral("Serveur down"));
        client->deleteLater();
        // Nettoyage des maps
        unregisterClient(client);
    }

    socketToName.clear();
    nameToSocket.clear();

    return true;
}

bool CArreraServeur::sendMessage(const QString &nameClient, const QString &message)
{
    // Lookup O(1)
    if (QWebSocket *sock = nameToSocket.value(nameClient, nullptr)) {
        sock->sendTextMessage(message);
        return true;
    }
    return false;
}

void CArreraServeur::onNewConnectionOfClient()
{
    if (!serverWebSocket)
        return;

    QWebSocket *client = serverWebSocket->nextPendingConnection();
    if (!client)
        return;

    // Parentage pour destruction automatique à la fin de this
    client->setParent(this);
    registerClient(client);

    // textMessageReceived (et non textFrameReceived) pour matcher la signature
    connect(client, &QWebSocket::textMessageReceived,
            this, &CArreraServeur::onMessageReceived);

    connect(client, &QWebSocket::disconnected,
            this, &CArreraServeur::onClientDeconected);

    emit connectClient();
}

void CArreraServeur::onMessageReceived(const QString &message)
{
    QWebSocket *client = qobject_cast<QWebSocket *>(sender());
    if (!client)
        return;

    // Gestion du protocole "namesoft"
    if (message.contains(QStringLiteral("namesoft"), Qt::CaseInsensitive)) {
        QString name = message;
        name.remove(QStringLiteral("namesoft"), Qt::CaseInsensitive)
            .remove(QLatin1Char(' '));
        name = name.simplified().toLower();

        setClientName(client, name);
    } else {
        signalEmitted = true;
        const QString name = socketToName.value(client);
        emit messageReceived(name, message + QStringLiteral("\n"));

        if (message != QLatin1String("Message Received"))
            client->sendTextMessage(QStringLiteral("Message Received"));
    }
}

void CArreraServeur::onClientDeconected()
{
    QWebSocket *client = qobject_cast<QWebSocket *>(sender());
    if (!client)
        return;

    unregisterClient(client);
    client->deleteLater();
    emit clientDeconected();
}

/* ----------- Utilitaires privés ----------- */

void CArreraServeur::registerClient(QWebSocket *client)
{
    // Ajoute s'il n'existe pas déjà
    if (!socketToName.contains(client))
        socketToName.insert(client, QString());
}

void CArreraServeur::unregisterClient(QWebSocket *client)
{
    if (!client)
        return;

    // Retire le nom associé (si any)
    const auto it = socketToName.find(client);
    if (it != socketToName.end()) {
        const QString name = it.value();
        socketToName.erase(it);
        if (!name.isEmpty() && nameToSocket.value(name) == client) {
            nameToSocket.remove(name);
        }
    }
}

void CArreraServeur::setClientName(QWebSocket *client, const QString &nameSoft)
{
    if (!client)
        return;

    // Si le nom est déjà pris par un autre client, on le remplace
    if (QWebSocket *old = nameToSocket.value(nameSoft, nullptr)) {
        if (old != client) {
            // Déassocier l'ancien
            socketToName[old] = QString();
        }
    }

    // Mettre à jour les deux maps
    socketToName[client] = nameSoft;
    nameToSocket[nameSoft] = client;
}
