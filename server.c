#include<stdio.h>
#include<stdlib.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/socket.h>
#include<sys/un.h>
#include<sys/types.h>
#include<errno.h>
#include<ctype.h>

#define SOCKET_NAME "/home/ujjwalws/Documents/Sockets/UnixSimpleSocket"
#define BUFSIZE 1024

/*Static function prototypes*/
static double parse_expr(void); // forward
static void skip_spaces(void);
static double parse_factor(void);
static double parse_term(void);
static double parse_expr(void);
static double evaluate(const char *input);

/*Socket structure*/
struct sockaddr_un name;
/* ===== Expression Parser ===== */
const char *expr; // global pointer



static void skip_spaces(void) {
    while (isspace((unsigned char)*expr)) expr++;
}



static double parse_factor(void) {
    skip_spaces();
    double result = 0.0;

    if (*expr == '(') {
        expr++; // skip '('
        result = parse_expr();
        skip_spaces();
        if (*expr == ')') expr++;
        else fprintf(stderr, "Error: missing )\n");
    } else if (isdigit((unsigned char)*expr) || *expr == '-') {
        char *end;
        result = strtod(expr, &end);
        expr = end;
    } else {
        fprintf(stderr, "Error: unexpected char '%c'\n", *expr);
        expr++;
    }
    return result;
}

static double parse_term(void) {
    double result = parse_factor();
    skip_spaces();
    while (*expr == '*' || *expr == '/') {
        char op = *expr++;
        double rhs = parse_factor();
        if (op == '*') result *= rhs;
        else if (op == '/') result /= rhs;
        skip_spaces();
    }
    return result;
}

static double parse_expr(void) {
    double result = parse_term();
    skip_spaces();
    while (*expr == '+' || *expr == '-') {
        char op = *expr++;
        double rhs = parse_term();
        if (op == '+') result += rhs;
        else if (op == '-') result -= rhs;
        skip_spaces();
    }
    return result;
}

static double evaluate(const char *input) {
    expr = input;
    return parse_expr();
}

int main(int argc, char *argv[])
{
       int connection_socket,data_socket,ret,bytes_read; 
       char buffer[BUFSIZE];
       char response[BUFSIZE];

       /*Unlink existing socket*/
	unlink(SOCKET_NAME);

	/*Create a Master Socket*/
        connection_socket = socket(AF_UNIX,SOCK_STREAM,0);
	if(connection_socket == -1){
	   perror("socket()");
	   exit(EXIT_FAILURE);
	}
	
	/*Initialize socket structure*/
	memset(&name,0,sizeof(struct sockaddr_un));
	name.sun_family = AF_UNIX;
	strncpy(name.sun_path,SOCKET_NAME,sizeof(name.sun_path - 1));
	
	/*Bind socket to SOCKET_NAME*/
	ret = bind(connection_socket,(const struct sockaddr*)&name, sizeof(struct sockaddr_un));
	if(ret == -1){
	    perror("bind()");
	    close(connection_socket);
	    exit(EXIT_FAILURE);
	} 
	
	/*Prepare for accepting connection*/
	ret = listen(connection_socket,20);
	if(ret == -1){
	     perror("listen()");
	     close(connection_socket);
	     exit(EXIT_FAILURE);
	}
	
	/*Start the forever server*/
	for(;;)
	{
	    /*Wait to accept client request with blocking call*/
	    data_socket = accept(connection_socket,NULL,NULL);
	    if(data_socket < 0){
	        perror("accept()");
	        close(connection_socket);
	        exit(EXIT_FAILURE);
	    }
	    
	    printf("Client connected...\n");
	    
	  while ((bytes_read = read(data_socket, buffer, BUFSIZE - 1)) > 0) {
            buffer[bytes_read] = '\0';
            buffer[strcspn(buffer, "\r\n")] = 0; // strip newline

            printf("Expression: %s\n", buffer);

            double result = evaluate(buffer);
            snprintf(response, sizeof(response), "Result = %.2f\n", result);

            send(data_socket, response, strlen(response), 0);
         }

        printf("Client disconnected.\n");
        close(data_socket);
	    
	
	}
	close(connection_socket);
	return 0;
	
}
	
	

