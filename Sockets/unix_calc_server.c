#include "common.h"

#include <ctype.h>
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/wait.h>
#include <unistd.h>

#define DEFAULT_SOCKET "/tmp/ipc_calc.sock"
#define BUFFER_SIZE 512

static const char *expr;

static void skip_spaces(void)
{
    while (isspace((unsigned char)*expr)) {
        expr++;
    }
}

static double parse_expression(void);

static double parse_factor(void)
{
    skip_spaces();
    if (*expr == '(') {
        expr++;
        double value = parse_expression();
        skip_spaces();
        if (*expr++ != ')') {
            die("missing parenthesis");
        }
        return value;
    }
    if (*expr == '-') {
        expr++;
        return -parse_factor();
    }
    char *end = NULL;
    errno = 0;
    double value = strtod(expr, &end);
    if (end == expr || errno == ERANGE) {
        die("invalid numeric token");
    }
    expr = end;
    return value;
}

static double parse_term(void)
{
    double value = parse_factor();
    for (;;) {
        skip_spaces();
        char op = *expr;
        if (op != '*' && op != '/') {
            return value;
        }
        expr++;
        double rhs = parse_factor();
        value = (op == '*') ? value * rhs : value / rhs;
    }
}

static double parse_expression(void)
{
    double value = parse_term();
    for (;;) {
        skip_spaces();
        char op = *expr;
        if (op != '+' && op != '-') {
            return value;
        }
        expr++;
        double rhs = parse_term();
        value = (op == '+') ? value + rhs : value - rhs;
    }
}

static double evaluate(const char *input)
{
    expr = input;
    double value = parse_expression();
    skip_spaces();
    if (*expr != '\0' && *expr != '\n') {
        die("unexpected trailing token");
    }
    return value;
}

static void reap(int signo)
{
    (void)signo;
    while (waitpid(-1, NULL, WNOHANG) > 0) {
    }
}

static void serve(int client)
{
    char request[BUFFER_SIZE];
    ssize_t n = read(client, request, sizeof(request) - 1);
    if (n <= 0) {
        return;
    }
    request[n] = '\0';
    double result = evaluate(request);
    char response[BUFFER_SIZE];
    int len = snprintf(response, sizeof(response), "pid=%ld result=%.3f\n",
                       (long)getpid(), result);
    write_all(client, response, (size_t)len);
}

int main(int argc, char **argv)
{
    const char *path = argc > 1 ? argv[1] : DEFAULT_SOCKET;

    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = reap;
    sa.sa_flags = SA_RESTART | SA_NOCLDSTOP;
    sigaction(SIGCHLD, &sa, NULL);

    int server = socket(AF_UNIX, SOCK_STREAM, 0);
    if (server == -1) {
        die_errno("socket");
    }

    struct sockaddr_un addr;
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    snprintf(addr.sun_path, sizeof(addr.sun_path), "%s", path);

    unlink(path);
    if (bind(server, (struct sockaddr *)&addr, sizeof(addr)) == -1 ||
        listen(server, 32) == -1) {
        die_errno("bind/listen");
    }

    printf("unix_calc_server listening on %s\n", path);
    fflush(stdout);

    for (;;) {
        int client = accept(server, NULL, NULL);
        if (client == -1) {
            if (errno == EINTR) {
                continue;
            }
            die_errno("accept");
        }
        pid_t child = fork();
        if (child == -1) {
            die_errno("fork");
        }
        if (child == 0) {
            xclose(server);
            serve(client);
            xclose(client);
            _exit(0);
        }
        xclose(client);
    }
}
