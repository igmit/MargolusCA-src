#include "margolus_base.h"
#include <fstream>
#include <iostream>
#include <cereal/archives/json.hpp>
#include <cereal/access.hpp>

//#include <omp.h>
//#define foreach(T, c, i) for(T::iterator i = c.begin(); i!=c.end(); ++i)

Margolus::Margolus() : Margolus(1, 1, 1) {
}

Margolus::Margolus(Sizes sizes) : Margolus(sizes.x, sizes.y, sizes.z) {
}

Margolus::Margolus(cuint& sizeX, cuint& sizeY, cuint sizeZ)
: CellularAutomata(sizeX, sizeY, sizeZ) {
    blocks[0].rotate = UnMoved;
    memset(blocks[0].move, 0, sizeof (blocks[0].move));
    blocks3D[0].rotate = UnMoved3;
    memset(blocks3D[0].move, 0, sizeof (blocks3D[0].move));
}

Margolus::~Margolus() {
    delete steamEnergy_;
    steamEnergy_ = 0;
    delete T;
    T = 0;
    subs.clear();
    energy.clear();
    energyCell.clear();
}

void Margolus::Calculation(cuint& dx, cuint& dy, cuint& dz) {
    //#pragma omp parallel for
    for (uint ix = dx; ix < GetSizeX() - 1; ix += 2) {
        for (uint iy = dy; iy < GetSizeY() - 1; iy += 2) {
            for (uint iz = dz; iz < GetSizeZ() - 1; iz += 2) {
                if (CheckEmpty(ix, iy, iz)) {
                    continue;
                }
                blockSize3D = 1;
                CreateRotateNotBlock3D(blocks3D[0], ix, iy, iz);
                if (modifierMove && CheckMod(ix, iy, iz)) {
                    if (CheckActive(ix, iy)) {
                        if (CheckCanRotate3D(ClockWiceX, ix, iy, iz)) {
                            CreateRotateBlock3D(blocks3D[blockSize3D], ClockWiceX, ix, iy, iz, false, true);
                            ++blockSize3D;
                            CreateRotateBlock3D(blocks3D[blockSize3D], ClockWiceX, ix, iy, iz, true, true);
                            ++blockSize3D;
                            CreateRotateBlock3D(blocks3D[blockSize3D], ClockWiceX, ix, iy, iz);
                            ++blockSize3D;
                        }
                        if (CheckCanRotate3D(CounterClockWiceX, ix, iy, iz)) {
                            CreateRotateBlock3D(blocks3D[blockSize3D], CounterClockWiceX, ix, iy, iz, false, true);
                            ++blockSize3D;
                            CreateRotateBlock3D(blocks3D[blockSize3D], CounterClockWiceX, ix, iy, iz, true, true);
                            ++blockSize3D;
                            CreateRotateBlock3D(blocks3D[blockSize3D], CounterClockWiceX, ix, iy, iz);
                            ++blockSize3D;
                        }
                        if (CheckCanRotate3D(ClockWiceY, ix, iy, iz)) {
                            CreateRotateBlock3D(blocks3D[blockSize3D], ClockWiceY, ix, iy, iz, false, true);
                            ++blockSize3D;
                            CreateRotateBlock3D(blocks3D[blockSize3D], ClockWiceY, ix, iy, iz, true, true);
                            ++blockSize3D;
                            CreateRotateBlock3D(blocks3D[blockSize3D], ClockWiceY, ix, iy, iz);
                            ++blockSize3D;
                        }
                        if (CheckCanRotate3D(CounterClockWiceY, ix, iy, iz)) {
                            CreateRotateBlock3D(blocks3D[blockSize3D], CounterClockWiceY, ix, iy, iz, false, true);
                            ++blockSize3D;
                            CreateRotateBlock3D(blocks3D[blockSize3D], CounterClockWiceY, ix, iy, iz, true, true);
                            ++blockSize3D;
                            CreateRotateBlock3D(blocks3D[blockSize3D], CounterClockWiceY, ix, iy, iz);
                            ++blockSize3D;
                        }
                        if (CheckCanRotate3D(ClockWiceZ, ix, iy, iz)) {
                            CreateRotateBlock3D(blocks3D[blockSize3D], ClockWiceZ, ix, iy, iz, false, true);
                            ++blockSize3D;
                            CreateRotateBlock3D(blocks3D[blockSize3D], ClockWiceZ, ix, iy, iz, true, true);
                            ++blockSize3D;
                            CreateRotateBlock3D(blocks3D[blockSize3D], ClockWiceZ, ix, iy, iz);
                            ++blockSize3D;
                        }
                        if (CheckCanRotate3D(CounterClockWiceZ, ix, iy, iz)) {
                            CreateRotateBlock3D(blocks3D[blockSize3D], CounterClockWiceZ, ix, iy, iz, false, true);
                            ++blockSize3D;
                            CreateRotateBlock3D(blocks3D[blockSize3D], CounterClockWiceZ, ix, iy, iz, true, true);
                            ++blockSize3D;
                            CreateRotateBlock3D(blocks3D[blockSize3D], CounterClockWiceZ, ix, iy, iz);
                            ++blockSize3D;
                        }
                    } else {
                        CreateRotateBlock3D(blocks3D[blockSize3D], ClockWiceX, ix, iy, iz, false, true);
                        ++blockSize3D;
                        CreateRotateBlock3D(blocks3D[blockSize3D], CounterClockWiceX, ix, iy, iz, false, true);
                        ++blockSize3D;
                        CreateRotateBlock3D(blocks3D[blockSize3D], ClockWiceY, ix, iy, iz, false, true);
                        ++blockSize3D;
                        CreateRotateBlock3D(blocks3D[blockSize3D], CounterClockWiceY, ix, iy, iz, false, true);
                        ++blockSize3D;
                        CreateRotateBlock3D(blocks3D[blockSize3D], ClockWiceZ, ix, iy, iz, false, true);
                        ++blockSize3D;
                        CreateRotateBlock3D(blocks3D[blockSize3D], CounterClockWiceZ, ix, iy, iz, false, true);
                        ++blockSize3D;
                    }
                } else {
                    if (CheckCanRotate3D(ClockWiceX, ix, iy, iz)) {
                        CreateRotateBlock3D(blocks3D[blockSize3D], ClockWiceX, ix, iy, iz);
                        ++blockSize3D;
                    }
                    if (CheckCanRotate3D(CounterClockWiceX, ix, iy, iz)) {
                        CreateRotateBlock3D(blocks3D[blockSize3D], CounterClockWiceX, ix, iy, iz);
                        ++blockSize3D;
                    }
                    if (CheckCanRotate3D(ClockWiceY, ix, iy, iz)) {
                        CreateRotateBlock3D(blocks3D[blockSize3D], ClockWiceY, ix, iy, iz);
                        ++blockSize3D;
                    }
                    if (CheckCanRotate3D(CounterClockWiceY, ix, iy, iz)) {
                        CreateRotateBlock3D(blocks3D[blockSize3D], CounterClockWiceY, ix, iy, iz);
                        ++blockSize3D;
                    }
                    if (CheckCanRotate3D(ClockWiceZ, ix, iy, iz)) {
                        CreateRotateBlock3D(blocks3D[blockSize3D], ClockWiceZ, ix, iy, iz);
                        ++blockSize3D;
                    }
                    if (CheckCanRotate3D(CounterClockWiceZ, ix, iy, iz)) {
                        CreateRotateBlock3D(blocks3D[blockSize3D], CounterClockWiceZ, ix, iy, iz);
                        ++blockSize3D;
                    }
                }
                double Z = 0.0;
                // + Energy
                for (uint i = 0; i < blockSize3D; ++i) {
                    double energy = CalculationBlockEnergy(blocks3D[i], ix, iy, iz); //кДж/моль
                    blocks3D[i].energy = exp(-energy / (R * *T));
                    Z += blocks3D[i].energy;
                }
                //normalization
                double sumProbability = 0.0;
                double rnd = (double) (rand()) / RAND_MAX;
                for (uint i = 0; i < blockSize3D; ++i) {
                    sumProbability += blocks3D[i].energy / Z;
                    if (rnd <= sumProbability) {
                        ChangeBlock(blocks3D[i], ix, iy, iz);
                        break;
                    }
                }
            }
        }
    }
}

void Margolus::Calculation(cuint& dx, cuint& dy) {

    vector<DataObj> data;

    if (saveFlag)
        this->saveField(dx, dy, false);

    #pragma omp parallel for
    for (uint ix = dx; ix < GetSizeX() - 1; ix += 2) {
        for (uint iy = dy; iy < GetSizeY() - 1; iy += 2) {
            if (CheckEmpty(ix, iy)) {
                continue;
            }
            // 1
            blockSize = 1;
            CreateRotateNotBlock(blocks[0], ix, iy);
            if (modifierMove && CheckMod(ix, iy)) {
                if (CheckActive(ix, iy)) {
                    if (CheckCanClockWice(ix, iy)) {
                        CreateRotateBlock(blocks[blockSize], ClockWice, ix, iy, false, true);
                        ++blockSize;
                        CreateRotateBlock(blocks[blockSize], ClockWice, ix, iy, true, true);
                        ++blockSize;
                        CreateRotateBlock(blocks[blockSize], ClockWice, ix, iy);
                        ++blockSize;
                    }
                    if (CheckCanCounterClockWice(ix, iy)) {
                        CreateRotateBlock(blocks[blockSize], CounterClockWice, ix, iy, false, true);
                        ++blockSize;
                        CreateRotateBlock(blocks[blockSize], CounterClockWice, ix, iy, true, true);
                        ++blockSize;
                        CreateRotateBlock(blocks[blockSize], CounterClockWice, ix, iy);
                        ++blockSize;
                    }
                } else {
                    CreateRotateBlock(blocks[blockSize], ClockWice, ix, iy, false, true);
                    ++blockSize;
                    CreateRotateBlock(blocks[blockSize], CounterClockWice, ix, iy, false, true);
                    ++blockSize;
                }
            } else {
                if (CheckCanClockWice(ix, iy)) {
                    CreateRotateBlock(blocks[blockSize], ClockWice, ix, iy);
                    ++blockSize;
                }
                if (CheckCanCounterClockWice(ix, iy)) {
                    CreateRotateBlock(blocks[blockSize], CounterClockWice, ix, iy);
                    ++blockSize;
                }
            }

            double Z = 0.0;
            // + Energy
            for (uint i = 0; i < blockSize; ++i) {
                double energy = CalculationBlockEnergy(blocks[i], ix, iy); //кДж/моль
                blocks[i].energy = exp(-energy / (R * *T));
                Z += blocks[i].energy;
            }
            //normalization
            double sumProbability = 0.0;
            double rnd = (double) (rand()) / RAND_MAX;

            for (uint i = 0; i < blockSize; ++i) {
                sumProbability += blocks[i].energy / Z;
                if (rnd <= sumProbability) {

                    data.push_back(DataObj(ix, iy, vector<Block>(blocks, blocks + sizeof blocks / sizeof blocks[0]), i));

                    ChangeBlock(blocks[i], ix, iy);
                    break;
                }
            }

        }
    }


    if (saveFlag && saveJsonFlag) {

        string name = std::to_string(this->iteration) + "_" + std::to_string(this->subIteration) + "_"
                + std::to_string(dx) + std::to_string(dy)
                + ".json";

        std::ofstream os(this->path + "blocks" + name);
        {
            cereal::JSONOutputArchive archive(os); // stream to cout

            archive(
                    cereal::make_nvp("data", data),
                    cereal::make_nvp("energy", energy),
                    cereal::make_nvp("energyCell", energyCell),
                    cereal::make_nvp("steamEnergy_", *steamEnergy_)

                    );
        }

    }

    subIteration++;

}

void Margolus::saveField(int dx, int dy, bool init) {

    int solidCount = 0;
    int boundaryLayerCount = 0;
    int adsorbedCount = 0;

    int activeCount = 0;

    vector<string> substances;
    vector<FieldCell> field;

    for (pSub & sub : subs) {
        substances.push_back(sub->GetName());
    }

    Cell curCell;

    for (uint ix = 0; ix < this->GetSizeX(); ++ix) {
        for (uint iy = 0; iy < this->GetSizeY(); ++iy) {
            for (uint iz = 0; iz < this->GetSizeZ(); ++iz) {

                curCell = cells[ix][iy][iz];

                if (saveJsonFlag && curCell.GetSubCount() > 0)
                    field.push_back(FieldCell(ix, iy, curCell));

                if (subIteration > 0)
                    continue;

                if (curCell.HaveSolid()) {
                    ++solidCount;
                }

                if (curCell.HaveActive())
                    activeCount++;

                if (curCell.HaveActive()) {
                    if (ix != 0 && GetCell(ix - 1, iy, iz).HaveSolid()) {
                        ++adsorbedCount;
                    } else if (iy != 0 && GetCell(ix, iy - 1, iz).HaveSolid()) {
                        ++adsorbedCount;
                    } else if (iz != 0 && GetCell(ix, iy, iz - 1).HaveSolid()) {
                        ++adsorbedCount;
                    } else if (ix != this->GetSizeX() - 1 && GetCell(ix + 1, iy, iz).HaveSolid()) {
                        ++adsorbedCount;
                    } else if (iy != this->GetSizeY() - 1 && GetCell(ix, iy + 1, iz).HaveSolid()) {
                        ++adsorbedCount;
                    } else if (iz != this->GetSizeZ() - 1 && GetCell(ix, iy, iz + 1).HaveSolid()) {
                        ++adsorbedCount;
                    }
                }

                if (!curCell.HaveSolid()) {
                    if (ix != 0 && GetCell(ix - 1, iy, iz).HaveSolid()) {
                        ++boundaryLayerCount;
                    } else if (iy != 0 && GetCell(ix, iy - 1, iz).HaveSolid()) {
                        ++boundaryLayerCount;
                    } else if (iz != 0 && GetCell(ix, iy, iz - 1).HaveSolid()) {
                        ++boundaryLayerCount;
                    } else if (ix != this->GetSizeX() - 1 && GetCell(ix + 1, iy, iz).HaveSolid()) {
                        ++boundaryLayerCount;
                    } else if (iy != this->GetSizeY() - 1 && GetCell(ix, iy + 1, iz).HaveSolid()) {
                        ++boundaryLayerCount;
                    } else if (iz != this->GetSizeZ() - 1 && GetCell(ix, iy, iz + 1).HaveSolid()) {
                        ++boundaryLayerCount;
                    }
                }

            }
        }
    }

    if (subIteration == 0) {
        statisticsData.push_back(StatisticsRecord(iteration, solidCount, boundaryLayerCount, adsorbedCount, activeCount));
    }

    if (saveJsonFlag) {

        string name;

        if (!init)
            name = std::to_string(this->iteration) + "_" + std::to_string(this->subIteration) + "_"
            + std::to_string(dx) + std::to_string(dy)
            + ".json";
        else
            name = "_init.json";

        std::ofstream fields(this->path + "fields" + name);
        {
            cereal::JSONOutputArchive archive(fields); // stream to cout

            archive(
                    //cereal::make_nvp("subcount", subs.size()),
                    cereal::make_nvp("substances", substances),

                    cereal::make_nvp("field", field)

                    );



        }
    }

}

void Margolus::SetTempPtr(double* pointer) {
    if (T == 0 || T == nullptr) {
        T = pointer;
    }
    //return T;
}

void Margolus::SetTemperature(double Temp) {
    *T = Temp;
}

void Margolus::SetSteamPtr(double* pointer) {
    if (steamEnergy_ == 0 || steamEnergy_ == nullptr) {
        steamEnergy_ = pointer;
    }
    //return steamEnergy_;
}

void Margolus::SetSteamEnergy(double steamEnergy) {
    *steamEnergy_ = steamEnergy;
}

void Margolus::SetEnergy(const string& name1, const string& name2,
        double* energyH, double* energyS) {
    for (Energy & en : energy) {
        if ((en.name1 == name1 && en.name2 == name2)
                || (en.name1 == name2 && en.name2 == name1)) {
            en.energyH = energyH;
            en.energyS = energyS;
            return;
        }
    }
    AddEnergy(name1, name2, energyH, energyS);
}

void Margolus::AddEnergy(const string& name1, const string& name2,
        double* energyH, double* energyS) {
    Energy en;
    en.name1 = name1;
    en.name2 = name2;
    en.energyH = energyH;
    en.energyS = energyS;
    energy.push_back(en);
}

void Margolus::AddEnergy(Energy & en) {
    energy.push_back(en);
}

vector<Energy> Margolus::GetEnergies() const {
    return energy;
}

vector<Energy> Margolus::GetEnergiesCell() const {
    return energyCell;
}

double Margolus::GetEnergy(const string& name1, const string& name2) {
    for (Energy & en : energy) {
        if ((en.name1 == name1 && en.name2 == name2)
                || (en.name1 == name2 && en.name2 == name1)) {
            return en.energy;
        }
    }
    return 0;
}

double Margolus::GetEnergyH(const string& name1, const string& name2) {
    for (Energy & en : energy) {
        if ((en.name1 == name1 && en.name2 == name2)
                || (en.name1 == name2 && en.name2 == name1)) {
            return *en.energyH;
        }
    }
    return 0;
}

double Margolus::GetEnergyS(const string& name1, const string& name2) {
    for (Energy & en : energy) {
        if ((en.name1 == name1 && en.name2 == name2)
                || (en.name1 == name2 && en.name2 == name1)) {
            return *en.energyS;
        }
    }
    return 0;
}

void Margolus::SetEnergyCell(const string& name1, const string& name2,
        double* energyH, double* energyS) {
    for (Energy & en : energyCell) {
        if ((en.name1 == name1 && en.name2 == name2)
                || (en.name1 == name2 && en.name2 == name1)) {
            en.energyH = energyH;
            en.energyS = energyS;
            return;
        }
    }
    AddEnergyCell(name1, name2, energyH, energyS);
}

void Margolus::AddEnergyCell(const string& name1, const string& name2,
        double* energyH, double* energyS) {
    Energy en;
    en.name1 = name1;
    en.name2 = name2;
    en.energyH = energyH;
    en.energyS = energyS;
    energyCell.push_back(en);
}

void Margolus::AddEnergyCell(Energy & en) {
    energyCell.push_back(en);
}

double Margolus::GetEnergyCell(const string& name1, const string& name2) {
    for (Energy & en : energyCell) {
        if ((en.name1 == name1 && en.name2 == name2)
                || (en.name1 == name2 && en.name2 == name1)) {
            return en.energy;
        }
    }
    return 0;
}

double Margolus::GetEnergyHCell(const string& name1, const string& name2) {
    for (Energy & en : energyCell) {
        if ((en.name1 == name1 && en.name2 == name2)
                || (en.name1 == name2 && en.name2 == name1)) {
            return *en.energyH;
        }
    }
    return 0;
}

double Margolus::GetEnergySCell(const string& name1, const string& name2) {
    for (Energy & en : energyCell) {
        if ((en.name1 == name1 && en.name2 == name2)
                || (en.name1 == name2 && en.name2 == name1)) {
            return *en.energyS;
        }
    }
    return 0;
}

void Margolus::SetMoveModifier(bool value) {
    modifierMove = value;
}

void Margolus::ClearEnergy() {
    energy.clear();
    energyCell.clear();
}

void Margolus::UpdateEnergies() {
    for (Energy & en : energy) {
        en.UpdateEnergy(*T);
    }
    for (Energy & en : energyCell) {
        en.UpdateEnergy(*T);
    }
}

void Margolus::PrintParameters() const {
    cout << "\n------------------------------\n---Output field parameters:---\n------------------------------\n";
    cout << "Size X:\t" << GetSizeX() << endl;
    cout << "Size Y:\t" << GetSizeY() << endl;
    cout << "Size Z:\t" << GetSizeZ() << endl;
    cout << "Temperature:\t" << *T << endl;
    cout << "Steam energy:\t" << *steamEnergy_ << endl;
    cout << "Substances count:\t" << subs.size() << endl;
    for (const pSub & sub : subs) {
        cout << sub->GetName() << "\t" << sub->GetFillCount() << endl;
    }
    cout << "Energy count:\t" << energy.size() << endl;
    for (const Energy & en : energy) {
        cout << en.name1 << "-" << en.name2 << " E=" << en.energy << " H="
                << *en.energyH << " S=" << *en.energyS << endl;
    }
    cout << "Energy cell count:\t" << energyCell.size() << endl;
    for (const Energy & en : energyCell) {
        cout << en.name1 << "-" << en.name2 << " E=" << en.energy << " H="
                << *en.energyH << " S=" << *en.energyS << endl;
    }
    cout << "------------------------------\n---Output field parameters:---\n------------------------------\n";
    cout << "Press any key to continue ... \n";
    string s;
    cin >> s;
}

void Margolus::PrintBlock(Block& block) const {
    cout << "\n";
    for (uint x = 0; x < 2; ++x) {
        for (uint y = 0; y < 2; ++y) {
            cout << " - ";
            for (pSub sub : block.cells[x][y].GetSubs()) {
                switch (sub->GetType()) {
                    case SOLID:
                        cout << "S";
                        break;
                    case ACTIVE:
                        cout << "A";
                        break;
                    case MODIFIER:
                        cout << "M";
                }
            }
            cout << "\t";
        }
        cout << "\n";
    }
}

bool Margolus::CheckEmpty(cuint& ix, cuint& iy) const {
    for (uint x = 0; x < 2; ++x) {
        for (uint y = 0; y < 2; ++y) {
            if (modifierMove && cells[x + ix][y + iy][0].HaveLiquid()) {
                return false;
            } else if (cells[x + ix][y + iy][0].HaveActive()) {
                return false;
            }
        }
    }
    return true;
}

bool Margolus::CheckEmpty(cuint& ix, cuint& iy, cuint& iz) const {
    for (uint x = 0; x < 2; ++x) {
        for (uint y = 0; y < 2; ++y) {
            for (uint z = 0; z < 2; ++z) {
                if (modifierMove && cells[x + ix][y + iy][z + iz].HaveLiquid()) {
                    return false;
                } else if (cells[x + ix][y + iy][z + iz].HaveActive()) {
                    return false;
                }
            }
        }
    }
    return true;
}

bool Margolus::CheckMod(cuint& ix, cuint& iy) const {
    for (uint x = 0; x < 2; ++x) {
        for (uint y = 0; y < 2; ++y) {
            if (cells[x + ix][y + iy][0].HaveModifier()) {
                return true;
            }
        }
    }
    return false;
}

bool Margolus::CheckMod(cuint& ix, cuint& iy, cuint& iz) const {
    for (uint x = 0; x < 2; ++x) {
        for (uint y = 0; y < 2; ++y) {
            for (uint z = 0; z < 2; ++z) {
                if (cells[x + ix][y + iy][z + iz].HaveModifier()) {
                    return true;
                }
            }
        }
    }
    return false;
}

bool Margolus::CheckActive(cuint& ix, cuint& iy) const {
    for (uint x = 0; x < 2; ++x) {
        for (uint y = 0; y < 2; ++y) {
            if (cells[x + ix][y + iy][0].HaveActive()) {
                return true;
            }
        }
    }
    return false;
}

bool Margolus::CheckActive(cuint& ix, cuint& iy, cuint& iz) const {
    for (uint x = 0; x < 2; ++x) {
        for (uint y = 0; y < 2; ++y) {
            for (uint z = 0; z < 2; ++z) {
                if (cells[x + ix][y + iy][z + iz].HaveActive()) {
                    return true;
                }
            }
        }
    }
    return false;
}

void Margolus::PareEnergy(Cell& cellIn, Cell& cellOut, double& energy) {
    if (cellOut.HaveSolid()) {
        if (cellIn.HaveModifier()) {
            for (pSub & subIn : cellIn.GetSubs()) {
                if (subIn->GetType() != ACTIVE) {
                    energy += GetEnergy(subIn->GetName(), cellOut.GetSub(0)->GetName());
                }
            }
        } else {
            for (pSub & subIn : cellIn.GetSubs()) {
                energy += GetEnergy(subIn->GetName(), cellOut.GetSub(0)->GetName());
            }
        }
    }
}

void Margolus::PareEnergyFull(Cell& cellIn, Cell& cellOut, double& energy) {
    // modifier & active in one cell
    for (pSub & subA : cellIn.GetSubs()) {
        if (subA->GetType() == ACTIVE) {
            for (pSub & subM : cellIn.GetSubs()) {
                if (subM->GetType() == MODIFIER) {
                    energy += GetEnergyCell(subA->GetName(), subM->GetName());
                }
            }
        }
    }
    // other
    if (cellOut.HaveSolid()) {
        if (cellIn.HaveModifier()) {
            for (pSub & subIn : cellIn.GetSubs()) {
                if (subIn->GetType() != ACTIVE) {
                    energy += GetEnergy(subIn->GetName(), cellOut.GetSub(0)->GetName());
                }
            }
        } else {
            for (pSub & subIn : cellIn.GetSubs()) {
                energy += GetEnergy(subIn->GetName(), cellOut.GetSub(0)->GetName());
            }
        }
    } else {
        if (cellIn.HaveSolid()) {
            if (cellOut.HaveModifier()) {
                for (pSub & subOut : cellOut.GetSubs()) {
                    if (subOut->GetType() != ACTIVE) {
                        energy += GetEnergy(subOut->GetName(), cellIn.GetSub(0)->GetName());
                    }
                }
            } else {
                for (pSub & subOut : cellOut.GetSubs()) {
                    energy += GetEnergy(subOut->GetName(), cellIn.GetSub(0)->GetName());
                }
            }
        } else {
            for (pSub & subIn : cellIn.GetSubs()) {
                for (pSub & subOut : cellOut.GetSubs()) {
                    energy += GetEnergy(subIn->GetName(), subOut->GetName());
                }
            }
        }
    }
}

double Margolus::CalculationBlockEnergy(const Block& block, cuint& ix, cuint& iy) {
    double sumEnergy = 0.0;

    Cell cellIn, cellOut;
    //x gran 1
    if (ix > 0) {
        cellIn = block.cells[0][1];
        cellOut = cells[ix - 1][iy + 1][0];
        PareEnergy(cellIn, cellOut, sumEnergy);
        cellIn = block.cells[0][0];
        cellOut = cells[ix - 1][iy][0];
        PareEnergy(cellIn, cellOut, sumEnergy);
    }
    //x gran 2
    if (ix < GetSizeX() - 2) {
        cellIn = block.cells[1][1];
        cellOut = cells[ix + 2][iy + 1][0];
        PareEnergy(cellIn, cellOut, sumEnergy);
        cellIn = block.cells[1][0];
        cellOut = cells[ix + 2][iy][0];
        PareEnergy(cellIn, cellOut, sumEnergy);
    }
    //y gran 1
    if (iy > 0) {
        cellIn = block.cells[1][0];
        cellOut = cells[ix + 1][iy - 1][0];
        PareEnergy(cellIn, cellOut, sumEnergy);
        cellIn = block.cells[0][0];
        cellOut = cells[ix][iy - 1][0];
        PareEnergy(cellIn, cellOut, sumEnergy);
    }
    //y gran 2
    if (iy < GetSizeY() - 2) {
        cellIn = block.cells[1][1];
        cellOut = cells[ix + 1][iy + 2][0];
        PareEnergy(cellIn, cellOut, sumEnergy);
        cellIn = block.cells[0][1];
        cellOut = cells[ix][iy + 2][0];
        PareEnergy(cellIn, cellOut, sumEnergy);
    }
    //inside block
    for (uint i = 0; i < 4; ++i) {
        cellIn = block.cells[indexF[i][0]][indexF[i][1]];
        cellOut = block.cells[indexF[i + 1][0]][indexF[i + 1][1]];
        PareEnergyFull(cellIn, cellOut, sumEnergy);
    }

    if (!moveForward || block.move[movement] > 0) {
        sumEnergy += block.move[movement] * *steamEnergy_;
    }

    return sumEnergy;
}

double Margolus::CalculationBlockEnergy(const Block3D& block, cuint& ix,
        cuint& iy, cuint& iz) {
    double sumEnergy = 0.0;

    Cell cellIn, cellOut;

    //x gran 1
    if (ix > 0) {
        for (uint z = 0; z < 2; ++z) {
            cellIn = block.cells[0][1][z];
            cellOut = cells[ix - 1][iy + 1][iz + z];
            PareEnergy(cellIn, cellOut, sumEnergy);
            cellIn = block.cells[0][0][z];
            cellOut = cells[ix - 1][iy][iz + z];
            PareEnergy(cellIn, cellOut, sumEnergy);
        }
    }
    //x gran 2
    if (ix < GetSizeX() - 2) {
        for (uint z = 0; z < 2; ++z) {
            cellIn = block.cells[1][1][z];
            cellOut = cells[ix + 2][iy + 1][iz + z];
            PareEnergy(cellIn, cellOut, sumEnergy);
            cellIn = block.cells[1][0][z];
            cellOut = cells[ix + 2][iy][iz + z];
            PareEnergy(cellIn, cellOut, sumEnergy);
        }
    }
    //y gran 1
    if (iy > 0) {
        for (uint z = 0; z < 2; ++z) {
            cellIn = block.cells[1][0][z];
            cellOut = cells[ix + 1][iy - 1][iz + z];
            PareEnergy(cellIn, cellOut, sumEnergy);
            cellIn = block.cells[0][0][z];
            cellOut = cells[ix][iy - 1][iz + z];
            PareEnergy(cellIn, cellOut, sumEnergy);
        }
    }
    //y gran 2
    if (iy < GetSizeY() - 2) {
        for (uint z = 0; z < 2; ++z) {
            cellIn = block.cells[1][1][z];
            cellOut = cells[ix + 1][iy + 2][iz + z];
            PareEnergy(cellIn, cellOut, sumEnergy);
            cellIn = block.cells[0][1][z];
            cellOut = cells[ix][iy + 2][iz + z];
            PareEnergy(cellIn, cellOut, sumEnergy);
        }
    }
    //z gran 1
    if (iz > 0) {
        for (uint i = 0; i < 4; ++i) {
            cellIn = block.cells[indexF[i][0]][indexF[i][1]][0];
            cellOut = cells[ix + indexF[i][0]][iy + indexF[i][1]][iz - 1];
            PareEnergy(cellIn, cellOut, sumEnergy);
        }
    }
    //z gran 2
    if (iz < GetSizeZ() - 2) {
        for (uint i = 0; i < 4; ++i) {
            cellIn = block.cells[indexF[i][0]][indexF[i][1]][1];
            cellOut = cells[ix + indexF[i][0]][iy + indexF[i][1]][iz + 2];
            PareEnergy(cellIn, cellOut, sumEnergy);
        }
    }

    //inside block
    for (uint i = 0; i < 4; ++i) {
        for (uint z = 0; z < 2; ++z) {
            cellIn = block.cells[indexF[i][0]][indexF[i][1]][z];
            cellOut = block.cells[indexF[i + 1][0]][indexF[i + 1][1]][z];
            PareEnergyFull(cellIn, cellOut, sumEnergy);
        }
        {
            cellIn = block.cells[indexF[i][0]][indexF[i][1]][0];
            cellOut = block.cells[indexF[i][0]][indexF[i][1]][1];
            PareEnergyFull(cellIn, cellOut, sumEnergy);
        }
    }

    if (!moveForward || block.move[movement] > 0) {
        sumEnergy += block.move[movement] * *steamEnergy_;
    }

    return sumEnergy;
}

void Margolus::CreateRotateNotBlock(Block & block, cuint& ix, cuint& iy) {
    //block.rotate = UnMoved;
    //memset(block.move, 0, sizeof(block.move));
    for (uint x = 0; x < 2; ++x) {
        for (uint y = 0; y < 2; ++y) {
            block.cells[x][y] = cells[x + ix][y + iy][0];
        }
    }
}

void Margolus::CreateRotateNotBlock3D(Block3D & block, cuint& ix, cuint& iy, cuint& iz) {
    //block.rotate = UnMoved3;
    //memset(block.move, 0, sizeof(block.move));
    for (uint x = 0; x < 2; ++x) {
        for (uint y = 0; y < 2; ++y) {
            for (uint z = 0; z < 2; ++z) {
                block.cells[x][y][z] = cells[x + ix][y + iy][z + iz];
            }
        }
    }
}

void Margolus::CreateRotateBlock(Block & block, Rotate rotate, cuint& ix,
        cuint& iy, bool act, bool mod) {
    block.rotate = rotate;
    memset(block.move, 0, sizeof (block.move));
    // front
    for (uint x = 0; x < 2; ++x) {
        for (uint y = 0; y < 2; ++y) {
            block.cells[x][y].Clear();
            for (pSub & sub : cells[ix + x][iy + y][0].GetSubs()) {
                if (sub->GetType() == ACTIVE || sub->GetType() == MODIFIER) {
                    x == 0 ? --block.move[1] : --block.move[2];
                    y == 0 ? --block.move[3] : --block.move[4];
                }
            }
        }
    }

    if (rotate == ClockWice) {
        for (uint i = 0; i < 4; ++i) {
            Cell cell = cells[ix + indexF[i][0]][iy + indexF[i][1]][0];

            if (cell.HaveSolid()) {
                block.cells[indexF[i][0]][indexF[i][1]] = cell;
            } else {
                if (cell.HaveActive()) {
                    uint q = act ? 1 : 0;
                    for (pSub & sub : cell.GetSubs()) {
                        if (sub->GetType() == ACTIVE) {
                            block.cells[indexF[i + q][0]][indexF[i + q][1]].AddSub(sub);
                        }
                    }
                }

                if (cell.HaveModifier()) {
                    if (mod) {
                        for (pSub & sub : cell.GetSubs()) {
                            if (sub->GetType() == MODIFIER) {
                                int u = 1;
                                while (true) {
                                    if (cells[ix + indexF[i + u][0]][iy + indexF[i + u][1]][0].HaveSolid()) {
                                        ++u;
                                    } else {
                                        block.cells[indexF[i + u][0]][indexF[i + u][1]].AddSub(sub);
                                        break;
                                    }
                                }
                            }
                        }
                    } else {
                        for (pSub & sub : cell.GetSubs()) {
                            if (sub->GetType() == MODIFIER) {
                                block.cells[indexF[i][0]][indexF[i][1]].AddSub(sub);
                            }
                        }
                    }
                }
            }
        }
    } else {
        for (uint i = 0; i < 4; ++i) {
            Cell cell = cells[ix + indexB[i][0]][iy + indexB[i][1]][0];

            if (cell.HaveSolid()) {
                block.cells[indexB[i][0]][indexB[i][1]] = cell;
            } else {
                if (cell.HaveActive()) {
                    uint q = act ? 1 : 0;
                    for (pSub & sub : cell.GetSubs()) {
                        if (sub->GetType() == ACTIVE) {
                            block.cells[indexB[i + q][0]][indexB[i + q][1]].AddSub(sub);
                        }
                    }
                }

                if (cell.HaveModifier()) {
                    if (mod) {
                        for (pSub & sub : cell.GetSubs()) {
                            if (sub->GetType() == MODIFIER) {
                                int u = 1;
                                while (true) {
                                    if (cells[ix + indexB[i + u][0]][iy + indexB[i + u][1]][0].HaveSolid()) {
                                        ++u;
                                    } else {
                                        block.cells[indexB[i + u][0]][indexB[i + u][1]].AddSub(sub);
                                        break;
                                    }
                                }
                            }
                        }
                    } else {
                        for (pSub & sub : cell.GetSubs()) {
                            if (sub->GetType() == MODIFIER) {
                                block.cells[indexB[i][0]][indexB[i][1]].AddSub(sub);
                            }
                        }
                    }
                }
            }
        }
    }

    for (uint x = 0; x < 2; ++x) {
        for (uint y = 0; y < 2; ++y) {
            for (pSub & sub : block.cells[x][y].GetSubs()) {
                if (sub->GetType() == ACTIVE || sub->GetType() == MODIFIER) {
                    x == 0 ? ++block.move[1] : ++block.move[2];
                    y == 0 ? ++block.move[3] : ++block.move[4];
                }
            }
        }
    }
}

void Margolus::CreateRotateBlock3D(Block3D & block, Rotate3D rotate, cuint& ix,
        cuint& iy, cuint& iz, bool act, bool mod) {
    block.rotate = rotate;
    memset(block.move, 0, sizeof (block.move));
    // front
    for (uint x = 0; x < 2; ++x) {
        for (uint y = 0; y < 2; ++y) {
            for (uint z = 0; z < 2; ++z) {
                block.cells[x][y][z].Clear();
                for (pSub & sub : cells[ix + x][iy + y][iz + z].GetSubs()) {
                    if (sub->GetType() == ACTIVE || sub->GetType() == MODIFIER) {
                        x == 0 ? --block.move[1] : --block.move[2];
                        y == 0 ? --block.move[3] : --block.move[4];
                        z == 0 ? --block.move[5] : --block.move[6];
                    }
                }
            }
        }
    }

    switch (rotate) {
        case ClockWiceX:
            for (uint x = 0; x < 2; ++x) {
                for (uint i = 0; i < 4; ++i) {
                    Cell cell = cells[ix + x][iy + indexF[i][0]][iz + indexF[i][1]];

                    if (cell.HaveSolid()) {
                        block.cells[x][indexF[i][0]][indexF[i][1]] = cell;
                    } else {
                        if (cell.HaveActive()) {
                            uint q = act ? 1 : 0;
                            for (pSub & sub : cell.GetSubs()) {
                                if (sub->GetType() == ACTIVE) {
                                    block.cells[x][indexF[i + q][0]][indexF[i + q][1]].AddSub(sub);
                                }
                            }
                        }

                        if (cell.HaveModifier()) {
                            if (mod) {
                                for (pSub & sub : cell.GetSubs()) {
                                    if (sub->GetType() == MODIFIER) {
                                        int u = 1;
                                        while (true) {
                                            if (cells[ix + x][iy + indexF[i + u][0]][iz + indexF[i + u][1]].HaveSolid()) {
                                                ++u;
                                            } else {
                                                block.cells[x][indexF[i + u][0]][indexF[i + u][1]].AddSub(sub);
                                                break;
                                            }
                                        }
                                    }
                                }
                            } else {
                                for (pSub & sub : cell.GetSubs()) {
                                    if (sub->GetType() == MODIFIER) {
                                        block.cells[x][indexF[i][0]][indexF[i][1]].AddSub(sub);
                                    }
                                }
                            }
                        }
                    }
                }
            }
            break;
        case CounterClockWiceX:
            for (uint x = 0; x < 2; ++x) {
                for (uint i = 0; i < 4; ++i) {
                    Cell cell = cells[ix + x][iy + indexB[i][0]][iz + indexB[i][1]];

                    if (cell.HaveSolid()) {
                        block.cells[x][indexB[i][0]][indexB[i][1]] = cell;
                    } else {
                        if (cell.HaveActive()) {
                            uint q = act ? 1 : 0;
                            for (pSub & sub : cell.GetSubs()) {
                                if (sub->GetType() == ACTIVE) {
                                    block.cells[x][indexB[i + q][0]][indexB[i + q][1]].AddSub(sub);
                                }
                            }
                        }

                        if (cell.HaveModifier()) {
                            if (mod) {
                                for (pSub & sub : cell.GetSubs()) {
                                    if (sub->GetType() == MODIFIER) {
                                        int u = 1;
                                        while (true) {
                                            if (cells[ix + x][iy + indexB[i + u][0]][iz + indexB[i + u][1]].HaveSolid()) {
                                                ++u;
                                            } else {
                                                block.cells[x][indexB[i + u][0]][indexB[i + u][1]].AddSub(sub);
                                                break;
                                            }
                                        }
                                    }
                                }
                            } else {
                                for (pSub & sub : cell.GetSubs()) {
                                    if (sub->GetType() == MODIFIER) {
                                        block.cells[x][indexB[i][0]][indexB[i][1]].AddSub(sub);
                                    }
                                }
                            }
                        }
                    }
                }
            }
            break;
        case ClockWiceY:
            for (uint y = 0; y < 2; ++y) {
                for (uint i = 0; i < 4; ++i) {
                    Cell cell = cells[ix + indexF[i][1]][iy + y][iz + indexF[i][0]];

                    if (cell.HaveSolid()) {
                        block.cells[indexF[i][1]][y][indexF[i][0]] = cell;
                    } else {
                        if (cell.HaveActive()) {
                            uint q = act ? 1 : 0;
                            for (pSub & sub : cell.GetSubs()) {
                                if (sub->GetType() == ACTIVE) {
                                    block.cells[indexF[i + q][1]][y][indexF[i + q][0]].AddSub(sub);
                                }
                            }
                        }

                        if (cell.HaveModifier()) {
                            if (mod) {
                                for (pSub & sub : cell.GetSubs()) {
                                    if (sub->GetType() == MODIFIER) {
                                        int u = 1;
                                        while (true) {
                                            if (cells[ix + indexF[i + u][1]][iy + y][iz + indexF[i + u][0]].HaveSolid()) {
                                                ++u;
                                            } else {
                                                block.cells[indexF[i + u][1]][y][indexF[i + u][0]].AddSub(sub);
                                                break;
                                            }
                                        }
                                    }
                                }
                            } else {
                                for (pSub & sub : cell.GetSubs()) {
                                    if (sub->GetType() == MODIFIER) {
                                        block.cells[indexF[i][1]][y][indexF[i][0]].AddSub(sub);
                                    }
                                }
                            }
                        }
                    }
                }
            }
            break;
        case CounterClockWiceY:
            for (uint y = 0; y < 2; ++y) {
                for (uint i = 0; i < 4; ++i) {
                    Cell cell = cells[ix + indexB[i][1]][iy + y][iz + indexB[i][0]];

                    if (cell.HaveSolid()) {
                        block.cells[indexB[i][1]][y][indexB[i][0]] = cell;
                    } else {
                        if (cell.HaveActive()) {
                            uint q = act ? 1 : 0;
                            for (pSub & sub : cell.GetSubs()) {
                                if (sub->GetType() == ACTIVE) {
                                    block.cells[indexB[i + q][1]][y][indexB[i + q][0]].AddSub(sub);
                                }
                            }
                        }

                        if (cell.HaveModifier()) {
                            if (mod) {
                                for (pSub & sub : cell.GetSubs()) {
                                    if (sub->GetType() == MODIFIER) {
                                        int u = 1;
                                        while (true) {
                                            if (cells[ix + indexB[i + u][1]][iy + y][iz + indexB[i + u][0]].HaveSolid()) {
                                                ++u;
                                            } else {
                                                block.cells[indexB[i + u][1]][y][indexB[i + u][0]].AddSub(sub);
                                                break;
                                            }
                                        }
                                    }
                                }
                            } else {
                                for (pSub & sub : cell.GetSubs()) {
                                    if (sub->GetType() == MODIFIER) {
                                        block.cells[indexB[i][1]][y][indexB[i][0]].AddSub(sub);
                                    }
                                }
                            }
                        }
                    }
                }
            }
            break;
        case ClockWiceZ:
            for (uint z = 0; z < 2; ++z) {
                for (uint i = 0; i < 4; ++i) {
                    Cell cell = cells[ix + indexF[i][0]][iy + indexF[i][1]][iz + z];

                    if (cell.HaveSolid()) {
                        block.cells[indexF[i][0]][indexF[i][1]][z] = cell;
                    } else {
                        if (cell.HaveActive()) {
                            uint q = act ? 1 : 0;
                            for (pSub & sub : cell.GetSubs()) {
                                if (sub->GetType() == ACTIVE) {
                                    block.cells[indexF[i + q][0]][indexF[i + q][1]][z].AddSub(sub);
                                }
                            }
                        }

                        if (cell.HaveModifier()) {
                            if (mod) {
                                for (pSub & sub : cell.GetSubs()) {
                                    if (sub->GetType() == MODIFIER) {
                                        int u = 1;
                                        while (true) {
                                            if (cells[ix + indexF[i + u][0]][iy + indexF[i + u][1]][z].HaveSolid()) {
                                                ++u;
                                            } else {
                                                block.cells[indexF[i + u][0]][indexF[i + u][1]][z].AddSub(sub);
                                                break;
                                            }
                                        }
                                    }
                                }
                            } else {
                                for (pSub & sub : cell.GetSubs()) {
                                    if (sub->GetType() == MODIFIER) {
                                        block.cells[indexF[i][0]][indexF[i][1]][z].AddSub(sub);
                                    }
                                }
                            }
                        }
                    }
                }
            }
            break;
        case CounterClockWiceZ:
            for (uint z = 0; z < 2; ++z) {
                for (uint i = 0; i < 4; ++i) {
                    Cell cell = cells[ix + indexB[i][0]][iy + indexB[i][1]][iz + z];

                    if (cell.HaveSolid()) {
                        block.cells[indexB[i][0]][indexB[i][1]][z] = cell;
                    } else {
                        if (cell.HaveActive()) {
                            uint q = act ? 1 : 0;
                            for (pSub & sub : cell.GetSubs()) {
                                if (sub->GetType() == ACTIVE) {
                                    block.cells[indexB[i + q][0]][indexB[i + q][1]][z].AddSub(sub);
                                }
                            }
                        }

                        if (cell.HaveModifier()) {
                            if (mod) {
                                for (pSub & sub : cell.GetSubs()) {
                                    if (sub->GetType() == MODIFIER) {
                                        int u = 1;
                                        while (true) {
                                            if (cells[ix + indexB[i + u][0]][iy + indexB[i + u][1]][iz + z].HaveSolid()) {
                                                ++u;
                                            } else {
                                                block.cells[indexB[i + u][0]][indexB[i + u][1]][z].AddSub(sub);
                                                break;
                                            }
                                        }
                                    }
                                }
                            } else {
                                for (pSub & sub : cell.GetSubs()) {
                                    if (sub->GetType() == MODIFIER) {
                                        block.cells[indexB[i][0]][indexB[i][1]][z].AddSub(sub);
                                    }
                                }
                            }
                        }
                    }
                }
            }
            break;
    }

    for (uint x = 0; x < 2; ++x) {
        for (uint y = 0; y < 2; ++y) {
            for (uint z = 0; z < 2; ++z) {
                for (pSub & sub : block.cells[x][y][z].GetSubs()) {
                    if (sub->GetType() == ACTIVE || sub->GetType() == MODIFIER) {
                        x == 0 ? ++block.move[1] : ++block.move[2];
                        y == 0 ? ++block.move[3] : ++block.move[4];
                        z == 0 ? ++block.move[5] : ++block.move[6];
                    }
                }
            }
        }
    }
}

bool Margolus::CheckCanClockWice(cuint& ix, cuint& iy) const {
    for (uint i = 0; i < 4; ++i) {
        if (cells[ix + indexF[i][0]][iy + indexF[i][1]][0].HaveActive() &&
                cells[ix + indexF[i + 1][0]][iy + indexF[i + 1][1]][0].HaveSolid()) {
            return false;
        }
    }
    return true;
}

bool Margolus::CheckCanCounterClockWice(cuint& ix, cuint& iy) const {
    for (uint i = 0; i < 4; ++i) {
        if (cells[ix + indexF[i][0]][iy + indexF[i][1]][0].HaveSolid() &&
                cells[ix + indexF[i + 1][0]][iy + indexF[i + 1][1]][0].HaveActive()) {
            return false;
        }
    }
    return true;
}

bool Margolus::CheckCanRotate3D(Rotate3D rotate, cuint& ix, cuint& iy, cuint& iz) const {
    // check
    switch (rotate) {
        case ClockWiceX:
            for (uint x = 0; x < 2; ++x) {
                for (uint i = 0; i < 4; ++i) {
                    if (cells[ix + x][iy + indexF[i][0]][iz + indexF[i][1]].HaveActive() &&
                            cells[ix + x][iy + indexF[i + 1][0]][iz + indexF[i + 1][1]].HaveSolid()) {
                        return false;
                    }
                }
            }
            break;
        case CounterClockWiceX:
            for (uint x = 0; x < 2; ++x) {
                for (uint i = 0; i < 4; ++i) {
                    if (cells[ix + x][iy + indexF[i][0]][iz + indexF[i][1]].HaveSolid() &&
                            cells[ix + x][iy + indexF[i + 1][0]][iz + indexF[i + 1][1]].HaveActive()) {
                        return false;
                    }
                }
            }
            break;
        case ClockWiceY:
            for (uint y = 0; y < 2; ++y) {
                for (uint i = 0; i < 4; ++i) {
                    if (cells[ix + indexF[i][1]][iy + y][iz + indexF[i][0]].HaveActive() &&
                            cells[ix + indexF[i + 1][1]][iy + y][iz + indexF[i + 1][0]].HaveSolid()) {
                        return false;
                    }
                }
            }
            break;
        case CounterClockWiceY:
            for (uint y = 0; y < 2; ++y) {
                for (uint i = 0; i < 4; ++i) {
                    if (cells[ix + indexF[i][1]][iy + y][iz + indexF[i][0]].HaveSolid() &&
                            cells[ix + indexF[i + 1][1]][iy + y][iz + indexF[i + 1][0]].HaveActive()) {
                        return false;
                    }
                }
            }
            break;
        case ClockWiceZ:
            for (uint z = 0; z < 2; ++z) {
                for (uint i = 0; i < 4; ++i) {
                    if (cells[ix + indexF[i][0]][iy + indexF[i][1]][iz + z].HaveActive() &&
                            cells[ix + indexF[i + 1][0]][iy + indexF[i + 1][1]][iz + z].HaveSolid()) {
                        return false;
                    }
                }
            }
            break;
        case CounterClockWiceZ:
            for (uint z = 0; z < 2; ++z) {
                for (uint i = 0; i < 4; ++i) {
                    if (cells[ix + indexF[i][0]][iy + indexF[i][1]][iz + z].HaveSolid() &&
                            cells[ix + indexF[i + 1][0]][iy + indexF[i + 1][1]][iz + z].HaveActive()) {
                        return false;
                    }
                }
            }
            break;
    }
    return true;
}

void Margolus::ChangeBlock(const Block& block, cuint& ix, cuint& iy) {
    for (uint x = 0; x < 2; ++x) {
        for (uint y = 0; y < 2; ++y) {
            cells[ix + x][iy + y][0] = block.cells[x][y];
        }
    }
}

void Margolus::ChangeBlock(const Block3D& block, cuint& ix, cuint& iy, cuint& iz) {
    for (uint x = 0; x < 2; ++x) {
        for (uint y = 0; y < 2; ++y) {
            for (uint z = 0; z < 2; ++z) {
                cells[ix + x][iy + y][iz + z] = block.cells[x][y][z];
            }
        }
    }
}
