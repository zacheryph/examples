#![allow(incomplete_features)]
#![feature(const_fn)]
#![feature(const_generics)]

mod conf;

use conf::{BinaryConfig, BinConfig, Config};
use goblin::elf::Elf;
use goblin::mach::{self, Mach};
use goblin::Object;
use std::fs::{File, OpenOptions};
use std::io::{Read, Seek, SeekFrom, Write};
use std::path::PathBuf;
use structopt::StructOpt;

#[no_mangle]
#[cfg_attr(target_os = "linux", link_section = ".bincrypt")]
#[cfg_attr(target_os = "macos", link_section = "__DATA,__bincrypt")]
pub static CONFIG: BinaryConfig<BinConfig, 512> = BinaryConfig::new();

#[derive(StructOpt)]
#[structopt(name = "bincrypt", about = "Example binary embedded configuration")]
struct Cmd {
    #[structopt(long)]
    template: bool,

    #[structopt(value_name = "file", long, parse(from_os_str))]
    write: Option<PathBuf>,
}

fn main() {
    let settings = CONFIG.decode();
    let args = Cmd::from_args();

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

fn read_config(path: &PathBuf) -> Config {
    let buf = std::fs::read(path).unwrap();
    toml::from_slice(&buf).unwrap()
}

fn write_settings(settings: &PathBuf) {
    let conf: Config = read_config(settings);
    let binconf = BinConfig {
        pack_hash: [1; 20],
        local_hash: [2; 20],
        config: conf,
    };

    let args: Vec<String> = std::env::args().into_iter().collect();
    let mut file = OpenOptions::new()
        .read(true)
        .write(true)
        .open(&args[0])
        .unwrap();
    let mut data: Vec<u8> = Vec::new();
    file.read_to_end(&mut data).unwrap();

    match Object::parse(&data).unwrap() {
        Object::Mach(Mach::Binary(bin)) => {
            write_mach(file, bin, &binconf);
        }
        Object::Elf(bin) => {
            write_elf(file, bin, &binconf);
        }
        r => {
            println!("Unsupported Binary Type: {:?}", r);
        }
    }
}

fn write_mach(mut f: File, bin: mach::MachO, values: &BinConfig) {
    let segment = bin
        .segments
        .iter()
        .find(|s| s.name().unwrap() == "__DATA")
        .unwrap();
    let (offset, size) = segment
        .sections()
        .unwrap()
        .iter()
        .find(|sec| sec.0.name().unwrap() == "__bincrypt")
        .map(|x| (x.0.offset, x.0.size))
        .unwrap();
    println!("found Segment/Section");
    let raw = bincode::serialize(values).unwrap();
    if raw.len() > size as usize {
        panic!("Serialized size exceeds capacity. {} > {}", raw.len(), size);
    }

    println!("Attempting to write config ({} bytes into {} byte segment)", raw.len(), size);
    f.seek(SeekFrom::Start(offset as u64)).unwrap();
    f.write(&raw).unwrap();
    f.sync_data().unwrap();
}

// not currently working. Linux doesn't let you write in place.
fn write_elf(mut f: File, bin: Elf, values: &BinConfig) {
    let section = bin
        .section_headers
        .iter()
        .find(|sec| &bin.shdr_strtab[sec.sh_name] == ".bincrypt")
        .unwrap();
    println!("found Segment/Section");
    let raw = bincode::serialize(values).unwrap();
    if raw.len() > section.sh_size as usize {
        panic!("Serialized size exceeds capacity.");
    }

    f.seek(SeekFrom::Start(section.sh_offset)).unwrap();
    f.write(&raw).unwrap();
    f.sync_data().unwrap();
}
