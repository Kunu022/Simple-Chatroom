use std::env;
extern crate bindgen;

fn main() {
    let lib_path = r"../../target/debug/chatroom_lib";
    println!("cargo:rustc-link-search=native=lib");

    println!("cargo:rustc-link-lib=dylib=chatroom_lib");
}