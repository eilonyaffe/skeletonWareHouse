#include "../include/Order.h"
#include <string>
using namespace std; 


Order::Order(int id, int customerId, int distance): id(id), customerId(customerId), distance(distance), status(OrderStatus::PENDING), collectorId(-1), driverId(-1){}

int Order::getId() const{
    return id;
}

int Order::getCustomerId() const{
    return customerId;
}

void Order::setStatus(OrderStatus status){
    Order::status = status;
}

void Order::setCollectorId(int collectorId){
    Order::collectorId = collectorId;
}

void Order::setDriverId(int driverId){
    Order::driverId = driverId;
}

int Order::getCollectorId() const{
    return collectorId;
}

int Order::getDriverId() const{
    return driverId;
}

int Order::getDistance() const{
    return distance;
}

OrderStatus Order::getStatus() const{
    return status;
}

string Order::getStatusAsString() const{
    if(Order::status == OrderStatus::PENDING){
        return "Pending";
    }
    else if(Order::status == OrderStatus::COLLECTING){
        return "Collecting";
    }
    else if(Order::status == OrderStatus::DELIVERING){
        return "Delivering";
    }
    else {
        return "Completed";
    }
}

const string Order::toString() const{
    return "OrderID: " + to_string(this->getId()) + ", CustomerID: " + to_string(this->getCustomerId()) + ", Status: " + this->getStatusAsString(); 
}






