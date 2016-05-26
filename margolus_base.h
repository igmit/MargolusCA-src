#ifndef MARGOLUS_H
#define	MARGOLUS_H

#include <string.h>
#include <cmath>
#include "cellularautomata.h"
#include <iostream>
#include <fstream>
#include <sstream>

#include <cereal/archives/json.hpp>
#include <cereal/access.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/types/common.hpp>
#include <cereal/types/array.hpp>

enum Rotate {
    UnMoved,
    ClockWice,
    CounterClockWice
};

enum Rotate3D {
    UnMoved3,
    ClockWiceX,
    CounterClockWiceX, 
    ClockWiceY,
    CounterClockWiceY,
    ClockWiceZ,
    CounterClockWiceZ
};

struct Energy {
    ~Energy() {
        /*delete energyH;
        energyH = 0;
        delete energyS;
        energyS = 0;*/
    }
    string name1;
    string name2;
    
    double* energyH;
    double* energyS;
    double energy;
    
    void UpdateEnergy(double T) {
        energy = *energyH - T * *energyS;
    }
    
    template<class Archive>
    void serialize(Archive & archive)
    {
      archive(
              cereal::make_nvp("name1", name1),
              cereal::make_nvp("name2", name2),
              cereal::make_nvp("energy", energy),
              cereal::make_nvp("H", *energyH),
              cereal::make_nvp("S", *energyS)
        );
    }
};

struct simpleBlock {
    double energy;
    int move[7];
    
    
};

struct Block : public simpleBlock {
    Cell cells[2][2] = {{Cell(), Cell()},
                        {Cell(), Cell()}};
    Rotate rotate;
    
    template<class Archive>
    void serialize(Archive & archive) {

        if (energy > 0.00000001) {

            string rot;
            switch (rotate) {
                case UnMoved: rot = "um";
                    break;
                case ClockWice: rot = "cw";
                    break;
                case CounterClockWice: rot = "ccw";
                    break;
                default: rot = "";
            }

            if (rot != "") {
                
                archive(cereal::make_nvp("cells", cells),
                        cereal::make_nvp("rotate", rot),

                        cereal::make_nvp("energy", energy)
                        );
            }

        }
    }
};

struct Block3D : public simpleBlock {
    Cell cells[2][2][2] = {{{Cell(), Cell()},
                        {Cell(), Cell()}},
                        {{Cell(), Cell()},
                        {Cell(), Cell()}}};
    Rotate3D rotate;
};

struct DataObj {
    int x;
    int y;
    vector<Block> blocks;
    int selectedBlock;
    DataObj(int x, int y, vector<Block> blocks, int selectedBlock) :
        x(x), y(y), blocks(blocks), selectedBlock(selectedBlock) {
    }

    template<class Archive>
    void serialize(Archive & archive)
    {
      
      archive(
              cereal::make_nvp("x", x),
              cereal::make_nvp("y", y),
              cereal::make_nvp("blocks", blocks),
              cereal::make_nvp("selectedBlock", selectedBlock)
        );
    }
};

struct StatisticsRecord{
    int iteration;
    int solidCount = 0;
    int boundaryLayerCount = 0;
    int adsorbedCount = 0;
    int activeCount = 0;
    
    StatisticsRecord(int iteration, int solidCount, int boundaryLayerCount, int adsorbedCount, int activeCount) :
        iteration(iteration), solidCount(solidCount), boundaryLayerCount(boundaryLayerCount), adsorbedCount(adsorbedCount), activeCount(activeCount) {
    }

};

struct FieldCell {
    int x;
    int y;
    Cell cell;

    FieldCell(int x, int y, Cell cell) :
    x(x), y(y), cell(cell) {
    }

    template<class Archive>
    void serialize(Archive & archive) {

        archive(
                cereal::make_nvp("x", x),
                cereal::make_nvp("y", y),
                cereal::make_nvp("types", cell)
                );
    }

};


class Margolus : public CellularAutomata {
public:
    Margolus();
    Margolus(Sizes sizes);
    Margolus(cuint& sizeX, cuint& sizeY, cuint sizeZ = 1);
    ~Margolus();
    
    string path;
    int subIteration;
    bool saveFlag = false;
    
    bool saveJsonFlag = true;
    
    vector<StatisticsRecord> statisticsData;
    
    void saveField(int dx, int dy, bool init);

    void Calculation(cuint& dx, cuint& dy);
    void Calculation(cuint& dx, cuint& dy, cuint& dz);
    
    void SetTempPtr(double* pointer);
    void SetTemperature(double Temp);
    inline double GetTemperature() const { return *T; }
    void SetSteamPtr(double* pointer);
    void SetSteamEnergy(double steamEnergy);
    inline double GetSteamEnergy() const { return *steamEnergy_; }
    
    void SetEnergy(const string& name1, const string& name2, double* energyH,
        double* energyS);
    void AddEnergy(const string& name1, const string& name2, double* energyH,
        double* energyS);
    void AddEnergy(Energy & en);
    
    vector<Energy> GetEnergies() const;
    vector<Energy> GetEnergiesCell() const;
    double GetEnergy(const string& name1, const string& name2);
    double GetEnergyH(const string& name1, const string& name2); 
    double GetEnergyS(const string& name1, const string& name2);

    void SetEnergyCell(const string& name1, const string& name2, double* energyH,
        double* energyS);
    void AddEnergyCell(const string& name1, const string& name2, double* energyH,
        double* energyS);
    void AddEnergyCell(Energy & en);
    double GetEnergyCell(const string& name1, const string& name2);
    double GetEnergyHCell(const string& name1, const string& name2); 
    double GetEnergySCell(const string& name1, const string& name2);
    
    void SetMoveModifier(bool value);
    void ClearEnergy();
    void UpdateEnergies();
    void PrintParameters() const;
    
    uint movement = 0;
    bool finished = false;
    //void PrintBlocks();
    
    template<class Archive>
    void serialize(Archive & archive)
    {
      archive();
    }
    
protected:
    void PrintBlock(Block & block) const;
    bool CheckEmpty(cuint& ix, cuint& iy) const;
    bool CheckEmpty(cuint& ix, cuint& iy, cuint& iz) const;
    bool CheckMod(cuint& ix, cuint& iy) const;
    bool CheckMod(cuint& ix, cuint& iy, cuint& iz) const;
    bool CheckActive(cuint& ix, cuint& iy) const;
    bool CheckActive(cuint& ix, cuint& iy, cuint& iz) const;
    void PareEnergy(Cell& cellIn, Cell& cellOut, double& energy);
    void PareEnergyFull(Cell& cellIn, Cell& cellOut, double& energy);
    
    virtual double CalculationBlockEnergy(const Block& block, cuint& ix, cuint& iy);
    virtual double CalculationBlockEnergy(const Block3D& block, cuint& ix, cuint& iy, cuint& iz);
    
    void CreateRotateNotBlock(Block & block, cuint& ix, cuint& iy);
    void CreateRotateNotBlock3D(Block3D & block, cuint& ix, cuint& iy, cuint& iz);
    void CreateRotateBlock(Block & block, Rotate rotate, cuint& ix, cuint& iy,
        bool act = true, bool mod = false);
    void CreateRotateBlock3D(Block3D & block, Rotate3D rotate, cuint& ix,
        cuint& iy, cuint& iz, bool act = true, bool mod = false);
    bool CheckCanClockWice(cuint& ix, cuint& iy) const;
    bool CheckCanCounterClockWice(cuint& ix, cuint& iy) const;
    bool CheckCanRotate3D(Rotate3D rotate, cuint& ix, cuint& iy,
        cuint& iz) const;
    //void CreateBlock(list<Block>(& block), int& ix, int& iy);
    void ChangeBlock(const Block& block, cuint& ix, cuint& iy);
    void ChangeBlock(const Block3D& block, cuint& ix, cuint& iy, cuint& iz);
    
    int indexF[9][2] = {{0, 0}, {1, 0}, {1, 1}, {0, 1}, {0, 0}, {1, 0}, {1, 1},
        {0, 1}, {0, 0}};
    int indexB[9][2] = {{0, 0}, {0, 1}, {1, 1}, {1, 0}, {0, 0}, {0, 1}, {1, 1},
        {1, 0}, {0, 0}};
    //int shifts[4][2] = {{-1, 0}, {0, 1}, {1, 0}, {0, -1}};
    Block blocks[7];
    Block3D blocks3D[19];
    uint blockSize = 0;
    uint blockSize3D = 0;
    double* steamEnergy_ = 0;
    vector<Energy> energy;
    vector<Energy> energyCell;
    const double R = 8.3144598;
    double* T = 0;
    bool modifierMove = false;
    bool moveForward  = false;
};

#endif	/* MARGOLUS_H */
