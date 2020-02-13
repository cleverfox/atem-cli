#ifndef KBDREADER_H
#define KBDREADER_H

#include <QtCore>
#include <QObject>
#include <QString>

#define CONFIG_FILE "keyboard.txt"

class KBDReader : public QThread {
  Q_OBJECT
  private:
    QString dev;
    QMap<int,QStringList> commands;
  public:
    KBDReader(QObject * parent, QString device): QThread(parent){this->dev = device;}
    void parse_commands();

    void run();

signals:
    void cmdReady(QStringList cmd);

};

#endif

