use super::bincrypt::{BinaryConfig, BinHash, BinaryLocator};
use serde::{Deserialize, Serialize};

#[no_mangle]
#[cfg_attr(target_os = "linux", link_section = ".bincrypt")]
#[cfg_attr(target_os = "macos", link_section = "__DATA,__bincrypt")]
pub static CONFIG: BinaryConfig<BinConfig, 512> = BinaryConfig::new();

impl BinaryLocator for BinConfig {
    const SECTION: &'static str = "bincrypt";
}

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
