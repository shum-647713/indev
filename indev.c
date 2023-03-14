#include <errno.h>
#include <error.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/inotify.h>
#include <unistd.h>

#define ERROR(msg) error(EXIT_FAILURE, errno, msg)
#define HELP_MSG "Usage: indev <command> [<targets>]"
#define MASK IN_CLOSE_WRITE

void watch_target(const int fd, const char *pathname) {
  if (inotify_add_watch(fd, pathname, MASK) == -1) {
    ERROR("inotify_add_watch");
  }
}

int main(int argc, char *argv[]) {
  if (argc < 2) {
    puts(HELP_MSG);
    exit(EXIT_SUCCESS);
  }

  char *cmd = argv[1];

  int fd = inotify_init();
  if (fd == -1) {
    ERROR("inotify_init");
  }

  if (argc == 2) {
    watch_target(fd, ".");
  }

  for (int i = 2; i < argc; ++i) {
    watch_target(fd, argv[i]);
  }

  char buf[1024];
  for (;;) {
    ssize_t len = read(fd, buf, sizeof(buf));
    if (len == -1) {
      ERROR("read");
    }

    if (system(cmd) == -1) {
      ERROR("system");
    }
  }
}
