use anyhow::{anyhow, bail, Result};
use serde::de::{self, DeserializeOwned, Visitor};
use serde::{Deserialize, Deserializer, Serialize, Serializer};
use std::convert::TryInto;
use std::hash::{Hash, Hasher};
use std::{fs, fmt};
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

#[derive(Debug)]
pub struct BinHash(u64);

impl Default for BinHash {
    fn default() -> Self {
        BinHash(0)
    }
}

// we just use two random u64 values
fn new_hasher() -> ahash::AHasher {
    ahash::AHasher::new_with_keys(16465507875524386226, 2272181227724212771)
}

impl<T> From<&T> for BinHash
where
    T: Hash,
{
    fn from(val: &T) -> Self {
        let mut hash = new_hasher();
        val.hash(&mut hash);
        BinHash(hash.finish())
    }
}

impl Serialize for BinHash {
    fn serialize<S>(&self, serializer: S) -> Result<S::Ok, S::Error>
    where
        S: Serializer,
    {
        if serializer.is_human_readable() {
            let b64 = base64::encode(u64::to_be_bytes(self.0));
            serializer.serialize_str(&b64)
        }
        else {
            serializer.serialize_u64(self.0)
        }
    }
}

struct BinHashVisitor;

impl<'de> Visitor<'de> for BinHashVisitor {
    type Value = BinHash;

    fn expecting(&self, formatter: &mut fmt::Formatter) -> fmt::Result {
        formatter.write_str("a base64 string or u64")
    }

    fn visit_u64<E>(self, value: u64) -> Result<Self::Value, E>
    where
        E: de::Error,
    {
        Ok(BinHash(value))
    }

    fn visit_str<E>(self, value: &str) -> Result<Self::Value, E>
    where
        E: de::Error,
    {
        match base64::decode(value) {
            Ok(v) => {
                if v.len() != 4 {
                    return Err(de::Error::invalid_length(v.len(), &"expected base64 encoded value of 4 bytes"));
                }
                let v = u64::from_be_bytes(v[0..3].try_into().unwrap());
                Ok(BinHash(v))
            },
            Err(e) => Err(de::Error::custom(e))
        }
    }
}

impl<'de> Deserialize<'de> for BinHash {
    fn deserialize<D>(deserializer: D) -> Result<BinHash, D::Error>
    where
        D: Deserializer<'de>
    {
        deserializer.deserialize_u64(BinHashVisitor)
    }
}
