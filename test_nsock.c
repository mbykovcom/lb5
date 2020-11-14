#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <net/sock.h>

MODULE_LICENSE("GPL");

#define BACKLOG	5
#define PORT	2048

/* sockets */
static struct socket *test_nsock_client;
static struct socket *test_nsock_server;

struct sockaddr_in nsock_addr = {
	.sin_port	= htons(PORT),
};

int addrlen = sizeof(nsock_addr);

int __init test_nsock_init(void)
{
	/* create server socket */
	sock_create_kern(&init_net, PF_INET, SOCK_STREAM, IPPROTO_TCP,
			 &test_nsock_server);

	/* bind socket to port */
	test_nsock_server->ops->bind(test_nsock_server,
				     (struct sockaddr *) &nsock_addr,
				     addrlen);

	/* listen */
	test_nsock_server->ops->listen(test_nsock_server, BACKLOG);

	/* print server socket port */
	printk(KERN_ALERT "listening socket port: %d\n",
	       ntohs(nsock_addr.sin_port));

	/* create client socket */
	sock_create_lite(PF_INET, SOCK_STREAM, IPPROTO_TCP, &test_nsock_client);
	test_nsock_client->ops = test_nsock_server->ops;

	/* accept connection */
	test_nsock_server->ops->accept(test_nsock_server, test_nsock_client, 0,
				       true);

	/* get socket port */
	test_nsock_server->ops->getname(test_nsock_client,
					(struct sockaddr *) &nsock_addr, 1);

	/* print server socket port */
	printk(KERN_ALERT "listening socket port: %d\n",
	       ntohs(nsock_addr.sin_port));

	return 0;
}

void __exit test_nsock_exit(void)
{
	sock_release(test_nsock_client);
	sock_release(test_nsock_server);
}

module_init(test_nsock_init);
module_exit(test_nsock_exit);
