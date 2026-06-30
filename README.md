# file-triage

`file-triage` is a small C command-line program that inspects a file, prints
its first 16 bytes in hexadecimal, detects a supported file type from its magic
bytes, and checks whether the filename extension matches the detected type.

Currently supported formats:

- PNG
- JPEG
- PDF
- ZIP

## Compile

Use a normal C compiler such as `cc`:

```sh
cc filetriage.c -o filetriage
```

## Run

```sh
./filetriage <file-path>
```

Example: if a PNG file has been renamed to `image.txt`, the program reports
that the contents appear to be PNG while the filename extension says `.txt`.

Magic-byte detection only checks the starting bytes of a file. It does not fully
validate that the entire file is a correct PNG, JPEG, PDF, or ZIP file.
