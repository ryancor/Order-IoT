#ifndef COMMON_H
#define COMMON_H

#define _XOPEN_SOURCE 700
#include <fcntl.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>

typedef struct {
  uint64_t      pfn : 54;
  unsigned int  soft_dirty : 1;
  unsigned int  file_page : 1;
  unsigned int  swapped : 1;
  unsigned int  present : 1;
} PagemapEntry;

// parse the pagemap entry for the given virtual address
int pagemap_get_entry(PagemapEntry *entry, int pagemap_fd, uintptr_t vaddr) {
  size_t nread;
  ssize_t ret;
  uint64_t data;

  nread = 0;
  while(nread < sizeof(data)) {
    ret = pread(pagemap_fd, &data, sizeof(data),
      (vaddr / sysconf(_SC_PAGE_SIZE)) * sizeof(data) + nread);
    nread += ret;

    if(ret <= 0) {
      return 1;
    }
  }
  entry->pfn = data & (((uint64_t)1 << 54) - 1);
  entry->soft_dirty = (data >> 54) & 1;
  entry->file_page = (data >> 61) & 1;
  entry->swapped = (data >> 62) & 1;
  entry->present = (data >> 63) &1;

  return 0;
}

// convert the given virtual address to physical using /proc/PID/pagemap
int virt_to_phys_user(uintptr_t *paddr, pid_t pid, uintptr_t vaddr) {
  char pagemap_file[BUFSIZ];
  int pagemap_fd;

  snprintf(pagemap_file, sizeof(pagemap_file), "/proc/%ju/pagemap", (uintmax_t)pid);
  pagemap_fd = open(pagemap_file, O_RDONLY);
  if(pagemap_fd < 0) {
    return 1;
  }

  PagemapEntry entry;
  if(pagemap_get_entry(&entry, pagemap_fd, vaddr)) {
    return 1;
  }
  close(pagemap_fd);
  *paddr = (entry.pfn * sysconf(_SC_PAGE_SIZE)) + (vaddr % sysconf(_SC_PAGE_SIZE));

  return 0;
}
#endif
