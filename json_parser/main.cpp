#include <iostream>
#include <cstdlib>
#include <string>
#include <map>
#include <fstream>
#include <vector>

using namespace std;

string cutBetweenSameChars(string source, char c)
{
	int index_o = -1;
	int index_c = -1;
	int i = 0;

	while(index_c == -1)
	{
		if(source[i]==c)
		{	
			if(index_o == -1)
			{
				index_o = i;
			}
			else if(index_c == -1)
			{
				index_c = i;
			}
		}

		if(index_c == -1 && i == source.length() - 1) index_c = i+1;
		
		i++;
	}

	return source.substr(index_o+1, index_c - index_o - 1);
}

string cutBetweenPairChars(string source, char from, char to)
{
	int index_o = -1;
	int index_c = -1;
	int i = 0;
	int count = 0;

	while(index_c == -1)
	{
		if(index_o != -1)
		{
			if(source[i]=='{' || source[i]=='[')
			{	
				count++;
			}
			else if(source[i]=='}' || source[i]==']')
			{
				count--;
			}
		}

		if(source[i]==from)
		{	
			if(index_o == -1) index_o = i;
		}
		if(source[i]==to)
		{	
			// cout << "to: " << to << " " << i << " " << count << " " << index_c << "\n";
			if(count == 0)
			{
				if(index_c == -1 && index_o != -1) index_c = i;
			}
		}
		
		if(index_c == -1 && i == source.length() - 1)
		{
			// last property of object
			index_c = i+1;
		}

		i++;
	}

	return source.substr(index_o+1, index_c - index_o - 1);
}

string cutToChar(string source, char c)
{
	int index_c = -1;
	int i = 0;
	int count = 0;

	while(index_c == -1)
	{
		if(source[i]=='{' || source[i]=='[')
		{	
			count++;
		}
		else if(source[i]=='}' || source[i]==']')
		{
			count--;
		}
		if(source[i]==c)
		{	
			// cout << "to: " << to << " " << i << " " << count << " " << index_c << "\n";
			if(count == 0)
			{
				if(index_c == -1) index_c = i;
			}
		}
		
		if(index_c == -1 && i == source.length() - 1)
		{
			// last item of array
			index_c = i+1;
			// cout << "LASTITEMSOURCE: " << source << "| SUB: " << source.substr(0, index_c);
		}

		i++;
	}

	return source.substr(0, index_c);
}

class Obj 
{
	public:
		string content;
		int type = 0;
		map<string, Obj> c; // children
		Obj(string _content)
		{
			content = _content;
		}
		void parse()
		{
			c.clear();

			string inner = cutBetweenPairChars(content, '{', '}');
			string inner_arr = cutBetweenPairChars(content, '[', ']');
			type = 0; // default type object

			// cout << "\n" << inner.length() << "  " <<  inner_arr.length() << "\n";
			// cout << inner << "\n" << inner_arr << "\n";
			if(content == inner && content == inner_arr)
			{
				// plain value
				type = 2; 
			}
			else
			{
				// array or object
				if(inner_arr.length() > inner.length() || content.length() == inner.length())
				{
					type = 1;
					inner = inner_arr;
				}
			}

			// cout << "CONTENT: " << content << "\n";
			// cout << "INNERARR: " << inner_arr << "\n";
			// cout << "INNER: " << inner << "\n";
			// cout << "TYPE: " << type << "\n\n";

			if(type == 0)
			{
				// object

				while(inner.length() > 0)
				{
					string property_name = cutBetweenSameChars(inner, '"');
					string property_content = cutBetweenPairChars(inner, ':', ',');
				
					// cout << "\nNAME:\n" << property_name << "\n\n";
					// cout << "\nCONTENT:\n" << property_content << "\n\n";

					int len = property_name.length() + property_content.length() + inner.find('"') + 3;
					// cout << "\nLEN:\n" << len << "   " << inner.length() << "\n\n";

					if(len >= inner.length()) inner = "";
					else inner = inner.substr(len, inner.length()-len);

					// cout << "\nINNER:\n" << inner << "\n\n";

					Obj nc = Obj(property_content);
					nc.parse();

					c.insert (
						pair<string,Obj>(
							property_name,
							nc
						)
					);
					// cout << "\n\n\nCONTENT\n    " << content << "\nNAME\n    " << property_name << "\nVALUE\n    " << property_content << "\n";
				}
				
				// cout << "\n\n" << inner << "\n\n";
			}
			else if(type == 1)
			{
				// array
				// cout << "\n\n\nCONTENT\n    " << content << "\nINNER\n    " << inner << "\n\n";
				int index = 0;
				while(inner.length() > 0)
				{
					string property_content = cutToChar(inner, ',');
					// cout << "\nCONTENT:\n" << property_content << "\n\n";

					int len = property_content.length() + 1;
					// cout << "\nLEN:\n" << len << "   " << inner.length() << "\n\n";

					if(len >= inner.length()) inner = "";
					else inner = inner.substr(len, inner.length()-len);

					// cout << "\nNEWINNER:\n" << inner << "\n\n";

					Obj nc = Obj(property_content);
					nc.parse();

					c.insert (
						pair<string,Obj>(
							to_string(index),
							nc
						)
					);

					// cout << "\nNEWARRKEY: " << to_string(index) << "\n";

					index++;
					// cout << "\n\n\nCONTENT\n    " << content << "\nVALUE\n    " << property_content << "\n";
				}
				// cout << "\nARRLEN: " << index << "\n";
			}
			
		}

		Obj v(string _c_name) // value at
		{
			map<string, Obj>::iterator it = c.find(_c_name);
			if(it != c.end())
			{
				return c.at(_c_name);
			}

			cout << "\nUNDEFINED KEY " << _c_name << "!\n";
			
			return Obj("");
		}

		string getString()
		{
			return cutBetweenSameChars(content, '"');
		}

		int getInt()
		{
			return stoi(content);
		}

		vector<string> keysVector()
		{
			vector<string> v;
			for(map<string,Obj>::iterator it = c.begin(); it != c.end(); ++it)
			{
				v.push_back(it->first);
			}

			return v;
		}

		string keysString()
		{
			string v;
			vector<string> keys = keysVector();

			for(int i = 0; i < keys.size(); i++)
			{
				v += "\"" + keys[i] + "\"";
				if(i != keys.size()-1) v += ", ";
			}

			return v;
		}
};

string execute()
{
	system("curl -u metrics_5l6jGB04:6XuasvcK8p https://db.hexio.cloud/query -X POST -d 'db=metrics_5l6jGB04&q=SELECT Max(teplota) FROM cidlo02' > curl_out.txt 2>curl_err.txt");
	ifstream ifs("curl_out.txt");
	string content(
		(istreambuf_iterator<char>(ifs)),
		(istreambuf_iterator<char>())
	);
	return content;
}

int main()
{
	string r;
	r = execute();
	
	// string a = "{\"foo\":\"bar\",\"asd\":123,\"more\":{\"a\":\"1\", \"b\":12}}";
	// string a = "{\"results\":[{\"statement_id\":0,\"series\":[{\"name\":\"cidlo02\",\"columns\":[\"time\",\"max\"],\"values\":[[\"2020-02-19T12:38:59.788179712Z\",2500]]}]}]}";
	// string a = "{\"foo\":\"bar\"}";

	Obj o = Obj(r);
	o.parse();

	// cout << "\nRESPONSE:\n" << r << "\n\n";
	// cout << o.content << "\n\n";

	// cout << o.v("foo").content << "\n\n";
	// cout << o.v("asd").content << "\n\n";
	// cout << o.v("more").content << "\n\n";
	// cout << o.v("more").v("a").getInt() << "\n\n";
	// cout << o.v("more").v("b").getInt() << "\n\n";

	// cout << o.v("a").v("3").content;

	// cout << "\nKEYS: " << o.v("results").keysString() << "";
	// cout << "\nTYPE: " << o.v("results").type << "\n";

	// cout << o.v("results").content;

	// cout << o.v("results").keysString() << "\n";
	// cout << o.v("results").v("0").keysString() << "\n";
	// cout << o.v("results").v("0").v("series").keysString() << "\n";
	// cout << o.v("results").v("0").v("series").v("0").keysString() << "\n";
	// cout << o.v("results").v("0").v("series").v("0").v("values").keysString() << "\n";
	// cout << o.v("results").v("0").v("series").v("0").v("values").v("0").keysString() << "\n";
	// cout << o.v("results").v("0").v("series").v("0").v("values").v("0").v("1").keysString() << "\n";

	int temperature =  o.v("results").v("0").v("series").v("0").v("values").v("0").v("1").getInt();
	cout << "\nMaximalni teplota je: " << temperature << "\n\n";

	// cout << o.content;

	// cout << "\n\n";

	return 1;
}