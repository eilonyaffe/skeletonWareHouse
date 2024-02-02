#include "../include/Customer.h"
#include <string>
#include <vector>
using namespace std; 


Customer::Customer(int id, const string &name, int locationDistance, int maxOrders):id(id),name(name),locationDistance(locationDistance), maxOrders(maxOrders), ordersId(), orderCount(0){}


const string &Customer::getName() const{
    return name;
}

int Customer::getId() const{
    return id;
}

int Customer::getCustomerDistance() const{
    return locationDistance;
}

int Customer::getMaxOrders() const{
    return maxOrders;
}

int Customer::getNumOrders() const{
    return this->orderCount;
}

bool Customer::canMakeOrder() const{
    return (this->orderCount < maxOrders);
}

const vector<int> &Customer::getOrdersIds() const{
    return this->ordersId;
}

int Customer::addOrder(int orderId){
    if(this->canMakeOrder()){
        ordersId.push_back(orderId); 
        Customer::orderCount++;
        return orderId;
    }
    else{
        return -1;
    }
}

SoldierCustomer::SoldierCustomer(int id, const string &name, int locationDistance, int maxOrders): Customer(id,name,locationDistance,maxOrders){}


SoldierCustomer *SoldierCustomer::clone() const{
    return new SoldierCustomer(*this);
}

string SoldierCustomer::toString() const{
    string info = "id is: " + to_string(this->getId()) + " name is: " + this->getName() + " locationDistance is: " + to_string(this->getCustomerDistance()) + " maxorders is: " + to_string(this->getMaxOrders()); 
    return info; 
}

CivilianCustomer::CivilianCustomer(int id, const string &name, int locationDistance, int maxOrders): Customer(id,name,locationDistance,maxOrders){}

CivilianCustomer *CivilianCustomer::clone() const{
    return new CivilianCustomer(*this);
}

string CivilianCustomer::toString() const{
    string info = "id is: " + to_string(this->getId()) + " name is: " + this->getName() + " locationDistance is: " + to_string(this->getCustomerDistance()) + " maxorders is: " + to_string(this->getMaxOrders()); 
    return info; 
}

