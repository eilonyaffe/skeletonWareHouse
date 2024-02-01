#include "../include/Order.h"
#include "../include/Volunteer.h"

#include <string>
#include <vector>
#include <iostream>

using namespace std; //TODO delete later?

//volunteer implementation
Volunteer::Volunteer(int id, const string &name): completedOrderId(-1), activeOrderId(-1), id(id), name(name){}

int Volunteer::getId() const{
    return id;
}

const string &Volunteer::getName() const{
    return name;
}

int Volunteer::getActiveOrderId() const{
    return activeOrderId;
}

int Volunteer::getCompletedOrderId() const{
    return completedOrderId;
}

void Volunteer::resetCompletedOrderId(){
    Volunteer::completedOrderId = -1;
}

bool Volunteer::isBusy() const{
    return (activeOrderId != -1);
}

//TODO: need to think how to construct new volunteer, assign him a task at the same time, and commit step


//CollectorVolunteer implementation
CollectorVolunteer::CollectorVolunteer(int id, const string &name, int coolDown): Volunteer(id,name), coolDown(coolDown), timeLeft(-1){}

CollectorVolunteer *CollectorVolunteer::clone() const{
    return new CollectorVolunteer(*this);
}

void CollectorVolunteer::step(){ //TODO in warehouse.cpp: if not busy, assigned to new order. also make sure to call this only if busy. it suits only for collectors with an order
    bool completedTask = this->decreaseCoolDown();
    if(completedTask){
        completedOrderId = activeOrderId;
        activeOrderId = -1; //TODO need to write = NO_ORDER ?? or -1 is enough?
    }
}

int CollectorVolunteer::getCoolDown() const{
    return CollectorVolunteer::coolDown;
}

int CollectorVolunteer::getTimeLeft() const{
    return timeLeft;
}

bool CollectorVolunteer::decreaseCoolDown(){ 
    timeLeft--;
    if(timeLeft == 0){
        return true;
    }
    else{
        return false;
    }
}

bool CollectorVolunteer::hasOrdersLeft() const{
    return true;
}

bool CollectorVolunteer::canTakeOrder(const Order &order) const{ //TODO wonder why needed order object here. maybe only needed for limited or driver
    return (!(this->isBusy()));
}

void CollectorVolunteer::acceptOrder(const Order &order){ //TODO logicallu, should make sure to first "canTakeOrder", maybe do it from warehouse.cpp
    CollectorVolunteer::timeLeft = CollectorVolunteer::coolDown;
    CollectorVolunteer::activeOrderId = order.getId();
}

string CollectorVolunteer::toString() const{
    string info = "id is: " + to_string(this->getId()) + " name is: " + this->getName() + " cooldown is: " + to_string(this->getCoolDown()); 
    return info; //TODO check what should i return
}

string CollectorVolunteer::timeOrDistLeft() const{
    return to_string(this->getTimeLeft());
}

string CollectorVolunteer::numOrdsLeft() const{
    return "No Limit";
}

string CollectorVolunteer::volunteerType() const{
    return "Collector";
}


//LimitedCollectorVolunteer implementation
LimitedCollectorVolunteer::LimitedCollectorVolunteer(int id, const string &name, int coolDown ,int maxOrders): CollectorVolunteer(id,name,coolDown), maxOrders(maxOrders), ordersLeft(maxOrders){}

LimitedCollectorVolunteer *LimitedCollectorVolunteer::clone() const{
    return new LimitedCollectorVolunteer(*this);
}

bool LimitedCollectorVolunteer::hasOrdersLeft() const{
    return (LimitedCollectorVolunteer::ordersLeft > 0); //has 1 or more
}

bool LimitedCollectorVolunteer::canTakeOrder(const Order &order) const{ 
    return (CollectorVolunteer::canTakeOrder(order) && this->hasOrdersLeft());
}

void LimitedCollectorVolunteer::acceptOrder(const Order &order){ //TODO should first check if can take order
    CollectorVolunteer::acceptOrder(order);
    LimitedCollectorVolunteer::ordersLeft--;
}

int LimitedCollectorVolunteer::getMaxOrders() const{
    return LimitedCollectorVolunteer::maxOrders;
}

int LimitedCollectorVolunteer::getNumOrdersLeft() const{
    return LimitedCollectorVolunteer::ordersLeft;
}

string LimitedCollectorVolunteer::toString() const{
    string info = "id is: " + to_string(this->getId()) + " name is: " + this->getName() + " cooldown is: " + to_string(this->getCoolDown()) + " maxorders is: " + to_string(this->getMaxOrders()); 
    return info; //TODO check what should i return
}

string LimitedCollectorVolunteer::numOrdsLeft() const{
    return to_string(this->getNumOrdersLeft());
}


//DriverVolunteer implementation
DriverVolunteer::DriverVolunteer(int id, const string &name, int maxDistance, int distancePerStep): Volunteer(id,name), maxDistance(maxDistance), distancePerStep(distancePerStep), distanceLeft(-1){}

DriverVolunteer *DriverVolunteer::clone() const{
    return new DriverVolunteer(*this);
}

int DriverVolunteer::getDistanceLeft() const{
    return DriverVolunteer::distanceLeft;
}

int DriverVolunteer::getMaxDistance() const{
    return DriverVolunteer::maxDistance;
}

int DriverVolunteer::getDistancePerStep() const{
    return DriverVolunteer::distancePerStep;
}

bool DriverVolunteer::decreaseDistanceLeft(){
    DriverVolunteer::distanceLeft -=  DriverVolunteer::distancePerStep;
    return (DriverVolunteer::distanceLeft <= 0);
}

bool DriverVolunteer::hasOrdersLeft() const{
    return true;
}

bool DriverVolunteer::canTakeOrder(const Order &order) const{
    return (!(this->isBusy()) && order.getDistance()<=maxDistance);
}

void DriverVolunteer::acceptOrder(const Order &order){ //TODO first check canTakeOrder
    DriverVolunteer::distanceLeft = order.getDistance();
    DriverVolunteer::activeOrderId = order.getId();
}

void DriverVolunteer::step(){
    bool hasMoreDistance = this->decreaseDistanceLeft();
    if(hasMoreDistance){
        completedOrderId = activeOrderId;
        activeOrderId = -1; //TODO need to write = NO_ORDER ?? or -1 is enough?
    }
}

string DriverVolunteer::toString() const{
    string info = "id is: " + to_string(this->getId()) + " name is: " + this->getName() + " maxDistance is: " + to_string(this->getMaxDistance()) + " distancePerStep is: " + to_string(this->getDistancePerStep()); 
    return info; //TODO check what should i return}
}

string DriverVolunteer::timeOrDistLeft() const{
    return to_string(this->getDistanceLeft());
}

string DriverVolunteer::numOrdsLeft() const{
    return "No Limit";
}

string DriverVolunteer::volunteerType() const{
    return "Driver";
}

//LimitedDriverVolunteer implementation
LimitedDriverVolunteer::LimitedDriverVolunteer(int id, const string &name, int maxDistance, int distancePerStep,int maxOrders): DriverVolunteer(id,name,maxDistance,distancePerStep), maxOrders(maxOrders), ordersLeft(maxOrders){}

LimitedDriverVolunteer *LimitedDriverVolunteer::clone() const{
    return new LimitedDriverVolunteer(*this);
}

int LimitedDriverVolunteer::getMaxOrders() const{
    return LimitedDriverVolunteer::maxOrders;
}

int LimitedDriverVolunteer::getNumOrdersLeft() const{
    return LimitedDriverVolunteer::ordersLeft;
}

bool LimitedDriverVolunteer::hasOrdersLeft() const{
    return (LimitedDriverVolunteer::ordersLeft > 0); 
}

bool LimitedDriverVolunteer::canTakeOrder(const Order &order) const{
    return (DriverVolunteer::canTakeOrder(order) && this->hasOrdersLeft());
}

void LimitedDriverVolunteer::acceptOrder(const Order &order){ //TODO should first check if can take order
    DriverVolunteer::acceptOrder(order);
    LimitedDriverVolunteer::ordersLeft--;
}

string LimitedDriverVolunteer::toString() const{
    string info = "id is: " + to_string(this->getId()) + " name is: " + this->getName() + " maxDistance is: " + to_string(this->getMaxDistance()) + " distancePerStep is: " + to_string(this->getDistancePerStep()) + " maxorders is: " + to_string(this->getMaxOrders()); 
    return info; //TODO check what should i return
}

string LimitedDriverVolunteer::numOrdsLeft() const{
    return to_string(this->getNumOrdersLeft());
}

// int main(){ //TODO delete later, only testing
//     Volunteer *drivVol = new LimitedDriverVolunteer(20917804, "Eilon", 15, 6,0);
//     Volunteer *drivVol2 = drivVol->clone();
//     drivVol2->setActiveID(34);
//     cout << drivVol2->getActiveOrderId() << endl;

//     cout << drivVol->getActiveOrderId() << endl;

//     return 0;
// }

// int main(){ //TODO delete later, only testing
//     LimitedDriverVolunteer *drivVol = new LimitedDriverVolunteer(20917804, "Eilon", 15, 6,2);
//     cout << drivVol->toString() << endl;
//     LimitedDriverVolunteer *drivVolClone = drivVol->clone();
//     Order *longOrd = new Order(69,420,16);
//     Order *shortOrd = new Order(14,58,13);

//     cout << drivVol->getDistanceLeft() << endl;
//     cout << drivVol->getMaxDistance() << endl;
//     cout << drivVol->getDistancePerStep() << endl;
//     cout << drivVol->hasOrdersLeft() << endl;

//     cout << drivVol->canTakeOrder(*longOrd) << endl;
//     cout << drivVol->canTakeOrder(*shortOrd) << endl;

//     drivVol->acceptOrder(*shortOrd);
//     cout << "accepted order" << endl;

//     while(drivVol->getDistanceLeft() > 0){
//         drivVol->step();
//         cout << "distance left: " << drivVol->getDistanceLeft() << endl;
//     }
//     cout << drivVol->getNumOrdersLeft() << endl;
//     drivVol->acceptOrder(*shortOrd);
//     cout << drivVol->getNumOrdersLeft() << endl;
//     cout << drivVol->hasOrdersLeft() << endl;


//     return 0;
// }



