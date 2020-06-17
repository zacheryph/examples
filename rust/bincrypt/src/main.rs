#![allow(incomplete_features)]
#![feature(const_fn)]
#![feature(const_generics)]

mod bincrypt;
mod conf;

use conf::{BinConfig, Config, CONFIG};
use gumdrop::Options;
use std::path::PathBuf;

#[derive(Debug, Options)]
// #[structopt(name = "bincrypt", about = "Example binary embedded configuration")]
struct Cmd {
    #[options(help = "print help")]
    help: bool,
    #[options(help = "display version info")]
    version: bool,

    #[options(no_short, help = "print config template")]
    template: bool,

    #[options(
        no_short,
        parse(from_str),
        meta = "FILE",
        help = "update embedded config"
    )]
    write: Option<PathBuf>,
}

fn main() {
    let args = Cmd::parse_args_default_or_exit();
    if args.version {
        println!("{} {}", env!("CARGO_PKG_NAME"), env!("CARGO_PKG_VERSION"));
        return;
    }
    if args.help_requested() {
        println!("{}", Cmd::usage());
        return;
    }

    let settings = CONFIG.decode();

    if args.template {
        println!(
            "{}",
            serde_json::to_string_pretty(&Config::default()).unwrap()
        );
        return;
    }

    if args.write.is_some() {
        write_settings(&args.write.unwrap())
    } else {
        println!("Current Settings:");
        let conf = serde_json::to_string_pretty(&settings.config).unwrap();
        println!("{}", conf);
    }
}

fn write_settings(conf_file: &PathBuf) {
    let buf = std::fs::read(conf_file).unwrap();
    let conf: Config = serde_json::from_slice(&buf).unwrap();
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
