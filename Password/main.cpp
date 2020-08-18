#include "passwordstorage.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    PasswordStorage w;
    w.show();
    return a.exec();
}
