#ifndef MYSERVER_H
#define MYSERVER_H

#include <QMainWindow>
#include <QtNetwork>
#include <QMessageBox>

namespace Ui {
class MyServer;
}

/**
 * @brief The MyServer class - класс сервера, передающий данные по TCP протоколу
 */
class MyServer : public QMainWindow
{
    Q_OBJECT

public:
    /**
     * @brief MyServer - конструктор класса
     * @param nPort - номер порта, по которому происходит подключение
     * @param parent
     */
    explicit MyServer(quint16 nPort, QWidget *parent = nullptr);
    ~MyServer();

private:
    Ui::MyServer *ui;
    /**
     * @brief m_ptcpServer - основа управления сервера
     */
    QTcpServer* m_ptcpServer;
    /**
     * @brief m_nNextBlockSize - хранит длину следующего полученного от сокета блока
     */
    quint16 m_nNextBlockSize;
    /**
     * @brief send_text - метод для отправление текстовых данных
     * @param pSocket - сокет в который записываются данные
     * @param str - строка, которую нужно отправить
     */
    /**
     * @brief socket_client - переменная, хранящая сокет клиента
     */
    QTcpSocket* socket_client;

    bool flag = true;
    void send_text(QTcpSocket* pSocket, const QString& str);

private slots:
    virtual void slotNewConnection();
    void slotReadClient();
    void slotSendtoClient();
};

#endif // MYSERVER_H
