#include "ChatWindow.h"
#include <QMetaObject>
#include <QApplication>
#include "chatroom_ffi.h"

static QApplication *app = nullptr;
static ChatWindow *instance_window = nullptr;

//initialize app instance
extern "C" DLL_EXPORT void c_init_qt_app() {
    if (!app) {
        int argc = 0;
        char* argv[] = { nullptr };
        app = new QApplication(argc, argv);
    }
}


//create the window
extern "C" DLL_EXPORT void c_init_chat_window() {
    if (!instance_window) {
        instance_window = new ChatWindow();
        UsernamePrompt u;
        if (u.exec() != QDialog::Accepted) return;
        QString username = u.username();
        c_send_username(username.toUtf8().constData());

        instance_window->show();
    }
}

//app execution
extern "C" DLL_EXPORT void c_exec_qt() {
    if (app) {
        app->exec();
    }
}

//update user messages
extern "C" DLL_EXPORT void c_add_user_message(const char* msg) {
    if (!instance_window) return;
    QString tMessage = QString::fromUtf8(msg);
    QMetaObject::invokeMethod(instance_window,[tMessage]() { instance_window->addUserMessage(tMessage); }
        ,Qt::QueuedConnection
    );
}

//update server messages
extern "C" DLL_EXPORT void c_add_server_message(const char* msg) {
    if (!instance_window) return;
    QString tMessage = QString::fromUtf8(msg);
    QMetaObject::invokeMethod(instance_window,[tMessage]() { instance_window->addServerMessage(tMessage); },
        Qt::QueuedConnection
    );
}

//update user list
extern "C" DLL_EXPORT void c_update_user_list(const char** users, int count) {
    if (!instance_window) return;
    QStringList list;
    for (int i = 0; i < count; ++i)
        list << QString::fromUtf8(users[i]);
    QMetaObject::invokeMethod(instance_window,[list]() { instance_window->updateUserList(list); },
        Qt::QueuedConnection
    );
}

//callback when the user messages
//typedef void (*SendCallback)(const char* msg);
SendCallback g_send_callback = nullptr;

extern "C" DLL_EXPORT void c_register_send_callback(SendCallback cb) {
    g_send_callback = cb;
}

UsernameCallback g_username_callback = nullptr;
extern "C" DLL_EXPORT void c_register_username_callback(UsernameCallback cb) {
    g_username_callback = cb;
}
extern "C" DLL_EXPORT void c_send_username(const char* name) {
    if (g_username_callback) {
        g_username_callback(name);
    }
}