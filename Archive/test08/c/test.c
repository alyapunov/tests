
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

int main() {
	setbuf(stdout, NULL ); //disable output buffering

	char *str = malloc(9);
	strcpy(str, "abcdefgh");

	str = realloc(str, strlen(str) + 9);
	strcat(str, "efghefgh");     //sprintf(str,"%s%s",str,"efghefgh");

	int imax = 1024 / strlen(str) * 1024 * 4;

	printf("%s", "exec.tm.sec\tstr.length\n"); //fflush(stdout);

	time_t starttime = time(NULL );
	char *gstr = malloc(1);
	gstr[0] = 0;
	int i = 0;
	char *pos;
	int lngth;

	char *pos_c = gstr;
	int str_len = strlen(str);

	size_t repl_count = 0;

	while (i++ < imax + 1000) {
		lngth = strlen(str) * i;
		gstr = realloc(gstr, lngth + str_len + 1);
		strcat(gstr, str);    //sprintf(gstr,"%s%s",gstr,str);
		pos_c += str_len;

		pos = gstr;
		while ((pos = strstr(pos, "efgh"))) {
			memcpy(pos, "____", 4);
			repl_count++;
		}

		if (lngth % (1024 * 256) == 0) {
			printf("%dsec\t\t%dkb replaces=%d\n",
					(int) (time(NULL ) - starttime),
					lngth / 1024, (int)repl_count); //fflush(stdout);
		}
	}
//printf("%s\n",gstr);

}

