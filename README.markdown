# Common

This project contains common code, shared between our various projects,
namely [encrypt][1] and [stegfs][2]. It may be useful for others,
either just as a learning exercise or possibly even using some or all of
it in your own code.

It includes a test suite that hopefully shows how to make use of some of
the extensions.

## CCcypt

Common cryptographic functions, typically parsing of string values to
enum constants.

## CLI

Various formatted CLI output functions, including a progress bar.

## Common

Common constants.

## Config

Parsing of config files and command line arguments (where arguments
override/replace values from files). Can also handle updating of config
files if necessary.

## Dir

A couple recursive directory functions, creating and reading. The others
are more specific to [stegfs][2] though may be useful for something.

## ECC

Error correction code handling.

## Error

Error handling, including dumping a stacktrace (if possible) on error.

## List

Custom linked list implementation.

## Map

Custom key/value map implementation.

## Mem

Do-or-die memory allocation.

## Non-GNU

Various functions available on GNU systems but not elsewhere.

## Pair

Pairs of values.

## TLV

Custom tag-length-value implementation.

## Version

Application versioning, including getting build and system information
for `--help` command line argument.

[1]: https://albinoloverats.net/projects/encrypt
[2]: https://albinoloverats.net/projects/stegfs
