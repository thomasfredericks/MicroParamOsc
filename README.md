# MicroParamOsc Arduino Library

Binds `MicroParam` instances to Open Sound Control (OSC) addresses for sending and receiving OSC messages.

Namespace: global

## Functions

### microParamOscDispatch

```cpp
bool success = microParamOscDispatch( message, bindings, bindingsCount);
```
Dispatches an incoming OSC message to the matching bindings. `message` (`MicroOscMessage &`) is the incoming OSC message, `bindings` (`MicroParamOscBind *`) is the array of parameter bindings, and `bindingsCount` (`size_t`) is the number of bindings in the array. Returns `true` if a binding was updated successfully, otherwise `false`.

### microParamOscSend

```cpp
microParamOscSend( osc, binding);
```
Sends a single binding's parameters via OSC. `osc` (`MicroOsc &`) is the OSC instance used to send the message, and `binding` (`MicroParamOscBind &`) is the binding whose parameters are sent.

### microParamOscSend

```cpp
microParamOscSend( osc, bindings, bindingsCount);
```
Sends multiple bindings' parameters via OSC. `osc` (`MicroOsc &`) is the OSC instance used to send the messages, `bindings` (`MicroParamOscBind *`) is the array of parameter bindings to send, and `bindingsCount` (`size_t`) is the number of bindings in the array.

## Structs and Classes

### MicroParamOscBind

#### Constructor(s)

```cpp
MicroParamOscBind paramBinding(address, typeTags, param);
```
Constructs a binding for a single parameter. `address` (`const char *`) is the OSC address for this binding, `typeTags` (`const char *`) is the OSC type tags describing parameter types, and `param` (`MicroParam &`) is the reference to the parameter being bound.

```cpp
MicroParamOscBind paramBinding(address, typeTags, paramArray);
```
Constructs a binding for an array of parameters. `address` (`const char *`) is the OSC address for this binding, `typeTags` (`const char *`) is the OSC type tags describing parameter types, and `paramArray` (`MicroParam **`) is the array of pointers to parameters being bound.

#### Method generateHash (static)

```cpp
uint32_t hash = MicroParamOscBind.generateHash( s);
```
Generates a hash for a given string using FNV-1a. `s` (`const char *`) is the string to hash. Returns the hash value (`uint32_t`).

#### Method getAddress

```cpp
const char *address = paramBinding.getAddress();
```
Returns the OSC address for this binding. Returns the address (`const char *`).

#### Method getAddressHash

```cpp
uint32_t hash = paramBinding.getAddressHash();
```
Returns the hash of the OSC address. Returns the hash value (`uint32_t`).

#### Method getTypeTags

```cpp
const char *tags = paramBinding.getTypeTags();
```
Returns the OSC type tags for this binding. Returns the type tags (`const char *`).

#### Method getTypeTagsHash

```cpp
uint32_t hash = paramBinding.getTypeTagsHash();
```
Returns the hash of the OSC type tags. Returns the hash value (`uint32_t`).

#### Method getCount

```cpp
size_t count = paramBinding.getCount();
```
Returns the number of parameters in this binding. Returns the count (`size_t`).

#### Method getParam

```cpp
MicroParam &param = paramBinding.getParam();
```
Returns the first parameter in this binding. Returns the parameter (`MicroParam &`).

```cpp
MicroParam &param = paramBinding.getParam( index);
```
Returns the parameter at the given index. `index` (`size_t`) is the index of the parameter. Returns the parameter (`MicroParam &`).

#### Method matchesAddress

```cpp
bool match = paramBinding.matchesAddress( addr);
```
Checks if the binding's address matches the given string. `addr` (`const char *`) is the OSC address to compare. Returns `true` if the addresses match.

```cpp
bool match = paramBinding.matchesAddress( hash);
```
Checks if the binding's address hash matches the given hash. `hash` (`uint32_t`) is the hash to compare. Returns `true` if the hashes match.

#### Method matchesTypeTags

```cpp
bool match = paramBinding.matchesTypeTags( hash);
```
Checks if the type tags hash matches the given hash. `hash` (`uint32_t`) is the hash to compare. Returns `true` if the hashes match.

```cpp
bool match = paramBinding.matchesTypeTags( tags);
```
Checks if the type tags match the given string. `tags` (`const char *`) is the type tags to compare. Returns `true` if the type tags match.


