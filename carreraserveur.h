#ifndef CARRERASERVEUR_H
#define CARRERASERVEUR_H

#include <QObject>
#include <QWebSocketServer>
#include <QWebSocket>
#include <QHash>
#include <QtGlobal>
#include <QHostAddress>
#include <QWebSocketProtocol>
#include <algorithm>

/*debug
#include <iostream>
using namespace std;
*/


class CArreraServeur : public QObject
{
    Q_OBJECT
public:
    explicit CArreraServeur(QObject *parent = nullptr);
    ~CArreraServeur() override;

    // Démarre/arrête le serveur. Idempotents.
    bool startServeur(quint16 port);
    bool stopServeur();

    // Envoie un message à un client identifié par son "nameSoft".
    bool sendMessage(const QString &nameClient, const QString &message);

signals:
    void messageReceived(const QString &nameSoft, const QString &message);
    void connectClient();
    void clientDeconected();

private slots:
    void onNewConnectionOfClient();
    void onMessageReceived(const QString &message);
    void onClientDeconected();

private:
    // Non copiable/non déplaçable (facultatif, mais évite les erreurs)
    Q_DISABLE_COPY_MOVE(CArreraServeur)

    // Utilitaires internes
    void registerClient(QWebSocket *client);
    void unregisterClient(QWebSocket *client);
    void setClientName(QWebSocket *client, const QString &nameSoft);

private:
    // Le serveur (créé une seule fois). Parent = this.
    QWebSocketServer *serverWebSocket = nullptr;

    // Mapping clients <-> noms pour accès O(1) dans les deux sens.
    QHash<QWebSocket*, QString> socketToName;
    QHash<QString, QWebSocket*> nameToSocket;

    // Pas d’utilité à volatile ici.
    bool signalEmitted = false;
};

#endif // CARRERASERVEUR_H
