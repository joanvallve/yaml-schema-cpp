# Introduction

The *yaml-schema-cpp* is a C++ library for specification and validation of YAML files.
This is useful for C++ applications where the configuration is done via YAML files.

The specification is done by an **schema** file, including:
- Structure and fields.
- Documentation of each field.
- Type of the field (allowing custom classes).
- Fields can be mandatory or optional.
- Default values can be specified for optional fields.
- The values of can be limited to a set of options.
- Use of polymorfism in type checking.
- Boolean expressions.

Also, an executable `yaml_template_generator` is provided to automatically generate template YAML files from schema files.

**Future features:**
- Default values for custom classes.
- Options for custom classes.
- Scalar expressions for default and options.

This library uses [exprtk](http://www.partow.net/programming/exprtk/index.html) as boolean expressions interpreter. The file "exprtk.hpp" is redistributed without modifications, under the MIT license.

# Installation

Requirements: C++14 and cmake 3.10 or newer. 
It is tested with Ubuntu 18.04 and 20.04.

## Dependencies

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

## Clone and compile

```bash
git clone -b main https://gitlab.iri.upc.edu/labrobotica/algorithms/yaml-schema-cpp.git
cd yaml-schema-cpp
mkdir build
cmake ..
make -j4
sudo make install
```
You can run the tests with `ctest -j4`.

# Using `yaml_schema_cpp`

## The `.schema` file

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
    oven:
      _mandatory: true
      _type: string
      _doc: "Model of the oven"
    stove:
      number:
        _mandatory: true
        _type: int
        _doc: "number of stoves"
      stove_type:
        _mandatory: false
        _type: string
        _default: "gas"
        _options: ["natural gas", "butane", "propane", "electric", "induction", "coil", "other"]
        _doc: "The type of the stoves"
      power:
        _mandatory: $stove_type == 'electric' or stove_type == 'induction' or stove_type == 'coil'
        _type: double
        _doc: "The power of the stoves (W)"
  dinning_room:
    TV:
      follow: TV.schema
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
    has_corridor:
      _mandatory: true
      _type: bool
      _doc: "If the house has corridor or not"
    corridor_description:
      _mandatory: $has_corridor
      _type: string
      _doc: "Description of the corridor"
  address:
    _mandatory: false
    _type: string
    _doc: "Address is hardcoded in this schema file"
    _value: "Whichever street, 11, Springfield"
```

In this example, the schema specifies the structure and contents of an input YAML file. There are some reserved fields to specify the input field, presented below.

### `_type`
A **string** specifying which type the input field should be. 
The `_type` can be:
- Trivial types (`bool`, `char`, `int`, `unsigned int`, `long int`, `long unsigned int`, `float`, `double`, `std::string`).
- Eigen types (`Eigen::MatrixXd`, `Eigen::VectorXd`, `Eigen::Matrix2d`, `Eigen::Vector2d`, `Eigen::Matrix3d`, `Eigen::Vector3d`, ...)
- "derived": The input field can be a custom type deriving from a specified base class (see `_base`).
- Custom types (new feature comming soon)

If the type string ends with `[]`, the input field is specified to be a sequence.

### `_mandatory`
A **bool** value or expression (see below) specifying if the input field is required or just optional.

### `_doc`
A **string** with a brief documentation.

### `_value` (optional)
Specify the value in the schema file. It means not allowing to define the field in the input file. This is useful for derived classes, to specify some parameters of its base class.

### `_default` (optional)
In case the input field is missing (only allowed if `_mandatory` is `false`), then it is added with the `_default` value in the input yaml node.

### `_options` (optional)
A sequence of valid values.

### `_base` (required if `_type` is "derived")
The base class that the input field type should inherit from. This checks the input against the base class schema.

### The `follow` key
In both input and schema files, the key `follow` is interpreted as a copy-paste of the contents of another file (either `.schema` or `.yaml`).

### Expressions
We allow the use of [exprtk](http://www.partow.net/programming/exprtk/index.html) expressions in `_mandatory` field. 
There are some rules for the expressions:
1. It should start by a `$` symbol.
2. All parameters involved in an expression have to be at the same level of the specified node.
3. All parameters involved in an expression have to be mandatory at all cases (i.e. true, not an expression) or have a default value defined.
4. The exprtk syntax has to be followed (see [exprtk readme](https://www.partow.net/programming/exprtk/code/readme.txt) for more information).
5. The result of the expression will be casted to `bool`. If the result is a numeric value, it will be converted to `true` except for 0 (`false`).

Some examples of expresssions:
```yaml
  _mandatory: $mode == 'auto'            # assuming 'mode' is a string parameter with mandatory=true
  _mandatory: $enabled                   # assuming 'enabled' is a bool parameter with mandatory=true
  _mandatory: $n_threads > 0             # assuming 'n_threads' is a int/double parameter with mandatory=true
  _mandatory: $enabled and not(disabled) # assuming 'enabled' and 'disabled' are bool parameters with mandatory=true
```

## C++ API: Load and check YAML inputs

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

ERROR in 'map1/param2': Wrong value. Allowed values defined in OPTIONS.
Schema specification:
  DOC: some doc
  MANDATORY: true
  TYPE: string
  OPTIONS:
   - string
   - strong
   - streng

ERROR in 'map1/param3': Wrong type.
Schema specification:
  DOC: some doc
  MANDATORY: true
  TYPE: double

ERROR in 'param4': Missing mandatory field.
Schema specification:
  DOC: some doc
  MANDATORY: true
  TYPE: string

ERROR in 'param5[1]': Wrong type.
Schema specification:
	DOC: some doc
  MANDATORY: false
	TYPE: int

ERROR in 'param6': Already defined in schema, not allowed to be changed.
Schema specification:
  DOC: some doc
  MANDATORY: false
  VALUE: 3.14
  TYPE: double

```

# YAML template generator

We provide the executable `yaml_template_generator` to generate automatically YAML input files containing the 
fields and structure, types, documentation, etc. specified in the schema files. Call it with:

```bash
yaml_template_generator schema_name schema_folders output_file
```

**`schema_name`**: Schema to be applied (just its name, without path or extension).

**`schema_folders`**: Path to the folder(s) that contains all the schema files (they are searched recursively). Provide more than one folder with '[path1 path2 ...]'.

**`output_file`**: (OPTIONAL) Path and name of the output file. If not provided, it will be placed in `$HOME` with a default name based on schema_name (requires `$HOME` to be defined).

**NOTE 1:** Paths can be absolute (starting by '/') or relative.

**NOTE 2:** `output_file` will be modified to avoid overriding existing files.