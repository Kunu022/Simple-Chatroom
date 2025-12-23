use tokio::{
    net::{TcpListener, TcpStream},
    sync::broadcast,
    sync::Mutex,
    io::{AsyncBufReadExt, AsyncWriteExt, BufReader},
};
use serde::{Serialize, Deserialize};
use chrono::Local;
use std::error::Error;
use std::sync::{Arc,OnceLock};
use std::{collections::HashMap, net::SocketAddr};

static USERS: OnceLock<Arc<Mutex<HashMap<SocketAddr, String>>>> = OnceLock::new();
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

#[derive(Debug, Clone, Serialize, Deserialize)]
enum MessageType {
    UserMessage,
    ServerMessage,
    UserList,
}

#[tokio::main]
async fn main() -> Result<(), Box<dyn Error>> {
    //online user list
    USERS.set(Arc::new(Mutex::new(HashMap::new()))).ok();

    let listener = TcpListener::bind("0.0.0.0:8082").await?;
    println!("listening on {}", listener.local_addr()?);
    //buffer
    let (tx, _) = broadcast::channel::<String>(100);

    loop {
        let (socket, addr) = listener.accept().await?;
        println!("[{}] new connection from {}", Local::now().format("%H:%M:%S"), addr);

        let tx = tx.clone();
        let rx = tx.subscribe();

        tokio::spawn(async move {
            handle_connection(socket, tx, rx).await;
        });
    }
}

async fn handle_connection(socket: TcpStream, tx: broadcast::Sender<String>, mut rx: broadcast::Receiver<String>) {
    let (reader, mut writer) = socket.into_split();
    let mut reader = BufReader::new(reader);

    let mut username = String::new();
    if let Ok(_) = reader.read_line(&mut username).await{
        let username = username.trim().to_string();

        //update and broadcast userlist
        let addr = writer.as_ref().peer_addr().unwrap_or_else(|_| "0.0.0.0:0".parse().unwrap());
        USERS.get()
            .unwrap()
            .lock()
            .await
            .insert(addr, username.clone());

        broadcast_user_list(&tx).await;

        //join notification
        let join_msg = Message {
            username: "".to_string(),
            content: format!("{} joined", username),
            time: Local::now().format("%H:%M:%S").to_string(),
            message_type: MessageType::ServerMessage,
        };
        let join_json = serde_json::to_string(&join_msg).unwrap() + "\n";
        tx.send(join_json).unwrap();

        let mut line = String::new();
        loop {
            tokio::select! {
                //recienve and read client messsages
                result = reader.read_line(&mut line) => {
                    match result{
                        Ok(0) => break,
                        Ok(_) => {
                            let input = line.trim().to_string();

                            //parse json
                            let client_msg: Message = match serde_json::from_str(&input) {
                                Ok(msg) => msg,
                                Err(_) => { line.clear(); continue; }
                            };
                            let message = Message {
                                username: username.clone(),
                                content: client_msg.content,
                                time: Local::now().format("%H:%M:%S").to_string(),
                                message_type: MessageType::UserMessage,
                            };

                            let json = serde_json::to_string(&message).unwrap() + "\n";
                            tx.send(json).unwrap();
                            line.clear();
                        }
                        Err(e) => {
                            println!("reading error: {}", e);
                            break;
                        }
                    }
                }

                //send message to client
                result = rx.recv() => {
                    let msg = result.unwrap();
                    if let Err(e) = writer.write_all(msg.as_bytes()).await {
                        println!("writing error: {}", e);
                        break;
                    }
                }
            }
        }


        //leave notification
        let leave_msg = Message {
            username: "".to_string(),
            content: format!("{} left", username),
            time: Local::now().format("%H:%M:%S").to_string(),
            message_type: MessageType::ServerMessage,
        };
        let leave_json = serde_json::to_string(&leave_msg).unwrap() + "\n";
        tx.send(leave_json).unwrap();

        let addr = writer.as_ref().peer_addr().unwrap_or_else(|_| "0.0.0.0:0".parse().unwrap());
        USERS.get().unwrap().lock().await.remove(&addr);
        broadcast_user_list(&tx).await;

        println!("[{}] {} disconnected", Local::now().format("%H:%M:%S"), username);
    }else{
    println!("failed reading username");
    }
}

async fn broadcast_user_list(tx: &broadcast::Sender<String>) {
    println!("broadcasting");
    let users_map = USERS.get().unwrap().lock().await;
    let list: Vec<String> = users_map.values().cloned().collect();

    let update = UserListMessage {
        message_type: MessageType::UserList,
        users: list,
    };

    let json = serde_json::to_string(&update).unwrap() + "\n";
    let _ = tx.send(json);
}
