#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <linux/input.h>
#include <string.h>
#include <stdio.h>
#include "kbdreader.h"
#include <QFile>
#include <stdlib.h>

void KBDReader::parse_commands() {
  QFile file(CONFIG_FILE);
  if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    return;
  commands.clear();

  while (!file.atEnd()) {
    QByteArray line = file.readLine();
    QList<QByteArray> ba=line.split(';');
    const char *keycode=ba.at(0).data(); 
    int ikeycode=atoi(keycode);
    ba.removeFirst();
    QList<QByteArray>::iterator i;
    QStringList lst;
    for(i=ba.begin();i!=ba.end();i++){
      lst << QString(i->data()).trimmed();
      qInfo() << QString(i->data());
    }
    commands.insert(ikeycode,lst);
  }
}

void KBDReader::run(){
  struct input_event ev;
  ssize_t n;
  int fd;
  char* dev_c=this->dev.toLatin1().data();

  fd = open(dev_c, O_RDONLY);
  if (fd == -1) {
    fprintf(stderr, "Cannot open %s: %s.\n", dev_c, strerror(errno));
    return;
  }

  parse_commands();

  while (1) {
    n = read(fd, &ev, sizeof ev);
    if (n == (ssize_t)-1) {
      if (errno == EINTR)
        continue;
      else
        break;
    } else
      if (n != sizeof ev) {
        errno = EIO;
        break;
      }
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
}


