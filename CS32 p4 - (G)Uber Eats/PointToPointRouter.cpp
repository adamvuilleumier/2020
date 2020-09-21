#include "provided.h"
#include "ExpandableHashMap.h"
#include <queue>
#include <list>
using namespace std;

class PointToPointRouterImpl
{
public:
    PointToPointRouterImpl(const StreetMap* sm);
    ~PointToPointRouterImpl();
    DeliveryResult generatePointToPointRoute(
        const GeoCoord& start,
        const GeoCoord& end,
        list<StreetSegment>& route,
        double& totalDistanceTravelled) const;
private:
    const StreetMap* map;
};

PointToPointRouterImpl::PointToPointRouterImpl(const StreetMap* sm)
{
    map = sm;
}

PointToPointRouterImpl::~PointToPointRouterImpl()
{

}

DeliveryResult PointToPointRouterImpl::generatePointToPointRoute(
    const GeoCoord& start,
    const GeoCoord& end,
    list<StreetSegment>& route,
    double& totalDistanceTravelled) const
{
    ExpandableHashMap<GeoCoord, StreetSegment> pointToPrev; //maps coord to the streetSegment before it

    totalDistanceTravelled = 0;
    route.clear(); //resets distance travelled and route

    vector<StreetSegment> segmentChoices;
    if (!map->getSegmentsThatStartWith(start, segmentChoices)
        || !map->getSegmentsThatStartWith(end, segmentChoices))
        return BAD_COORD; //returns bad coord if start or end weren't loaded into hashmap

    queue<GeoCoord> path;
    path.push(start);

    while (path.front() != end) //creates a bunch of paths until it reaches end
    {
        if (path.empty()) //if the queue has run out of options to try, no route
            return NO_ROUTE;

        GeoCoord current = path.front();
        path.pop();

        map->getSegmentsThatStartWith(current, segmentChoices);

        bool itemSwap = true;
        while (itemSwap)
        {
            vector<StreetSegment>::iterator next = segmentChoices.begin();
            vector<StreetSegment>::iterator it = next;
            next++;
            itemSwap = false;

            while (next != segmentChoices.end()) //bubble sort by distance from end of segment to end destination
            {
                if (distanceEarthMiles(it->end, end) > distanceEarthMiles(next->end, end))
                {
                    StreetSegment temp = (*it); //swap the two segments if out of order
                    (*it) = (*next);
                    (*next) = temp;
                    itemSwap = true;
                }
                it++; next++;
            }
        }

        for (auto it = segmentChoices.begin(); it != segmentChoices.end(); it++)
        {
            path.push(it->end);
            pointToPrev.associate(it->end, *it);
        }
    }

    GeoCoord curr = end;  //starting from the end, traces path back to start, chooses one of the paths made above
    while (curr != start) //adds segments to route and increments distance each iteration
    {
        const StreetSegment* prevPtr = pointToPrev.find(curr);

        if (prevPtr == nullptr)  //this shouldnt happen, but just to avoid dereferencing nullptr
            return NO_ROUTE;

        route.push_front(*prevPtr);

        totalDistanceTravelled += distanceEarthMiles(prevPtr->start, curr);
        curr = prevPtr->start; //sets new curr GeoCoord to start of prev segment
    }

    return DELIVERY_SUCCESS;
}

//******************** PointToPointRouter functions ***************************

// These functions simply delegate to PointToPointRouterImpl's functions.
// You probably don't want to change any of this code.

PointToPointRouter::PointToPointRouter(const StreetMap* sm)
{
    m_impl = new PointToPointRouterImpl(sm);
}

PointToPointRouter::~PointToPointRouter()
{
    delete m_impl;
}

DeliveryResult PointToPointRouter::generatePointToPointRoute(
    const GeoCoord& start,
    const GeoCoord& end,
    list<StreetSegment>& route,
    double& totalDistanceTravelled) const
{
    return m_impl->generatePointToPointRoute(start, end, route, totalDistanceTravelled);
}