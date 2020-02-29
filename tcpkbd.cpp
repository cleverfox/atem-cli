#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include "tcpkbd.h"
#include <QFile>
#include <stdlib.h>
#include "qatemmixeffect.h"
#include "qatemdownstreamkey.h"

void TCPKbd::onAtemConnected(QAtemConnection *atemconn){
  qInfo() << "onAtemConnected";
  this->atemconn=atemconn;
  syncled();
}

/*
void TCPKbd::onAtemDownstreamKeyOnChanged(quint8 keyer, bool state) {
}

void TCPKbd::onAtemAuxSourceChanged(quint8 aux, quint16 source) {
}
*/

void TCPKbd::syncled(void) {
  if(atemconn){
    uint8_t prev=atemconn->mixEffect(0)->previewInput();
    uint8_t prog=atemconn->mixEffect(0)->programInput();
    uint8_t d0=atemconn->downstreamKey(0)->onAir();
    uint8_t d1=atemconn->downstreamKey(1)->onAir();
    /*
    leds[(atemconn->mixEffect(0)->previewInput()-1)*2]=1;
    leds[(atemconn->mixEffect(0)->programInput()-1)*2]=2;
    leds[6]=atemconn->downstreamKey(1)->onAir()?2:0;
    */
    for(int i=0;i<8;i++){
      char color='b';
      QMap<int, QString>::iterator it = leds.find(i);
      if(it!=leds.end()){
        QString act=*it;
        if(act[0]=="D"){
          int nr=act.remove(0,1).toInt();
          if(nr==0){ color=d0?'y':'b';}
          if(nr==1){ color=d1?'y':'b';}
        }
        if(act[0]=="C"){
          int nr=act.remove(0,1).toInt();
          if(prev==nr) color='g';
          if(prog==nr) color='r';
        }
      }
      QString toSend=QString("C%1%2\n").arg(i).arg(color);
      pSocket->write(toSend.toUtf8());
    }
    //qout << "DSKEY: [ 0 ] "<< m_downstreamKey_0->onAir() << endl;
  }
}

void TCPKbd::parse_commands() {
  QFile file(TCPKBD_FILE);
  if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    return;
  commands.clear();
  leds.clear();

  while (!file.atEnd()) {
    QByteArray line = file.readLine();
    QList<QByteArray> ba=line.split(';');
    if(ba.length()<2) continue;
    const char *keycode=ba.at(0).data(); 
    int ikeycode=atoi(keycode);
    ba.removeFirst();
    QList<QByteArray>::iterator i;
    QStringList lst;
    for(i=ba.begin();i!=ba.end();i++){
      QString c=QString(i->data()).trimmed();
      if(c[0]=='C' || c[0]=='D'){
        leds.insert(ikeycode,c);
      }else{
        lst << c;
      }
    }
    commands.insert(ikeycode,lst);
  }
  qInfo() << "-------";
  qInfo() << "cmd"<<commands;
  qInfo() << "led"<<leds;
}

void TCPKbd::handle_kbdcmd(QByteArray cmd) {
  if(cmd[0]!='P') return;
  int button=QString(cmd.remove(0,1)).toInt();

  QMap<int, QStringList>::iterator i = commands.find(button);
  if(i!=commands.end()){
    QStringList::iterator l;
    for(l=(*i).begin();l!=(*i).end();l++){
      emit cmdReady((*l).split(" "));
    }
  }
}

void TCPKbd::readTcpData() {
    QByteArray data = pSocket->readAll();
    readBuffer.append(data);
    int i;
    while((i=readBuffer.indexOf("\n"))>=0){
      QByteArray cmd=readBuffer.left(i);
      handle_kbdcmd(cmd);
      readBuffer=readBuffer.remove(0,i+1);
    }
}

void TCPKbd::run(){
  qInfo() << "TCPKbd" << dev;
  pSocket = new QTcpSocket();
  connect( pSocket, SIGNAL(readyRead()), SLOT(readTcpData()) );

  pSocket->connectToHost(dev, 1929);
  if( pSocket->waitForConnected() ) {
    pSocket->setTextModeEnabled(true);
    /*
    pSocket->write("C0w\nC2w\nC4w\nC6w\n",16);
    qInfo() << pSocket->waitForBytesWritten();
    */
  }

  parse_commands();
  /*
     if (ev.type == EV_KEY && ev.value == 1){
      QMap<int, QStringList>::iterator i = commands.find((int)ev.code);
      if(i!=commands.end()){
        QStringList::iterator l;
        for(l=(*i).begin();l!=(*i).end();l++){
          emit cmdReady((*l).split(" "));
        }
      }
    }
  }
  */
}


