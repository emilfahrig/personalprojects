#include "passwordstorage.h"
#include "ui_passwordstorage.h"
#include "QGridLayout"
#include "QTextEdit"
#include "QStackedWidget"
#include "QPlainTextEdit"
#include "QLabel"
#include "user.h"
#include "hashtable.h"
#include <QDebug>

PasswordStorage::PasswordStorage(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::PasswordStorage)
{
    ui->setupUi(this);
    createAccount = new QWidget;
    createAccountLayout = new QGridLayout;
    createAccount->setLayout(createAccountLayout);
}

PasswordStorage::~PasswordStorage()
{
    delete ui;
}

void PasswordStorage::on_LogIn_clicked()
{
    QString userName = ui->getUsername->text();
    QString password = ui->getPassword->text();
    currentUser = getKey(userName);

    if  (database.checkAuthenticity(currentUser, getEncryption(password)))  {
        ui->stackedWidget->setCurrentIndex(2);
        setUpWelcomeScreen();
    }

    else  {
        ui->notFoundLabel->setText("Sorry. Account not found. Create an account below.");
    }
}

void PasswordStorage::on_createAccount_clicked()
{
    info_label = new QLabel[9];
    info_LineEdit = new QLineEdit[9];

    info_label[0].setText("Username: ");
    info_label[1].setText("Password: ");
    info_label[2].setText("First Name: ");
    info_label[3].setText("Last Name: ");
    info_label[4].setText("Address: ");
    info_label[5].setText("City: ");
    info_label[6].setText("Zip: ");
    info_label[7].setText("Phone: ");
    info_label[8].setText("Email: ");

    for  (int i = 0; i < 9; i++)  {
        createAccountLayout->addWidget(&info_label[i], i, 0);
        createAccountLayout->addWidget(&info_LineEdit[i], i, 1);
    }

    QPushButton *createAccountButton = new QPushButton;
    createAccountButton->setText("Create Account");
    createAccountLayout->addWidget(createAccountButton, 9, 0);
    connect(createAccountButton, SIGNAL(released()), this, SLOT(CreateAccountPressed()));

    createAccount->show();
}

void PasswordStorage::CreateAccountPressed()
{
    int username_key_input = getKey(info_LineEdit[0].text());
    int encryption_key_input = getEncryption(info_LineEdit[1].text());

    QString userName_input = info_LineEdit[0].text();
    QString password_input = info_LineEdit[1].text();
    QString firstName_input = info_LineEdit[2].text();
    QString lastName_input = info_LineEdit[3].text();
    QString address_input = info_LineEdit[4].text();
    QString city_input = info_LineEdit[5].text();
    int zip_input = (info_LineEdit[6].text()).toInt();
    quint64 phone_input = (info_LineEdit[7].text()).toUInt();
    QString email_input = info_LineEdit[8].text();

    if  (userName_input == "" || password_input == "" || firstName_input == "" || lastName_input == "" || address_input == "" || city_input == "" || zip_input == 0 || phone_input == 0 || email_input == "")  {
        createAccount->close();
        ui->notFoundLabel->setText("Missing Fields");
    }

    else  {
        User* newUser = new User(userName_input, password_input, firstName_input, lastName_input, address_input, city_input, zip_input, phone_input, email_input);
        database.addToPlaylist(username_key_input, encryption_key_input, newUser);

        for  (int i = 0; i < 9; i++)  {
            info_LineEdit[i].clear();
        }

        createAccount->close();
    }
}

int PasswordStorage::getKey(QString username_input)
{
    int ASCIICounter = 0;
    int length = username_input.length();
    QChar value;

    for (int i = 0; i < length; i++)  {
        value = username_input[i];
        ASCIICounter += value.toLatin1();
    }

    return ASCIICounter;
}

void PasswordStorage::on_settingButton_clicked()
{
    QString firstName = database.getFirstName(currentUser);
    QString lastName = database.getLastName(currentUser);
    QString userName = database.getUsername(currentUser);
    QString address = database.getAddress(currentUser);
    QString city = database.getCity(currentUser);
    int zipCode = database.getZip(currentUser);
    quint64 phoneNumber = database.getPhone(currentUser);
    QString email = database.getEmail(currentUser);

    ui->stackedWidget->setCurrentIndex(1);

    ui->firstName_label->setText(firstName);
    ui->lastName_label->setText(lastName);
    ui->username_label->setText(userName);
    ui->address_label->setText(address + " " + city + " " + QString::number(zipCode));
    ui->phone_label->setText(QString::number(phoneNumber));
    ui->email_label->setText(email);
}

int PasswordStorage::getEncryption(QString password_input)
{
    int ASCIICounter = 0;
    int length = password_input.length();
    QChar value;

    for (int i = 0; i < length; i++)  {
        value = password_input[i];
        ASCIICounter += value.toLatin1();
    }

    return ASCIICounter;
}

void PasswordStorage::setUpWelcomeScreen()
{
    QString firstName = database.getFirstName(currentUser);
    QString lastName = database.getLastName(currentUser);
    ui->welcome_label_2->setText("Welcome " + firstName + " " + lastName);
}

void PasswordStorage::on_backButton_clicked()
{
    ui->stackedWidget->setCurrentIndex(2);
}

void PasswordStorage::on_changePassword_clicked()
{
    int numPrompts = 3;
    int numResponse = 3;
    changePassword = new QWidget;
    changePasswordLayout = new QGridLayout;
    prompts = new QLabel[numPrompts];
    responses = new QLineEdit[numResponse];
    QPushButton* changePasswordButton = new QPushButton;

    prompts[0].setText("Username");
    prompts[1].setText("Email");
    prompts[2].setText("New Password");
    changePasswordButton->setText("Change");

    for  (int prompt = 0; prompt < numPrompts; prompt++)  {
        changePasswordLayout->addWidget(&prompts[prompt], prompt, 0);
    }

    for  (int response = 0; response < numResponse; response++)  {
        changePasswordLayout->addWidget(&responses[response], response, 1);
    }

    changePasswordLayout->addWidget(changePasswordButton, 3, 0);
    connect(changePasswordButton, SIGNAL(released()), this, SLOT(ChangePasswordPressed()));

    changePassword->setLayout(changePasswordLayout);
    changePassword->show();
}

void PasswordStorage::ChangePasswordPressed()
{
    int userName_input_key = getKey(responses[0].text());
    QString email_input = responses[1].text();
    QString password_input = responses[2].text();
    int new_encryption_key = getEncryption(password_input);

    if  (database.checkAuthenticityChangePassword(userName_input_key, email_input))  {
        database.changePassword(userName_input_key, password_input, new_encryption_key);
    }

    changePassword->close();
}

void PasswordStorage::on_logOut_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
}
