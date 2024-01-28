#include "../include/Customer.h"
#include <iostream> //TODO delete later
#include <string>
#include <vector>
using namespace std; //TODO delete later?


Customer::Customer(int id, const string &name, int locationDistance, int maxOrders):id(id),name(name),locationDistance(locationDistance), maxOrders(maxOrders){
    vector<int> ordersID;
    Customer::ordersId = ordersID; //keep? maybe autocreates when doing first push
    Customer::orderCount = 0;
}

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
        ordersId.push_back(orderId); //TODO maybe check if i should check if the ID isnt already there, prevent dups. remember now the last inserted order is cust' curr order
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
    return info; //TODO check what should i return
}

CivilianCustomer::CivilianCustomer(int id, const string &name, int locationDistance, int maxOrders): Customer(id,name,locationDistance,maxOrders){}

CivilianCustomer *CivilianCustomer::clone() const{
    return new CivilianCustomer(*this);
}

string CivilianCustomer::toString() const{
    string info = "id is: " + to_string(this->getId()) + " name is: " + this->getName() + " locationDistance is: " + to_string(this->getCustomerDistance()) + " maxorders is: " + to_string(this->getMaxOrders()); 
    return info; //TODO check what should i return
}

// int main(){ //TODO delete later, only testing
//     CivilianCustomer *Soldat = new CivilianCustomer(209, "Jeff", 7, 3);
//     cout << Soldat->getName() << endl;
//     cout << Soldat->getId() << endl;
//     cout << Soldat->getCustomerDistance() << endl;
//     cout << Soldat->getMaxOrders() << endl;
//     cout << Soldat->getNumOrders() << endl;
//     cout << Soldat->canMakeOrder() << endl;
//     Soldat->addOrder(8);
//     Soldat->addOrder(17);

//     const vector<int> vect =  Soldat->getOrdersIds();
//     cout << vect.at(0) << endl;
//     cout << vect.at(1) << endl;

//     CivilianCustomer *winterSoldat = Soldat->clone();
//     cout << winterSoldat->getName() << endl;
//     cout << winterSoldat->getId() << endl;
//     cout << winterSoldat->getCustomerDistance() << endl;
//     cout << winterSoldat->getMaxOrders() << endl;
//     cout << winterSoldat->getNumOrders() << endl;
//     cout << winterSoldat->canMakeOrder() << endl;

//     const vector<int> vect2 =  winterSoldat->getOrdersIds();
//     cout << vect2.at(0) << endl;
//     cout << vect2.at(1) << endl;

//     //TODO testing if everything works!
//     return 0;
// }
