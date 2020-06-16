use anyhow::{anyhow, bail, Result};
use serde::de::DeserializeOwned;
use serde::Serialize;
use std::fs;
use std::io::{Seek, SeekFrom, Write};
use std::marker::PhantomData;

pub struct BinaryConfig<T, const SIZE: usize>([u8; SIZE], PhantomData<T>);

pub trait BinaryLocator {
    const SECTION: &'static str;
}

impl<T, const SIZE: usize> BinaryConfig<T, SIZE>
where
    T: Default + BinaryLocator + Serialize + DeserializeOwned,
{
    pub const fn new() -> Self {
        Self([0; SIZE], PhantomData)
    }

    pub fn decode(&self) -> T {
        match bincode::deserialize(&self.0) {
            Ok(s) => s,
            Err(e) => {
                println!("Deserialization Error. Ignoring. Bad Binary? {}", e);
                Default::default()
            }
        }
    }

    #[cfg(target_os = "macos")]
    pub fn write(&self, payload: &T) -> Result<()> {
        use goblin::mach;

        let mut data = read_binary()?;
        let mach = mach::MachO::parse(&data, 0)?;
        let segment = mach
            .segments
            .iter()
            .find(|s| s.name().unwrap() == "__DATA")
            .ok_or(anyhow!("Binary Segment not found"))?;
        let (offset, size) = segment
            .sections()?
            .iter()
            .find(|sec| sec.0.name().unwrap() == format!("__{}", T::SECTION))
            .map(|x| (x.0.offset, x.0.size))
            .ok_or(anyhow!("Binary Section not found"))?;
        println!("Found Mach Binary segment/section.");

        write_binary(&mut data, &payload, offset.into(), size)?;
        Ok(())
    }

    #[cfg(target_os = "linux")]
    pub fn write(&self, payload: &T) -> Result<()> {
        use goblin::elf::Elf;

        let mut data = read_binary()?;
        let elf = Elf::parse(&data)?;
        let section = elf
            .section_headers
            .iter()
            .find(|sec| &elf.shdr_strtab[sec.sh_name] == ".bincrypt")
            .ok_or(anyhow!("Binary Section not found"))?;
        println!("Found ELF Section.");

        write_binary(&mut data, &payload, section.sh_offset, section.sh_size)?;
        Ok(())
    }

    #[cfg(not(any(target_os = "macos", target_os = "linux")))]
    pub fn write(&self, payload: &T) -> Result<()> {
        panic!("Not Supported")
    }
}

fn read_binary() -> Result<Vec<u8>> {
    let args: Vec<String> = std::env::args().into_iter().collect();
    let bytes = fs::read(&args[0])?;
    Ok(bytes)
}

fn write_binary<T: Serialize>(
    data: &mut Vec<u8>,
    payload: &T,
    offset: u64,
    size: u64,
) -> Result<()> {
    let payload = bincode::serialize(payload)?;
    if payload.len() > size as usize {
        bail!(
            "Serialized size exceeds static capacity. {} > {}",
            payload.len(),
            size
        );
    }

    println!(
        "Writing config ({} bytes into {} byte segment)",
        payload.len(),
        size
    );
    let mut data = std::io::Cursor::new(data);
    data.seek(SeekFrom::Start(offset as u64)).unwrap();
    data.write(&payload)?;
    let data = data.into_inner();

    let args: Vec<String> = std::env::args().into_iter().collect();
    let file = &args[0];
    let tmpfile = format!("{}.new", file);

    let perms = fs::metadata(&file)?.permissions();
    fs::write(&tmpfile, &data)?;
    fs::rename(&tmpfile, &file)?;
    fs::set_permissions(&file, perms)?;

    Ok(())
}
