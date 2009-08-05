#ifndef _SOCKETFUN_
#define _SOCKETFUN_

#ifdef __cplusplus
extern "C" {
#endif

/* 
 * keep polling for an open socket
 * after success, serve the socket and output on stdout the following line
 *      sorterserver_setup: serving socket id. 6000
 *
 * assuming the socket opened is numbered 6000
 *
 * this routine continues to randomly poke for an open port in the
 *     4k port numbers starting from 6000
 * the final port number in use is returned as *pn
 *
 * return -1 for failure
 */
int server_setup(char * dn, int * pn);

int serve_socket(char *hn, int port);
		/* This serves a socket at the given host name and port
		   number.  It returns an fd for that socket.  Hn should
                   be the name of the server's machine. */

int accept_connection(int s);
		/* This accepts a connection on the given socket (i.e.
                   it should only be called on by the server.  It returns
                   the fd for the connection.  This fd is of course r/w */

int request_connection(char *hn, int port);
		/* This is what the client calls to connect to a server's
                   port.  It returns the fd for the connection. */


#ifdef __cplusplus
}  /* extern C */
#endif

#endif
