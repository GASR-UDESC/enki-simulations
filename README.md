# Robot Swarm Projects

A collection of small projects with enki simulator

## Runnig with docker

### Dependencies

- docker

### 1. Buildin image

This command is create a image with all dependencies.

```sh
docker build . -t enki-s:1
```

### up image to acces project

This command up container with a shared volume with base project folder, and access bash of this container.

```sh
docker run --rm -it -v "$(pwd):/usr/src/enki-s" enki-s:1
```

## 2. Runnig on native install

### Dependencies native

#### [Enki simulator](https://github.com/enki-community/enki) dependencies

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
