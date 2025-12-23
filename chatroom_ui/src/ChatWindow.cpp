//
// Created by kunu on 12/4/2025.
//

#include "ChatWindow.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QListWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QApplication>
#include <QPalette>
#include <QColor>
#include "chatroom_ffi.h"

ChatWindow::ChatWindow(QWidget *parent): QWidget(parent) {

    QPalette darkPalette;
    darkPalette.setColor(QPalette::Window, QColor(16, 8, 43));
    darkPalette.setColor(QPalette::WindowText, Qt::white);
    darkPalette.setColor(QPalette::Base, QColor(35, 35, 50));
    darkPalette.setColor(QPalette::AlternateBase, QColor(40, 40, 55));
    darkPalette.setColor(QPalette::Text, Qt::white);
    darkPalette.setColor(QPalette::Button, QColor(50, 50, 70));
    darkPalette.setColor(QPalette::ButtonText, Qt::white);
    darkPalette.setColor(QPalette::Highlight, QColor(100, 180, 255));
    darkPalette.setColor(QPalette::HighlightedText, Qt::black);
    darkPalette.setColor(QPalette::Disabled, QPalette::Text, QColor(120, 120, 120));

    qApp->setPalette(darkPalette);

    setStyleSheet(R"(
        QWidget { background-color: #191921; color: #e0e0e0; font-family: Segoe UI, Arial; }
        QListWidget {
            background-color: #252535;
            border: none;
            border-radius: 6px;
            padding: 8px;
        }
        QListWidget::item {
            padding: 5px 5px 0px ;
            border-radius: 6px;
            margin: 2px 0;
            background: transparent;
        }

        QListWidget::item:hover {
        background-color: #303045;
        }

        QListWidget::item:selected {
        background-color: #4080ff;
        color: white;
        outline: none;
        border: none;
        }

        QListWidget::item:focus {
        outline: none;
        border: none;
        }

        QLineEdit {
            background-color: #303048;
            border: 2px solid #404060;
            border-radius: 8px;
            padding: 5px 5px;
            font-size: 16px;
            selection-background-color: #4080ff;
        }
        QLineEdit:focus {
            border: 2px solid #5531e8;
            background-color: #383850;
        }

        QPushButton {
            background-color: #5531e8;
            color: white;
            border: none;
            border-radius: 8px;
            font-weight: bold;
            min-width: 60px;
        }
        QPushButton:hover { background-color: #5090ff; }
        QPushButton:pressed { background-color: #3070e0; }
    )");
    inputBox = new QLineEdit;
    userChatDisplay = new QListWidget;
    userChatDisplay->setWordWrap(true);
    userChatDisplay->setTextElideMode(Qt::ElideRight);
    serverChatDisplay = new QListWidget;
    usersList = new QListWidget;
    sendButton = new QPushButton("Send");
    auto *mainLayout = new QHBoxLayout;

    //input
    auto *inputBoxlayout = new QHBoxLayout;
    inputBox->setMinimumHeight(40);
    sendButton->setFixedSize(40,40);
    inputBoxlayout->addWidget(inputBox);
    inputBoxlayout->addWidget(sendButton);

    //mid collomn
    auto *midColumn = new QVBoxLayout;
    midColumn->addWidget(userChatDisplay);
    midColumn->addLayout(inputBoxlayout);

    //displays
    usersList->setMaximumWidth(150);
    serverChatDisplay->setMaximumWidth(150);

    //left column
    auto *leftColumn = new QVBoxLayout;
    leftColumn->addWidget(usersList);
    leftColumn->addSpacing(45);

    //right column
    auto *rightColumn = new QVBoxLayout;
    rightColumn->addWidget(serverChatDisplay);
    rightColumn->addSpacing(45);

    //main layout
    mainLayout->addLayout(leftColumn);
    mainLayout->addLayout(midColumn);
    mainLayout->addLayout(rightColumn);

    QFont font("Segoe UI", 12);
    font.setWeight(QFont::Medium);
    userChatDisplay->setFont(font);

    setLayout(mainLayout);

    connect(sendButton, &QPushButton::clicked, this, [this]() {
    if (!inputBox->text().isEmpty() && g_send_callback) {
        g_send_callback(inputBox->text().toUtf8().constData());
        inputBox->clear();
    }
    });

    connect(inputBox, &QLineEdit::returnPressed, this, [this]() {
    if (!inputBox->text().isEmpty() && g_send_callback) {
        g_send_callback(inputBox->text().toUtf8().constData());
        inputBox->clear();
    }
});

}

void ChatWindow::addUserMessage(const QString &msg) {
    auto *item = new QListWidgetItem(msg);
    item->setForeground(QColor("#d9d9d9"));
    userChatDisplay->addItem(item);
    userChatDisplay->scrollToBottom();
}

void ChatWindow::addServerMessage(const QString &msg) {
    serverChatDisplay->addItem(msg);
}

void ChatWindow::updateUserList(const QStringList &users) {
    usersList->clear();
    usersList->addItems(users);
}

UsernamePrompt::UsernamePrompt(QWidget *parent): QDialog(parent) {
    setWindowTitle("Enter username");
    setModal(true);

    auto *layout = new QVBoxLayout();
    nameInputBox = new QLineEdit();
    nameInputBox->setPlaceholderText("Enter username");
    layout->addWidget(nameInputBox);

    auto *confirmButton = new QPushButton("confirm", this);
    layout->addWidget(confirmButton);
    setLayout(layout);

    connect(confirmButton, &QPushButton::clicked, this, [this] {
        if (!nameInputBox->text().isEmpty())
            accept();
    });
}

QString UsernamePrompt::username() const {
    return nameInputBox->text();
}

