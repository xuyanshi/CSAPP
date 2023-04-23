#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char *argv[]) {
  // simulator send task to scheduler
  int send_task[2];
  // scheduler make decision and send back to simulator
  int receive_schedule[2];
  pid_t cpid;
  char buf;

  if (argc != 4) {
    fprintf(stderr, "Usage: %s <string>\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  if (pipe(send_task) == -1) {
    perror("pipe");
    exit(EXIT_FAILURE);
  }

  if (pipe(receive_schedule) == -1) {
    perror("pipe");
    exit(EXIT_FAILURE);
  }

  cpid = fork();
  if (cpid == -1) {
    perror("fork");
    exit(EXIT_FAILURE);
  }

  if (cpid == 0) {
    // Close unused write end of send_task
    close(send_task[1]);
    // Close unused read end of receive_schedule
    close(receive_schedule[0]);

    dup2(send_task[0], STDIN_FILENO);
    dup2(receive_schedule[1], STDOUT_FILENO);

    if (strcmp(argv[2], "java") == 0) {
      
      if (strcmp(argv[3], "rr") == 0) {
        system("java -cp ./java/target:./java/lib/gson-2.8.5.jar Main rr");
      } else if (strcmp(argv[3], "fifo") == 0) {
        system("java -cp ./java/target:./java/lib/gson-2.8.5.jar Main fifo");
      } else {
        system("java -cp ./java/target:./java/lib/gson-2.8.5.jar Main new");
      }

    } else if (strcmp(argv[2], "python") == 0) {

      if (strcmp(argv[3], "rr") == 0) {
        system("python ./python/scheduler.py rr");
      } else if (strcmp(argv[3], "fifo") == 0) {
        system("python ./python/scheduler.py fifo");
      } else {
        system("python ./python/scheduler.py new");
      }

    } else {
      fprintf(stderr, "Usage: %s <string>\n", argv[0]);
    }

    close(send_task[0]);
    close(receive_schedule[1]);
    _exit(EXIT_SUCCESS);

  } else {
    // Close unused read end of send_task
    close(send_task[0]);
    // Close unused write end of receive_schedule
    close(receive_schedule[1]);

    dup2(send_task[1], STDOUT_FILENO);
    dup2(receive_schedule[0], STDIN_FILENO);

    char *base = "./cpp/build/src/sim ./configs/sim_config.json ./traces/";
    char *cmd = (char *) malloc(strlen(base) + strlen(argv[1]));
    strcpy(cmd, base);
    strcat(cmd, argv[1]);

    system(cmd);

    close(send_task[1]);
    close(receive_schedule[0]);
    wait(NULL);
    exit(EXIT_SUCCESS);
  }
}
