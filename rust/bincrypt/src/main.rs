use goblin::elf::Elf;
use goblin::mach::{self, Mach};
use goblin::Object;
use std::fs::{File, OpenOptions};
use std::io::{Read, Seek, Write, SeekFrom};
use structopt::StructOpt;

#[no_mangle]
#[cfg_attr(target_os = "linux", link_section = ".bincrypt")]
#[cfg_attr(target_os = "macos", link_section = "__DATA,__bincrypt")]
pub static CONFIG: [u8; 512] = [0; 512];

#[derive(StructOpt)]
#[structopt(name = "bincrypt", about = "Example binary embedded configuration")]
struct Cmd {
    #[structopt(short, long)]
    add: bool,

    value: Option<String>,
}

fn main() {
    let mut settings: Vec<String> = match bincode::deserialize(&CONFIG) {
        Ok(s) => s,
        Err(e) => {
            println!("Deserialization Error. Ignoring. Bad Binary? {}", e);
            Vec::new()
        },
    };

    let args = Cmd::from_args();

    match args.add {
        true => {
            let v = args.value.expect("Required for --add");
            println!("Adding Value: {}", v);
            settings.push(v);

            write_settings(&settings);
        },
        false => {
            println!("Current Settings:");
            settings.iter().for_each(|v| println!("    {}", v));
        }
    };
}

fn write_settings(values: &Vec<String>) {
    let args: Vec<String> = std::env::args().into_iter().collect();
    let mut file = OpenOptions::new()
        .read(true)
        .write(true)
        .open(&args[0]).unwrap();
    let mut data: Vec<u8> = Vec::new();
    file.read_to_end(&mut data).unwrap();


    match Object::parse(&data).unwrap() {
        Object::Mach(Mach::Binary(bin)) => {
            write_mach(file, bin, values);
        },
        Object::Elf(bin) => {
            write_elf(file, bin, values);
        },
        r => {
            println!("Unsupported Binary Type: {:?}", r);
        },
    }
}

fn write_mach(mut f: File, bin: mach::MachO, values: &Vec<String>) {
    let segment = bin.segments.iter().find(|s| {
        s.name().unwrap() == "__DATA"
    }).unwrap();
    let (offset, size) = segment.sections().unwrap().iter()
        .find(|sec| sec.0.name().unwrap() == "__bincrypt")
        .map(|x| (x.0.offset, x.0.size))
        .unwrap();
    println!("found Segment/Section");
    let raw = bincode::serialize(values).unwrap();
    if raw.len() > size as usize {
        panic!("Serialized size exceeds capacity.");
    }

    f.seek(SeekFrom::Start(offset as u64)).unwrap();
    f.write(&raw).unwrap();
    f.sync_data().unwrap();
}

fn write_elf(mut f: File, bin: Elf, values: &Vec<String>) {
    let section = bin.section_headers.iter().find(|sec| {
        &bin.shdr_strtab[sec.sh_name] == ".bincrypt"
    }).unwrap();
    println!("found Segment/Section");
    let raw = bincode::serialize(values).unwrap();
    if raw.len() > section.sh_size as usize {
        panic!("Serialized size exceeds capacity.");
    }

    f.seek(SeekFrom::Start(section.sh_offset)).unwrap();
    f.write(&raw).unwrap();
    f.sync_data().unwrap();

    //     match bin.section_headers.iter().find(|sec| &bin.shdr_strtab[sec.sh_name] == ".raw_config") {
    //         Some(sect) => println!("RAW! offset:{} size:{}", sect.sh_offset, sect.sh_size),
    //         None => println!("raw_config not found"),
    //     }

    //     match bin.section_headers.iter().find(|sec| &bin.shdr_strtab[sec.sh_name] == ".bincrypt_config") {
    //         Some(sect) => {
    //             println!("BINCRYPT! offset:{} size:{}", sect.sh_offset, sect.sh_size);
    //             let sect_end = sect.sh_offset + sect.sh_size;
    //             let sect = &data[sect.sh_offset as usize..sect_end as usize];
    //             let idx = sect.windows(PREFIX.len()).position(|x| x == PREFIX).unwrap();
    //             println!("IDX? {}", idx);

    //         },
    //         None => println!("raw_config not found"),
    //     }
    //     Ok(())
    // },
}
