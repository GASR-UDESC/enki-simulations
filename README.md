# Robot Swarm Projects

A collection of small projects with enki simulator

## Dependencies

### [Enki simulator](https://github.com/enki-community/enki) dependencies

- qt5
- cmake
- cpp compiler

## Scripts

### Setup Project

To prepare project to compile run:

```sh
$ make setup
```

to clone enki into this project root path.

## Compile Project

To compile this project including required folders and dependecies run:

```sh
$ make compile
```

## Running examples

To run one of theses examples:

```sh
$ make run run_file=<project_name>
```

Ex:

```sh
$ make run run_file=enki_grouping
```
