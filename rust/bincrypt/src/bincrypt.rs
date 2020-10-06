use anyhow::Result;
use serde::de::{self, Visitor};
use serde::{Deserialize, Deserializer, Serialize, Serializer};
use std::convert::TryInto;
use std::hash::{Hash, Hasher};
use std::fmt;

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
        } else {
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
                    return Err(de::Error::invalid_length(
                        v.len(),
                        &"expected base64 encoded value of 4 bytes",
                    ));
                }
                let v = u64::from_be_bytes(v[0..3].try_into().unwrap());
                Ok(BinHash(v))
            }
            Err(e) => Err(de::Error::custom(e)),
        }
    }
}

impl<'de> Deserialize<'de> for BinHash {
    fn deserialize<D>(deserializer: D) -> Result<BinHash, D::Error>
    where
        D: Deserializer<'de>,
    {
        deserializer.deserialize_u64(BinHashVisitor)
    }
}
