use super::bincrypt::BinHash;
use binary_enclave::{enclave, Enclave};
use serde::{Deserialize, Serialize};

#[enclave(botpack)]
pub static CONFIG: Enclave<BinConfig, 512> = Enclave::new();

#[derive(Default, Debug, Serialize, Deserialize)]
pub struct BinConfig {
    pub pack_hash: BinHash,
    pub local_hash: BinHash,
    pub config: Config,
}

#[derive(Debug, Hash, Serialize, Deserialize)]
pub struct Config {
    pub pack: Pack,
    pub local: Option<Local>,
}

#[derive(Debug, Hash, Default, Serialize, Deserialize)]
pub struct Pack {
    packname: String,
    shell_hash: String,
    salts: Vec<String>,
    dcc_prefix: String,
    owners: Vec<String>,
    hubs: Vec<String>,
}

#[derive(Debug, Hash, Default, Serialize, Deserialize)]
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
