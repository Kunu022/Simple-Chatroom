//
// Created by kunu on 12/4/2025.
//

#ifndef CHATROOM_FFI_H
#define CHATROOM_FFI_H
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#ifdef _WIN32
#define DLL_EXPORT __declspec(dllexport)
#else
#endif

typedef void (*SendCallback)(const char *msg);
typedef void (*UsernameCallback)(const char *name);

extern SendCallback g_send_callback;
extern UsernameCallback g_username_callback;
    DLL_EXPORT void c_init_qt_app();
    DLL_EXPORT void c_init_chat_window();
    DLL_EXPORT void c_exec_qt();
    DLL_EXPORT void c_add_user_message(const char* msg);
    DLL_EXPORT void c_add_server_message(const char* msg);
    DLL_EXPORT void c_update_user_list(const char** users, int count);
    DLL_EXPORT void c_register_send_callback(SendCallback cb);
    DLL_EXPORT void c_register_username_callback(UsernameCallback cb);
    DLL_EXPORT void c_send_username(const char* name);

#ifdef __cplusplus
}
#endif

#endif //CHATROOM_FFI_H
