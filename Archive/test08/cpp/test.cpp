#include <iostream>
#include <string>
#include <time.h>
#include <string.h>

using namespace std;

size_t
mystrstr(const string &h, const string &n, size_t start)
{
	//*
	{
		return h.find(n, start);
	}
	//*/
	{
		const char *hh = h.c_str();
		const char *nn = n.c_str();
		const char *r = strstr(hh + start, nn);
		if (!r)
			return string::npos;
		return r - hh;
	}
}

int main()
{
	string str = "abcdefgh";
	str += "efghefgh";
	int imax = 1024 / str.length() * 1024 * 4;

	cout << "exec.tm.sec\tstr.length" << endl;

	time_t currentTime = time(NULL);

	string find= "efgh";
	string replace ="____";
	string gstr;
	int i=0;
	int length;
	size_t repl_count = 0;

	while(i++ < imax + 1000) {
		gstr += str;
		//gstr = gstr;
		size_t start, sizeSearch=find.size(), end=0;

		//while((start=gstr.find(find,end))!=string::npos) {
		while((start=mystrstr(gstr, find, end))!=string::npos) {
			end=start+sizeSearch;
			gstr.replace(start,sizeSearch,replace);
			repl_count++;
		}
		length = str.length()*i;
		if((length%(1024 * 256))==0) {
			cout << time(NULL) - currentTime << "sec\t\t" << length/1024 << "kb  replaces=" << repl_count << endl;
		}
	}

	return 0;
}
