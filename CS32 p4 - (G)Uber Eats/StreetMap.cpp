#include "provided.h"
#include "ExpandableHashMap.h"
#include <fstream>
#include <string>
#include <vector>
#include <functional>
using namespace std;

unsigned int hasher(const GeoCoord& g)
{
    return std::hash<string>()(g.latitudeText + g.longitudeText);
}

class StreetMapImpl
{
public:
    StreetMapImpl();
    ~StreetMapImpl();
    bool load(string mapFile);
    bool getSegmentsThatStartWith(const GeoCoord& gc, vector<StreetSegment>& segs) const;

private:
    ExpandableHashMap<GeoCoord, vector<StreetSegment*>> coordsToSegments;
};

StreetMapImpl::StreetMapImpl()
{

}

StreetMapImpl::~StreetMapImpl()
{

}

bool StreetMapImpl::load(string mapFile)
{
    ifstream infile(mapFile);    // infile is a name of our choosing
    if (!infile)		        // Did opening the file fail?
        return false;

    string line;
    string name = "";
    int numSegs = 0;
    string segs;
    while (getline(infile, line)) //iterates through each line of input file
    {
        bool nameLine = false; //sets default setting to not a street name line
        for (int k = 0; k < line.size(); k++)
        {
            if (isalpha(line[k]))
            {
                infile >> name; //if there is a letter in the line, it is a street name line
                nameLine = true;
                break;
            }
        }

        if (nameLine)  //if  the line examined is a street name, continue
            continue;

        if (line.size() <= 2) //if size of line is <= 2 characters, it is the line telling how many segments on the street
        {
            infile >> segs;
            stringstream stringToInt(segs);
            stringToInt >> numSegs;
            continue; //take note of how many segs to expect, and continue
        }

        if (numSegs > 0) //for as many lines as there are segments, add this segment to a vector of segments for each point
        {
            numSegs--;

            string startLong;
            string startLat;
            string endLong;
            string endLat;

            infile >> startLat >> startLong >> endLat >> endLong;

            //associates segment to start geocoord
            vector<StreetSegment*> forwardSegVector;
            vector<StreetSegment*>* foundVector = coordsToSegments.find(GeoCoord(startLat, startLong));

            if (foundVector != nullptr) //if there are already streetSegments associated with geoCoord
                forwardSegVector = *foundVector;  //copy existing segments into forwardSegVector

            StreetSegment* fSegPtr = new StreetSegment(GeoCoord(startLat, startLong), GeoCoord(endLat, endLong), name);
            forwardSegVector.push_back(fSegPtr);  //push new streetSegment into vector

            coordsToSegments.associate(GeoCoord(startLat, startLong), forwardSegVector);

            //associates segment to end geocoord
            vector<StreetSegment*> reverseSegVector;
            vector<StreetSegment*>* foundVector2 = coordsToSegments.find(GeoCoord(endLat, endLong));

            if (foundVector2 != nullptr) //if there are already streetSegments associated with geoCoord
                reverseSegVector = *foundVector2;  //copy existing segments into forwardSegVector

            StreetSegment* rSegPtr = new StreetSegment(GeoCoord(endLat, endLong), GeoCoord(startLat, startLong), name);
            reverseSegVector.push_back(rSegPtr);  //push new streetSegment into vector

            coordsToSegments.associate(GeoCoord(endLat, endLong), reverseSegVector);
        }
    }
    return true;
}

bool StreetMapImpl::getSegmentsThatStartWith(const GeoCoord& gc, vector<StreetSegment>& segs) const
{
    const vector<StreetSegment*>* foundVectorPtr = coordsToSegments.find(gc);
    if (foundVectorPtr == nullptr)
        return false;

    vector<StreetSegment*> foundVector = *foundVectorPtr;

    for (vector<StreetSegment*>::iterator it = foundVector.begin();
        it != foundVector.end(); it++) //add all associated segments to passed-in segs vector
    {
        segs.push_back(**it);
    }

    return true;
}

//******************** StreetMap functions ************************************

// These functions simply delegate to StreetMapImpl's functions.
// You probably don't want to change any of this code.

StreetMap::StreetMap()
{
    m_impl = new StreetMapImpl;
}

StreetMap::~StreetMap()
{
    delete m_impl;
}

bool StreetMap::load(string mapFile)
{
    return m_impl->load(mapFile);
}

bool StreetMap::getSegmentsThatStartWith(const GeoCoord& gc, vector<StreetSegment>& segs) const
{
    return m_impl->getSegmentsThatStartWith(gc, segs);
}