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

This library uses *exprtk* (http://www.partow.net/programming/exprtk/index.html). The file "exprtk.hpp" is redistributed without modifications, under the MIT license.

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

if (not succeed)
  std::cout << server.getLog() << std::endl;

YAML::Node input_node = server.getNode();
```

Or, if you have more than one yaml file to load:

```c++
YamlServer server = YamlServer({"one/path","another/path"});

server.loadYaml("path/to/input1.yaml")

if (not server.applySchema("requeriments.schema"))
  std::cout << server.getLog() << std::endl;

YAML::Node input_node_1 = server.getNode();

server.loadYaml("path/to/input2.yaml")

if (server.applySchema("other_requeriments.schema"))
  std::cout << server.getLog() << std::endl;

YAML::Node input_node_2 = server.getNode();
```

If the input yaml file is not validated, `YamlServer::applySchema()` returns false and the errors found are reported with `YamlServer::getLog()`.
The log output will be something like this:

```
---------------------------------------------------------------------------------
Log of applySchema to YAML file
  yaml file: /path/to/the/input.yaml
  schema: Example.schema
---------------------------------------------------------------------------------

map1/param2: wrong value, it should be one of the following: 
- string
- strong
- streng
-- 'map1/param2':
	Doc: some doc
	Type: string
	Accepted values: string, strong, streng, 

map1/param3: wrong type, it should be double
-- 'map1/param3':
	Doc: some doc
	Type: double

Input yaml does not contain field: param4
-- 'param4':
	Doc: some doc
	Type: string

param5[1]: wrong type, it should be int
-- 'param5[1]':
	Doc: some doc
	Type: int

```

### The `.schema` file

The specifications are contained in schema files.
Schema files are YAML files as well, but they have the extension `.schema`.

The following is an example of a schema file:
```yaml
house:
  owners:
    _mandatory: true
    _type: string[]
    _doc: "Sequence of all owners' names"
  kitchen:
    hoven:
      _mandatory: true
      _type: string
      _doc: "Model of the hoven"
    stove:
      number:
        _mandatory: true
        _type: int
        _doc: "number of stoves"
      stove_type:
        _mandatory: false
        _type: string
        _default: "gas"
        _options: ["gas", "electric", "induction", "coil", "other"]
        _doc: "The type of the stoves"
      power:
        _mandatory: false
        _type: double
        _doc: "The power of the stoves (W)"
  dinning_room:
    TV:
      _mandatory: false
      _type: string
      _doc: "The model of the TV, if any"
    sofa:
      _mandatory: true
      _type: unsigned int
      _doc: "sofa number of seats"
    table:
      _mandatory: true
      _type: Eigen::Vector2d
      _doc: "Table dimensions (m)"
  bedrooms:
    _mandatory: true
    _type: derived[]
    _base: BedroomClass
    _doc: "A sequence of bedrooms" 
  corridor:
    _mandatory: false
    _type: string
    _doc: "A string describing the corridor"
```

In this example, an input yaml file is specified. In the schema file, an input field can be specified or not (ex: "house", "kitchen", "stove" and "dinning_room" are not specified). There are some reserved fields to specify the input field, presented below.

#### `_type`
A **string** specifying which type the input field should be. 
The `_type` can be:
- Trivial types (`bool`, `char`, `int`, `unsigned int`, `long int`, `long unsigned int`, `float`, `double`, `std::string`).
- Eigen types (`Eigen::MatrixXd`, `Eigen::VectorXd`, `Eigen::Matrix2d`, `Eigen::Vector2d`, `Eigen::Matrix3d`, `Eigen::Vector3d`, ...)
- "derived": The input field can be a custom type deriving from a specified base class (see `_base`).
- Custom types (new feature comming soon)

If the type string ends with `[]`, the input field is specified to be a sequence.

#### `_mandatory`
A **bool** specifying if the input field is required or just optional.

#### `_doc`
A **string** with a brief documentation.

#### `_default` (optional)
In case the input field is missing (allowed only if `_mandatory` is `false`), then it is added with the `_default` value in the input yaml node.

#### `_options` (optional)
A sequence of valid values.

#### `_base` (required if `_type` is "derived")
The base class that the input field type should inherit from. This checks the input against the base class schema.


### The `follow` key

In both input and schema files, the key `follow` is interpreted as a copy-paste of the contents of another file (either `.schema` or `.yaml`).
