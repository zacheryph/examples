extern crate csv;
#[macro_use]
extern crate serde_derive;

use csv::ReaderBuilder;

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

fn read_passwd() -> Result<(), std::io::Error> {
    let mut rdr = ReaderBuilder::new()
        .delimiter(b':')
        .has_headers(false)
        .from_path("/etc/passwd")
        .unwrap();

    for row in rdr.deserialize() {
        let usr: Passwd = row?;
        println!("User: {:?}", usr);
    }

    Ok(())
}

fn main() {
    match read_passwd() {
        Err(err) => println!("Error: {:?}", err),
        _ => {}
    }
}
