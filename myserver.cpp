#include "myserver.h"
#include "ui_myserver.h"

MyServer::MyServer(quint16 nPort, QWidget *parent) :
    QMainWindow(parent), ui(new Ui::MyServer), m_nNextBlockSize(0)
{
    ui->setupUi(this);
    ui->m_ptxt->setReadOnly(true);

    m_ptcpServer = new QTcpServer(this);
    connect(m_ptcpServer, SIGNAL(newConnection()), this, SLOT(slotNewConnection()));

    if (!m_ptcpServer->listen(QHostAddress::Any, nPort))
    {
        QMessageBox::critical(nullptr, "Ошибка сервера", "Невозможно начать работу:" + m_ptcpServer->errorString());
        m_ptcpServer->close();
        return;
    }

    connect(ui->m_ptxtInput, SIGNAL(returnPressed()), this, SLOT(slotSendtoClient()));
    connect(ui->pcmd, SIGNAL(clicked()), SLOT(slotSendtoClient()));
}

MyServer::~MyServer()
{
    delete ui;
}

void MyServer::slotNewConnection()
{
    QTcpSocket* pClientSocket = m_ptcpServer->nextPendingConnection();

    connect(pClientSocket, SIGNAL(readyRead()), this, SLOT(slotReadClient()));

    ui->m_ptxt->setText("Клиент успешно подключен");

    //send_text(pClientSocket, "Server Response: Connected! Your number is "
    //        + QString::number(pClientSocket->socketDescriptor()));
    socket_client = pClientSocket;
}

void MyServer::slotReadClient()
{
    QTcpSocket* pClientSocket = static_cast<QTcpSocket*>(sender());
    QDataStream in(pClientSocket);
    in.setVersion(QDataStream::Qt_5_0);

    for (;;)
    {
        if (!m_nNextBlockSize)
        {
            if (pClientSocket->bytesAvailable() < sizeof(quint16))
                break;
            in >> m_nNextBlockSize;
        }

        if (pClientSocket->bytesAvailable() < m_nNextBlockSize)
            break;

        QString str;
        in >> str;

        if (str.contains("NAMES: "))
        {
            ui->comboBox->clear();
            QStringList list = str.split(" ");
            for (uint i = 1; i < list.size(); i++)
                ui->comboBox->addItem(list[i]);
        }
        else
        {
            QString strMessage = "Клиент отправил : " + str;
            ui->m_ptxt->append(strMessage);
        }
        m_nNextBlockSize = 0;
    }
}

void MyServer::slotSendtoClient()
{
    //QString str = ui->m_ptxtInput->text();
    QString str = ui->comboBox->currentText();
    QString str2 = ui->m_ptxtInput->text();
    if (socket_client == nullptr)
    {
        ui->m_ptxt->append("Нет подключений");
        ui->m_ptxtInput->setText("");
    }
    else
    {
        if (str2 != "")
        {
            send_text(socket_client, str+" "+str2);
            ui->m_ptxt->append("Вы отправили: " + str+" "+str2);
        }
        else
        {
            send_text(socket_client, str);
            ui->m_ptxt->append("Вы отправили: " + str);
        }

        ui->m_ptxtInput->setText("");
    }
}

void MyServer::send_text(QTcpSocket* pSocket, const QString& str)
{
    QByteArray arrBlock;
    QDataStream out(&arrBlock, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_0);

    out << quint16(0) << str;

    out.device()->seek(0);
    out << quint16(arrBlock.size() - sizeof(quint16));
    pSocket->write(arrBlock);
}
