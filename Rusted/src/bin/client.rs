use serde::{Deserialize, Serialize};
use chrono::Local;
use tokio::{
    io::{AsyncWriteExt, BufReader, AsyncBufReadExt},
    net::TcpStream,
    sync::Mutex,
};
use std::{
    ffi::{CStr, CString},
    os::raw::c_char,
    sync::{Arc, OnceLock},
};

static USERNAME: OnceLock<String> = OnceLock::new();

#[derive(Debug, Clone, Serialize, Deserialize)]
struct Message {
    username: String,
    content: String,
    time: String,
    message_type: MessageType,
}

#[derive(Debug, Clone, Serialize, Deserialize)]
struct UserListMessage {
    message_type: MessageType,
    users: Vec<String>,
}

#[derive(Debug, Clone, Serialize, Deserialize,PartialEq,Eq)]
enum MessageType {
    UserMessage,
    ServerMessage,
    UserList,
}

#[link(name = "chatroom_lib")]
unsafe
extern "C" {
    fn c_init_qt_app();
    fn c_init_chat_window();
    fn c_exec_qt();
    fn c_add_user_message(msg: *const c_char);
    fn c_add_server_message(msg: *const c_char);
    fn c_update_user_list(users: *const *const c_char, count: i32);
    fn c_register_send_callback(cb: extern "C" fn(*const c_char));
    fn c_register_username_callback(cb: extern "C" fn(*const c_char));
}

//global writer
static WRITER: OnceLock<Arc<Mutex<tokio::net::tcp::OwnedWriteHalf>>> = OnceLock::new();

extern "C" fn handle_ui_message(msg: *const c_char) {
    let c_str = unsafe { CStr::from_ptr(msg) };
    let text = match c_str.to_str() {
        Ok(s) => s.trim(),
        Err(_) => return,
    };
    if text.is_empty() {
        return;
    }

    //own message
    // let echo = format!("You > {}", text);
    // let c_echo = CString::new(echo).unwrap_or_default();
    // unsafe { c_add_user_message(c_echo.as_ptr()); }

    //seding the message to server
    if let Some(writer) = WRITER.get() {
        let writer = Arc::clone(writer);
        let content = text.to_string();

        let msg = Message {
            username: USERNAME.get().cloned().unwrap_or_else(|| "Guest".into()),
            content,
            time: Local::now().format("%H:%M:%S").to_string(),
            message_type: MessageType::UserMessage,
        };

        let json = match serde_json::to_string(&msg) {
            Ok(j) => j,
            Err(_) => return,
        };

        tokio::spawn(async move {
            let mut guard = writer.lock().await;
            let _ = guard.write_all(json.as_bytes()).await;
            let _ = guard.write_all(b"\n").await;
        });
    }
}

#[tokio::main]
async fn main() {

    //connect
    let (read_half, write_half) = connect_and_setup().await;

    //writer
    WRITER.set(Arc::new(Mutex::new(write_half)))
        .expect("WRITER already set");

    //recieve messages and update the ui
    tokio::spawn(async move {
        let mut lines = BufReader::new(read_half).lines();

        while let Ok(Some(line)) = lines.next_line().await {
            println!("raw: {}", line);
            //check if the message is a userlist
            if let Ok(list_msg) = serde_json::from_str::<UserListMessage>(&line) {
                if list_msg.message_type == MessageType::UserList {
                    let users = list_msg.users;

                    let c_strings: Vec<CString> =
                        users.iter().map(|s| CString::new(s.as_str()).unwrap()).collect();

                    let ptrs: Vec<*const c_char> =
                        c_strings.iter().map(|s| s.as_ptr()).collect();

                    unsafe {
                        println!("called userlst");
                        c_update_user_list(ptrs.as_ptr(), ptrs.len() as i32);
                    }

                    continue;
                }
        }
        //if not then proceed
            if let Ok(msg) = serde_json::from_str::<Message>(&line) {
                let formatted = match msg.message_type {
                    MessageType::UserMessage => {
                        format!("[{}] {} > {}", msg.time, msg.username, msg.content)
                    }
                    MessageType::ServerMessage => {
                        format!("*** {} ", msg.content)
                    }
                    MessageType::UserList => continue,
                };
                let c_msg = CString::new(formatted).unwrap_or_default();
                unsafe {
                    match msg.message_type {
                        MessageType::UserMessage => c_add_user_message(c_msg.as_ptr()),
                        MessageType::ServerMessage => c_add_server_message(c_msg.as_ptr()),
                        MessageType::UserList => {},
                    }
                }
            }
        }
    });

    //creating chat window instance
    unsafe {
        c_init_qt_app();
        c_register_username_callback(handle_username);
        c_register_send_callback(handle_ui_message);
        c_init_chat_window();

        c_exec_qt();
    }
    let username = USERNAME.get().cloned().unwrap_or_else(|| "Guest".into());


    println!("ui closed");
}

//getting the username
extern "C" fn handle_username(ptr: *const c_char) {
    let c_str = unsafe { CStr::from_ptr(ptr) };
    let username = match c_str.to_str() {
        Ok(s) => s.to_string(),
        Err(_) => return,
    };
    USERNAME.set(username.clone()).ok();
    println!("username is : {}", username);

    if let Some(writer) = WRITER.get() {
        let writer = Arc::clone(writer);
        tokio::spawn(async move {
            let mut guard = writer.lock().await;
            let _ = guard.write_all(username.as_bytes()).await;
            let _ = guard.write_all(b"\n").await;
        });
    }
}


//connecting and initial username sending
async fn connect_and_setup() -> (tokio::net::tcp::OwnedReadHalf, tokio::net::tcp::OwnedWriteHalf,) {
    //let stream = TcpStream::connect("192.168.0.11:8082")
    let stream = TcpStream::connect("127.0.0.1:8082")
        .await
        .expect("Failed to connect");

    let (read_half, write_half) = stream.into_split();

    (read_half, write_half)
}

