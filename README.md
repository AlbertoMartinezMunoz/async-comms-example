# async-comms-example
Test repo to develop different async comms approaches

## Build

### Build C4 diagrams

The C4 diagrams are made using [PlantUML](https://plantuml.com/). First, you have to install the `plantuml` package:

```shell
sudo apt-get update
sudo apt-get install plantuml
```

Then, to build the diagrams, you only have to run the `plantuml` command:

```shell
plantuml -tsvg <path-to-the-files>
```
