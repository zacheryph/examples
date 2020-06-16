#![allow(incomplete_features)]
#![feature(const_fn)]
#![feature(const_generics)]

mod bincrypt;
mod conf;

use conf::{BinConfig, Config, CONFIG};
use std::path::PathBuf;
use structopt::StructOpt;

#[derive(StructOpt)]
#[structopt(name = "bincrypt", about = "Example binary embedded configuration")]
struct Cmd {
    #[structopt(long)]
    template: bool,

    #[structopt(value_name = "file", long, parse(from_os_str))]
    write: Option<PathBuf>,
}

fn main() {
    let args = Cmd::from_args();
    let settings = CONFIG.decode();

    if args.template {
        println!("{}", toml::to_string_pretty(&Config::default()).unwrap());
        return;
    }

    if args.write.is_some() {
        write_settings(&args.write.unwrap())
    } else {
        println!("Current Settings:");
        let conf = toml::to_string_pretty(&settings.config).unwrap();
        println!("{}", conf);
    }
}

fn write_settings(conf_file: &PathBuf) {
    let buf = std::fs::read(conf_file).unwrap();
    let conf: Config = toml::from_slice(&buf).unwrap();
    let binconf = BinConfig {
        pack_hash: [1; 20],
        local_hash: [2; 20],
        config: conf,
    };

    match CONFIG.write(&binconf) {
        Err(err) => println!("Write Failed: {}", err),
        _ => println!("Successfully write config"),
    };
}
