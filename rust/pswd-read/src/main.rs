extern crate bincode;
extern crate csv;
#[macro_use]
extern crate serde_derive;
extern crate sled;

use std::io;

use bincode::{deserialize, serialize};
use csv::ReaderBuilder;
use sled::{ConfigBuilder, Tree};

#[derive(Debug, Serialize, Deserialize)]
struct Passwd {
    username: String,
    password: String,
    uid: i32,
    gid: i32,
    gecos: String,
    home: String,
    shell: String,
}

fn open_sled() -> Result<Tree, io::Error> {
    let config = ConfigBuilder::new().path("./sled.db").build();

    let tree = Tree::start(config);
    match tree {
        Ok(t) => Ok(t),
        Err(_) => Err(io::Error::new(io::ErrorKind::Other, "Error Opening Tree")),
    }
}

fn read_passwd(tree: &Tree) -> Result<(), io::Error> {
    let mut rdr = ReaderBuilder::new()
        .delimiter(b':')
        .has_headers(false)
        .from_path("/etc/passwd")
        .unwrap();

    for row in rdr.deserialize() {
        let usr: Passwd = row?;
        println!("User: {:?}", usr);

        tree.set(serialize(&usr.username).unwrap(), serialize(&usr).unwrap())
            .unwrap();
    }

    Ok(())
}

fn main() {
    let tree = open_sled().expect("DB Error");

    match read_passwd(&tree) {
        Err(err) => println!("Error: {:?}", err),
        _ => {}
    };

    let k = serialize(&"context").unwrap();
    let rec = tree.get(&k).unwrap();
    match rec {
        Some(r) => {
            let u: Passwd = deserialize(&r).unwrap();
            println!("Pulled User: {:?}", u);
        }
        None => println!("User not found"),
    }
}
