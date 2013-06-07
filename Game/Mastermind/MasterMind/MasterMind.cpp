// NavalBattles.cpp : Defines the entry point for the console application.
//
//second version 
// more descriptive
// extension to the real rules / 6 color, 4 peg, 4 
//manual or IA
// - IA mode = r (random / redundant), re (random / exclusive), 

#include "stdafx.h"

#include <string>
#include <map>
#include <deque>
#include <vector>
#include <algorithm>
#include <time.h>
#include <iostream>
#include <sstream>
using namespace std;

#include "boost/bind.hpp"
#include "boost/assign.hpp"
using namespace boost::assign;

enum color_e { white = 0, blue, green, yellow, orange, red };
typedef map<color_e, char> color_code_map_t;
typedef pair<color_e, char> color_t;
typedef vector<color_t> guess_code_t;
typedef vector<color_t> hidden_code_t;

const color_code_map_t color_code = map_list_of (color_e::white, 'A')(color_e::blue, 'B')(color_e::green, 'C')(color_e::yellow, 'D')(color_e::orange, 'E')(color_e::red, 'F') ;
const auto comb_length = 4;
const auto nb_line = 8;
hidden_code_t TheHiddenCode(comb_length);
vector<char> answer(comb_length);
//how many of 4(k) peg can make from 6(N) ==> N^k
string guess;

struct acc_char {
	void operator () (color_t c) {
		acc_ += c.second;
	}
	string get() {return acc_;}
	string acc_;
};

template<typename T> string colorCodeString(const T& m) {
	bool is_vector_of_peg = std::is_same<T, guess_code_t>::value || std::is_same<T, hidden_code_t>::value ;
	if(!is_vector_of_peg)
		return ""; //or throw ?
	acc_char acc; 
	return std::for_each(begin(m), end(m), acc).get(); 
}
color_t ChooseRandomPeg() { return *color_code.find((color_e)(rand() % color_code.size())); }

struct Count {
	Count( hidden_code_t code, int& incorrect_place, int& exact )
		: incorrect_place_(incorrect_place), exact_(exact=0), code_(code) { }
	char operator()( color_t c, char g ){ 
		bool correct_place = (c.second == toupper(g));
		if(correct_place) { //Argh, conditionnal are ugly !
			exact_++;
			return 'W';
		} else {
			//does that color be somewhere else in the code... 
			auto f = std::find_if(begin(code_), end(code_), [&g](const color_t& tobecheck_) { return (tobecheck_.second == toupper(g));} );
			auto misplaced = (f != code_.end());
			incorrect_place_ += (int) (misplaced);
			if(misplaced)
				return 'B';
			else
				return ' ';
		}
		
	}
	~Count(){ 
	}
	
	int &incorrect_place_, &exact_;
	hidden_code_t code_;
};

class WinnerOrLoser {
	public:
	WinnerOrLoser(int exact, int code_length, int nb_try, int nb_line) : state_(false), msg_("not found") {
		state_ = (exact == code_length);
		stringstream builder;
		if(state_) {
			builder << "found in " << nb_try << "/" << nb_line;
			msg_ = builder.str();
		} 
	}
	~WinnerOrLoser(){}
	bool getState() {return state_;}
	string getMsg() {return msg_;}
private:
	bool state_;
	string msg_;
};

void combinations_with_repetitions_recursion(deque< vector<char> >& combi, const vector<char> &elems, 
											 unsigned long req_len,
											vector<unsigned long> &pos, 
											unsigned long depth)
{
	// Have we selected the number of required elements?
	if (depth >= req_len) {
		vector<char> depth_r_combi;
		for (unsigned long ii = 0; ii < pos.size(); ++ii){
			depth_r_combi += elems[pos[ii]];
		}
		//copy(depth_r_combi.begin(), depth_r_combi.end(), ostream_iterator<char>(cout, " ")), cout << endl;
		combi.push_back(depth_r_combi);
		return;
	}

	// Try to select new elements to the right of the last selected one.
	for (unsigned long ii = 0; ii < elems.size(); ++ii) {
		pos[depth] = ii;
		combinations_with_repetitions_recursion(combi, elems, req_len, pos, depth + 1);
	}
	return;
}

deque<vector<char>> combinations_with_repetitions(const vector<char> &elems, unsigned long r)
{
	if(r > elems.size()){
		throw std::logic_error("combination length (r) cannot be greater than the elements list (n)");
	}

	vector<unsigned long> positions(r, 0);
	deque<vector<char>> combi;
	combinations_with_repetitions_recursion(combi, elems, r, positions, 0);

	return combi;
}

int main() {
	
	int color, exact = color = 0;

	cout << "Code of " << TheHiddenCode.size() << " peg" << endl;
	cout << "Number of try to guess " << nb_line <<  endl;
	cout << "Color are ", for_each(color_code.begin(), color_code.end(), [](const color_t& c){cout << c.second << " " ;}), cout << endl;

	vector<char> elements;
	std::for_each(color_code.begin(), color_code.end(), [&elements](const color_code_map_t::value_type& p) { elements.push_back(p.second); });
	auto combi_found = combinations_with_repetitions(elements, TheHiddenCode.size());
	std::cout << "total combination you can try = " << combi_found.size() << std::endl;
	
	srand( time(0) ),
		generate( TheHiddenCode.begin(), TheHiddenCode.end(), ChooseRandomPeg );

	int nb_try = 0; 
	while( exact < TheHiddenCode.size() && nb_try < nb_line) {
		cout << "\n\nguess--> ", cin >> guess;
		if(guess.size() == 4) {
		exact = color = 0;
		transform( begin(TheHiddenCode), end(TheHiddenCode),
			begin(guess), begin(answer),
			Count( TheHiddenCode, color, exact ) );
		cout << "Answer => " << "Incorrect place " << color << " Correct place " << exact << " : " ,
			copy(answer.begin(), answer.end(), ostream_iterator<char>(cout, " ")), 
			cout << endl;
		nb_try++;
		} else {
			cout << "incorrect number peg on the line" << endl;
		}
	}
	cout << "Hidden code " << colorCodeString(TheHiddenCode) << " " << WinnerOrLoser(exact, TheHiddenCode.size(), nb_try, nb_line).getMsg() <<  "!\n";
}

