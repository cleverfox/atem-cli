#ifndef TCPKBD_H
#define TCPKBD_H

#include <QtCore>
#include <QObject>
#include <QString>
#include <QTcpSocket>
#include "qatemconnection.h"

#define TCPKBD_FILE "tcpkbd.txt"

class TCPKbd : public QObject {
  Q_OBJECT
  private:
    QObject *parent;
    QString dev;
    QMap<int,QStringList> commands;
    QMap<int,QString> leds;
    QTcpSocket * pSocket;
    QAtemConnection *atemconn;
    QByteArray readBuffer;
    void handle_kbdcmd(QByteArray cmd);
  public:
    TCPKbd(QObject * parent, QString device): QObject(parent){
      this->dev = device;
      this->parent = parent;
    }
    void parse_commands();

    void run();

public slots:
  void readTcpData();
/*
  void onAtemDownstreamKeyOnChanged(quint8 keyer, bool state);
  void onAtemAuxSourceChanged(quint8 aux, quint16 source);
  */
  void syncled();
  void onAtemConnected(QAtemConnection *atemconn);

signals:
    void cmdReady(QStringList cmd);

};

#endif

