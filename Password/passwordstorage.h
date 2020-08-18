#ifndef PASSWORDSTORAGE_H
#define PASSWORDSTORAGE_H
#include "QLabel"
#include "QLineEdit"
#include "QGridLayout"
#include "hashtable.h"
#include "user.h"

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class PasswordStorage; }
QT_END_NAMESPACE

class PasswordStorage : public QMainWindow
{
    Q_OBJECT

public:
    PasswordStorage(QWidget *parent = nullptr);
    ~PasswordStorage();
    int username_key;
    QWidget *createAccount = nullptr;
    QGridLayout *createAccountLayout = nullptr;
    QLabel *info_label = nullptr;
    QLabel* missingField = nullptr;
    QLineEdit *info_LineEdit = nullptr;
    QWidget* changePassword = nullptr;
    QGridLayout* changePasswordLayout = nullptr;
    QLabel* prompts = nullptr;
    QLineEdit* responses = nullptr;
    int currentUser;
    HashTable database;
    int valuee = 0;

private slots:
    void on_LogIn_clicked();
    void on_createAccount_clicked();
    void CreateAccountPressed();
    void on_settingButton_clicked();
    void on_backButton_clicked();
    void ChangePasswordPressed();
    void on_changePassword_clicked();
    void on_logOut_clicked();

private:
    Ui::PasswordStorage *ui;
    int getKey(QString username_input);
    int getEncryption(QString password_input);
    void setUpWelcomeScreen();
    QString userName;
};
#endif // PASSWORDSTORAGE_H
