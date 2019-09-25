# GESAMT-based protein distance computation library

Distance is computed as 1 - Q-Score. If computation fails (structures are too dissimilar), distance is set to 1.

## Requirements
Cmake, C++17 compiler (for `<filesystem>`), pthreads, zlib, Java JDK.


### Directory with mmCIF files
All the mmCIF files must have a name in the following format:

`<pdbid>_updated.cif`

where `<pdbid>` is a PDBID in lowercase letters.

### List of structures to preload
When the first distance computation is issued, structures specified in this file will be preloaded. Each line of the
 file has the format: `<PDBID>:<CHAINID>`. For example:
```
12AS:A
13PK:B
153L:A
15C8:H  
```

## Building the library:

```
$ mkdir build && cd build
$ cmake ..
$ make
```

Output binaries are located in `build/distance` directory.

## Testing the library
`LD_LIBRARY_PATH` must be set correctly for Java to find the shared library `libProteinDistance.so`.

```
$ LD_LIBRARY_PATH=. java -jar java_distance.jar
```


