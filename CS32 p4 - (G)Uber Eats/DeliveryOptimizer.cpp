#include "provided.h"
#include <vector>
using namespace std;

class DeliveryOptimizerImpl
{
public:
    DeliveryOptimizerImpl(const StreetMap* sm);
    ~DeliveryOptimizerImpl();
    void optimizeDeliveryOrder(
        const GeoCoord& depot,
        vector<DeliveryRequest>& deliveries,
        double& oldCrowDistance,
        double& newCrowDistance) const;
};

//this function returns closest DeliveryRequest then deletes it from list
DeliveryRequest closest(vector<DeliveryRequest>& list, const GeoCoord& source)
{
    vector<DeliveryRequest>::iterator it = list.begin();
    DeliveryRequest closest = *it;

    while (it != list.end()) //finds closest delivery location
    {
        if (distanceEarthMiles(source, it->location) < distanceEarthMiles(source, closest.location))
        {
            closest = *it;
        }
        it++;
    }

    it = list.begin(); //gets position of the closest DeliveryRequest
    while (it->location != closest.location)
    {
        it++;
    }
    list.erase(it); //deletes entry from list at that position (ensures no repeats)

    return closest;
}

DeliveryOptimizerImpl::DeliveryOptimizerImpl(const StreetMap* sm)
{
}

DeliveryOptimizerImpl::~DeliveryOptimizerImpl()
{
}

void DeliveryOptimizerImpl::optimizeDeliveryOrder(
    const GeoCoord& depot,
    vector<DeliveryRequest>& deliveries,
    double& oldCrowDistance,
    double& newCrowDistance) const
{
    oldCrowDistance = 0;
    newCrowDistance = 0;

    //calculates initial crow distance
    vector<DeliveryRequest>::iterator first = deliveries.begin();
    vector<DeliveryRequest>::iterator second = first;
    second++;

    oldCrowDistance += distanceEarthMiles(depot, first->location); //accounts for distance from depot to first delivery

    while (second != deliveries.end())
    {
        oldCrowDistance += distanceEarthMiles(first->location, second->location);
        first++; second++;
    }

    //algorithm starts at depot, and then continues to add the shortest delivery to next
    vector<DeliveryRequest> newOrderRequests;
    newOrderRequests.push_back(closest(deliveries, depot));
    while (!deliveries.empty())
    {
        newOrderRequests.push_back(closest(deliveries, newOrderRequests.front().location));
    }
    deliveries = newOrderRequests;

    //calculates new crow distance
    vector<DeliveryRequest>::iterator one = deliveries.begin();
    vector<DeliveryRequest>::iterator two = one;
    two++;

    oldCrowDistance += distanceEarthMiles(depot, one->location); //accounts for distance from depot to first delivery

    while (two != deliveries.end())
    {
        oldCrowDistance += distanceEarthMiles(one->location, two->location);
        one++; two++;
    }
}

//******************** DeliveryOptimizer functions ****************************

// These functions simply delegate to DeliveryOptimizerImpl's functions.
// You probably don't want to change any of this code.

DeliveryOptimizer::DeliveryOptimizer(const StreetMap* sm)
{
    m_impl = new DeliveryOptimizerImpl(sm);
}

DeliveryOptimizer::~DeliveryOptimizer()
{
    delete m_impl;
}

void DeliveryOptimizer::optimizeDeliveryOrder(
    const GeoCoord& depot,
    vector<DeliveryRequest>& deliveries,
    double& oldCrowDistance,
    double& newCrowDistance) const
{
    return m_impl->optimizeDeliveryOrder(depot, deliveries, oldCrowDistance, newCrowDistance);
}