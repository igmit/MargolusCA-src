#include "cellularautomata.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>
#include <png.h>

CellularAutomata::CellularAutomata() : CellularAutomata(1, 1, 1) { }

CellularAutomata::CellularAutomata(Sizes sizes) : CellularAutomata(sizes.x, sizes.y, sizes.z) { }
    
CellularAutomata::CellularAutomata(cuint& sizeX, cuint& sizeY, cuint sizeZ) {
    size.x = sizeX;
    size.y = sizeY;
    size.z = sizeZ;
    cells = new Cell**[size.x];
    for (uint ix = 0; ix < size.x; ++ix) {
        cells[ix] = new Cell*[size.y];
        for (uint iy = 0; iy < size.y; ++iy) {
            cells[ix][iy] = new Cell[size.z];
        }
    }
    //SetEmptyField();
}

CellularAutomata::~CellularAutomata() {
    subs.clear();
    for (uint ix = 0; ix < GetSizeX(); ++ix) {
        for (uint iy = 0; iy < GetSizeY(); ++iy) {
            delete [] cells[ix][iy];
        }
        delete [] cells[ix];
    }
    delete [] cells;
}

void CellularAutomata::ReSize(cuint& sizeX, cuint& sizeY, cuint sizeZ) {
    size.x = sizeX;
    size.y = sizeY;
    size.z = sizeZ;
    cells = new Cell**[size.x];
    for (uint ix = 0; ix < size.x; ++ix) {
        cells[ix] = new Cell*[size.y];
        for (uint iy = 0; iy < size.y; ++iy) {
            cells[ix][iy] = new Cell[size.z];
            for (uint iz = 0; iz < size.z; ++iz) {
                cells[ix][iy][iz] = Cell();
            }
        }
    }
}

Cell CellularAutomata::GetCell(uint x, uint y, uint z) const {
    return cells[x][y][z];
}

void CellularAutomata::SetCell(const Cell& cell, uint x, uint y, uint z) {
    cells[x][y][z] = cell;
}

void CellularAutomata::ClearAllBorders() {
    ClearUpX();
    ClearUpY();
    ClearUpZ();
    ClearDownX();
    ClearDownY();
    ClearDownZ();
}

void CellularAutomata::ClearUpX() {
    for (uint iy = 0; iy < size.y; ++iy) {
        for (uint iz = 0; iz < size.z; ++iz) {
            cells[size.x - 1][iy][iz].Clear();
        }
    }
}

void CellularAutomata::ClearUpY() {
    for (uint ix = 0; ix < size.x; ++ix) {
        for (uint iz = 0; iz < size.z; ++iz) {
            cells[ix][size.y - 1][iz].Clear();
        }
    }
}

void CellularAutomata::ClearUpZ() {
    for (uint ix = 0; ix < size.x; ++ix) {
        for (uint iy = 0; iy < size.y; ++iy) {
            cells[ix][iy][size.z - 1].Clear();
        }
    }
}

void CellularAutomata::ClearDownX() {
    for (uint iy = 0; iy < size.y; ++iy) {
        for (uint iz = 0; iz < size.z; ++iz) {
            cells[0][iy][iz].Clear();
        }
    }
}

void CellularAutomata::ClearDownY() {
    for (uint ix = 0; ix < size.x; ++ix) {
        for (uint iz = 0; iz < size.z; ++iz) {
            cells[ix][0][iz].Clear();
        }
    }
}

void CellularAutomata::ClearDownZ() {
    for (uint ix = 0; ix < size.x; ++ix) {
        for (uint iy = 0; iy < size.y; ++iy) {
            cells[ix][iy][0].Clear();
        }
    }
}

void CellularAutomata::SetEmptyField() {
    for (uint ix = 0; ix < size.x; ++ix) {
        for (uint iy = 0; iy < size.y; ++iy) {
            for (uint iz = 0; iz < size.z; ++iz) {
                cells[ix][iy][iz] = Cell();
            }
        }
    }
}

uint CellularAutomata::SolidCount() {
    uint count = 0;
    for (uint ix = 0; ix < size.x; ++ix) {
        for (uint iy = 0; iy < size.y; ++iy) {
            for (uint iz = 0; iz < size.z; ++iz) {
                if (cells[ix][iy][iz].HaveSolid()) {
                    ++count;
                }
            }
        }
    }
    return count;
}

uint CellularAutomata::Count(SUBTYPE type) {
    uint count = 0;
    for (uint ix = 0; ix < size.x; ++ix) {
        for (uint iy = 0; iy < size.y; ++iy) {
            for (uint iz = 0; iz < size.z; ++iz) {
                for (pSub & sub : cells[ix][iy][iz].GetSubs()) {
                    if (sub->GetType() == type) {
                        ++count;
                    }
                }
            }
        }
    }
    return count;
}

uint CellularAutomata::Count(const string& name) {
    uint count = 0;
    for (uint ix = 0; ix < size.x; ++ix) {
        for (uint iy = 0; iy < size.y; ++iy) {
            for (uint iz = 0; iz < size.z; ++iz) {
                for (pSub & sub : cells[ix][iy][iz].GetSubs()) {
                    if (sub->GetName() == name) {
                        ++count;
                    }
                }
            }
        }
    }
    return count;
}

uint CellularAutomata::SidesCount() {
    uint count = 0;
    for (uint ix = 0; ix < size.x; ++ix) {
        for (uint iy = 0; iy < size.y; ++iy) {
            for (uint iz = 0; iz < size.z; ++iz) {
                if (cells[ix][iy][iz].HaveSolid()) {
                    count +=6;
                    if (ix != 0 && cells[ix - 1][iy][iz].HaveSolid()) {
                        --count;
                    }
                    if (ix != (size.x - 1) && cells[ix + 1][iy][iz].HaveSolid()) {
                        --count;
                    }
                    if (iy != 0 && cells[ix][iy - 1][iz].HaveSolid()) {
                        --count;
                    }
                    if (iy != (size.y - 1) && cells[ix][iy + 1][iz].HaveSolid()) {
                        --count;
                    }
                    if (iz != 0 && cells[ix][iy][iz - 1].HaveSolid()) {
                        --count;
                    }
                    if (iz != (size.z - 1) && cells[ix][iy][iz + 1].HaveSolid()) {
                        --count;
                    }
                }
            }
        }
    }
    
    return count;
}

uint CellularAutomata::SidesCount(SUBTYPE type) {
    uint count = 0;
    for (uint ix = 0; ix < size.x; ++ix) {
        for (uint iy = 0; iy < size.y; ++iy) {
            for (uint iz = 0; iz < size.z; ++iz) {
                for (pSub & sub : cells[ix][iy][iz].GetSubs()) {
                    if (sub->GetType() == type) {
                        count +=6;
                        if (ix != 0) {
                            for (pSub & sub1 : cells[ix - 1][iy][iz].GetSubs()) {
                                if (sub1->GetType() == type) {
                                    --count;
                                }
                            }
                        }
                        if (ix != (size.x - 1)) {
                            for (pSub & sub1 : cells[ix + 1][iy][iz].GetSubs()) {
                                if (sub1->GetType() == type) {
                                    --count;
                                }
                            }
                        }
                        if (iy != 0) {
                            for (pSub & sub1 : cells[ix][iy - 1][iz].GetSubs()) {
                                if (sub1->GetType() == type) {
                                    --count;
                                }
                            }
                        }
                        if (iy != (size.y - 1)) {
                            for (pSub & sub1 : cells[ix][iy + 1][iz].GetSubs()) {
                                if (sub1->GetType() == type) {
                                    --count;
                                }
                            }
                        }
                        if (iz != 0) {
                            for (pSub & sub1 : cells[ix][iy][iz - 1].GetSubs()) {
                                if (sub1->GetType() == type) {
                                    --count;
                                }
                            }
                        }
                        if (iz != (size.z - 1)) {
                            for (pSub & sub1 : cells[ix][iy][iz + 1].GetSubs()) {
                                if (sub1->GetType() == type) {
                                    --count;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    
    return count;
}

uint CellularAutomata::SidesCount(const string& name) {
    uint count = 0;
    for (uint ix = 0; ix < size.x; ++ix) {
        for (uint iy = 0; iy < size.y; ++iy) {
            for (uint iz = 0; iz < size.z; ++iz) {
                for (pSub & sub : cells[ix][iy][iz].GetSubs()) {
                    if (sub->GetName() == name) {
                        count +=6;
                        if (ix != 0) {
                            for (pSub & sub1 : cells[ix - 1][iy][iz].GetSubs()) {
                                if (sub1->GetName() == name) {
                                    --count;
                                }
                            }
                        }
                        if (ix != (size.x - 1)) {
                            for (pSub & sub1 : cells[ix + 1][iy][iz].GetSubs()) {
                                if (sub1->GetName() == name) {
                                    --count;
                                }
                            }
                        }
                        if (iy != 0) {
                            for (pSub & sub1 : cells[ix][iy - 1][iz].GetSubs()) {
                                if (sub1->GetName() == name) {
                                    --count;
                                }
                            }
                        }
                        if (iy != (size.y - 1)) {
                            for (pSub & sub1 : cells[ix][iy + 1][iz].GetSubs()) {
                                if (sub1->GetName() == name) {
                                    --count;
                                }
                            }
                        }
                        if (iz != 0) {
                            for (pSub & sub1 : cells[ix][iy][iz - 1].GetSubs()) {
                                if (sub1->GetName() == name) {
                                    --count;
                                }
                            }
                        }
                        if (iz != (size.z - 1)) {
                            for (pSub & sub1 : cells[ix][iy][iz + 1].GetSubs()) {
                                if (sub1->GetName() == name) {
                                    --count;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    
    return count;
}

bool CellularAutomata::AddToCell(pSub sub, int x, int y, int z) {
    if (x < 0) {
        std::cout << "Error add sub, xCoord < 0\n";
        return false;
    }
    if (x >= size.x) {
        std::cout << "Error add sub, xCoord >= sizeX\n";
        return false;
    }
    if (y < 0) {
        std::cout << "Error add sub, yCoord < 0\n";
        return false;
    }
    if (y >= size.y) {
        std::cout << "Error add sub, yCoord >= sizeY\n";
        return false;
    }
    if (z < 0) {
        std::cout << "Error add sub, zCoord < 0\n";
        return false;
    }
    if (z >= size.z) {
        std::cout << "Error add sub, zCoord >= sizeZ\n";
        return false;
    }
    if (sub == nullptr) {
        std::cout << "Error add sub, no sub\n";
        return false;
    }
    return cells[x][y][z].AddSub(sub);
}

void CellularAutomata::AddSub(pSub newSub) {
    subs.push_back(newSub);
}

void CellularAutomata::AddSub(const SUBTYPE type, const string& name,
        const XColor color, const vector<locate>& locations) {
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
        default:
            return;
    }
    AddSub(sub);
}

pSub CellularAutomata::GetSub(cuint& i) const {
    return subs[i];
}

pSub CellularAutomata::GetSub(const string& name) {
    for (pSub & sub : subs) {
        if (sub->GetName() == name) {
            return sub;
        }
    }
    return nullptr;
}

int CellularAutomata::GetSubI(const string& name) {
    for (int i = 0; i < subs.size(); ++i) {
        if (subs[i]->GetName() == name) {
            return i;
        }
    }
    return -1;
}

vector<pSub> CellularAutomata::GetSubs() const {
    return subs;
}

void CellularAutomata::ClearSubs() {
    subs.clear();
}

void CellularAutomata::LoadFieldBin(cchar* path){
    //ofstream file; file.open(path, ios::binary); int text; while ((text = file..get()) != EOF){
    //    cout << static_cast<char>(text); }
    ifstream in(path, ios::binary | ios::in); //Открыли для только для чтения в бинарном режиме
    in.read((char*)&iteration, sizeof(iteration));
    in.read((char*)&subs, sizeof(subs));
    in.read((char*)&cells, sizeof(cells)); //Считали информацию в объект О
    in.close(); //Закрыли открытый файл 
//char text[1000]; while(!file.eof()) { file<<text; cout<<text<<endl; }
    std::cout << "Loading field " << path << " Done!\n";
}

void CellularAutomata::LoadFieldText(cchar* path) {
    //SetEmptyField();
    ifstream in;
    in.open(path);
    // load subs
    {
        string s;
        in >> s;
        in >> s;
        iteration = stoi(s);
        in >> s;
        in >> s;
        int subCount = stoi(s);
        string sub[5];
        for (uint i = 0; i < subCount; ++i) {
            in >> sub[0];
            in >> sub[1];
            in >> sub[2];
            in >> sub[3];
            in >> sub[4];
            AddSub(SUBTYPE(stoi(sub[0])), sub[1], XColor(stoi(sub[2]), stoi(sub[3]), stoi(sub[4])));
        }
    }
    string vs[4];
    do {
        in >> vs[0];
        if (vs[0] == "") {
            break;
        }
        in >> vs[1];
        in >> vs[2];
        in >> vs[3];
        AddToCell(GetSub(vs[3]), stoi(vs[0]), stoi(vs[1]), stoi(vs[2]));
    } while (!in.eof());
    in.close();
    std::cout << "Loading field " << path << " Done!\n";
}

void CellularAutomata::LoadFieldTextOld(cchar* path) {
    //SetEmptyField();
    ifstream in;
    in.open(path);
    // load subs
    {
        string s;
        in >> s;
        in >> s;
        iteration = stoi(s);
        in >> s;
        in >> s;
        int subCount = stoi(s);
        string sub[3];
        for (uint i = 0; i < subCount; ++i) {
            in >> sub[0];
            in >> sub[1];
            in >> sub[2];
            AddSub(SUBTYPE(stoi(sub[0])), sub[1], XColor(stoi(sub[2])));
        }
    }
    string vs[4];
    do {
        in >> vs[0];
        if (vs[0] == "") {
            break;
        }
        in >> vs[1];
        in >> vs[2];
        in >> vs[3];
        int cl = stoi(vs[2]);
        if (cl == 4) {
            AddToCell(GetSub("aerogel"), stoi(vs[0]), stoi(vs[1]));
        } else {
            if (cl == 10) {
                AddToCell(GetSub("active"), stoi(vs[0]), stoi(vs[1]));
            }
        }
        int cm = stoi(vs[3]);
        if (cm == 1) {
            AddToCell(GetSub("methanol"), stoi(vs[0]), stoi(vs[1]));
        }
    } while (!in.eof());
    in.close();
    std::cout << "Loading field " << path << " Done!\n";
}

void CellularAutomata::LoadFieldTextXYZN(cchar* path) {
    //SetEmptyField();
    //"x_y_z_name"
    ifstream in;
    in.open(path);
    iteration = 0;
    string s[4];
    // load subs
    do {
        in >> s[0];
        in >> s[1];
        in >> s[2];
        in >> s[3];
        AddToCell(GetSub(s[3]), stoi(s[0]), stoi(s[1]), stoi(s[2]));
    } while (!in.eof());
    in.close();
    std::cout << "Loading field " << path << " Done!\n";
}

void CellularAutomata::LoadFieldTextXY_M(cchar* path, string sol, string mod) {
    //SetEmptyField();
    //"x_y_-_mod"
    ifstream in;
    in.open(path);
    iteration = 0;
    string s[4];
    // load subs
    do {
        in >> s[0];
        in >> s[1];
        in >> s[2];
        in >> s[3];
        switch (stoi(s[3])) {
            case 0:
                AddToCell(GetSub(sol), stoi(s[0]), stoi(s[1]));
                break;
            case 1:
                AddToCell(GetSub(mod), stoi(s[0]), stoi(s[1]));
                break;
        }
    } while (!in.eof());
    in.close();
    std::cout << "Loading field " << path << " Done!\n";
}

void CellularAutomata::LoadFieldTextXYZM(cchar* path, string sol, string mod) {
    //SetEmptyField();
    //"x_y_-_mod"
    ifstream in;
    in.open(path);
    iteration = 0;
    string s[4];
    // load subs
    // no subs, use name
    do {
        in >> s[0];
        in >> s[1];
        in >> s[2];
        in >> s[3];
        switch (stoi(s[3])) {
            case 0:
                AddToCell(GetSub(sol), stoi(s[0]), stoi(s[1]), stoi(s[2]));
                break;
            case 1:
                AddToCell(GetSub(mod), stoi(s[0]), stoi(s[1]), stoi(s[2]));
                break;
        }
    } while (!in.eof());
    in.close();
    std::cout << "Loading field " << path << " Done!\n";
}

void CellularAutomata::LoadFieldTextXYZX(cchar* path) {
    //SetEmptyField();
    //"x_y_-_mod"
    ifstream in;
    in.open(path);
    iteration = 0;
    {
        string s;
        in >> s;
        in >> s;
        in >> s;
        in >> s;
        for (uint i = 0; i < 2; ++i) {
            in >> s;
            in >> s;
            in >> s;
        }
    }
    string s[4];
    // load subs
    do {
        in >> s[0];
        in >> s[1];
        in >> s[2];
        in >> s[3];
        AddToCell(GetSub(s[3]), stoi(s[0]), stoi(s[1]), stoi(s[2]));
    } while (!in.eof());
    in.close();
    std::cout << "Loading field " << path << " Done!\n";
}

void CellularAutomata::LoadFieldTextXYZ(cchar* path, string name) {
    //SetEmptyField();
    //"x_y_z"
    ifstream in;
    in.open(path);
    iteration = 0;
    string s[3];
    // load subs
    // no subs, use name
    do {
        in >> s[0];
        in >> s[1];
        in >> s[2];
        AddToCell(GetSub(name), stoi(s[0]), stoi(s[1]), stoi(s[2]));
    } while (!in.eof());
    in.close();
    std::cout << "Loading field " << path << " Done!\n";
}

void CellularAutomata::LoadFieldTextXYZR(cchar* path, string name) {
    //SetEmptyField();
    //"x_y_z_r"
    ifstream in;
    in.open(path);
    iteration = 0;
    string s[4];
    int sum = 0;
    // load subs
    // no subs, use name
    do {
        in >> s[0];
        in >> s[1];
        in >> s[2];
        in >> s[3];
        
        float xx = atof(s[0].c_str());
        float yy = atof(s[1].c_str());
        float zz = atof(s[2].c_str());
        float rr = atof(s[3].c_str());

        int x1 = (int)max(xx - rr, (float)0);
        int y1 = (int)max(yy - rr, (float)0);
        int z1 = (int)max(zz - rr, (float)0);
        float x2 = min(xx + rr, (float)(size.x - 1));
        float y2 = min(yy + rr, (float)(size.y - 1));
        float z2 = min(zz + rr, (float)(size.z - 1));

        for (int x = x1; x < x2; ++x) {
            for (int y = y1; y < y2; ++y) {
                for (int z = z1; z < z2; ++z) {
                    if (sqrt((x - xx) * (x - xx) + (y - yy) * (y - yy) + (z - zz) * (z - zz)) <= rr) {
                        if (AddToCell(GetSub(name), x, y, z)) {
                            ++sum;
                        }
                    }
                }
            }
        }
    } while (!in.eof());
    in.close();
    std::cout << sum << " points added!\n";
    std::cout << "Loading field " << path << " Done!\n";
}

void CellularAutomata::SaveFieldBin(cchar* path){
    ofstream out(path, ios::binary | ios::out); //Открыли для записи в бинарном режиме
    out.write((char*)&iteration, sizeof(iteration));
    out.write((char*)&subs, sizeof(subs));
    out.write((char*)&cells, sizeof(cells)); //Записали объект А в открытый файл
    out.close(); //Закрыли открытый файл
}


void CellularAutomata::SaveFieldText(cchar* path) {
//    ofstream out;
//    out.open(path, ios_base::trunc);
//    out << "iteration: " << iteration << "\n";
//    out << "subcount: " << subs.size() << "\n";
//    for (pSub & sub : subs) {
//        out << sub->GetType() << " " << sub->GetName() << " " << sub->GetColor().r
//                << " " << sub->GetColor().g  << " " << sub->GetColor().b << "\n";
//    }
//    for (uint ix = 0; ix < size.x; ++ix) {
//        for (uint iy = 0; iy < size.y; ++iy) {
//            for (uint iz = 0; iz < size.z; ++iz) {
//                for (pSub & sub : cells[ix][iy][iz].GetSubs()) {
//                    out << ix << "\t" << iy << "\t" << iz << "\t" << sub->GetName() << "\n";
//                }
//            }
//        }	
//    }
//    out.close();
    
    
}

void CellularAutomata::SaveToImage(cchar* path, uchar* imageData) {
    //printf ( "Текущее время и дата: %s", asctime (timeinfo) );
//    FILE *f = fopen(path, "wb"); //f = fopen(pathImg.c_str(),"wb");
//    if (!f) {
//        printf("Can't open ScreenShot File! %s", path);
//        return;
//    }
//    uint width  = size.x;
//    uint height = size.y;
//    uint filesize = 54 + 3 * width * height;
//    uchar bmpfileheader[14] = {'B', 'M', 0, 0, 0, 0, 0, 0, 0, 0, 54, 0, 0, 0};
//    uchar bmpinfoheader[40] = {40, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 24, 0};
//    bmpfileheader[ 2] = (uchar)(filesize);
//    bmpfileheader[ 3] = (uchar)(filesize >> 8);
//    bmpfileheader[ 4] = (uchar)(filesize >> 16);
//    bmpfileheader[ 5] = (uchar)(filesize >> 24);
//    bmpinfoheader[ 4] = (uchar)(width);
//    bmpinfoheader[ 5] = (uchar)(width >> 8);
//    bmpinfoheader[ 6] = (uchar)(width >> 16);
//    bmpinfoheader[ 7] = (uchar)(width >> 24);
//    bmpinfoheader[ 8] = (uchar)(height);
//    bmpinfoheader[ 9] = (uchar)(height >> 8);
//    bmpinfoheader[10] = (uchar)(height >> 16);
//    bmpinfoheader[11] = (uchar)(height >> 24);
//    //unsigned char tempColors = 0;
//    fwrite(bmpfileheader, 1, 14, f);
//    fwrite(bmpinfoheader, 1, 40, f);
//    //for (int i = 0; i < wigth * heigth * 3; i += 3) {
//     //   tempColors = imageData[i];
//     //   imageData[i] = imageData[i + 2];
//     //   imageData[i + 2] = tempColors;
//    //}
//    fwrite(imageData, 1, width * height * 3, f);
//    fclose(f);
}

void CellularAutomata::SaveToImagePNG(cchar* path, uchar* imageData) {
    //printf ( "Текущее время и дата: %s", asctime (timeinfo) );
    FILE *f = fopen(path, "wb");
    if (!f) {
        printf("Can't open ScreenShot File! %s", path);
        return;
    }
    png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
    if (!png_ptr) {
        fclose(f);
        return;
    }
    png_infop png_info;
    if (!(png_info = png_create_info_struct(png_ptr))) {
        png_destroy_write_struct(&png_ptr, nullptr);
        fclose(f);
        return;
    }
    if (setjmp(png_jmpbuf(png_ptr))) {
        png_destroy_write_struct(&png_ptr, nullptr);
        fclose(f);
        return;
    }
    png_init_io(png_ptr, f);
    uint width  = size.x;
    uint height = size.y;
    png_set_IHDR(png_ptr, png_info, width, height, 8, PNG_COLOR_TYPE_RGB,
        PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT,
        PNG_FILTER_TYPE_DEFAULT);

    uchar data[width * height * 3];
    uchar *rows[height];

    //uchar tempColors = 0;
    //for (uint i = 0; i < width * height * 4; i += 4) {
    //    tempColors = imageData[i + 1];
    //    imageData[i + 1] = imageData[i + 3];
    //    imageData[i + 3] = tempColors;
    //}
    for (uint i = 0; i < height; ++i) {
        rows[height - i - 1] = data + (i * width * 3);
        for (uint j = 0; j < width; ++j) {
            int i1 = (i * width + j) * 3;
            int i2 = (i * width + j) * 4;
            data[i1++] = imageData[i2++];
            data[i1++] = imageData[i2++];
            data[i1++] = imageData[i2++];
        }
    }

    png_set_rows(png_ptr, png_info, rows);
    png_write_png(png_ptr, png_info, PNG_TRANSFORM_IDENTITY, nullptr);
    png_write_end(png_ptr, png_info);

    png_destroy_write_struct(&png_ptr, nullptr);

    fclose(f);
}

uint CellularAutomata::Layer() {
    if (adsorbLayer == 0) {
        for (uint ix = 0; ix < size.x; ++ix) {
            for (uint iy = 0; iy < size.y; ++iy) {
                for (uint iz = 0; iz < size.z; ++iz) {
                    if (!GetCell(ix, iy, iz).HaveSolid()) {
                        if (ix != 0 && GetCell(ix - 1, iy, iz).HaveSolid()) {
                            ++adsorbLayer;
                        } else if (iy != 0 && GetCell(ix, iy - 1, iz).HaveSolid()) {
                            ++adsorbLayer;
                        } else if (iz != 0 && GetCell(ix, iy, iz - 1).HaveSolid()) {
                            ++adsorbLayer;
                        } else if (ix != size.x - 1 && GetCell(ix + 1, iy, iz).HaveSolid()) {
                            ++adsorbLayer;
                        } else if (iy != size.y - 1 && GetCell(ix, iy + 1, iz).HaveSolid()) {
                            ++adsorbLayer;
                        } else if (iz != size.z - 1 && GetCell(ix, iy, iz + 1).HaveSolid()) {
                            ++adsorbLayer;
                        }
                    }
                }
            }
        }
    }
    return adsorbLayer;
}

uint CellularAutomata::Adsorbed() const {
    uint result = 0;
    for (uint ix = 0; ix < size.x; ++ix) {
        for (uint iy = 0; iy < size.y; ++iy) {
            for (uint iz = 0; iz < size.z; ++iz) {
                if (GetCell(ix, iy, iz).HaveActive()) {
                    if (ix != 0 && GetCell(ix - 1, iy, iz).HaveSolid()) {
                        ++result;
                    } else if (iy != 0 && GetCell(ix, iy - 1, iz).HaveSolid()) {
                        ++result;
                    } else if (iz != 0 && GetCell(ix, iy, iz - 1).HaveSolid()) {
                        ++result;
                    } else if (ix != size.x - 1 && GetCell(ix + 1, iy, iz).HaveSolid()) {
                        ++result;
                    } else if (iy != size.y - 1 && GetCell(ix, iy + 1, iz).HaveSolid()) {
                        ++result;
                    } else if (iz != size.z - 1 && GetCell(ix, iy, iz + 1).HaveSolid()) {
                        ++result;
                    }
                }
            }
        }
    }
    return result;
}

void CellularAutomata::ImportSubs(CellularAutomata* CA) {
    subs.clear();
    for (pSub & sub : CA->GetSubs()) {
        subs.push_back(sub);
    }
}
