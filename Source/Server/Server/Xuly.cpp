#include "Xuly.h"

string getFileName(string s)
{
	string res = "";
	for (int i = s.size() - 1; i >= 0; i--)
	{
		if (s[i] == '\\') break;
		res = s[i] + res;
	}
	return res;
}

string getFilePath(string s)
{
	ifstream fi;
	fi.open("filePath.txt");
	if (fi)
	{
		while (!fi.eof())
		{
			string line = "";
			getline(fi, line);
			if (line.find(s) != string::npos)
				return line;

		}
		fi.close();
	}
	return "";
}