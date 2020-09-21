#include "provided.h"
#include <vector>
using namespace std;

class PointToPointRouter;
class DeliveryOptimizer;

class DeliveryPlannerImpl
{
public:
    DeliveryPlannerImpl(const StreetMap* sm);
    ~DeliveryPlannerImpl();
    DeliveryResult generateDeliveryPlan(
        const GeoCoord& depot,
        const vector<DeliveryRequest>& deliveries,
        vector<DeliveryCommand>& commands,
        double& totalDistanceTravelled) const;
private:
    const StreetMap* map;
    PointToPointRouter* p2p;
    DeliveryOptimizer* optimizer;
};

string angleToDir(double angle);

DeliveryPlannerImpl::DeliveryPlannerImpl(const StreetMap* sm)
{
    p2p = new PointToPointRouter(sm);
    optimizer = new DeliveryOptimizer(sm);
}

DeliveryPlannerImpl::~DeliveryPlannerImpl()
{
}

DeliveryResult DeliveryPlannerImpl::generateDeliveryPlan(
    const GeoCoord& depot,
    const vector<DeliveryRequest>& deliveries,
    vector<DeliveryCommand>& commands,
    double& totalDistanceTravelled) const
{
    list<StreetSegment> route;  //resets distance travelled and creates list to put street segments in
    totalDistanceTravelled = 0;

    vector<DeliveryRequest> newDeliveryOrder = deliveries; //reorders delivery order in new vector
    double oldCrow; double newCrow;
    optimizer->optimizeDeliveryOrder(depot, newDeliveryOrder, oldCrow, newCrow);

    for (int i = 0; i < deliveries.size(); i++)
    {
        DeliveryRequest nextDelivery = newDeliveryOrder[i]; //focuses on one delivery at a time
        double distThisLeg = 0; //tracks distance travelled for each delivery

        DeliveryResult returnval = p2p->generatePointToPointRoute(depot,
            nextDelivery.location, route, distThisLeg);
        totalDistanceTravelled += distThisLeg; //fills route with path of streetSegments

        if (returnval == BAD_COORD)
            return BAD_COORD;
        if (returnval == NO_ROUTE)
            return NO_ROUTE;

        list<StreetSegment>::iterator it = route.begin();
        list<StreetSegment>::iterator next = it;
        next++;
        //sets iterators to first and second segments of route

        DeliveryCommand dcFirst;
        dcFirst.initAsProceedCommand(angleToDir(angleOfLine(*it)), it->name, distanceEarthMiles(it->start, it->end));
        commands.push_back(dcFirst);
        it++; next++;
        //first command is always proceed, so now iterators point to 2nd and 3rd segments

        while (next != route.end())
        {
            DeliveryCommand dc;
            double betweenAngle = angleBetween2Lines(*it, *next);
            string dir = angleToDir(angleOfLine(*it));

            if (betweenAngle == 0 || it->name == next->name) //if there is no turn, give proceed command
            {
                double dist = distanceEarthMiles(it->start, next->end);

                if (commands.back().streetName() == it->name)
                {
                    commands.back().increaseDistance(dist); //if the last command was proceed, just add distance
                    continue;
                }
                else
                    dc.initAsProceedCommand(dir, it->name, dist);
            }
            else if (betweenAngle >= 1 && betweenAngle < 180)
                dc.initAsTurnCommand("left", next->name);
            else if (betweenAngle >= 180 && betweenAngle <= 359)
                dc.initAsTurnCommand("right", next->name);

            commands.push_back(dc);
            it++;  next++;
        }

        //the path is at the delivery location, just need to give deliver command
        DeliveryCommand dcLast;
        dcLast.initAsDeliverCommand(deliveries[i].item);
        commands.push_back(dcLast);
    }
    return DELIVERY_SUCCESS;
}

string angleToDir(double angle)
{
    if (angle >= 0 && angle < 22.5)
        return "east";
    if (angle >= 22.5 && angle < 67.5)
        return "northeast";
    if (angle >= 67.5 && angle < 112.5)
        return "north";
    if (angle >= 112.5 && angle < 157.5)
        return "northwest";
    if (angle >= 157.5 && angle < 202.5)
        return "west";
    if (angle >= 202.5 && angle < 247.5)
        return "southwest";
    if (angle >= 247.5 && angle < 292.5)
        return "south";
    if (angle >= 292.5 && angle < 337.5)
        return "southeast";
    return "east";
}

//******************** DeliveryPlanner functions ******************************

// These functions simply delegate to DeliveryPlannerImpl's functions.
// You probably don't want to change any of this code.

DeliveryPlanner::DeliveryPlanner(const StreetMap* sm)
{
    m_impl = new DeliveryPlannerImpl(sm);
}

DeliveryPlanner::~DeliveryPlanner()
{
    delete m_impl;
}

DeliveryResult DeliveryPlanner::generateDeliveryPlan(
    const GeoCoord& depot,
    const vector<DeliveryRequest>& deliveries,
    vector<DeliveryCommand>& commands,
    double& totalDistanceTravelled) const
{
    return m_impl->generateDeliveryPlan(depot, deliveries, commands, totalDistanceTravelled);
}