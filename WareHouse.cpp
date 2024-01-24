#include "../include/WareHouse.h"
#include "../include/Order.h"
#include "../include/Customer.h"
#include "../include/Volunteer.h"

#include <iostream>
#include <fstream>
#include <sstream>

#include <string>
#include <vector>

using namespace std;


WareHouse::WareHouse(const string &configFilePath){ //TODO intialize the rest of the vectors and fields if needed here
    WareHouse::isOpen = false; //TODO should construct with true?
    WareHouse::customerCounter = 1; //TODO remember started with 1. first assign customer, then increment by 1
    WareHouse::volunteerCounter = 1;
    WareHouse::orderCounter = 1;
    WareHouse::volunteers = this->parseVolunteers(configFilePath);
    WareHouse::customers = this->parseCustomers(configFilePath);
    WareHouse::flagCustomer = new SoldierCustomer(-1,"def",-1,-1);
    WareHouse::flagVolunteer = new DriverVolunteer(-1,"def",-1,-1);
    WareHouse::flagOrder = new Order(-1,-1,-1);
}

void WareHouse::start(){
    cout << "Warehouse is open!" << endl;
    this->open();
}

void WareHouse::addOrder(Order* order){ 
    WareHouse::pendingOrders.push_back(order); //because it is a new order, added to the end of the vector
    WareHouse::orderCounter++;
}

bool WareHouse::newOrderbyID(int customerId){
    bool canPlace = this->custCanMakeOrder(customerId);
    if(canPlace){
        Customer& custWithOrd = this->getCustomer(customerId);
        int customerDistance = custWithOrd.getCustomerDistance();
        Order *ord = new Order(WareHouse::orderCounter, customerId, customerDistance);
        custWithOrd.addOrder(orderCounter);
        this->addOrder(ord);
    }
    return canPlace;
}

void WareHouse::AddCustomer(string customerName, string customerType, int distance, int maxOrders){
    if(customerType == "soldier"){
        SoldierCustomer *soldierCust = new SoldierCustomer(WareHouse::customerCounter, customerName, distance, maxOrders);
        customers.push_back(soldierCust);
        WareHouse::customerCounter++;
    }

    else if(customerType == "civilian"){
        CivilianCustomer *civilianCust = new CivilianCustomer(WareHouse::customerCounter, customerName, distance, maxOrders);
        customers.push_back(civilianCust);
        WareHouse::customerCounter++;
    }
}


//TODO addaction

//

Customer &WareHouse::getCustomer(int customerId) const{ //only called if the customer exists. 
    for (const auto& customer : WareHouse::customers) {
        if(customer->getId() == customerId){
            return (*customer);
        }
    }
    return *(WareHouse::flagCustomer); //avoids warnings

}

Volunteer &WareHouse::getVolunteer(int volunteerId) const{ //only called if the volunteer exists. 
    for (const auto& volunteer : WareHouse::volunteers) {
        if(volunteer->getId() == volunteerId){
            return (*volunteer);
        } 
    }
    return *(WareHouse::flagVolunteer); //avoids warnings
}

bool WareHouse::volunteerExists(int volunteerId){ //TODO keep? maybe not used at all
    for (const auto& volunteers : volunteers) {
        if(volunteers->getId() == volunteerId){
            return true;
        } 
    }
    return false;
}

bool WareHouse::customerExists(int customerId){ //TODO keep? maybe not used at all
    for (const auto& customers : customers) {
        if(customers->getId() == customerId){
            return true;
        } 
    }
    return false;
}

bool WareHouse::custCanMakeOrder(int customerId){ //checks if exists and can make order. for add order operation
    bool canMake = false;
    for (const auto& customers : customers) {
        if(customers->getId() == customerId && customers->canMakeOrder()){
            return true;
        } 
    }
    return canMake;
}

Order &WareHouse::getOrder(int orderId) const{
    for (const auto& currOrder : WareHouse::pendingOrders) {
        if(currOrder->getId() == orderId){
            return (*currOrder);
        }
    }
    for (const auto& currOrder : WareHouse::inProcessOrders) {
        if(currOrder->getId() == orderId){
            return (*currOrder);
        }
    }
    for (const auto& currOrder : WareHouse::completedOrders) {
        if(currOrder->getId() == orderId){
            return (*currOrder);
        }
    }
    return *(WareHouse::flagOrder); //avoids warnings
}

bool WareHouse::orderExists(int orderId){ //TODO keep? maybe not used at all
    return (orderId<WareHouse::orderCounter); //the current counter is next to be assigned
}

//TODO getActions

//

//TODO close

//


void WareHouse::open(){
    WareHouse::isOpen = true;
}

vector<Volunteer*> WareHouse::parseVolunteers(const string &configFilePath){
    std::fstream file(configFilePath, std::ios::in);
    vector<string> volunteersTemp; 
    vector<Volunteer*> volunteersVector;
    std::string line;
    while (std::getline(file, line)) {
        if (line.find("volunteer") == 0) {
            std::istringstream iss(line);
            std::string word;
            while (iss >> word) {
                if (word.find('#') != std::string::npos) {
                    break;
                }
                else{
                    volunteersTemp.push_back(word);
                }
            }
        }
    }
    file.close();

    for(size_t i=1; i<volunteersTemp.size();++i){
        if(volunteersTemp[i] == "volunteer"){
            //Do nothing
        }
        else{
            string name = volunteersTemp[i];
            if(volunteersTemp[i+1] == "collector"){
                int coolDown = stoi(volunteersTemp[i+2]);
                CollectorVolunteer *tempVol = new CollectorVolunteer(this->volunteerCounter, name, coolDown);
                volunteersVector.push_back(tempVol);
                this->volunteerCounter++;
                i += 2;
            }
            else if(volunteersTemp[i+1] == "limited_collector"){
                int coolDown = stoi(volunteersTemp[i+2]);
                int maxOrders = stoi(volunteersTemp[i+3]);
                LimitedCollectorVolunteer *tempVol = new LimitedCollectorVolunteer(this->volunteerCounter, name, coolDown, maxOrders);
                volunteersVector.push_back(tempVol);
                this->volunteerCounter++;
                i += 3;
            }
            else if(volunteersTemp[i+1] == "driver"){
                int maxDistance = stoi(volunteersTemp[i+2]);
                int distancePerStep = stoi(volunteersTemp[i+3]);
                DriverVolunteer *tempVol = new DriverVolunteer(this->volunteerCounter, name, maxDistance, distancePerStep);
                volunteersVector.push_back(tempVol);
                this->volunteerCounter++;
                i += 3;
            }
            else if(volunteersTemp[i+1] == "limited_driver"){
                int maxDistance = stoi(volunteersTemp[i+2]);
                int distancePerStep = stoi(volunteersTemp[i+3]);
                int maxOrders = stoi(volunteersTemp[i+4]);
                DriverVolunteer *tempVol = new DriverVolunteer(this->volunteerCounter, name, maxDistance, distancePerStep);
                volunteersVector.push_back(tempVol);
                this->volunteerCounter++;
                i += 4;
            }
        }
    }

    // std::cout << "Volunteers vector:" << std::endl;
    // for (const auto& volunteersVector : volunteersVector) {
    //     std::cout << volunteersVector->toString() << std::endl; //delete later
    // }
    return volunteersVector;
}

vector<Customer*> WareHouse::parseCustomers(const string &configFilePath){
    std::fstream file(configFilePath, std::ios::in);
    vector<string> customersTemp; 
    vector<Customer*> customersVector;
    std::string line;
    while (std::getline(file, line)) {
        if (line.find("customer") == 0) {
            std::istringstream iss(line);
            std::string word;
            while (iss >> word) {
                if (word.find('#') != std::string::npos) {
                    break;
                }
                else{
                    customersTemp.push_back(word);
                }
            }
        }
    }
    file.close();
    
    for(size_t i=1; i<customersTemp.size();++i){
        if(customersTemp[i] == "customer"){
            //Do nothing
        }
        else{
            string name = customersTemp[i];
            if(customersTemp[i+1] == "soldier"){
                int distance = stoi(customersTemp[i+2]);
                int maxOrders = stoi(customersTemp[i+3]);
                SoldierCustomer *tempCust = new SoldierCustomer(this->customerCounter, name, distance, maxOrders);
                customersVector.push_back(tempCust);
                this->customerCounter++;
                i += 3;
            }
            else if(customersTemp[i+1] == "civilian"){
                int distance = stoi(customersTemp[i+2]);
                int maxOrders = stoi(customersTemp[i+3]);
                CivilianCustomer *tempCust = new CivilianCustomer(this->customerCounter, name, distance, maxOrders);
                customersVector.push_back(tempCust);
                this->customerCounter++;
                i += 3;
            }
        }
    }
    
    // std::cout << "Customers vector:" << std::endl;
    // for (const auto& customersVector : customersVector) {
    //     std::cout << customersVector->toString() << std::endl; //delete later
    // }
    return customersVector;
}

// int main(){
//     string fileLocation = "../bin/rest/configFileExample.txt";
//     WareHouse wh(fileLocation);
//     bool madeOrder = wh.newOrderbyID(2);
//     cout << madeOrder << endl;
//     Order &ord = wh.getOrder(1);
//     cout << ord.toString() << endl;

//     return 0;
// }









