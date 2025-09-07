# myOwnCLib
My implementations of differents useful things that don't natively exist in C

# How to integrate to your code

You can either pre-compile what you need and finally link everything together. Or you can also directly integrate the sources in your compilation command

## Requirements for each implementation

You'll see a lot of files are shared.

Every path written here is relative to the repository's root

Anything that is not mentioned here either

1. Uses no C code, only definitions or macros
2. Has been forgotten, in which case open an issue

If it doesn't fall under the second case, then simply include the header in your code and your compiler will do the remaining part of the job

### Variable Size Strings

```txt
strings/customStrings.c
memory/memfuncs.c
endianness/endianness.c
```

### Memory

```txt
memory/memfuncs.c
endianness/endianness.c
```

### Variable Size Integers

```txt
variableSizeInt/customInteger.c
strings/customStrings.c
memory/memfuncs.c
endianness/endianness.c
```

### Matrices

```txt
maths/matrices/matrix.c
memory/memfuncs.c
endianness/endianness.c
```

And you can add the different sources contained inside `maths/matrices/` if you want to use more matrices features.

### Mathematical Vectors

```txt
maths/vectors/vectors.c
maths/matrices/matrix.c
memory/memfuncs.c
endianness/endianness.c
```

### Collections

#### Lists

```txt
collections/lists/list.c
memory/memfuncs.c
endianness/endianness.c
```

#### Linked Lists

```txt
collections/linkedLists/linkedLists.c
collections/lists/list.c
memory/memfuncs.c
endianness/endianness.c
```

#### Dictionaries

```txt
collections/dicts/dict.c
collections/lists/list.c
memory/memfuncs.c
endianness/endianness.c
```

#### Queues

```txt
collections/queues/queue.c
collections/lists/lists.c
memory/memfuncs.c
endianness/endianness.c
```

#### Stacks

```txt
collections/queues/stack.c
collections/lists/lists.c
memory/memfuncs.c
endianness/endianness.c
```

### Compression

#### BPE (Byte-Pair Encoding)

```txt
compression/bpe.c
collections/lists/lists.c
memory/memfuncs.c
```