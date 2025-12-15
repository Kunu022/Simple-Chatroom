//
// Created by kunu on 12/4/2025.
//

#ifndef CHATWINDOW_H
#define CHATWINDOW_H

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QListWidget>
#include <QLineEdit>
#include <QPushButton>

#include <QWidget>
#include <QListWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QDialog>

class ChatWindow : public QWidget {
    Q_OBJECT
public:
    explicit ChatWindow(QWidget *parent = nullptr);

public slots:
    void addUserMessage(const QString &msg);
    void addServerMessage(const QString &msg);
    void updateUserList(const QStringList &users);

    signals:
        void sendMessage(const QString &msg);

private:
    QLineEdit *inputBox;
    QListWidget *userChatDisplay;
    QListWidget *serverChatDisplay;
    QListWidget *usersList;
    QPushButton *sendButton;
};

class UsernamePrompt : public QDialog {
    Q_OBJECT
public:
    explicit UsernamePrompt(QWidget *parent = nullptr);
    QString username() const;

private:
    QLineEdit *nameInputBox;

};

#endif //CHATWINDOW_H
