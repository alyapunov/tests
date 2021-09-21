#include <iostream>
#include <alya.h>
#include <timer.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <string>

using namespace std;

static void
check(bool e, const char *str)
{
	if (!e) {
		std::cerr << str << std::endl;
		exit(-1);
	}
}

int main(int n, const char** c)
{
	if (n <= 1) {
		COUT("Usage: ", c[0], " <path to explore>");
		return 0;
	}
	string path = c[1];
	if (path[path.length() - 1] != '/')
		path += "/";
	DIR *dir = opendir(path.c_str());
	check(dir, "opendir failed");
	while (struct dirent *ent = readdir(dir)) {
		string filename = path + ent->d_name;
		struct stat st;
		int res = stat(filename.c_str(), &st);
		check(res == 0, "stat failed");
		if ((st.st_mode & S_ISUID) && (st.st_mode & S_ISGID)) {
			COUT(filename, " SUID SGID");
		} else if (st.st_mode & S_ISUID) {
			COUT(filename, " SUID");
		} else if (st.st_mode & S_ISGID) {
			COUT(filename, " SGID");
		}
	}
	closedir(dir);
}
