/* Language stuff generator for win32.
 *
 * (C) 2003-2009 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for furhter details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 *
 * Written by Dominick Meglio <codemastr@unrealircd.com>
 *
 */

#include <string.h>
#include <stdio.h>
#include <cstdlib>
#include <cctype>

char *strip(char *str)
{
	char *c;
	if ((c = strchr(str,'\n')))
		*c = 0;
	if ((c = strchr(str,'\r')))
		*c = 0;
	return str;
}

int main(int argc, char *argv[])
{
	if (argc < 4)
		exit(1);

	/* Build the index file */
	if (!strcmp(argv[1], "index"))
	{
		FILE *fd = fopen(argv[2], "rb");
		FILE *fdout = fopen(argv[3], "wb");
		char buf[1024];
		if (!fd || !fdout)
			exit(2);

		while (fgets(buf, 1023, fd))
		{
			if (isupper(*buf))
				fprintf(fdout, "%s", buf);
		}
		fclose(fd);
		fclose(fdout);
	}
	/* Build the language.h file */
	else if (!strcmp(argv[1], "language.h"))
	{
		FILE *fd = fopen(argv[2], "r");
		FILE *fdout = fopen(argv[3], "w");
		char buf[1024];
		int i = 0;

		if (!fd || !fdout)
			exit(2);

		fprintf(stderr, "Generating language.h... ");

		while (fgets(buf, 1023, fd)) {
			fprintf(fdout, "#define %-32s %d\n", strip(buf), i++);
		}

		fprintf(fdout, "#define NUM_STRINGS %d\n", i);
		fprintf(stderr, "%d strings\n", i);
		fclose(fd);
		fclose(fdout);
	}
	return 0;

}
