# YAML SCHEMA CPP library

The *yaml-schema-cpp* is a C++ library is intended to be used to specify and validate requirements to YAML files.

This is useful for applications where the configuration is done via YAML files.

The specification of this input files includes:
- Structure and fields.
- Type of the fields (allowing custom classes).
- Fields can be mandatory or optional.
- Default values can be specified for optional fields.
- The values of can be limited to a set of options.
- Use of polymorfism in type checking.

Future features:
- Building a template YAML file from schema.
- Default values for custom classes.
- Options for custom classes.

## Installation

Requirements: C++14 and cmake 3.10 or newer. 
It is tested with Ubuntu 18.04 and 20.04.

### Dependencies

**1. Boost**

```bash
sudo apt-get install libboost-all-dev
```

**2. Eigen 3**

```bash
sudo apt install libeigen3-dev
```

**3. yaml-cpp 0.6.2**

In ubuntu 20.04, it is the version available via apt: 

```bash
sudo apt install libyaml-cpp-dev
```
In ubuntu 18.04, it has to be installed from source:

```bash
git clone -b yaml-cpp-0.6.2 --depth 1 https://github.com/jbeder/yaml-cpp.git
cd yaml-cpp
mkdir -pv build
cd build
cmake -DCMAKE_BUILD_TYPE=release -DCMAKE_CXX_FLAGS=-fPIC ..
make
sudo make install
```

### Clone and compile

```bash
git clone -b main https://gitlab.iri.upc.edu/labrobotica/algorithms/yaml-schema-cpp.git
cd yaml-schema-cpp
mkdir build
cmake ..
make -j4
sudo make install
```
You can run the tests with `ctest -j4`.

## Using yaml_schema_cpp

### Load and check YAML inputs

The class `YamlServer` centralizes all the *yaml-schema-cpp* functionalities.
In your C++ code, you should instantiate an object of this type to load and check the input YAML files.

```c++
std::vector<std::string> schema_folders{"/some/path/containing/schema/files"};
YamlServer server = YamlServer(schema_folders, "another/path/containing/the/input.yaml");

bool succeed = server.applySchema("requeriments.schema");

YAML::Node input_node = server.getNode();
```

Or, if you have more than one yaml file to load:

```c++
YamlServer server = YamlServer({"one/path","another/path"});

server.loadYaml("path/to/input1.yaml")
bool succeed = server.applySchema("requeriments.schema");
YAML::Node input_node_1 = server.getNode();

server.loadYaml("path/to/input2.yaml")
bool succeed = server.applySchema("other_requeriments.schema");
YAML::Node input_node_2 = server.getNode();
```

### The `.schema` file

The specifications are contained in schema files.
Schema files are YAML files as well, but they have the extension `.schema`.

The following is an example of a schema file:
```yaml
house:
  kitchen:
    hoven:
      mandatory: true
      type: string
      doc: "Model of the hoven"
    stove:
      number:
        mandatory: true
        type: int
        doc: "number of stoves"
      stove_type:
        mandatory: false
        type: string
        default: "gas"
        options: ["gas", "electric", "induction", "coil", "other"]
        doc: "The type of the stoves"
      power:
        mandatory: false
        type: double
        doc: "The power of the stoves (W)"
  dinning_room:
    TV:
      mandatory: false
      type: string
      doc: "The model of the TV, if any"
    sofa:
      mandatory: true
      type: unsigned int
      doc: "sofa number of seats"
    table:
      mandatory: true
      type: Eigen::Vector2d
      doc: "Table dimensions (m)"
  bedrooms:
    mandatory: true
    type: derived[]
    base: BedroomClass
    doc: "A sequence of bedrooms" 
  corridor:
    mandatory: false
    type: string
    doc: "A string describing the corridor"
```

In this example, an input yaml file is specified. In the schema file, an input field can be specified or not (ex: "house", "kitchen", "stove" and "dinning_room" are not specified). There are some reserved fields to specify the input field, presented below.

#### `type`
A **string** specifying which type the input field should be. 
The `type` can be:
- Trivial types (`bool`, `char`, `int`, `unsigned int`, `long int`, `long unsigned int`, `float`, `double`, `std::string`).
- Eigen types (`Eigen::MatrixXd`, `Eigen::VectorXd`, `Eigen::Matrix2d`, `Eigen::Vector2d`, `Eigen::Matrix3d`, `Eigen::Vector3d`, ...)
- "derived": The input field can be a custom type deriving from a specified base class (see `base`).
- Custom types (new feature comming soon)

If the type string ends with `[]`, the input field should be a sequence.

#### `mandatory`
A **bool** specifying if the input field is required or just optional.

#### `doc`
A **string** with a brief documentation.

#### `default` (optional)
In case the input field is missing (allowed only if `mandatory` is `false`), then it is added with the `default` value in the input yaml node.

#### `options` (optional)
A sequence of valid values.

#### `base` (required if `type` is "derived")
The base class that the input field type should inherit from. This checks the input against the base class schema.


### The `follow` key

In both input and schema files, the key `follow` is interpreted as a copy-paste of the contents of another file.
