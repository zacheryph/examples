# bincrypt

This is an exercise at storing configuration data within a binary itself. The
idea originates from the Wraith IRC Botpack [https://github.com/wraith/wraith].
This was purely an exercise to see how easily it could be done within Rust.

After a bit of bikeshedding I stumbled upon const_fn/const_generic for being
able to make this a more general.. _solution_. If I ever get bored I may look
at making a macro that will allow hiding all of the link_section/writing into
generic implementations.

## Example Usage

```
$ ./bincrypt --template > template.toml
# edit template.toml

$ shasum bincrypt
37a07857f0be2574d241628211ea4375575e17d3  bincrypt

$ ./bincrypt --write tmpl.toml
found Segment/Section
Attempting to write config (344 bytes into 512 byte segment)

$ shasum bincrypt
b573545ba7cc35f16f5336425993cd59b5d6023a  bincrypt

$ ./bincrypt
# view config decoded from static variable
```
