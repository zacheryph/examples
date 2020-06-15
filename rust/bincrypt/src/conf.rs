use serde::{Deserialize, Serialize};
use serde::de::DeserializeOwned;
use std::marker::PhantomData;

pub struct BinaryConfig<T, const SIZE: usize>([u8; SIZE], PhantomData<T>);

impl<T, const SIZE: usize> Default for BinaryConfig<T, SIZE> {
    fn default() -> Self {
        BinaryConfig([0; SIZE], PhantomData)
    }
}

impl<T, const SIZE: usize> BinaryConfig<T, SIZE>
where
    T: Default + Serialize + DeserializeOwned,
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
}

#[derive(Default, Debug, Serialize, Deserialize)]
pub struct BinConfig {
    pub pack_hash: [u8; 20],
    pub local_hash: [u8; 20],
    pub config: Config,
}

#[derive(Debug, Serialize, Deserialize)]
pub struct Config {
    pack: Pack,
    local: Option<Local>,
}

#[derive(Debug, Default, Serialize, Deserialize)]
pub struct Pack {
    packname: String,
    shell_hash: String,
    salts: Vec<String>,
    dcc_prefix: String,
    owners: Vec<String>,
    hubs: Vec<String>,
}

#[derive(Debug, Default, Serialize, Deserialize)]
pub struct Local {
    bots: Vec<String>,
    uid: usize,
    autocron: bool,
    username: String,
    datadir: String,
    homedir: String,
    port_min: usize,
    port_max: usize,
}

impl Default for Config {
    fn default() -> Self {
        Self {
            pack: Pack::default(),
            local: Some(Local::default()),
        }
    }
}
