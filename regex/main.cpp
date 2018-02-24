/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   main.cpp
 * Author: Niccolò Ferrari
 *
 * Created on January 27, 2018, 2:16 PM
 */

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <regex>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <boost/xpressive/xpressive.hpp>

#if ( defined _WIN32 || defined _WIN64 )
#define SEPARATOR '\\'
#else
#define SEPARATOR '/'
#endif

using namespace std;
using namespace boost;

bool std_search(string text, std::regex rex) {
  return std::regex_search(text, rex);
}

bool std_match(string text, std::regex rex) {
  return std::regex_match(text, rex);
}

string std_replace(string text, std::regex rex, string replace) {
  return std::regex_replace(text, rex, replace);
}

bool xpr_search(string text, xpressive::sregex rex) {
  return xpressive::regex_search(text, rex);
}

bool xpr_match(string text, xpressive::sregex rex) {
  return xpressive::regex_match(text, rex);
}

string xpr_replace(string text, xpressive::sregex rex, string replace) {
  return xpressive::regex_replace(text, rex, replace);
}

string getName(string name) {
  string _name = "";
  char c = 0;
  int i = name.length();
  do {
    c = name.at(--i);
  } while (i > 0 && c != SEPARATOR);
  if (c == SEPARATOR) i++;
  for (int j = i; j < name.length(); j++) {
    _name += (name.at(j));
  }
  return _name;
}

void printHelp(string name) {
  cout << name << " [OPTIONS]" << endl;
  cout << "\t-e PATTERN:" << endl << "\t\tRegex pattern." << endl;
  cout << "\t-f FILENAME:" << endl << "\t\tInput filename" << endl;
  cout << "\t-m [OPT]:" << endl << "\t\tPrint only matching lines. If option \"all\" is enabled, prints only full-matching lines" << endl;
  cout << "\t-a:" << endl << "\t\tRead all file or stdin until EOF and then match regex. Be careful with big text files." << endl;
  cout << "\t-i:" << endl << "\t\tCase insensitive flag." << endl;
  cout << "\t-x:" << endl << "\t\tUse Boost::Xpressive regex engine, more powerful than c++14 one." << endl;
  cout << "\t-c [COLOR]:" << endl << "\t\tIf no color is specified disable text coloring. You can specify one of these colors:" << endl;
  cout << "\t\t\t- black" << endl;
  cout << "\t\t\t- red" << endl;
  cout << "\t\t\t- green" << endl;
  cout << "\t\t\t- yellow" << endl;
  cout << "\t\t\t- blue" << endl;
  cout << "\t\t\t- magenta" << endl;
  cout << "\t\t\t- cyan" << endl;
  cout << "\t\t\t- white" << endl;
  cout << "\t-l:" << endl << "\t\tDisable line number." << endl;
}

void printArgsError(string name) {
  string _name = getName(name);
  cerr << "ARGS ERROR" << endl;
  printHelp(_name);
}

typedef enum eColor {
  black = 0, red, green, yellow, blue, magenta, cyan, white
} Color;

Color getColor(string colName) {
  Color c = green;
  if (colName.compare("black") == 0) {
    c = black;
  } else if (colName.compare("red") == 0) {
    c = red;
  } else if (colName.compare("green") == 0) {
    c = green;
  } else if (colName.compare("yellow") == 0) {
    c = yellow;
  } else if (colName.compare("blue") == 0) {
    c = blue;
  } else if (colName.compare("magenta") == 0) {
    c = magenta;
  } else if (colName.compare("cyan") == 0) {
    c = cyan;
  } else if (colName.compare("white") == 0) {
    c = white;
  } else {
    c = green;
  }
  return c;
}

string getColorString(Color c) {
  string esc = "\e[1;3";
  esc += to_string(c) + "m";
  return esc;
}

/*
 * 
 */
int main(int argc, char** argv) {
  int c = 0;
  string _regex, filename, color;
  bool r = false, f = false, col = true;
  bool all_match = false, only_match = false, _icase = false, complete_file = false;
  bool xpr_en = false, n_line = true;

  while ((c = getopt(argc, argv, "e:f:m::aic::xl")) != -1)
    switch (c) {
      case 'e':
        r = true;
        _regex = string(optarg);
        break;
      case 'f':
        f = true;
        filename = string(optarg);
        break;
      case 'm':
        only_match = true;
        if (optarg != NULL && string(optarg).compare("all") == 0) {
          all_match = true;
        }
        break;
      case 'a':
        complete_file = true;
        break;
      case 'i':
        _icase = true;
        break;
      case 'c':
        if (optarg != NULL) {
          col = true;
          color = string(optarg);
        } else {
          col = false;
        }
        break;
      case 'x':
        xpr_en = true;
        break;
      case 'l':
        n_line = false;
        break;
      default:
        printArgsError(string(argv[0]));
        return 1;
    }
  if (!r) {
    printArgsError(string(argv[0]));
    return 1;
  }

  ifstream rgxFile;
  regex rx;
  xpressive::sregex rex;

  if (_icase) {
    if (!xpr_en) rx = regex(_regex, regex_constants::icase);
    else rex = xpressive::sregex::compile(_regex, xpressive::icase);
  } else {
    if (!xpr_en) rx = regex(_regex);
    else rex = xpressive::sregex::compile(_regex);
  }

  bool match = true;

  if (f) {
    rgxFile = ifstream(filename, ios::in);
  }

  string rc = (col ? getColorString(getColor(color)) : "");
  string rr = "\e[m";

  if (!complete_file) {
    int lnum = 0;
    for (string line; getline((!f) ? cin : rgxFile, line);) {
      lnum++;
      bool _match = (!xpr_en ? std_search(line, rx) : xpr_search(line, rex));
      if (all_match) {
        match = (!xpr_en ? std_match(line, rx) : xpr_match(line, rex));
      } else if (only_match) {
        match = _match;
      }
      if (match) {
        string res = (!xpr_en ? std_replace(line, rx, rc + "$0" + rr) : xpr_replace(line, rex, rc + "$&" + rr));
        if (n_line) cout << "\e[m" << ((_match && col) ? "\e[1;33m" : "") << "line #" << lnum << ":\e[m\t";
        cout << res << endl;
      }
    }
  } else if (complete_file) {
    stringstream buffer;
    if (f) {
      buffer << rgxFile.rdbuf();
    } else {
      int lnum = 0;
      for (string line; getline((!f) ? cin : rgxFile, line);) {
        lnum++;
        if (lnum > 1) buffer << endl;
        buffer << line << flush;
      }
    }
    string all_file = buffer.str();
    string res = (!xpr_en ? std_replace(all_file, rx, rc + "$0" + rr) : xpr_replace(all_file, rex, rc + "$&" + rr));
    cout << res << endl;
  } else {
    cerr << "PARAM ERROR." << endl;
    return 1;
  }
  return 0;
}

