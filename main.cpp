#include <iostream>
#include <iomanip>
#include <cmath>
#include <cstdlib>
#include <stdio.h>
#include <string.h>
#include <fstream>
//#include <png.h>
#include <vector>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h> 
#include <wchar.h>
#include <omp.h>
#include <mutex>
#include "generate.h"
#include "margolus.h"
#include "params.h"
#include "types.h"
#include "versioncheck.h"
//#include "mysqlconnect.h"
#include "functions.h"

#include <cereal/archives/json.hpp>
#include <cereal/access.hpp>


using namespace std;
//#define _WIN32

//#ifndef To_Win
#include <libconfig.h++>
using namespace libconfig;
//#endif

struct var {
    var(string name) : name(name) { }
    
    int*    intPtr    = nullptr;
    double* doublePtr = nullptr;
    string  name;
    string getName() {
        string result = "";
        for (uint i = 1; i < name.size() - 1; ++i) {
            result += name[i];
        }
        return result;
    }
    vector<string>  values;
};

struct varname {
    varname(string name, string value) : name(name), value(value) { }
    
    string name;
    string value;
};

struct varcalc {
    string str;
    string dir;
    vector<varname> varnames;
    
    string getValue(string name) {
        for (varname & vn : varnames) {
            if (vn.name == name) {
                return vn.value;
            }
        }
        return "";
    }
    
    string getName(string name) {
        string result = "";
        for (uint i = 1; i < name.size() - 1; ++i) {
            result += name[i];
        }
        return result;
    }
    
    string getTableNameSort() {
        vector<string> lst;
        for (varname & vn : varnames) {
            lst.push_back(getName(vn.name));
        }
        sort(lst.begin(), lst.end());
        string result = "";
        for (string & s : lst) {
            result += s;
        }
        return result;
    }
};

struct structure {
    string name;
    string type;
    string path;
};

enum exit_type {
    iteration,
    condition
};

exit_type ExitType(string value) {
    if (value == "iteration")   return iteration;
    if (value == "condition")   return condition;
    return iteration; // default
}

LOCATION GetLocation(string value) {
    if (value == "random")   return randomF;
    if (value == "topX")     return topX;
    if (value == "bottomX")  return bottomX;
    if (value == "topY")     return topY;
    if (value == "bottomY")  return bottomY;
    if (value == "topZ")     return topZ;
    if (value == "bottomZ")  return bottomZ;
    if (value == "adsorbed") return adsorbed;
    if (value == "outside")  return outside;
    return randomF; // default
}

SUBTYPE GetSubType(string value) {
    if (value == "ACTIVE")   return ACTIVE;
    if (value == "MODIFIER") return MODIFIER;
    if (value == "SOLID")    return SOLID;
    //return NONE;
}

enum stat_type {
    stat_adsorbed,
    stat_inside
};

struct statistics {
    string      of;
    stat_type   type;
    string      name;
    int         value = 0;
};

stat_type StatType (string value) {
    if (value == "adsorbed")  return stat_adsorbed;
    if (value == "inside")    return stat_inside;
    return stat_adsorbed; // default
}

template <class Type>
class VarPointer {
public:
    VarPointer(Type * p) : point(p) { }
    
    string  name;
    Type * point;
private:
};

margolus_model GetMargolusModel (string value) {
    if (value == "default")  return margolus_default;
    if (value == "quasi")    return margolus_quasi;
    if (value == "quasix")   return margolus_quasi_x;
    return margolus_default; // default
}

class Main {
public:
    
    bool saveFieldJsonFlag = true;
    uint totalSteps = 0;
    string saveDirectory;
    
    
    Main() { }
    ~Main() { delete defCAM; }
    
    void AddPrint() {
        printCA = true;
    }
    
    void AddExtra() {
        extra = true;
    }
    
    void AddRetention() {
        retention = true;
    }
    
    bool Init(cchar* settingFile) {
        // will be check configuration file
        bool goodLoad = true;
        Config cfg;
        // Прочитать файл. Или выйти с ошибкой
        // Класс в С++ не возвращает ошибку, а кидает исключение
        try {
            cfg.readFile(settingFile);
        } catch (const FileIOException &fioex) {
            cerr << "I/O error while reading file. Or file " << settingFile << " not fount!\n";
            goodLoad = false;
            return (EXIT_FAILURE);
        } catch (const ParseException &pex) {
            cerr << "Parse error at " << pex.getFile() << ":" << pex.getLine()
                    << " - " << pex.getError() << endl;
            goodLoad = false;
            return (EXIT_FAILURE);
        }

//        try {
//            string version = cfg.lookup("version");
//            cout << "Configuration version: " << version << endl;
//            int checker = -1;
//            checkConfigVersion(version, checker);
//            switch (checker) {
//                case 0:
//                    cerr << "Old program version or new config version!\n";
//                    cerr << "program version: " << program_version << endl;
//                    cerr << "config  version: " << version << endl;
//                    cerr << "Application break\n";
//                    goodLoad = false;
//                    return EXIT_FAILURE;
//                    break;
//                case 1:
//                    cerr << "Old configuration\n";
//                    cerr << "program version: " << program_version << endl;
//                    cerr << "config  version: " << version << endl;
//                    cerr << "Application continue load\n";
//                    break;
//                case 2:
//                    // all ok!
//                    break;
//                default:
//                    cerr << "Error " << checker << " check version\n";
//                    cerr << "program version: " << program_version << endl;
//                    cerr << "config  version: " << version << endl;
//                    goodLoad = false;
//                    break;
//            }
//        } catch (const SettingNotFoundException &nfex) {
//            cerr << "No 'version' setting in configuration file.\n";
//            goodLoad = false;
//        }
        
        try {
            string name = cfg.lookup("name");
            cout << "About application: this is " << name << endl;
        } catch (const SettingNotFoundException &nfex) {
            cerr << "No 'name' setting in configuration file.\n";
            goodLoad = false;
        }
        
        try {
            string smodel = cfg.lookup("margolus");
            model = GetMargolusModel(smodel);
        } catch (const SettingNotFoundException &nfex) {
            cerr << "No 'margolus' setting in configuration file.\n";
            goodLoad = false;
        }
        
        const Setting& root = cfg.getRoot();
        
        //load variables
        try {
            const Setting &variables = root["variables"];
            uint count = variables.getLength();
            string name;
            
            for (uint i = 0; i < count; ++i) {
                const Setting &variable = variables[i];
                variable.lookupValue("name", name);
                string shs = " 0123456789№-.,!><`'@%*()[]{}/~&$^+=#\"\\?:;\t\n";
                for (uint j = 0; j < shs.size(); j++) {
                    name = replace(name, shs[j], "");
                }
                //name = trim(name);
                if (name.length() < 1) {
                    cerr << "Error read variable name.\n";
                    return EXIT_FAILURE;
                }

                var v("{" + name + "}");
                
                uint cnt = variable["values"].getLength();
                for (uint i = 0; i < cnt; ++i) {
                    string value = variable["values"][i];
                    v.values.push_back(value);
                }
                vars.push_back(v);
            }
        } catch (const SettingNotFoundException &nfex) {
            cerr << "No 'variables' setting in configuration file.\n";
            goodLoad = false;
        }
        cout << "variable read ...\t done\n";
        {
            bool checkOK = true;
            //check if values count = 0
            for (var & v : vars) {
                if (v.values.size() == 0) {
                    checkOK = false;
                }
            }
            if (!checkOK) {
                cerr << "Some of values count in variables = 0!\n";
                return EXIT_FAILURE;
            }
            // check duplicate
            for (uint i = 0; i < vars.size(); ++i) {
                for (uint j = i + 1; j < vars.size(); ++j) {
                    if (vars[i].name == vars[j].name) {
                        checkOK = false;
                    }
                }
            }
            if (!checkOK) {
                cerr << "Dublicate variable names\n";
                return EXIT_FAILURE;
            }
        }
        
        try {
            const Setting &database = root["mysqldatabase"];
            if (database.lookupValue("enable", useMySQL)) {
//                if (useMySQL) {
//                    connectDetails connects;
//                    if (database.lookupValue("server", connects.server) &&
//                            database.lookupValue("user", connects.user) &&
//                            database.lookupValue("password", connects.password) &&
//                            database.lookupValue("database", connects.database) &&
//                            database.lookupValue("table", connects.table)) {
//                        mySQL = new MySQL(connects);
//                        if (mySQL->ConnectTest().GetStatus() != SQL_OK) {
//                            cerr << "MySQL connection Error!\n";
//                            useMySQL = false;
//                        }
//                    }
//                }
            } else {
                useMySQL = false;
            }
        } catch (const SettingNotFoundException &nfex) {
            cerr << "No 'mysqldatabase' connection setting in configuration file.\n";
            useMySQL = false;
        }
        cout << "mysqldatabase read ...\t done\n";
        //load field parameters
        try {
            const Setting &size = root["field"]["size"];
            const Setting &multi = root["field"]["multiply"];
            const Setting &fields = root["field"];
            if (size.lookupValue("x", sizes.x)
                    && size.lookupValue("y", sizes.y)
                    && size.lookupValue("z", sizes.z)
                    && multi.lookupValue("x", multiply.x)
                    && multi.lookupValue("y", multiply.y)
                    && multi.lookupValue("z", multiply.z)) {
                if (sizes.x > 1 && sizes.y > 1 && sizes.z >= 1
                        && multiply.x >= 1 && multiply.y>= 1 && multiply.z >= 1) {
                    defCAM = new Margolus(sizes.x * multiply.x, sizes.y * multiply.y, sizes.z * multiply.z);
                } else {
                    cerr << "Bad miltiply and field sizes in 'field' setting in configuration file.\n";
                    goodLoad = false;
                }
            }
            fields.lookupValue("rotateOnMultiply", rotate);
            fields.lookupValue("status", status);
            
            // generator
            if (status == "generate") {
                try {
                    const Setting &generation = root["generation"];
                    // TODO
                    string value = "";
                    generation.lookupValue("model", value);
                    generator.type = GenerateType(value);
                    switch (generator.type) {
                        case gen_random:
                            // do nothing
                            break;
                        case gen_chess:
                            // do nothing
                            break;
                        case gen_chess_l:
                            // do nothing
                            break;
                        case gen_chess_ms:
                            // rewrite for Modifier!
                            break;
                        default:
                            cerr << "Error parsing 'model'.\n";
                            goodLoad = false;
                            break;
                    }
                    const Setting &cube = generation["position"];
                    float sizeX, sizeY, sizeZ;
                    cube.lookupValue("sizeX", sizeX);
                    cube.lookupValue("sizeY", sizeY);
                    cube.lookupValue("sizeZ", sizeZ);
                    if (sizeX <= 0 || sizeX > 1 || sizeY <= 0 || sizeY > 1 ||
                            sizeZ <= 0 || sizeZ > 1) {
                        cerr << "Error value of sizeX-Z.\n";
                        goodLoad = false;
                    }
                    generator.x = sizeX;
                    generator.y = sizeY;
                    generator.z = sizeZ;
                    string value2 = "";
                    cube.lookupValue("location", value2);
                    generator.cube_loc = CubeLocation(value2);
                } catch (const SettingNotFoundException &nfex) {
                    cerr << "No 'generation' setting in configuration file.\n";
                    goodLoad = false;
                }
            } else {
                const Setting &struc = root["field"]["structure"];
                struc.lookupValue("path", strctr.path);
                struc.lookupValue("name", strctr.name);
                struc.lookupValue("type", strctr.type);
                const Setting &cube = struc["position"];
                float sizeX, sizeY, sizeZ;
                cube.lookupValue("sizeX", sizeX);
                cube.lookupValue("sizeY", sizeY);
                cube.lookupValue("sizeZ", sizeZ);
                if (sizeX <= 0 || sizeX > 1 || sizeY <= 0 || sizeY > 1 ||
                        sizeZ <= 0 || sizeZ > 1) {
                    cerr << "Error value of sizeX-Z.\n";
                    goodLoad = false;
                }
                generator.x = sizeX;
                generator.y = sizeY;
                generator.z = sizeZ;
                string value2 = "";
                cube.lookupValue("location", value2);
                generator.cube_loc = CubeLocation(value2);
            }
            
            const Setting &substances = root["field"]["substances"];
            int count = substances.getLength();
            
            for (uint i = 0; i < count; ++i) {
                const Setting &sub = substances[i];
                string name, types, texture;//, location;
                XColor color;
                vector<locate> locations;
                if (sub.lookupValue("name", name)
                        && sub.lookupValue("type",  types)
                        //&& sub.lookupValue("color", color)
                        //&& sub.lookupValue("texture", texture) // not need!
                        ) {
                    // TODO ? =(
                    const Setting &col = sub["color"];
                    col.lookupValue("r", color.r);
                    col.lookupValue("g", color.g);
                    col.lookupValue("b", color.b);
                    
                    SUBTYPE type = GetSubType(types);
                    //if (type != NONE)
                    const Setting &loc = sub["location"];
                    uint cnt = loc.getLength();
                    for (uint i = 0; i < cnt; ++i) {
                        double consist;
                        double* cons;
                        if (loc[i].lookupValue("consist", consist)) {
                            cons = new double(consist);
                        } else {
                            string varconsist;
                            loc[i].lookupValue("consist", varconsist);
                            if (!IsNameInVariables(cons, varconsist)) {
                                cerr << varconsist << " not found in variables!\n";
                                return EXIT_FAILURE;
                            }
                        }
                        locations.push_back(locate(GetLocation(loc[i]["locate"]), cons));
                    }
                    pSub sub;
                    switch (type) {
                        case ACTIVE:
                            sub = new Active(name, color, locations);
                            break;
                        case MODIFIER:
                            sub = new Modifier(name, color, locations);
                            break;
                        case SOLID:
                            sub = new Solid(name, color, locations);
                            break;
                    }
                    //sub->texture = texture;
                    iSubs.push_back(sub);
                } else {
                    cerr << "Empty or lost substances parameters in 'field' setting in configuration file.\n";
                    goodLoad = false;
                }
            }
        } catch (const SettingNotFoundException &nfex) {
            cerr << "No 'field' setting in configuration file.\n";
            goodLoad = false;
        }
        cout << "field read ...\t done\n";
        try {
            const Setting &calculation = root["calculation"];
            calculation.lookupValue("show", showConsole);
            calculation.lookupValue("statisticCalculationStep", calcStep);
            calculation.lookupValue("fieldSaveStep", saveStep);
            calculation.lookupValue("directory", saveCatalog);

            if (saveCatalog == "") {
                goodLoad = false;
            } else {
                //mkdir(saveCatalog.c_str(), 0777);
                //cout << "mkdir?" << mkdir(saveCatalog.c_str(), 0777) << endl;
                //saveCatalog += "//";
            }
            //int mkdir(const char *pathname, mode_t mode);
        } catch (const SettingNotFoundException &nfex) {
            cerr << "No 'calculation' setting in configuration file.\n";
            goodLoad = false;
        }

        if (!goodLoad) {
            return EXIT_FAILURE;
        }
        cout << "calculation read ...\t done\n";
        try {
            const Setting &processes = root["process"];
            string track = "";
            processes.lookupValue("tracked", track);
            tracked = -1;
            for (uint i = 0; i < iSubs.size(); ++i) {
                if (iSubs[i]->GetName() == track) {
                    tracked = i;
                }
            }
            //CAM->GetSubI(track);
            if (tracked == -1) {
                cerr << "Error tracked name in configuration file.\n";
                goodLoad = false;
            }
            try {
                const Setting &move = processes["movement"];
                double flow;
                double* pflow;
                if (move.lookupValue("flow", flow)) {
                    pflow = new double(flow);
                } else {
                    string varflow;
                    move.lookupValue("flow", varflow);
                    if (!IsNameInVariables(pflow, varflow)) {
                        cerr << varflow << " not found in variables!\n";
                        return EXIT_FAILURE;
                    }
                }
                defCAM->SetSteamPtr(pflow);
                
                string movement;
                move.lookupValue("move", movement);
                uint m1, m2;
                switch (movement[0]) {
                    case '-':
                        m1 = 1;
                        break;
                    case '+':
                        m1 = 2;
                        break;
                    default:
                        m1 = 0;
                        cerr << "Error sign in movement in configuration file.\n";
                        break;
                }
                switch (movement[1]) {
                    case 'x':
                        m2 = 0;
                        break;
                    case 'y':
                        m2 = (m1 == 0 ? 0 : 1);
                        break;
                    case 'z':
                        m2 = (m1 == 0 ? 0 : 2);
                        break;
                    default:
                        m1 = 0;
                        m2 = 0;
                        cerr << "Error axe in movement in configuration file.\n";
                        break;
                }
                if (defCAM->GetSizeZ() == 1 && m2 == 2) {
                    cerr << "Error axe Z in movement in configuration file.\n";
                }
                defCAM->movement = m1 + m2 * 2;
            } catch (const SettingNotFoundException &nfex) {
                defCAM->SetSteamEnergy(0.0f);
                defCAM->movement = 0;
                cerr << "Use default flow movement.\n";
            }
            // exit
            try {
                const Setting &exit = processes["exit"];
                string value = "";
                exit.lookupValue("type", value);
                exit.lookupValue("maxIteration", iterationX);
                et = ExitType(value);
                switch (et) {
                    case iteration:
                        exit.lookupValue("iteration", lastIteration);
                        break;
                    case condition:
                        exit.lookupValue("condition", exitCondition);
                        break;
                }
            } catch (const SettingNotFoundException &nfex) {
                cerr << "Error in exit settings.\n";
                goodLoad = false;
            }
            int cnt_pointsX = root["pointsX"].getLength();

            for (uint i = 0; i < cnt_pointsX; ++i) {
                float p = root["pointsX"][i];
                points.push_back((p * defCAM->GetSizeX()));
            }
            
            // TODO ENERGY!!!!

            const Setting &energies = root["process"]["energy"];
            uint cnt = energies.getLength();

            for (uint i = 0; i < cnt; ++i) {
                const Setting &en = energies[i];

                string name1, name2;//, H, S;//, location;
                if (en.lookupValue("name1", name1)
                        && en.lookupValue("name2", name2)) {
                    // TODO ? =(
                    double H = 0.0;
                    double* pH;
                    double S = 0.0;
                    double* pS;
                    
                    if (en.lookupValue("H", H)) {
                        pH = new double(H);
                    } else {
                        string varH;
                        en.lookupValue("H", varH);
                        if (!IsNameInVariables(pH, varH)) {
                            cerr << varH << " not found in variables!\n";
                            return EXIT_FAILURE;
                        }
                    }
                    if (en.lookupValue("S", S)) {
                        pS = new double(S);
                    } else {
                        string varS;
                        en.lookupValue("S", varS);
                        if (!IsNameInVariables(pS, varS)) {
                            cerr << varS << " not found in variables!\n";
                            return EXIT_FAILURE;
                        }
                    }
                    defCAM->AddEnergy(name1, name2, pH, pS);
                } else {
                    cerr << "Empty or lost energy parameters in 'process' setting in configuration file.\n";
                    goodLoad = false;
                }
            }
            
            const Setting &energiesCell = root["process"]["energyCell"];
            uint cntCell = energiesCell.getLength();

            for (uint i = 0; i < cntCell; ++i) {
                const Setting &en = energiesCell[i];

                string name1, name2;//, H, S;//, location;
                if (en.lookupValue("name1", name1)
                        && en.lookupValue("name2", name2)) {

                    double H = 0.0;
                    double* pH;
                    double S = 0.0;
                    double* pS;
                    
                    if (en.lookupValue("H", H)) {
                        pH = new double(H);
                    } else {
                        string varH;
                        en.lookupValue("H", varH);
                        if (!IsNameInVariables(pH, varH)) {
                            cerr << varH << " not found in variables!\n";
                            return EXIT_FAILURE;
                        }
                    }
                    if (en.lookupValue("S", S)) {
                        pS = new double(S);
                    } else {
                        string varS;
                        en.lookupValue("S", varS);
                        if (!IsNameInVariables(pS, varS)) {
                            cerr << varS << " not found in variables!\n";
                            return EXIT_FAILURE;
                        }
                    }
                    defCAM->AddEnergyCell(name1, name2, pH, pS);
                } else {
                    cerr << "Empty or lost energy parameters in 'process' setting in configuration file.\n";
                    goodLoad = false;
                }
            }
            double T;
            double* pT;
            
            if (root["process"].lookupValue("T", T)) {
                pT = new double(T);
            } else {
                string varT;
                root["process"].lookupValue("T", varT);
                if (!IsNameInVariables(pT, varT)) {
                    cerr << varT << " not found in variables!\n";
                    return EXIT_FAILURE;
                }
            }
            
            defCAM->SetTempPtr(pT);
            try {
                root["process"].lookupValue("modifierMove", modifierMove);
            } catch (...) {
                cout << "modifier move disabled\n";
                modifierMove = false;
            }
        } catch (const SettingNotFoundException &nfex) {
            cerr << "No 'process' setting in configuration file.\n";
            goodLoad = false;
        }
        cout << "process read ...\t done\n";
        //borders
        int cnt = root["clearBorders"].getLength();
        for (uint i = 0; i < cnt; ++i) {
            borders.push_back(GetLocation(root["clearBorders"][i]));
        }
        
        try {
            const Setting &stat = root["statistics"];
            
            uint stat_cnt = stat.getLength();
            bool checkSQL = false;
            for (uint i = 0; i < stat_cnt; ++i) {
                const Setting &st = stat[i];
                string stat_of, stat_type;
                if (st.lookupValue("of", stat_of) && st.lookupValue("type", stat_type)) {
                    statistics stt;
                    stt.of   = stat_of;
                    stt.type = StatType(stat_type);
                    if (useMySQL && stt.type == stat_inside) {
                        checkSQL = true;
                        outpath = stt.of;
                    }
                    stt.name = stat_type;
                    stats.push_back(stt);
                } else {
                    cerr << "Empty or lost parameters in 'statistics'.\n";
                    goodLoad = false;
                }
            }
//            if (useMySQL && !checkSQL) {
//                cerr << "Empty or lost inside parameter in 'statistics'.\n";
//                goodLoad = false;
//            }
        } catch (const SettingNotFoundException &nfex) {
            cerr << "No 'statistics' setting in configuration file.\n";
            goodLoad = false;
        }
        // fill pathes with variables
        {
            for (var & v : vars) {
                if (!VarExist(saveCatalog, v.name)) {
                    cerr << "Check save catalog to " << v.name << " variable!\n";
                    return EXIT_FAILURE;
                }
            }
            /*if (status != "generate") {
                for (var & v : vars) {
                    if (!VarExist(strctr.path, v.name)) {
                        cerr << "Check structure path to variables!\n";
                        return EXIT_FAILURE;
                    }
                }
            }*/
            
            uint count = 1;
            for (var & v : vars) {
                count *= v.values.size();
            }
            varcalcs.clear();
            for (uint i = 0; i < count; ++i) {
                varcalc vc;
                for (var & v : vars) {
                    vc.varnames.push_back(varname(v.name, v.name));
                }
                vc.dir = saveCatalog;
                if (status == "generate") {
                    vc.str = "";
                } else {
                    vc.str = strctr.path;
                }
                varcalcs.push_back(vc);
            }
            
            uint cp = 1;
            for (var & v : vars) {
                uint c = v.values.size();
                for (uint i = 0; i < varcalcs.size(); ++i) {
                    varcalcs[i].dir = VarReplace(varcalcs[i].dir, v.name, (i / cp) % c);
                    varcalcs[i].str = VarReplace(varcalcs[i].str, v.name, (i / cp) % c);
                }
                
                for (uint i = 0; i < varcalcs.size(); ++i) {
                    for (varname & vn : varcalcs[i].varnames) {
                        vn.value = VarReplace(vn.value, v.name, (i / cp) % c);
                    }
                }
                
                cp *= v.values.size();
            }
        }
        cout << "\nInit complete!\n\n";
        return goodLoad ? (EXIT_SUCCESS) : (EXIT_FAILURE);
    }
    
    bool VarExist(string full, string part) {
        for (uint i = 0; i < full.length(); ++i) {
            bool ok = true;
            uint j = 0;
            for (; j < part.size(); ++j) {
                if ((i + j >= full.length()) || full[i + j] != part[j]) {
                    ok = false;
                    break;
                }
            }
            if (!ok) {
                continue;
            }
            return true;
        }
        return false;
    }
    
    string VarReplace(string full, string part, int index) {
        string result = "";
        for (uint i = 0; i < full.length(); ++i) {
            if (full[i] == '{') {
                bool ok = true;
                uint j = 0;
                for (; j < part.size(); ++j) {
                    if (full[i + j] != part[j]) {
                        ok = false;
                        break;
                    }
                }
                
                if (ok) {
                    result += VarValue(part, index);
                    i += j - 1;
                } else {
                    result += full[i];
                }
            } else {
                result += full[i];
            }
        }
        return result;
    }
    
    string VarValue(string name, int index) {
        string result = "";
        for (var & v : vars) {
            if (v.name == name) {
                if (index < v.values.size()) {
                    return v.values[index];
                } else {
                    cerr << "Error 0 in variables!\n";
                }
            }
        }
        cerr << "Error 1 in variables!\n";
        return result;
    }
    
    void mkDir(string cpath) {
        string path = "";
        for (uint i = 0; i < cpath.length(); ++i) {
            if (cpath[i] != '/') {
                path += cpath[i];
            } else {
                ++i;
                if (i != cpath.length() && cpath[i] == '/') {
                    path += "//";
#ifdef To_Win
                    mkdir(path.c_str());
#else
                    mkdir(path.c_str(), 0777);
#endif   
                }
            }
        }
    }
    
    void Run() {
        // CA Margolus
        cout << "Run main program\n";
        cuint listcount = varcalcs.size();
        
        //chech first!!
        /*if (useMySQL) {
            //check if exist
            MySQLData data;
            string query = "select status, id from energy where "
                " Has = '" + varcalcs[0].getValue("Has")
                + "' and Sas = '" + varcalcs[0].getValue("Sas")
                + "' and Hms = '" + varcalcs[0].getValue("Hms")
                + "' and Sms = '" + varcalcs[0].getValue("Sms") + "'";
            mySQL->GetData(query, data);
            if (data.rows == 1) {
                if (data.GetData(0, 0) == "close") {
                    //if calculation done!
                    return;
                }
            } else {
                // new!
                string query = "Insert into energy (Has, Sas, Hms, Sms) values ('"
                        + varcalcs[0].getValue("Has") +"', '" + varcalcs[0].getValue("Sas") + "', '"
                        + varcalcs[0].getValue("Hms") +"', '" + varcalcs[0].getValue("Sms") + "')";
                mySQL->Execute(query);
            }
        }*/
        std::mutex mut;
        
#pragma omp parallel for num_threads(threads) shared (mut)
        for (uint listthis = 0; listthis < listcount; ++listthis) {
           
            //check first!
            //string idNote = "";
            if (useMySQL) {
//                string table = mySQL->GetTable();//varcalcs[0].getTableNameSort();
//                time_t rawtime;
//                time(&rawtime);
//                struct tm * timeinfo = localtime(&rawtime);
//                int year  = timeinfo->tm_year + 1900;
//                int month = timeinfo->tm_mon + 1;
//                int day   = timeinfo->tm_mday;
//                
//                if (table == "") {
//                    table = "table-" + to_string(year) + "-" + to_string(month)
//                            + "-" +  to_string(day);
//                    mySQL->SetTable(table);
//                }
//                // check table exist
//                {
//                    // if not exist - create table
//                    string query = "CREATE TABLE IF NOT EXISTS `" + table + "` ("
//                            "`id` int(11) NOT NULL AUTO_INCREMENT, ";
//                    if (retention) {
//                        query += "`H` double NOT NULL, "
//                                "`S` double NOT NULL, "
//                                "`T` double NOT NULL, "
//                                "`porosity` double NOT NULL, "
//                                "`flow` double NOT NULL, "
//                                "`summVol` int(11) NOT NULL, "
//                                "`solidVol` int(11) NOT NULL, "
//                                "`layerVol` int(11) NOT NULL, "
//                                "`adsorbVol` int(11) NOT NULL, "
//                                "`active` int(11) NOT NULL, "
//                                "`activeVol` int(11) NOT NULL, "
//                                "`K` double NOT NULL, ";
//                    } else if (extra) {
//                        query += "`H` double NOT NULL, "
//                                "`S` double NOT NULL, "
//                                "`T` double NOT NULL, "
//                                "`porosity` double NOT NULL, "
//                                "`flow` double NOT NULL, ";
//                        for (uint & p : points) {
//                            query += "`leng" + to_string(p) + "` double NOT NULL, ";
//                        }
//                    } else {
//                        for (var & v : vars) {
//                            query += "`" + v.getName() + "` double NOT NULL, ";
//                        }
//                        query += "`time` int(11) NOT NULL, ";
//                    }
//                    query += "PRIMARY KEY (`id`)"
//                            ") ENGINE=InnoDB AUTO_INCREMENT=1 DEFAULT CHARSET=utf8;";
//                    mut.lock();
//                    SQLResult res = mySQL->Execute(query);
//                    mut.unlock();
//                    if (res.GetStatus() != SQL_OK) {
//                        cerr << " MySQL error in create: " << res.GetMessage() << endl;
//                        continue;
//                    }
//                }
                
                /// old one
                
                /*string idQuery = "select `id` from `" + table + "` where ";
                for (uint i = 0; i < vars.size(); ++i) {
                    idQuery += "`" + vars[i].getName() + "` = '"
                            + varcalcs[listthis].getValue(vars[i].name)
                            + "' ";
                    if (i != vars.size() - 1) {
                        idQuery += " and ";
                    }
                }
                mut.lock();
                SQLResult res = mySQL->GetData(idQuery);
                mut.unlock();
                MySQLData data = res.GetData();
                if (res.GetStatus() != SQL_OK) {
                    cerr << " MySQL error: " << res.GetMessage() << endl;
                    continue;
                }*/
                /*if (data.GetRows() > 1) {
                    cerr << " Error! count more than 1!\n";
                    continue;
                }
                if (data.GetRows() == 1) {
                    cout << " already done!\n";
                    continue;
                }*/
                //idNote = data.GetData(0, 0);
            }
            cout << "Create directory\n";
            saveDirectory = varcalcs[listthis].dir + "//";
            mkDir(saveDirectory);
            // set values
            cout << "Set variables values\n";
            for (VarPointer<double> & vp : doubleVars) {
                for (varname & vn : varcalcs[listthis].varnames) {
                    if (vp.name == vn.name) {
                        double value = stod(vn.value);
                        *vp.point = value;//stod(vn.value);
                        break;
                    }
                }
            }
            for (VarPointer<int> & vp : intVars) {
                for (varname & vn : varcalcs[listthis].varnames) {
                    if (vp.name == vn.name) {
                        int value = stoi(vn.value);
                        *vp.point = value;
                        break;
                    }
                }
            }
            
            float teta = 0.0;
            // fill subs!
            cout << "Create new Cellular Automata\n";
            Margolus * CAM;
            switch (model) {
                case margolus_default:
                    CAM = new Margolus(defCAM->GetSizeX(), defCAM->GetSizeY(), defCAM->GetSizeZ());
                    break;
                case margolus_quasi:
                    CAM = new MargolusQuasi(defCAM->GetSizeX(), defCAM->GetSizeY(), defCAM->GetSizeZ());
                    break;
                case margolus_quasi_x:
                    CAM = new MargolusQuasiX(defCAM->GetSizeX(), defCAM->GetSizeY(), defCAM->GetSizeZ());
                    break;
            }
            //Margolus * CAM = new Margolus(defCAM->GetSizeX(), defCAM->GetSizeY(), defCAM->GetSizeZ());
            CAM->SetSteamPtr(new double(defCAM->GetSteamEnergy()));
            CAM->movement = defCAM->movement;
            // AddEnergy
            for (Energy & en : defCAM->GetEnergies()) {
                CAM->AddEnergy(en);
            }
            // AddEnergyCell
            for (Energy & en : defCAM->GetEnergiesCell()) {
                CAM->AddEnergyCell(en);
            }
            //clear subs!
            cout << "Set substances\n";
            {
                bool ok = true;
                CAM->ClearSubs();
                for (pSub & sub : iSubs) {
                    string name = sub->GetName();
                    SUBTYPE type = sub->GetType();
                    
                    XColor color = sub->GetColor();
                    vector<locate> locations;
                    // TODO
                    /*if (type == MODIFIER && status == "generate" && generator.type == gen_chess_ms) {
                        string S = varcalcs[listthis].getValue("Sms");
                        string H = varcalcs[listthis].getValue("Hms");
                        string C = varcalcs[listthis].getValue("Cmolfr");
                        if (S == "" || H == "" || C == "") {
                            cout << "Error S or H or C! WTF ?!\n";
                            ok = false;
                        }
                        if (atof(C.c_str()) > 1) {
                            cout << "Error! C value more than 1! WTF ?!\n";
                            ok = false;
                        }
                        if (!ok) {
                            break;
                        }
                        teta = Teta(atof(H.c_str()), atof(S.c_str()), atof(T.c_str()), atof(C.c_str()));
                        locations.push_back(locate(adsorbed, new double(teta)));
                        if (modifierMove) {
                            locations.push_back(locate(randomF, new double(atof(C.c_str()))));
                        }
                    } else {*/
                        locations = sub->GetLocations();
                    //}
                    
                    CAM->AddSub(type, name, color, locations);
                }
                if (!ok) {
                    delete CAM;
                    continue;
                }
                //CAM->GetSub(name)->texture = texture; not need
            }
            cout << "Set Temperature to \t" << defCAM->GetTemperature() << "\n";
            CAM->SetTempPtr(new double(defCAM->GetTemperature()));
            cout << "Update energies\n";
            CAM->UpdateEnergies();
            cout << "Set Empty field\n";
            CAM->SetEmptyField();
            CAM->SetMoveModifier(modifierMove);
            //unsigned int maxCount = CAM->GetSizeX() * CAM->GetSizeY() * CAM->GetSizeZ();
            if (status == "structure") {
                cout << "Gen structure\n";
                uint xw = (uint)(generator.x * CAM->GetSizeX());
                uint yw = (uint)(generator.y * CAM->GetSizeY());
                uint zw = (uint)(generator.z * CAM->GetSizeZ()); 
                uint x0 = (CAM->GetSizeX() - xw) / 2;
                uint y0 = (CAM->GetSizeY() - yw) / 2;
                uint z0 = (CAM->GetSizeZ() - zw) / 2;
                switch (generator.cube_loc) {
                    case cube_center:
                        break;
                    case cube_upX:
                        x0 = CAM->GetSizeX() - xw;
                        break;
                    case cube_downX:
                        x0 = 0;
                        break;
                    case cube_upY:
                        y0 = CAM->GetSizeY() - yw;
                        break;
                    case cube_downY:
                        y0 = 0;
                        break;
                    case cube_upZ:
                        z0 = CAM->GetSizeZ() - zw;
                        break;
                    case cube_downZ:
                        z0 = 0;
                        break;
                }
                Margolus SFile(xw, yw, zw);
                SFile.SetEmptyField();
                SFile.ImportSubs(CAM);
                generator.type = gen_nonsolid;
                if (strctr.type == "x_y_z") {
                    SFile.LoadFieldTextXYZ(varcalcs[listthis].str.c_str(), strctr.name);
                } else if (strctr.type == "x_y_z_r") {
                    SFile.LoadFieldTextXYZR(varcalcs[listthis].str.c_str(), strctr.name);
                } else if (strctr.type == "x_y_z_name") {
                    SFile.LoadFieldTextXYZN(varcalcs[listthis].str.c_str());
                } else if (strctr.type == "x_y_-_mod") {
                    generator.type = gen_active;
                    string sol_name = "", mod_name = "";
                    uint df = 0;
                    for (uint i = 0; i < strctr.name.size(); ++i) {
                        if (strctr.name[i] == ' ') {
                            ++df;
                        } else {
                            switch (df) {
                                case 0:
                                    sol_name += strctr.name[i];
                                    break;
                                case 1:
                                    mod_name += strctr.name[i];
                                    break;
                            }
                        }
                    }
                    SFile.LoadFieldTextXY_M(varcalcs[listthis].str.c_str(), sol_name, mod_name);
                } else if (strctr.type == "x_y_z_x") {
                    generator.type = gen_active;
                    SFile.LoadFieldTextXYZX(varcalcs[listthis].str.c_str());
                } else {
                    cerr << "Error structure type!\n";
                    delete CAM;
                    continue;
                }
                CAM->iteration = SFile.iteration;
                for (uint x = 0; x < SFile.GetSizeX(); ++x) {
                    for (uint y = 0; y < SFile.GetSizeY(); ++y) {
                        for (uint z = 0; z < SFile.GetSizeZ(); ++z) {
                            if (!SFile.GetCell(x, y, z).IsEmpty()) {
                                for (uint ix = 0; ix < multiply.x; ++ix) {
                                    for (uint iy = 0; iy < multiply.y; ++iy) {
                                        for (uint iz = 0; iz < multiply.z; ++iz) {
                                            CAM->SetCell(SFile.GetCell(x, y, z),
                                                    ix * sizes.x + x + x0,
                                                    iy * sizes.y + y + y0,
                                                    iz * sizes.z + z + z0);
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
                // clear start
                for (uint ix = 0; ix < 4; ++ix) {
                    for (uint iy = 0; iy < CAM->GetSizeY(); ++iy) {
                        for (uint iz = 0; iz < CAM->GetSizeZ(); ++iz) {
                            if (CAM->GetCell(ix, iy, iz).HaveSolid()) {
                                CAM->SetCell(Cell(), ix, iy, iz);
                            }
                        }
                    }
                }
                generator.Generation(CAM);
            }
            if (status == "file") {
                cout << "Gen from file\n";
                Margolus SFile(sizes.x, sizes.y, sizes.z);
                SFile.SetEmptyField();
                SFile.LoadFieldText(varcalcs[listthis].str.c_str());
                //SFile.LoadFieldTextOld(strctr.path.c_str());
                //SFile.LoadFieldBin(structureFile.c_str());
                CAM->iteration = SFile.iteration;
                // switch (code) {
                 //   case 0://bin
                 //       break;
                 //   case 1://text
                //        structure = readFieldFromFile(structurePath, sizes[0], sizes[1]);
                //        break;
                //}
                for (uint x = 0; x < SFile.GetSizeX(); ++x) {
                    for (uint y = 0; y < SFile.GetSizeY(); ++y) {
                        for (uint z = 0; z < SFile.GetSizeZ(); ++z) {
                            if (!SFile.GetCell(x, y, z).IsEmpty()) {
                                for (uint ix = 0; ix < multiply.x; ++ix) {
                                    for (uint iy = 0; iy < multiply.y; ++iy) {
                                        for (uint iz = 0; iz < multiply.z; ++iz) {
                                            CAM->SetCell(SFile.GetCell(x, y, z),
                                                    ix * sizes.x + x,
                                                    iy * sizes.y + y,
                                                    iz * sizes.z + z);
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            } 
            if (status == "generate") { //structure == "generate"
                cout << "Generation\n";
                CAM->iteration = 0;
                generator.Generation(CAM);
            }
            // fill Count
            fillCount = 0;
            for (uint ix = 0; ix < CAM->GetSizeX(); ++ix) {
                for (uint iy = 0; iy < CAM->GetSizeY(); ++iy) {
                    for (uint iz = 0; iz < CAM->GetSizeZ(); ++iz) {
                        for (pSub & sub : CAM->GetCell(ix, iy, iz).GetSubs()) {
                            if (sub->GetName() == CAM->GetSub(tracked)->GetName()) {
                                ++fillCount;
                            }
                        }
                    }
                }
            }
            if (printCA) {
                CAM->PrintParameters();
            }
            if (fillCount == 0) {
                cout << "No tracked substance in field!\n" << endl;
                continue;
            }
            CAM->finished = false;
            cout << "Run calculation\n";
            if (CAM->iteration == 0) {
                SaveFieldToFile(CAM, saveDirectory, CAM->iteration);
                if (showConsole) {
                    cout << (listthis + 1) << " >> " << listcount;
                }
                SaveStatistics(CAM, saveDirectory, false);
                Save2dImageToFile(CAM, saveDirectory, CAM->iteration, CAM->GetSizeZ() / 2);
                
                
            }
            //calculation
            // 2D
            CAM->path = saveDirectory;
            CAM->saveJsonFlag = saveFieldJsonFlag;
            
            if (CAM->GetSizeZ() == 1) {
                while (!CAM->finished) {
                    
                    /* add mod */
                    if (modifierMove) {
                        for (pSub & sub : CAM->GetSubs()) {
                            if (sub->GetType() == MODIFIER) {
                                uint d = 0;
                                for (uint iy = 0; iy < CAM->GetSizeY(); ++iy) {
                                    if (CAM->GetCell(0, iy).HaveModifier()) {
                                        ++d;
                                    }
                                }
                                while (d < (double)sub->GetFillCount() / CAM->GetSizeX()) {
                                    int iy = rand() % (CAM->GetSizeY());
                                    if (!CAM->GetCell(0, iy, 0).HaveModifier()
                                            && CAM->AddToCell(sub, 0, iy, 0)) {
                                        ++d;
                                    }
                                }
                            }
                        }
                    }
                    switch (1) {
                        case 0:
                            CAM->Calculation(0, 0);
                            CAM->Calculation(1, 1);
                            break;
                        case 1:
                            CAM->subIteration = 0;
                            CAM->saveFlag = CAM->iteration % saveStep == 0;
                            
                            CAM->Calculation(0, 0);
                            CAM->Calculation(0, 1);
                            CAM->Calculation(1, 0);
                            CAM->Calculation(1, 1);
                            CAM->Calculation(0, 0);
                            CAM->Calculation(1, 0);
                            CAM->Calculation(0, 1);
                            CAM->Calculation(1, 1);
                            break;
                        default:
                            break;
                    }
                    switch (model) {
                        case margolus_quasi:
                            static_cast<MargolusQuasi*>(CAM)->QuasiCalculation(0, 0);
                            static_cast<MargolusQuasi*>(CAM)->QuasiCalculation(0, 1);
                            static_cast<MargolusQuasi*>(CAM)->QuasiCalculation(1, 0);
                            static_cast<MargolusQuasi*>(CAM)->QuasiCalculation(1, 1);
                            static_cast<MargolusQuasi*>(CAM)->QuasiCalculation(0, 0);
                            static_cast<MargolusQuasi*>(CAM)->QuasiCalculation(1, 0);
                            static_cast<MargolusQuasi*>(CAM)->QuasiCalculation(0, 1);
                            static_cast<MargolusQuasi*>(CAM)->QuasiCalculation(1, 1);
                            break;
                        case margolus_quasi_x:
                            static_cast<MargolusQuasiX*>(CAM)->QuasiCalculation();
                            break;
                    }
                    CAM->iteration++;
                    //clear borders
                    if (!retention) {
                        ClearBorders(CAM);
                    }
                    if (CAM->iteration % calcStep == 0) {
                        if (showConsole) {
                            cout << (listthis + 1) << " >> " << listcount;
                        }
                        SaveStatistics(CAM, saveDirectory, true);
                    }

                    if (CAM->iteration % saveStep == 0) {
                        SaveFieldToFile(CAM, saveDirectory, CAM->iteration);
                        if (!saveFieldJsonFlag)
                            Save2dImageToFile(CAM, saveDirectory, CAM->iteration, CAM->GetSizeZ() / 2);
                    }
                    if (retention) {
                        // check to exit
                        switch (CAM->movement) {
                            case 0:
                                break;
                            case 1: // -x
                                for (int iy = 0; iy < CAM->GetSizeY(); ++iy) {
                                    for (int iz = 0; iz < CAM->GetSizeZ(); ++iz) {
                                        if (CAM->GetCell(0, iy, iz).HaveActive()) {
                                            CAM->finished = true;
                                            break;
                                        }
                                    }
                                }
                                break;
                            case 2: // +x
                                for (int iy = 0; iy < CAM->GetSizeY(); ++iy) {
                                    for (int iz = 0; iz < CAM->GetSizeZ(); ++iz) {
                                        if (CAM->GetCell(CAM->GetSizeX() - 1, iy, iz).HaveActive()) {
                                            CAM->finished = true;
                                            break;
                                        }
                                    }
                                }
                                break;
                            case 3: // -y
                                for (int ix = 0; ix < CAM->GetSizeX(); ++ix) {
                                    for (int iz = 0; iz < CAM->GetSizeZ(); ++iz) {
                                        if (CAM->GetCell(ix, 0, iz).HaveActive()) {
                                            CAM->finished = true;
                                            break;
                                        }
                                    }
                                }
                                break;
                            case 4: // +y
                                for (int ix = 0; ix < CAM->GetSizeX(); ++ix) {
                                    for (int iz = 0; iz < CAM->GetSizeZ(); ++iz) {
                                        if (CAM->GetCell(ix, CAM->GetSizeY() - 1, iz).HaveActive()) {
                                            CAM->finished = true;
                                            break;
                                        }
                                    }
                                }
                                break;
                                // -z && +z skipped
                            default:
                                break;
                        }
                    }
                    
                    // shared_ptr<Margolus> my_ptr(CAM);
                   // output( my_ptr);
                    //my_ptr.reset();
                    //os.close();
                }
            } else {
                while (!CAM->finished) {
                    
                    /* add mod */
                    if (modifierMove) {
                        for (pSub & sub : CAM->GetSubs()) {
                            if (sub->GetType() == MODIFIER) {
                                uint d = 0;
                                for (uint iy = 0; iy < CAM->GetSizeY(); ++iy) {
                                    for (uint iz = 0; iz < CAM->GetSizeZ(); ++iz) {
                                        if (CAM->GetCell(0, iy, iz).HaveModifier()) {
                                            ++d;
                                        }
                                    }
                                }
                                while (d < (double)sub->GetFillCount() / CAM->GetSizeX()) {
                                    int iy = rand() % (CAM->GetSizeY());
                                    int iz = rand() % (CAM->GetSizeZ());
                                    if (!CAM->GetCell(0, iy, iz).HaveModifier()
                                            && CAM->AddToCell(sub, 0, iy, iz)) {
                                        ++d;
                                    }
                                }
                            }
                        }
                    }
                    // TODO!
                    CAM->Calculation(0, 0, 0);
                    CAM->Calculation(1, 1, 1);
                    switch (model) {
                        case margolus_quasi:
                            dynamic_cast<MargolusQuasi*>(CAM)->QuasiCalculation(0, 0, 0);
                            dynamic_cast<MargolusQuasi*>(CAM)->QuasiCalculation(1, 1, 1);
                            break;
                        case margolus_quasi_x:
                            dynamic_cast<MargolusQuasiX*>(CAM)->QuasiCalculation();
                            break;
                    }
                    // end TODO
                    CAM->iteration++;
                    if (!retention) {
                        ClearBorders(CAM);
                    }
                    if (CAM->iteration % calcStep == 0) {
                        if (showConsole) {
                            cout << (listthis + 1) << " >> " << listcount;
                        }
                        SaveStatistics(CAM, saveDirectory, true);
                    }

                    if (CAM->iteration % saveStep == 0) {
                        SaveFieldToFile(CAM, saveDirectory, CAM->iteration);
                        Save2dImageToFile(CAM, saveDirectory, CAM->iteration, CAM->GetSizeZ() / 2);
                    }
                    if (retention) {
                        // check to exit
                        switch (CAM->movement) {
                            case 0:
                                break;
                            case 1: // -x
                                for (int iy = 0; iy < CAM->GetSizeY(); ++iy) {
                                    for (int iz = 0; iz < CAM->GetSizeZ(); ++iz) {
                                        if (CAM->GetCell(0, iy, iz).HaveActive()) {
                                            CAM->finished = true;
                                            break;
                                        }
                                    }
                                }
                                break;
                            case 2: // +x
                                for (int iy = 0; iy < CAM->GetSizeY(); ++iy) {
                                    for (int iz = 0; iz < CAM->GetSizeZ(); ++iz) {
                                        if (CAM->GetCell(CAM->GetSizeX() - 1, iy, iz).HaveActive()) {
                                            CAM->finished = true;
                                            break;
                                        }
                                    }
                                }
                                break;
                            case 3: // -y
                                for (int ix = 0; ix < CAM->GetSizeX(); ++ix) {
                                    for (int iz = 0; iz < CAM->GetSizeZ(); ++iz) {
                                        if (CAM->GetCell(ix, 0, iz).HaveActive()) {
                                            CAM->finished = true;
                                            break;
                                        }
                                    }
                                }
                                break;
                            case 4: // +y
                                for (int ix = 0; ix < CAM->GetSizeX(); ++ix) {
                                    for (int iz = 0; iz < CAM->GetSizeZ(); ++iz) {
                                        if (CAM->GetCell(ix, CAM->GetSizeY() - 1, iz).HaveActive()) {
                                            CAM->finished = true;
                                            break;
                                        }
                                    }
                                }
                                break;
                            case 5: // -z
                                for (int ix = 0; ix < CAM->GetSizeX(); ++ix) {
                                    for (int iy = 0; iy < CAM->GetSizeY(); ++iy) {
                                        if (CAM->GetCell(ix, iy, 0).HaveActive()) {
                                            CAM->finished = true;
                                            break;
                                        }
                                    }
                                }
                                break;
                            case 6: // +z
                                for (int ix = 0; ix < CAM->GetSizeX(); ++ix) {
                                    for (int iy = 0; iy < CAM->GetSizeY(); ++iy) {
                                        if (CAM->GetCell(ix, iy, CAM->GetSizeZ() - 1).HaveActive()) {
                                            CAM->finished = true;
                                            break;
                                        }
                                    }
                                }
                                break;
                            default:
                                break;
                        }
                    }
                   
                }
            }
            
            ofstream statisticsStream;
            statisticsStream.open(saveDirectory + "plots.txt", ios_base::trunc);
            
            uint fieldSize = CAM->GetVolume();
            
            for (int i = 0; i < CAM->statisticsData.size(); i++){
                StatisticsRecord sR = CAM->statisticsData.at(i);
                
                statisticsStream << sR.iteration << ",";
                statisticsStream << sR.solidCount << ",";
                statisticsStream << sR.boundaryLayerCount << ",";
                statisticsStream << sR.adsorbedCount << ",";
                statisticsStream << sR.activeCount << ",";
                statisticsStream << fieldSize << ",";
                
                int cChromotography = i > 0 ? CAM->statisticsData.at(i - 1).activeCount - sR.activeCount : 0;
                statisticsStream << cChromotography << ",";
                
                double cMobilePhase = (double)(sR.activeCount - sR.adsorbedCount) / (fieldSize - sR.solidCount - sR.boundaryLayerCount);
                double cSolidPhase = (double) ((double)sR.adsorbedCount / (double)sR.boundaryLayerCount);
                
                double K = cSolidPhase > 0 ? (double)(cMobilePhase / cSolidPhase)  : 0;
                
                statisticsStream << K;
                statisticsStream << "\n";
                
            }
            
            statisticsStream.close();
            
            
            // save last state
            //SaveFieldToFile(CAM, saveDirectory, CAM->iteration);
            //Save2dImageToFile(CAM, saveDirectory, CAM->iteration, CAM->GetSizeZ() / 2);
            if (useMySQL) {
//                string path = saveDirectory + "//" + outpath;
//                int times[points.size()];
//
//                int time = OutPickTime(CAM, path.c_str(), times);
//                cout << time << endl;
                // calculate out time
                //string table = mySQL->GetTable();
                //string query = "insert into `" + table + "` (";
//                string query = "insert into `` (";
//                if (retention) {
//                   
//                    query += "`H`, `S`, `T`, `porosity`, `flow`";
//                    query += ", `summVol`, `solidVol`, `layerVol`, `adsorbVol`, `active`, `activeVol`, `K`";
//                    query += ") values (";
//                    string ux = varcalcs[listthis].getValue("{H}");
//                    if (ux == "") {
//                        ux = "0";
//                    }
//                    query += "'" + ux + "'";
//                    ux = varcalcs[listthis].getValue("{S}");
//                    if (ux == "") {
//                        ux = "0";
//                    }
//                    query += ", '" + ux + "'";
//                    query += ", '" + to_string(CAM->GetTemperature()) + "'";
//                    query += ", '" + to_string(1 - (double)CAM->SolidCount() / CAM->GetVolume()) + "'";
//                    query += ", '" + to_string(CAM->GetSteamEnergy()) + "'";
//                    int ret[6] = { };
//                    //ret[0] // summVol
//                    //ret[1] // solidVol
//                    //ret[2] // layerVol
//                    //ret[3] // adsorbVol
//                    //ret[4] // active
//                    //ret[5] // activeVol
//                    CalcRetention(CAM, ret);
//                    double c_st = (double)ret[5] / (ret[0] - ret[1] - ret[2]);
//                    double c_m  = (double)ret[3] / ret[2];
//                    double k = c_st / c_m;
//                    for (int & p : ret) {
//                        query += ", '" + to_string(p) + "'";
//                    }
//                    query += ", '" + to_string(k) + "'";
//                    query += ")";
//                    
//                    std::ofstream os(saveDirectory + "out.json");
//                    {
//                        cereal::JSONOutputArchive archive(os); // stream to cout
//
//                        archive(
//                                cereal::make_nvp("H", varcalcs[listthis].getValue("{H}")),
//                                cereal::make_nvp("S", varcalcs[listthis].getValue("{S}")),
//                                cereal::make_nvp("T", to_string(CAM->GetTemperature())),
//                                cereal::make_nvp("porosity", to_string(1 - (double)CAM->SolidCount() / CAM->GetVolume()) ),
//                                cereal::make_nvp("flow", to_string(CAM->GetSteamEnergy()) ),
//                                
//                                cereal::make_nvp("summVol", to_string(ret[0]) ),
//                                cereal::make_nvp("solidVol", to_string(ret[1]) ),
//                                cereal::make_nvp("layerVol", to_string(ret[2]) ),
//                                cereal::make_nvp("adsorbVol", to_string(ret[3]) ),
//                                cereal::make_nvp("active", to_string(ret[4]) ),
//                                cereal::make_nvp("activeVol", to_string(ret[5])  ),
//                                cereal::make_nvp("K", to_string(k))
//                                );
//                    }
//                    
//                } else if (extra) {
//                    query += "`H`, `S`, `T`, `porosity`, `flow`";
//                    for (uint & p : points) {
//                        query += ", `leng" + to_string(p) + "`";
//                    }
//                    query += ") values (";
//                    string ux = varcalcs[listthis].getValue("{H}");
//                    if (ux == "") {
//                        ux = "0";
//                    }
//                    query += "'" + ux + "'";
//                    ux = varcalcs[listthis].getValue("{S}");
//                    if (ux == "") {
//                        ux = "0";
//                    }
//                    query += ", '" + ux + "'";
//                    query += ", '" + to_string(CAM->GetTemperature()) + "'";
//                    query += ", '" + to_string(1 - (double)CAM->SolidCount() / CAM->GetVolume()) + "'";
//                    query += ", '" + to_string(CAM->GetSteamEnergy()) + "'";
//                    for (int & p : times) {
//                        query += ", '" + to_string(p) + "'";
//                    }
//                    query += ")";
//                } else {
//                    for (var & v : vars) {
//                        query += "`" + v.getName() + "`, ";
//                    }
//                    query += " `time`) values (";
//                    for (var & v : vars) {
//                        query += "'" + varcalcs[listthis].getValue(v.name) + "', ";
//                    }
//                    query += "'" + to_string(time) + "')";
//                }
//                mut.lock();
//                SQLResult res = mySQL->Execute(query);
//                mut.unlock();
//                if (res.GetStatus() != SQL_OK) {
//                    cerr << " MySQL error: " << res.GetMessage() << endl;
//                    //continue;
//                }
            }
            std::ofstream os(saveDirectory + "out.json");
            {
                cereal::JSONOutputArchive archive(os); // stream to cout

                archive(
                        cereal::make_nvp("totalSteps", CAM->iteration)

                        );
            }
            delete CAM;
        }
    }

    void RunTest(cchar* param) {
        string fname = "test.out";
        string sparam = param;
        string inputs = sparam.substr(8);
        vector<string> vs = split(inputs, '-');
        if (vs.size() != 7) {
            cerr << "Expect 7 params\n";
            return;
        }
        int x;// = 400;
        int y;// = 400;
        int z = 1;
        int iter;// = 1000;
        int count;// = 1000;
        int gridtype;
        bool filemode; // true - new, false - append
        try {
            x = stoi(vs[0]);
            if (x < 1) {
                cerr << "x < 1\n";
                return;
            }
            y = stoi(vs[1]);
            if (y < 1) {
                cerr << "y < 1\n";
                return;
            }
            iter = stoi(vs[2]);
            if (iter < 1) {
                cerr << "Iteration count < 1\n";
                return;
            }
            count = stoi(vs[3]);
            if (count < 1) {
                cerr << "Calculation count < 1\n";
                return;
            }
            gridtype = stoi(vs[4]);
            if (gridtype < 0 || gridtype > 1) {
                cerr << "Error subiteration type (expect 0 or 1)\n";
                return;
            }
            int mType = stoi(vs[5]);
            switch (mType) {
                case 0:
                    model = margolus_default;
                    break;
                case 1:
                    model = margolus_quasi;
                    break;
                case 2:
                    model = margolus_quasi_x;
                    break;
                default:
                    model = margolus_default;
                    break;
            }
            if (vs[6] == "N") {
                filemode = true;
            } else if (vs[6] == "A") {
                filemode = false;
            } else {
                cerr << "Error file mode (N - new, A - append)\n";
                return;
            }
        } catch (...) {
            cerr << "Error parse params\n";
            return;
        }
        double steam = 0.0;
        double T = 310.0;
        double xr = 0.0;
        double yr = 0.0;
        int cnt = 0;
        int ** fld = new int*[x];
        for (int ix = 0; ix < x; ++ix) {
            fld[ix] = new int[y];
            for (int iy = 0; iy < y; ++iy) {
                fld[ix][iy] = 0;
            }
        }
        if (filemode) {
            // create new file
            ofstream out;
            out.open(fname.c_str(), ios_base::trunc);
            out.close();
        }
        for (int i = 0; i < count; ++i) {
            Margolus * CAM;
            switch (model) {
                case margolus_default:
                    CAM = new Margolus(x, y, z);
                    break;
                case margolus_quasi:
                    CAM = new MargolusQuasi(x, y, z);
                    break;
                case margolus_quasi_x:
                    CAM = new MargolusQuasiX(x, y, z);
                    break;
                default:
                    CAM = new Margolus(x, y, z);
                    break;
            }
            CAM->SetSteamPtr(new double(steam));
            CAM->movement = 0;
            //clear subs!
            string subname = "someAct";
            pSub sub = new Active(subname);
            CAM->ClearSubs();
            CAM->AddSub(sub);
            CAM->SetTempPtr(new double(T));
            CAM->UpdateEnergies();
            CAM->SetEmptyField();
            CAM->AddToCell(sub, x/2, y/2, z/2);

            CAM->SetMoveModifier(modifierMove);

            // fill Count
            fillCount = 0;
            for (uint ix = 0; ix < CAM->GetSizeX(); ++ix) {
                for (uint iy = 0; iy < CAM->GetSizeY(); ++iy) {
                    for (uint iz = 0; iz < CAM->GetSizeZ(); ++iz) {
                        for (pSub & sub : CAM->GetCell(ix, iy, iz).GetSubs()) {
                            if (sub->GetName() == subname) {
                                ++fillCount;
                            }
                        }
                    }
                }
            }
            if (printCA) {
                CAM->PrintParameters();
            }
            if (fillCount == 0) {
                cout << "No tracked substance in field!\n" << endl;
                return;
            }
            CAM->finished = false;
            cout << "Run calculation\n";
          
            //calculation
            // 2D
            if (CAM->GetSizeZ() == 1) {
                while (!CAM->finished) {
                    switch (gridtype) {
                        case 0:
                            CAM->Calculation(0, 0);
                            CAM->Calculation(1, 1);
                            break;
                        case 1:
                            CAM->Calculation(0, 0);
                            CAM->Calculation(0, 1);
                            CAM->Calculation(1, 0);
                            CAM->Calculation(1, 1);
                            CAM->Calculation(0, 0);
                            CAM->Calculation(1, 0);
                            CAM->Calculation(0, 1);
                            CAM->Calculation(1, 1);
                            break;
                        default:
                            break;
                    }
                    switch (model) {
                        case margolus_quasi:
                            static_cast<MargolusQuasi*>(CAM)->QuasiCalculation(0, 0);
                            static_cast<MargolusQuasi*>(CAM)->QuasiCalculation(0, 1);
                            static_cast<MargolusQuasi*>(CAM)->QuasiCalculation(1, 0);
                            static_cast<MargolusQuasi*>(CAM)->QuasiCalculation(1, 1);
                            static_cast<MargolusQuasi*>(CAM)->QuasiCalculation(0, 0);
                            static_cast<MargolusQuasi*>(CAM)->QuasiCalculation(1, 0);
                            static_cast<MargolusQuasi*>(CAM)->QuasiCalculation(0, 1);
                            static_cast<MargolusQuasi*>(CAM)->QuasiCalculation(1, 1);
                            break;
                        case margolus_quasi_x:
                            static_cast<MargolusQuasiX*>(CAM)->QuasiCalculation();
                            break;
                    }
                    CAM->iteration++;
                    /*if (CAM->iteration % 100 == 0) {
                        cout << i << " " << CAM->iteration << endl;
                    }*/
                    //clear borders
                    //ClearBorders(CAM);
                    if (CAM->iteration >= iter) {
                        CAM->finished = true;
                    }
                }
            } else {
                while (!CAM->finished) {
                    // TODO!
                    CAM->Calculation(0, 0, 0);
                    CAM->Calculation(1, 1, 1);
                    switch (model) {
                        case margolus_quasi:
                            dynamic_cast<MargolusQuasi*>(CAM)->QuasiCalculation(0, 0, 0);
                            dynamic_cast<MargolusQuasi*>(CAM)->QuasiCalculation(1, 1, 1);
                            break;
                        case margolus_quasi_x:
                            dynamic_cast<MargolusQuasiX*>(CAM)->QuasiCalculation();
                            break;
                    }
                    // end TODO
                    CAM->iteration++;
                    /*if (CAM->iteration % 100 == 0) {
                        cout << i << " " << CAM->iteration << endl;
                    }*/
                    //ClearBorders(CAM);
                    if (CAM->iteration >= iter) {
                        CAM->finished = true;
                    }
                }
            }
            ofstream out;
            out.open(fname.c_str(), ios_base::app);
            for (uint ix = 0; ix < CAM->GetSizeX(); ++ix) {
                for (uint iy = 0; iy < CAM->GetSizeY(); ++iy) {
                    for (uint iz = 0; iz < CAM->GetSizeZ(); ++iz) {
                        if (CAM->GetCell(ix, iy, iz).GetSubs().size() > 0) {
                            out << i << " " << ix << " " << iy << " " << iz << endl;
                            fld[ix][iy]++;
                            xr += (ix - x / 2) * (ix - x / 2);
                            yr += (iy - y / 2) * (iy - y / 2);
                            ++cnt;
                        }
                    }
                }
            }
            out.close();
            delete CAM;
        }
        Save2dFldToFile(fld, x, y);
        for (int ix = 0; ix < x; ++ix) {
            delete [] fld[ix];
        }
        delete [] fld;
        if (cnt == 0) {
            return;
        }
        cout << "xR = " << xr / cnt << endl;
        cout << "yR = " << yr / cnt << endl;
    }
    
private:
    void CalcRetention(Margolus * CAM, int * arr) {
        arr[0] = CAM->GetVolume(); // summVol
        arr[1] = CAM->SolidCount(); // solidVol
        arr[2] = CAM->Layer(); // layerVol
        arr[3] = CAM->Adsorbed(); // adsorbVol
        arr[4] = CAM->Count(ACTIVE) - CAM->Adsorbed(); // active
        arr[5] = arr[4] - arr[3]; // activeVol
        double c_st = (double)arr[5] / (arr[0] - arr[1] - arr[2]);
        double c_m  = (double)arr[3] / arr[2];
        double k = c_st / c_m;
        cout << " k is " << k << endl;
    }
    
    bool IsNameInVariables(double* &pointer, string variablename) {
        for (var & v : vars) {
            if (v.name == variablename) {
                if (v.doublePtr == nullptr || v.doublePtr == 0) {
                    // create pointer
                    v.doublePtr = new double(1.0);
                    VarPointer<double> point(v.doublePtr);
                    point.name = variablename;
                    doubleVars.push_back(point);
                }
                pointer = v.doublePtr;
                return true;
            }
        }
        return false;
    }
    
    bool IsNameInVariables(int* &pointer, string variablename) {
        for (var & v : vars) {
            if (v.name == variablename) {
                if (v.intPtr == nullptr || v.intPtr == 0) {
                    // create pointer
                    v.intPtr = new int(0);
                    VarPointer<int> point(v.intPtr);
                    point.name = variablename;
                    intVars.push_back(point);
                }
                pointer = v.intPtr;
                return true;
            }
        }
        return false;
    }
    
    int OutPickTime(Margolus * CAM, cchar* path, int * Ax) {
        int result = 0;
        double fr = 0.5;
        ifstream in;
        in.open(path);
        string s = "";
        string x = "";
        int idx = 0;
        uint count = CAM->GetSubCount();
        int outleng = (points.size() + 1) * count;
        while (!in.eof()) {
            in >> x;
            for (int i = 0; i < outleng; ++i) {
                in >> s;
                if (i == tracked) {
                    if (result == 0) {
                        int v = stoi(s);
                        if (v <= (int)(fr * fillCount)) {
                            result = stoi(x);
                        }
                    }
                }
                if (i == (tracked + (idx + 1) * count)) {
                    int v = stoi(s);
                    if (v <= (int)(fr * fillCount)) {
                        Ax[idx] = stoi(x);
                        ++idx;
                    }
                }
            }
            if (idx >= points.size()) {
                break;
            }
        }
        in.close();
        return result;
    }
    
    float Teta(float h, float s, float t, float c) {
        float R = 8.314;
        float lnK = h / (R * t) - s / R;
        float k = exp(-lnK);
        
        float teta = k * c / (1 + k * c);
        
        return teta;
    }
    
    void ClearBorders(Margolus * CAM) {
        for (LOCATION & border : borders) {
            switch (border) {
                case randomF:
                    CAM->ClearAllBorders();
                    break;
                case topX:
                    CAM->ClearUpX();
                    break;
                case bottomX:
                    CAM->ClearDownX();
                    break;
                case topY:
                    CAM->ClearUpY();
                    break;
                case bottomY:
                    CAM->ClearDownY();
                    break;
                case topZ:
                    CAM->ClearUpZ();
                    break;
                case bottomZ:
                    CAM->ClearDownZ();
                    break;
            }
        }
    }
    
    uint CountInside(Margolus * CAM) {
        uint result = 0;
        for (uint x = 0; x < CAM->GetSizeX(); ++x) {
            for (uint y = 0; y < CAM->GetSizeY(); ++y) {
                for (uint z = 0; z < CAM->GetSizeZ(); ++z) {
                    Cell cell = CAM->GetCell(x, y, z);
                    for (pSub & sub : cell.GetSubs()) {
                        if (sub->GetType() == ACTIVE) {
                            ++result;
                        }
                    }
                }
            }
        }
        return result;
    }
    
    void SaveStatistics(Margolus * CAM, string saveDirectory, bool add) {
        if (showConsole) {
            cout << ":\titer: " << CAM->iteration;
        }
        for (statistics & stat : stats) {
            switch (stat.type) {
                case stat_adsorbed:
                    stat.value = CalcStatisticAdsorbed(CAM);
                    SaveToFile(CAM->iteration, stat.value, add, saveDirectory, stat.of);
                    if (add) {
                        switch (et) {
                            case iteration:
                                if (showConsole) {
                                    cout << ">>" << lastIteration;
                                }
                                if (CAM->iteration >= lastIteration) {
                                    cout << " Done!\n";
                                    CAM->finished = true;
                                }
                                break;
                            case condition:
                                uint inside = CountInside(CAM);
                                if (showConsole) {
                                    cout << "\tinside: " << inside / (float) fillCount;
                                }
                                if (inside <= exitCondition) {
                                    cout << " Done!\n";
                                    CAM->finished = true;
                                }
                                break;
                        }
                    }
                    break;
                case stat_inside:
                    vector<uint> nextCount;
                    CountInField(CAM, nextCount);
                    stat.value = nextCount[tracked];
                    SaveToFile(CAM->iteration, nextCount, add, saveDirectory, stat.of);
                    if (add) {
                        switch (et) {
                            case iteration:
                                if (showConsole) {
                                    cout << ">>" << lastIteration;
                                }
                                if (CAM->iteration >= lastIteration) {
                                    cout << " Done!\n";
                                    CAM->finished = true;
                                }
                                break;
                            case condition:
                                if (showConsole) {
                                    cout << "\tinside: " << stat.value / (float) fillCount;
                                }
                                if (stat.value / (float) fillCount <= exitCondition) {
                                    cout << " Done!\n";
                                    CAM->finished = true;
                                }
                                break;
                        }
                    }
                    break;
            }
        }
        if (CAM->iteration >= iterationX) {
            cerr << "\nIteration limit is reached!\n";
            CAM->finished = true;
        }
        
        if (!CAM->finished && showConsole) {
            cout << endl;
        }
    }
    
    void CountInField(Margolus * CAM, vector<uint> &result) {
        //int[] result = new int[(points.length + 1) * (count + 1)];
        uint count = CAM->GetSubCount();
        for (uint i = 0; i < (points.size() + 1) * count; ++i) { //sizeof (points) / sizeof (int)
            result.push_back(0);
        }
        for (uint x = 0; x < CAM->GetSizeX(); ++x) {
            for (uint y = 0; y < CAM->GetSizeY(); ++y) {
                for (uint z = 0; z < CAM->GetSizeZ(); ++z) {
                    //TODO Проверить
                    Cell cell = CAM->GetCell(x, y, z);
                    for (uint i = 0; i < count; ++i) {
                        for (pSub & sub : cell.GetSubs()) {
                            if (sub->GetName() == CAM->GetSub(i)->GetName()) {
                                ++result[i];
                            }
                        }
                    }
                    for (uint k = 0; k < points.size(); ++k) {
                        if (x < points[k]) {
                            for (uint i = 0; i < count; ++i) {
                                for (pSub & sub : cell.GetSubs()) {
                                    if (sub->GetName() == CAM->GetSub(i)->GetName()) {
                                        result[(k + 1) * count + i]++;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    uint CalcStatisticAdsorbed(Margolus * CAM) {
        uint result = 0;
        for (uint ix = 0; ix < CAM->GetSizeX(); ++ix) {
            for (uint iy = 0; iy < CAM->GetSizeY(); ++iy) {
                for (uint iz = 0; iz < CAM->GetSizeZ(); ++iz) {
                    if (CAM->GetCell(ix, iy, iz).HaveActive()) {
                        if (ix != 0 && CAM->GetCell(ix - 1, iy, iz).HaveSolid()) {
                            ++result;
                        } else if (iy != 0 && CAM->GetCell(ix, iy - 1, iz).HaveSolid()) {
                            ++result;
                        } else if (iz != 0 && CAM->GetCell(ix, iy, iz - 1).HaveSolid()) {
                            ++result;
                        } else if (ix != CAM->GetSizeX() - 1 && CAM->GetCell(ix + 1, iy, iz).HaveSolid()) {
                            ++result;
                        } else if (iy != CAM->GetSizeY() - 1 && CAM->GetCell(ix, iy + 1, iz).HaveSolid()) {
                            ++result;
                        } else if (iz != CAM->GetSizeZ() - 1 && CAM->GetCell(ix, iy, iz + 1).HaveSolid()) {
                            ++result;
                        }
                    }
                }
            }
        }
        return result;
    }

    void SaveToFile(uint i, int result, bool add, string saveDirectory, string file) {
        ofstream out;
        if (add) {
            out.open(saveDirectory + "//" + file, ios_base::app);
        } else {
            out.open(saveDirectory + "//" + file, ios_base::trunc);
        }
        out << i << "\t" << result << endl;
        out.close();
    }

    void SaveToFile(cuint & i, const vector<uint> & result, const bool & add,
            string saveDirectory, string file) {
        ofstream out;
        if (add) {
            out.open(saveDirectory + "//" + file, ios_base::app);
        } else {
            out.open(saveDirectory + "//" + file, ios_base::trunc);
        }

        out << i;
        for (uint ii = 0; ii < result.size(); ++ii) {
            out << "\t" << result[ii];
        }
        out << endl;
        out.close();
    }

    void SaveFieldToFile(Margolus * CAM, string saveDirectory, cuint& i) {
        string fldPath = "field_";
        char cpath[33];
        itoa(i, cpath);
        string path = saveDirectory + fldPath + cpath + ".fsf";
        if (showConsole) {
            cout << "Saving field: '" << path;
        }
        CAM->SaveFieldText(path.c_str());
        //CAM->SaveFieldBin(path.c_str());
        if (showConsole) {
            cout << "' done!\n";
        }
    }
    
    void Save2dFldToFile(int ** fld, int x, int y) {
        uchar *img = NULL;
        if (img) {
            free(img);
        }
        int F = 4;
        img = (uchar *)malloc(F * x * y);
        int maxValue = 1;
        for (uint ix = 0; ix < x; ++ix) {
            for (uint iy = 0; iy < y; ++iy) {
                if (fld[ix][iy] > maxValue) {
                    maxValue = fld[ix][iy];
                }
            }
        }
        for (uint ix = 0; ix < x; ++ix) {
            for (uint iy = 0; iy < y; ++iy) {
                uint r = 0, g = 0, b = 0;
                
                int c = (fld[ix][iy] * 255) / maxValue;
                switch (c / 64) {
                    case 0:
                        g = (c % 64 * 255) / 64;
                        b = 255;
                        break;
                    case 1:
                        g = 255;
                        b = 255 - (c % 64 * 255) / 64;
                        break;
                    case 2:
                        r = (c % 64 * 255) / 64;
                        g = 255;
                        break;
                    case 3:
                        r = 255;
                        g = 255 - (c % 64 * 255) / 64;
                        break;
                }
                img[(ix + iy * x) * F + 0] = (uchar)(r);
                img[(ix + iy * x) * F + 1] = (uchar)(g);
                img[(ix + iy * x) * F + 2] = (uchar)(b);
                // PNG!
                img[(ix + iy * x) * F + 3] = (uchar)(255);
            }
        }

        string path = "map.png";
        //SavePNG(path.c_str(), img, x, y);
    }
    
    void SavePNG(cchar* path, uchar* imageData, int width, int height) {
//        FILE *f = fopen(path, "wb");
//        if (!f) {
//            printf("Can't open file! %s", path);
//            return;
//        }
//        png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
//        if (!png_ptr) {
//            fclose(f);
//            return;
//        }
//        png_infop png_info;
//        if (!(png_info = png_create_info_struct(png_ptr))) {
//            png_destroy_write_struct(&png_ptr, nullptr);
//            fclose(f);
//            return;
//        }
//        if (setjmp(png_jmpbuf(png_ptr))) {
//            png_destroy_write_struct(&png_ptr, nullptr);
//            fclose(f);
//            return;
//        }
//        png_init_io(png_ptr, f);
//        
//        png_set_IHDR(png_ptr, png_info, width, height, 8, PNG_COLOR_TYPE_RGB,
//            PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT,
//            PNG_FILTER_TYPE_DEFAULT);
//
//        uchar data[width * height * 3];
//        uchar *rows[height];
//
//        //uchar tempColors = 0;
//        //for (uint i = 0; i < width * height * 4; i += 4) {
//        //    tempColors = imageData[i + 1];
//        //    imageData[i + 1] = imageData[i + 3];
//        //    imageData[i + 3] = tempColors;
//        //}
//        for (uint i = 0; i < height; ++i) {
//            rows[height - i - 1] = data + (i * width * 3);
//            for (uint j = 0; j < width; ++j) {
//                int i1 = (i * width + j) * 3;
//                int i2 = (i * width + j) * 4;
//                data[i1++] = imageData[i2++];
//                data[i1++] = imageData[i2++];
//                data[i1++] = imageData[i2++];
//            }
//        }
//
//        png_set_rows(png_ptr, png_info, rows);
//        png_write_png(png_ptr, png_info, PNG_TRANSFORM_IDENTITY, nullptr);
//        png_write_end(png_ptr, png_info);
//
//        png_destroy_write_struct(&png_ptr, nullptr);
//
//        fclose(f);
    }
    
    void Save2dImageToFile(Margolus * CAM, string saveDirectory, cuint& i, cuint& iz = 0) {
        uchar *img = NULL;
        if (img) {
            free(img);
        }
        int F = 4;
        img = (uchar *)malloc(F * CAM->GetSizeX() * CAM->GetSizeY());

        for (uint ix = 0; ix < CAM->GetSizeX(); ++ix) {
            for (uint iy = 0; iy < CAM->GetSizeY(); ++iy) {
                uint c = 0, r = 0, g = 0, b = 0;
                Cell cell = CAM->GetCell(ix, iy, iz);
                c = cell.GetSubCount();
                if (c == 0) {
                    img[(ix + iy * CAM->GetSizeX()) * F + 0] = (uchar)(255);
                    img[(ix + iy * CAM->GetSizeX()) * F + 1] = (uchar)(255);
                    img[(ix + iy * CAM->GetSizeX()) * F + 2] = (uchar)(255);
                    // PNG!
                    img[(ix + iy * CAM->GetSizeX()) * F + 3] = (uchar)(255);
                    continue;
                }
                for (pSub & sub : cell.GetSubs()) {
                    r += CAM->GetSub(sub->GetName())->GetColor().r;
                    g += CAM->GetSub(sub->GetName())->GetColor().g;
                    b += CAM->GetSub(sub->GetName())->GetColor().b;
                }
                
                r /= c;
                g /= c;
                b /= c;
                
                img[(ix + iy * CAM->GetSizeX()) * F + 0] = (uchar)(r);
                img[(ix + iy * CAM->GetSizeX()) * F + 1] = (uchar)(g);
                img[(ix + iy * CAM->GetSizeX()) * F + 2] = (uchar)(b);
                // PNG!
                img[(ix + iy * CAM->GetSizeX()) * F + 3] = (uchar)(255);
            }
        }

        string imgPath = "img";
        char cpath[33];
        itoa(i, cpath);
        // BMP
        //string path = saveDirectory + imgPath + "it" + cpath + ".bmp";
        //CAM->SaveToImage(path.c_str(), img);
        
        // Yeah! PNG1!!
        string path = saveDirectory + imgPath + "it" + cpath + ".png";
        CAM->SaveToImagePNG(path.c_str(), img);
    }
    
    Margolus * defCAM;
    margolus_model model = margolus_default;
    Generate generator;
    int threads     = 1;
    uint fillCount  = 0;
    uint calcStep   = 1;
    uint saveStep   = 1;
    float exitCondition = 0.1f;
    uint lastIteration  = 0;
    string saveCatalog  = "";
    vector<uint> points;
    vector<pSub> iSubs;
    
    
    vector<LOCATION>    borders;
    vector<statistics>  stats;
    exit_type       et;
    structure       strctr;
    vector<var>     vars;
    vector<varcalc> varcalcs;
    Sizes   sizes;
    Sizes   multiply;
    bool    rotate = false;

    //uint code = 0; //тип струкруты - 0-bin ; 1-text
    string status = "";
    int tracked = -1;
    bool modifierMove = false;
    unsigned long long iterationX = 1000000;

    bool retention   = false;
    bool extra       = false;
    bool showConsole = true;
    bool printCA     = false;
    string outpath   = "";
    bool useMySQL    = true;
    
    //MySQL * mySQL;
    
    vector<VarPointer<int>>       intVars;
    vector<VarPointer<double>>    doubleVars;
    
    
};

int main(int argc, char* argv[]) {
    // NORMAL || TEST
#define NORMAL
    srand(time(NULL));
    //srand((unsigned) time(0));
#ifdef NORMAL
    Main* pApp = new Main();
    
    for (uint i = 1; i < argc; ++i) {
        if (GetParam(argv[i]) == param_noJson) {
            pApp->saveFieldJsonFlag = false;
            break;
        }  
    }
    
    // check print
    for (uint i = 1; i < argc; ++i) {
        if (GetParam(argv[i]) == param_print) {
            pApp->AddPrint();
            break;
        }
    }
    // check extra
    for (uint i = 1; i < argc; ++i) {
        if (GetParam(argv[i]) == param_extra) {
            pApp->AddExtra();
            break;
        }
    }
    // check retention
    for (uint i = 1; i < argc; ++i) {
        if (GetParam(argv[i]) == param_retention) {
            pApp->AddRetention();
            break;
        }
    }
    if (argc > 1) {
        if (argv[1][0] != '-') {
            if (pApp->Init(argv[1]) != EXIT_SUCCESS) {
                cout << "Error open conf configuration file " << argv[1] << endl;
                return 1;
            }
        } else {
            switch (GetParam(argv[1])) {
                case param_help:
                    PrintHelp();
                    return 0;
                case param_version:
                    PrintVersion();
                    return 0;
                case param_extra:
                case param_retention:
                case param_print:
                    if (pApp->Init(argv[2]) != EXIT_SUCCESS) {
                        cout << "Error open conf configuration file " << argv[2] << endl;
                        return 1;
                    }
                    break;
                case param_name:
                    cout << "Name is " << argv[0] << endl;
                    return 0;
                case param_path:
                    cout << "Path is " << AppPath() << endl;
                    return 0;
                case param_example:
                    // generate example conf file
                    PrintExample();
                    return 0;
                case param_output:
                    if (argc > 2) {
                        PrintHalfOutput(argv[2]);
                        return 0;
                    } else {
                        cout << "Maybe lost filename\n";
                        return 1;
                    }
                    break;
                case param_square:
                    if (argc > 2) {
                        PrintSurfaceSquare(argv[2], argv[1]);
                        return 0;
                    } else {
                        cout << "Maybe lost filename\n";
                        return 1;
                    }
                    break;
                case param_testrun:
                    pApp->RunTest(argv[1]);
                    return 0;
                case param_none:
                    cout << "Unknown parameter. Use --help option\n";
                    return 1;
                default:
                    cout << "Unknown parameter. Use --help option\n";
                    return 1;
            }
        }
    } else {
        cout << "Run with configuration file.\n";
        return 2;
    }
    unsigned t0 = clock();
    pApp->Run();
    //unsigned elapsed = clock() - t0;
    cout << "Прошло: " << double(clock() - t0) / CLOCKS_PER_SEC << " сек.\n";
    
   
    
#endif
    return 0;
}
