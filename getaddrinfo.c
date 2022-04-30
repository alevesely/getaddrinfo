/*
* getaddrinfo.c - written by vesely in milan on 20220426
* gcc -std=gnu99 -g -W -Wall -O0 -o getaddrinfo getaddrinfo.c
*/

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>

#include <assert.h>

static void usage(char const* argv0)
{
	fprintf(stdout,
		"%s: [-O...] host.name...\n"
		"\n\twhere O can be any of\n\n"
		"4   set ai_family=AF_INET,\n"
		"6   set ai_family=AF_INET6,\n"
		"0   set ai_family=AF_UNSPEC and ai_flags=0,\n"
		"p   or ai_flags AI_PASSIVE,\n"
		"c   or ai_flags AI_CANONNAME,\n"
		"n   or ai_flags AI_NUMERICHOST,\n"
		"v   or ai_flags AI_V4MAPPED,\n"
		"A   or ai_flags AI_ALL,\n"
		"a   or ai_flags AI_ADDRCONFIG,\n"
		"i   or ai_flags AI_IDN,\n"
		"C   or ai_flags AI_CANONIDN,\n"
		"N   or ai_flags AI_NUMERICSERV,\n"
		"H   pass hints=NULL,\n"
		"h   reset H, that is pass hints (the default),\n"
		"\n\tand\n"
		"host.name is the host to look up.  If it is empty,\n"
		"the call will have node=NULL and service=\"80\".\n", argv0);
}

#ifndef AI_IDN
# define AI_IDN        0x0040  /* IDN encode input (assuming it is encoded
                                   in the current locale's character set)
                                   before looking it up. */
#endif

#ifndef AI_CANONIDN
# define AI_CANONIDN   0x0080  /* Translate canonical name from IDN format. */
#endif

static inline char const *nullarg(char const *arg) {return arg? arg: "NULL";}

#define STRING2(P) #P
#define STRING(P) STRING2(P)

static char const *do_flags(int flags)
{
# define HINTFLAGS_MAX 17*9

	static char hintflags[HINTFLAGS_MAX];
	hintflags[0] = 0;
	int need_or = 0;

# define ADD_FLAG(F) \
	if (flags & F) {\
		if (need_or) \
			strcat(hintflags, " | "); \
		strcat(hintflags, STRING(#F)); \
		need_or = 1; } else (void)0

	ADD_FLAG(AI_PASSIVE);     // 1
	ADD_FLAG(AI_CANONNAME);   // 2
	ADD_FLAG(AI_NUMERICHOST); // 3
	ADD_FLAG(AI_V4MAPPED);    // 4
	ADD_FLAG(AI_ALL);         // 5
	ADD_FLAG(AI_ADDRCONFIG);  // 6
	ADD_FLAG(AI_IDN);         // 7
	ADD_FLAG(AI_CANONIDN);    // 8
	ADD_FLAG(AI_NUMERICSERV); // 9
	//    01234567890123456

	if (need_or == 0)
		strcpy(hintflags, "0");

	assert(strlen(hintflags) < HINTFLAGS_MAX);

# undef ADD_FLAG
# undef HINTFLAGS_MAX
	return hintflags;
}

static char const *do_family(int family)
{
	static char hintsfamily[20];
	switch (family)
	{
		case AF_UNSPEC: strcpy(hintsfamily, "AF_UNSPEC"); break;
		case AF_INET: strcpy(hintsfamily, "AF_INET"); break;
		case AF_INET6: strcpy(hintsfamily, "AF_INET6"); break;
		default:  strcpy(hintsfamily, "unknown"); break;
	}
	return hintsfamily;
}

static char *do_addr(const struct sockaddr *sa)
{
	static char address[80];
	switch(sa->sa_family)
	{
		case AF_INET:
			inet_ntop(AF_INET, &(((struct sockaddr_in *)sa)->sin_addr),
					address, sizeof address);
			break;

		case AF_INET6:
			inet_ntop(AF_INET6, &(((struct sockaddr_in6 *)sa)->sin6_addr),
					address, sizeof address);
			break;

		default:
			strcpy(address, "Unknown address family");
			return NULL;
	}

	return address;
}
int main(int argc, char *argv[])
{
	struct addrinfo hints;
	memset(&hints, 0, sizeof hints);

	int noopt = 0;
	int errs = 0;
	int do_hints = 1;
	for (int i = 1; i < argc; ++i)
	{
		char *arg = argv[i];

		if (noopt == 0 && arg[0] == '-')
		/*
		* options
		*/
		{
			if (arg[1] == '-')
			{
				if (arg[2] == 0)
				{
					noopt = 1;
					continue;
				}

				usage(argv[0]);
				return 0;
			}

			char o;
			for (int j = 1; (o = arg[j]) != 0; ++j)
				switch (o)
				{
					case '4':	hints.ai_family=AF_INET; break;
					case '6':	hints.ai_family=AF_INET6; break;
					case '0':	hints.ai_family=AF_UNSPEC;
								hints.ai_flags=0;
								break;
					case 'p':	hints.ai_flags |= AI_PASSIVE; break;
					case 'c':	hints.ai_flags |= AI_CANONNAME; break;
					case 'n':	hints.ai_flags |= AI_NUMERICHOST; break;
					case 'v':	hints.ai_flags |= AI_V4MAPPED; break;
					case 'A':	hints.ai_flags |= AI_ALL; break;
					case 'a':	hints.ai_flags |= AI_ADDRCONFIG; break;
					case 'i':	hints.ai_flags |= AI_IDN; break;
					case 'C':	hints.ai_flags |= AI_CANONIDN; break;
					case 'N':	hints.ai_flags |= AI_NUMERICSERV; break;
					case 'H':	do_hints = 0; break;
					case 'h':	do_hints = 1; break;
					default:
						fprintf(stderr, "Invalid option %c in %s\n", o, arg);
						errs = 1;
						break;
				}
		}
		else
		/*
		* arguments
		*/
		{
			struct addrinfo *res, *phints = do_hints? &hints: NULL;
			char *node, *service;
			if (*arg == 0)
			{
				node = NULL;
				service = "80";
			}
			else
			{
				node = arg;
				service = NULL;
			}

			int rtc = getaddrinfo(node, service, phints, &res);

			printf("\ngetaddrinfo(%s, %s, %s, &res) = %d\n",
				nullarg(node), nullarg(service),
				phints == NULL? "NULL": "hints", rtc);
			if (phints)
				printf("where hints has\n\tflags=%s\n\tfamily=%s\n",
					do_flags(phints->ai_flags), do_family(phints->ai_family));

			if (rtc != 0)
			{
				printf("ERRORE: %s\n", gai_strerror(rtc));
				continue;
			}

			int n = 0;
			for (struct addrinfo *r = res; r != NULL; r = r->ai_next)
			{
				printf("result %d:\n"
					"\tflags=%s\n"
					"\tfamily=%s\n"
					"\taddrlen=%d\n"
					"\taddr=%s\n"
					"\tcanon=%s\n",
					++n,
					do_flags(r->ai_flags),
					do_family(r->ai_family),
					r->ai_addrlen,
					do_addr(r->ai_addr),
					nullarg(r->ai_canonname));
			}
			freeaddrinfo(res); // still leaks memory
		}
	}

	return errs? 1: 0;
}
