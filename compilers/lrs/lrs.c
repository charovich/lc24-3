#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

int execute_command(const char* command) {
    pid_t pid = fork();
    if (pid == 0) {
        execl("/bin/bash", "/bin/bash", "-c", command, (char*)NULL);
        perror("execl");
        exit(1);
    } else if (pid < 0) {
        perror("fork");
        return 1;
    } else {
        int status;
        waitpid(pid, &status, 0);
        return (WIFEXITED(status) ? WEXITSTATUS(status) : 1);
    }
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <input_file> <output_file>\n", argv[0]);
        return 1;
    }

    char* inputFile = argv[1];
    char* outputFile = argv[2];

    printf("LostRS\n");
    printf("(c) 2025 Lost Project\n\n");
    printf("Parsing code...\n");

    char command[8192];
    snprintf(command, sizeof(command),
             "cat \"%s\" | "
             "sed 's/fn/def/g; s/print!/puts/g; s/if \\([^{]\\+\\) {/if (\\1):/g; s/([^=]*) == ([^=]*)/(\\\\1 == \\\\2)/g; s/def main() {/def main():/g; s/[{}]//g; s/;//g' > tmp.py && "
             "echo \"import lusl.std\" > tmp2.py && "
             "cat tmp.py >> tmp2.py && "
             "mv tmp2.py \"%s\" && cat \"%s\"",
             inputFile, outputFile, outputFile);

    int status = execute_command(command);
    if (status != 0) {
        fprintf(stderr, "Command execution failed with status: %d\n", status);
        return 1;
    }

    printf("\nGenerated file: %s\n", outputFile);
    printf("Done.\n");

    return 0;
}

