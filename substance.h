#ifndef SUBSTANCE_H
#define	SUBSTANCE_H

#include <iostream>
#include <vector>



using namespace std;

enum SUBTYPE {
    ACTIVE      = 1,
    SOLID       = 2,
    MODIFIER    = 3
};

enum LOCATION {
    randomF     = 0,
    topX        = 1,
    bottomX     = 2,
    topY        = 3,
    bottomY     = 4,
    topZ        = 5,
    bottomZ     = 6,
    adsorbed    = 7,
    outside     = 8
};

struct locate {
    locate(LOCATION loc = randomF, double* cons = new double(0.0))
            : location(loc), consist (cons) { }
    
    LOCATION    location;
    double*     consist;
};

struct XColor {
    XColor(int r = 0, int g = 0, int b = 0, int a = 255)
        : r(r), g(g), b(b), a(a) { }
    
    int r;
    int g;
    int b;
    int a;
};

/* Abstract class > Substance */

class Substance {
public:
    Substance(const string& name = "none", const XColor color = XColor(),
            const vector<locate>& locations = vector<locate>())
            : name(name), color(color), location(locations) { 
                //for (uint i = 0; i < locations.size(); ++i) {
                //    location.push_back(locate(locations[i].location, locations[i].consist));
                //}
                //L(locations) ;
            }
    virtual ~Substance() { }
    
    virtual SUBTYPE GetType() const = 0;
    string GetName() const { return name; }
    uint GetFillCount() const { return fillCount; }
    void SetFillCount(uint count) { fillCount = count; }
    XColor GetColor() const { return color; }
    vector<locate> GetLocations() const { return location; }
    string texture;
    
protected:
    string name;
    uint fillCount = 0;
    vector<locate> location;
    XColor color;
};

typedef Substance* pSub;

class Active : public Substance {
public:
    Active(const string& name = "acte", const XColor color = XColor(255),
            const vector<locate>& locations = vector<locate>()) :
            Substance(name, color, locations) { }
    ~Active() { }
    
    inline SUBTYPE GetType() const { return ACTIVE; }
};

class Modifier : public Substance {
public:
    Modifier(const string& name = "mode", const XColor color = XColor(0, 255),
            const vector<locate>& locations = vector<locate>()) :
            Substance(name, color, locations) { }
    ~Modifier() { }
    
    inline SUBTYPE GetType() const { return MODIFIER; }
};

class Solid : public Substance {
public:
    Solid(const string& name = "sole", const XColor color = XColor(0, 0, 255),
            const vector<locate>& locations = vector<locate>()) :
            Substance(name, color, locations) { }
    ~Solid() { }
    
    inline SUBTYPE GetType() const { return SOLID; }
};

#endif	/* SUBSTANCE_H */
