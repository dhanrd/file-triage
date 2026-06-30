#include <stddef.h>
#include <stdio.h>
#include <string.h>

typedef struct {
  const char *name;
  const unsigned char *signature;
  size_t signature_length;
  const char *extension;
} FileType;

static const unsigned char PNG_SIGNATURE[] = {0x89, 0x50, 0x4E, 0x47,
                                              0x0D, 0x0A, 0x1A, 0x0A};

static const unsigned char JPEG_SIGNATURE[] = {0xFF, 0xD8, 0xFF};

static const unsigned char PDF_SIGNATURE[] = {0x25, 0x50, 0x44, 0x46, 0x2D};

static const unsigned char ZIP_SIGNATURE[] = {0x50, 0x4B, 0x03, 0x04};

/* Each supported type is identified by a fixed signature at the start. */
static const FileType FILE_TYPES[] = {
    {"PNG", PNG_SIGNATURE, sizeof(PNG_SIGNATURE), ".png"},
    {"JPEG", JPEG_SIGNATURE, sizeof(JPEG_SIGNATURE), ".jpg"},
    {"PDF", PDF_SIGNATURE, sizeof(PDF_SIGNATURE), ".pdf"},
    {"ZIP", ZIP_SIGNATURE, sizeof(ZIP_SIGNATURE), ".zip"},
};

static const size_t FILE_TYPE_COUNT =
    sizeof(FILE_TYPES) / sizeof(FILE_TYPES[0]);

static const FileType *detect_file_type(const unsigned char *buffer,
                                        size_t bytes_read) {
  for (size_t i = 0; i < FILE_TYPE_COUNT; i++) {
    const FileType *current_type = &FILE_TYPES[i];

    if (bytes_read < current_type->signature_length) {
      continue;
    }

    if (memcmp(buffer, current_type->signature,
               current_type->signature_length) == 0) {
      return current_type;
    }
  }

  return NULL;
}

static const char *find_extension(const char *file_path) {
  size_t file_path_length = strlen(file_path);

  for (size_t i = file_path_length; i > 0; i--) {
    char current = file_path[i - 1];

    if (current == '/') {
      break;
    }

    if (current == '.') {
      return &file_path[i - 1];
    }
  }

  return NULL;
}

int main(int argc, char *argv[]) {
  if (argc != 2) {
    printf("Usage: ./filetriage <file-path>");
    return 1;
  }

  const char *file_path = argv[1];
  printf("%s\n", file_path);

  FILE *fp = fopen(file_path, "rb");
  if (fp == NULL) {
    printf("Could not open file: %s\n", file_path);
    return 1;
  }

  printf("Successfully opened %s\n", file_path);

  /* Move to the end so ftell can report the size, then rewind to read bytes. */
  int result = fseek(fp, 0, SEEK_END);
  if (result != 0) {
    printf("Error parsing file: %s\n", file_path);
    fclose(fp);
    return 1;
  }

  long file_size = ftell(fp);
  if (file_size == -1L) {
    printf("Could not determine file size\n");
    fclose(fp);
    return 1;
  }
  (void)file_size;

  fseek(fp, 0, SEEK_SET);

  unsigned char buffer[16];
  size_t bytes_read = fread(buffer, 1, sizeof(buffer), fp);

  for (size_t i = 0; i < bytes_read; i++) {
    printf("%02X ", buffer[i]);
  }

  printf("\n");

  const FileType *detected_type = detect_file_type(buffer, bytes_read);

  if (detected_type != NULL) {
    printf("Detected type: %s\n", detected_type->name);
  } else {
    printf("Detected type: unknown\n");
  }

  const char *extension = find_extension(file_path);

  if (extension == NULL) {
    printf("Extension validation: unavailable because the file has no "
           "extension\n");
  } else if (detected_type == NULL) {
    printf("Extension validation: unavailable because the file type is "
           "unknown\n");
  } else if (strcmp(extension, detected_type->extension) == 0) {
    printf("Extension match: yes\n");
  } else {
    printf("Extension mismatch: filename says %s, contents appear to be %s\n",
           extension, detected_type->name);
  }

  printf("\n");

  fclose(fp);

  return 0;
}
