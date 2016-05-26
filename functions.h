#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <vector>
#include <string>
#include "cellularautomata.h"
#include "program.h"

using namespace std;

static vector<string> split(string &s, char c) {
    vector<string> result;
    string value = "";
    for (int i = 0; i < s.size(); ++i) {
        if (s[i] == c) {
            result.push_back(value);
            value = "";
        } else {
            value += s[i];
        }
    }
    result.push_back(value);
    return result;
}

// trim from start
static inline string &ltrim(string &s) {
    s.erase(s.begin(), find_if(s.begin(), s.end(), not1(ptr_fun<int, int>(isspace))));
    return s;
}

// trim from end
static inline string &rtrim(string &s) {
    s.erase(find_if(s.rbegin(), s.rend(), not1(ptr_fun<int, int>(isspace))).base(), s.end());
    return s;
}

// trim from both ends
static inline string &trim(string &s) {
    return ltrim(rtrim(s));
}

static inline string &replace(string &s, char olds, string news) {
    int pos = s.find(olds); // find first space
    while (pos != string::npos) {
        s.replace(pos, 1, news);
        pos = s.find(olds, pos);
    }
    return s;
}

string dtos(double value, int digits, bool removeZeros = false) {
    string res = "";
    string val = std::to_string(value);
    int d = 0;
    bool dig = false;
    for (uint i = 0; i < val.size(); ++i) {
        if (val[i] == ',' || val[i] == '.') {
            if (!dig) {
                res += val[i];
                dig = true;
            }
            continue;
        }
        if (d < digits) {
            res += val[i];
            if (dig) {
                ++d;
            }
        }
    }
    
    if (removeZeros) {
        while (res.size() > 0 && res[res.size() - 1] == '0') {
            res.resize(res.size() - 1);
        }
    }
    
    if (res[res.size() - 1] == '.' || res[res.size() - 1] == ',') {
        res += "0";
    }
    return res;
}

/* reverse:  переворачиваем строку s на месте */
void reverse(char s[]) {
    uint i, j;
    char c;

    for (i = 0, j = strlen(s) - 1; i < j; ++i, --j) {
        c = s[i];
        s[i] = s[j];
        s[j] = c;
    }
}

void itoa(int n, char s[]) {
    int i, sign;

    if ((sign = n) < 0) { /* записываем знак */
        n = -n; /* делаем n положительным числом */
    }
    i = 0;
    do { /* генерируем цифры в обратном порядке */
        s[i++] = n % 10 + '0'; /* берем следующую цифру */
    } while ((n /= 10) > 0); /* удаляем */
    if (sign < 0) {
        s[i++] = '-';
    }
    s[i] = '\0';
    reverse(s);
}

string getConfVerion() {
    string result = "";
    int dot = 0;
    string ver = program_version;
    for (int i = 0; i < ver.size(); ++i) {
        if (ver[i] == '.') {
            ++dot;
        }
        if (dot > 2) {
            break;
        }
        result += ver[i];
    }
    return result;
}

void PrintExample() {
    // print to window
    cout << "version = \"" << getConfVerion() << "\"\n";
    cout << "name    = \"Configuration file for Margolus CA visualization\"\n";
    cout << endl;
    cout << "margolus = \"default\" /* default, quasi, quasix */\n";
    cout << endl;
    cout << "variables = ( { name = /* \"variablename\" */; values = [/* array of \"values\" */]; } /* , array of variables */\n";
    cout << "              /* in other blocks use {variablename} */\n";
    cout << "              );\n";
    cout << endl;
    cout << "mysqldatabase: {\n";
    cout << "    enable   = true; /* true, false - default disabled */\n";
    cout << "    server   = \"localhost\";\n";
    cout << "    user     = \"username\";\n";
    cout << "    password = \"password\";\n";
    cout << "    database = \"database\";\n";
    cout << "    table    = \"table\";\n";
    cout << "};\n";
    cout << endl;
    cout << "field: {\n";
    cout << "    status  = \"generate\"; /* generate, structure, file */\n";
    cout << "    structure = { name = \"name\"; type = \"x_y_z_r\"; path = \"field.txt\";\n";
    cout << "                /* type = x_y_z, x_y_z_r, x_y_z_name, x_y_-_mod, x_y_z_x */\n";
    cout << "                position = { sizeX = 0.5; sizeY = 0.5; sizeZ = 0.5;\n";
    cout << "                    location = \"center\"; }; }\n";
    cout << "    /* location - center, upX, downX, upY, downY, upZ, downZ */\n";
    cout << "    size     = { x = 1000; y = 100; z = 1; };\n";
    cout << "    multiply = { x = 1;    y = 1;   z = 1; };\n";
    cout << "    rotateOnMultiply = false;\n";
    cout << "    substances = (  { name = \"name\";   location = ({locate = \"bottomX\"; consist = 0.1;} /*, .. other locations */);\n";
    cout << "    /* consist may be declarate in variables */\n";
    cout << "    /* location - center, upX, downX, upY, downY, upZ, downZ */\n";
    cout << "                      type = \"ACTIVE\"; color = { r = 255; g = 0; b = 0; }; } /*, ... other substances */\n";
    cout << "                   /* type = ACTIVE, MODIFIER, SOLID */\n";
    cout << "                  \");\n";
    cout << "};\n";
    cout << endl;
    cout << "generation: {\n";
    cout << "    model = \"chess\"; /* random, chess, chess_l, chess_ms */\n";
    cout << "    position = { sizeX = 1.0; sizeY = 1.0; sizeZ = 1.0;\n";
    cout << "                    location = \"center\"; };\n";
    cout << "    /* location - center, upX, downX, upY, downY, upZ, downZ */\n";
    cout << "};\n";
    cout << endl;
    cout << "calculation: {\n";
    cout << "    show = true;\n";
    cout << "    statisticCalculationStep = 5;\n";
    cout << "    fieldSaveStep = 1000;\n";
    cout << "    directory = \"path\";\n";
    cout << "    /* all variables must be in directory path */\n";
    cout << "};\n";
    cout << endl;
    cout << "process: {\n";
    cout << "    T = \"{T}\";\n";
    cout << "    /* T may be declarate in variables */\n";
    cout << "    energy     = ( { name1 = \"name1\";  name2 = \"name2\"; H = \"{H}\"; S = \"{S}\";} /*, name1, name2, H, S */);\n";
    cout << "    energyCell = ( /* name1, name2, H, S */ );\n";
    cout << "    /* name1, name2 must be in field->substabces */\n";
    cout << "    /* H, S may be declarate in variables */\n";
    cout << "    modifierMove = false; /* default disabled */\n";
    cout << "    movement: { /* default disabled */\n";
    cout << "        flow = -500.0;\n";
    cout << "        move = \"+x\"; /* +x, -x, +y, -y, +z, -z */\n";
    cout << "    };\n";
    cout << "    tracked = \"active\"; /* must be in field->substabces */\n";
    cout << "    exit: {\n";
    cout << "        type = \"condition\"; /* type = iteration, condition */\n";
    cout << "        iteration = 1000;\n";
    cout << "        condition = 0.1;\n";
    cout << "        maxIteration = 2000000;\n";
    cout << "    };\n";
    cout << "};\n";
    cout << "clearBorders = [\"/* topX, bottomX, topY, bottomY, topZ, bottomZ, random (all) , ...*/\"];\n";
    cout << "pointsX = [/* array of points from 0 to 1, */];\n";
    cout << "statistics = ( { of=\"filename.stat\"; type=\"inside\"; } /*, ... */ );\n";
    cout << "/* type = inside, adsorbed */\n";
}

void PrintHalfOutput(cchar* fileName, double tick = 0.5) {
    string line = "";
    
    ifstream in;
    in.open(fileName);
    
    getline(in, line);
    vector<string> head = split(line, '\t');
    vector<int> maxValue;
    for (int i = 1; i < head.size(); ++i) {
        maxValue.push_back(stoi(head[i]));
    }

    int halfs[maxValue.size()];
for (int i = 0; i < maxValue.size(); i++)
halfs[i]=0;
    do {
        getline(in, line);
        head = split(line, '\t');
        if (line == "") {
            break;
        }
        int x = stoi(head[0]);
        for (int i = 1; i < head.size(); ++i) {
            int f = stoi(head[i]);
            if (f <= tick * maxValue[i - 1] && halfs[i - 1] == 0) {
                halfs[i - 1] = x;
            }
        }
    } while (!in.eof());
    in.close();
    for (int i = 0; i < maxValue.size(); ++i) {
        cout << halfs[i] << "\t";
    }
    cout << endl;
}

void PrintSurfaceSquare(cchar* fileName, cchar* sqSizes) {
    string s = sqSizes;
    string sizes = s.substr(8);
    cout << sizes << endl;
    vector<string> vs = split(sizes, '-');
    if (vs.size() != 3) {
        cerr << "Expect 3 sizes\n";
        return;
    }
    int x = stoi(vs[0]);
    int y = stoi(vs[1]);
    int z = stoi(vs[2]);
    CellularAutomata ca = CellularAutomata(x, y, z);
    
    ca.LoadFieldText(fileName);
    cout << ca.Layer() << endl;
}

#endif /* FUNCTIONS_H */
