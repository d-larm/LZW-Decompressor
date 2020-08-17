# LZW Decompressor by Daniel Alarms


 Runs only on Linux

# How to run

```sh
$ make
$ ./bin/LZW <input_path_1> <input_path_2> ...
```

  - Can decompress any number of files at once
  - The decoded text is outputted into a text file of the same name in the same directory as the input


# Clean run

```sh
$ make clean
$ ./bin/LZW <input_path_1> <input_path_2> ...
```

  - Cleans the project object files for a fresh build
