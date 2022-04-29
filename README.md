# getaddrinfo

Just adding an IPv6 address to an interface on your server can wreak
havoc with many scripts and programs which used to work just fine.
That's because, by default, IPv6 takes priority over the default route
by selecting an IPv6 address whenever possible.

Address selection can be controlled by editing /etc/gai.conf.  But how
does getaddrinfo behave exactly?  Find out by compiling this short
program.

## Usage:

```sh
$ gcc -std=gnu99 -g -W -Wall -O0 -o getaddrinfo getaddrinfo.c

$ ./getaddrinfo --help
./getaddrinfo: [-O...] host.name...

	where O can be any of

4   set ai_family=AF_INET,
6   set ai_family=AF_INET6,
0   set ai_family=AF_UNSPEC and ai_flags=0,
p   or ai_flags AI_PASSIVE,
c   or ai_flags AI_CANONNAME,
n   or ai_flags AI_NUMERICHOST,
v   or ai_flags AI_V4MAPPED,
A   or ai_flags AI_ALL,
a   or ai_flags AI_ADDRCONFIG,
i   or ai_flags AI_IDN,
C   or ai_flags AI_CANONIDN,
N   or ai_flags AI_NUMERICSERV,
H   pass hints=NULL,
h   reset H, that is pass hints (the default),

	and
host.name is the host to look up.  If it is empty,
the call will have node=NULL and service="80".
ale@pcale:~/c/getaddrinfo$ ./getaddrinfo -4c github.com

getaddrinfo(github.com, NULL, hints, &res) = 0
where hints has
	flags="AI_CANONNAME"
	family=AF_INET
result 1:
	flags="AI_CANONNAME"
	family=AF_INET
	addrlen=16
	addr=140.82.121.3
	canon=github.com
result 2:
	flags="AI_CANONNAME"
	family=AF_INET
	addrlen=16
	addr=140.82.121.3
	canon=NULL
result 3:
	flags="AI_CANONNAME"
	family=AF_INET
	addrlen=16
	addr=140.82.121.3
	canon=NULL
```
