//
// Created by kunu on 12/4/2025.
//

#ifndef CHATROOM_FFI_H
#define CHATROOM_FFI_H
#pragma once

#ifdef __cplusplus
extern "C" {
#endif


typedef void (*SendCallback)(const char* msg);

extern SendCallback g_send_callback;
void c_init_chat_window();
void c_add_user_message(const char* msg);
void c_add_server_message(const char* msg);
void c_update_user_list(const char** users, int count);
void c_register_send_callback(SendCallback cb);

#ifdef __cplusplus
}
#endif

#endif //CHATROOM_FFI_H
