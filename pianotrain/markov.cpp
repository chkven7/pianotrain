/* random_writer.cpp: Read in some text and construct a
 *                    Markov model of a specified order. Use the
 *                    model to generate authentic-sounding rubbish.
 * Author: Evan Dempsey
 * Last Modified: 11/Mar/2012
 */

#pragma warning(disable: 4786)
#include <iostream>
#include <vector>
#include <map>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <cstring>
#include <iostream>
#include <sstream> 
#include <vector>
#include <string>
#include <map>

using std::vector;
using std::string;
using std::cout;
using std::endl;
using std::map;
using std::ostream;
using std::ostringstream;
 
using namespace std;
 
typedef vector<int> intVector;
typedef map<string, intVector> markovMap;
 
int checkParam(char *param);
void markovModel(ifstream &in, markovMap &m, int n);
void makeRandom(markovMap &m, int n,char *);
void initRand();
 
int fmarkov(char *fil,int ord, int let,char *res)
{
    int order;
    int letters;
    ifstream in;
    markovMap markov;
  
    in.open(fil);
    if (in.fail())
    {
        cout << "Cannot open input file. Exiting..." << endl;
        return -1;
    }
 
    order = ord;
    letters = let;
 
    // Seed the random number generator
    // and generate the model
    srand(time(NULL));
    markovModel(in, markov, order);
    makeRandom(markov,letters,res); 
    in.close();
    return 0;
}
 
void markovModel(ifstream &in, markovMap &m, int n)
{
    int c;
    string token;
     
    c = in.get();
    while (in.good())
    {
        token += c;
        c = in.get();
 
        // Break if c indicates end of file
        if (!in.good())
            break;
 
        if (token.length() == n)
        {
            if (m.find(token) == m.end())
            {
                intVector vect;
                vect.push_back(c);
                m.insert(pair<string, intVector>(token, vect));
            }
 
            else
            {
                intVector vect;
                vect = m[token];
                vect.push_back(c);
                m[token] = vect;
            }
 
            token.erase(0, 1);
        }
    }
}
 
void makeRandom(markovMap &m, int n,char *res)
{
    int top = 0;
    int pos;
    int next,index;
    string seed,mout;
    intVector tmp;
    markovMap::iterator itr;
 
    for (itr = m.begin(); itr != m.end(); itr++)
    {
        tmp = itr->second;
 
        if (tmp.size() > top)
        {
            top = tmp.size();
            seed = itr->first;
        }
    }
 
    //cout << seed;
 
    for (int i=0; i<n; i++)
    {
        // Break if at any point the seed
        // does not exist in the model
        if (m.find(seed) == m.end())
            break;
 
        tmp = m[seed];
        pos = rand() % tmp.size();
        next = tmp[pos];
 
        // Output spaces instead of linebreaks
        if (next != '\n'){
            cout << (char)next;
		    mout+=(char)next;
        }else{
            cout << ' ';
			mout+=' ';
		}
 
        // Advance the seed
        seed += next;
        seed.erase(0, 1);
    }
 
    cout << endl;
	//mout+=endl;
	index=0;
    while (index<mout.length()){
         res[index]=mout.at(index);
         index=index+1;
	}
	res[index]=0;
}
 
// Check that the supplied parameter is numeric
int checkParam(char *param)
{
    int good = 1;
 
    for (int i=0; i<strlen(param); i++)
        if (!isdigit(param[i]))
            good = 0;
 
    return good;
}

/*
void main(void)
{
	char res[11];

	fmarkov("g.mb",3,10,res);
	return;
}
*/