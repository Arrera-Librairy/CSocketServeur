//Developpez par Enzo Bourkoua
//Le 10.12.2024
//Classe qui fonctionne de paire avec la classe CSocketClient. Comme le nom l'indique C'EST UN SOCKET

#include <QTcpServer>
#include <QTcpSocket>
#include <QObject>

class CSocketServeur : public QObject {
    Q_OBJECT

public:
    CSocketServeur(QObject *parent = nullptr);
    void startServer(int port);

private slots:
    void onNewConnection();
    void onReadyRead();

private:
    QTcpServer *server;
};
