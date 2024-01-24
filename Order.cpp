#include "../include/Order.h"
#include <iostream> //TODO delete later
#include <string>
using namespace std; //TODO delete later?


Order::Order(int id, int customerId, int distance): id(id), customerId(customerId), distance(distance), collectorId(-1), driverId(-1), status(OrderStatus::PENDING){}

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

const string Order::toString() const{
    string info = "id is: " + to_string(this->getId()) + " custID is: " + to_string(this->getCustomerId()) + " Distance is: " + to_string(this->getDistance()); 
    return info; //TODO check what should i return
}

// int main(){ //TODO delete later, just tests
//     Order *o = new Order(69,420,18);
//     string s = (*o).toString();
//     std::cout << s << std::endl;



//     return 0;
// }




