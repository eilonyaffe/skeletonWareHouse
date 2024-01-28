#include "../include/WareHouse.h"
#include "../include/Order.h"
#include "../include/Customer.h"
#include "../include/Volunteer.h"
#include "../include/Action.h"


#include <iostream>
#include <fstream>
#include <sstream>

#include <string>
#include <vector>

using namespace std;


WareHouse::WareHouse(const string &configFilePath){ //TODO intialize the rest of the vectors and fields if needed here
    WareHouse::isOpen = false; //TODO should construct with true?
    WareHouse::customerCounter = 0; 
    WareHouse::volunteerCounter = 0;
    WareHouse::orderCounter = 0;
    this->parseVolunteers(configFilePath);
    this->parseCustomers(configFilePath);
    WareHouse::flagCustomer = new SoldierCustomer(-1,"def",-1,-1);
    WareHouse::flagVolunteer = new DriverVolunteer(-1,"def",-1,-1);
    WareHouse::flagOrder = new Order(-1,-1,-1);
}

void WareHouse::start(){
    cout << "Warehouse is open!" << endl;
    this->open();
    bool flag = true; //still gets input from user
    while(flag){
        string sentence;
        vector<string> splittedWords;
        getline(std::cin, sentence);
        istringstream iss(sentence);
        string word;
        while(iss>>word){
            splittedWords.push_back(word);
        }
        
        for(int i=0; i<splittedWords.size(); i++){ //TODO maybe there's a more elegant way to do this part
            if(splittedWords.at(0) == "step"){
                BaseAction *actor = new SimulateStep(stoi(splittedWords.at(1)));
                actor->act(*this);
                break;
            }
            else if(splittedWords.at(0) == "order"){
                AddOrder *actor = new AddOrder(stoi(splittedWords.at(1)));
                actor->act(*this);
                break;
            }

            else if(splittedWords.at(0) == "customer"){
                AddCustomer *actor = new AddCustomer(splittedWords.at(1), splittedWords.at(2), stoi(splittedWords.at(3)), stoi(splittedWords.at(4)));
                actor->act(*this);
                break;
            }
            else if(splittedWords.at(0) == "orderStatus"){
                PrintOrderStatus *actor = new PrintOrderStatus(stoi(splittedWords.at(1)));
                actor->act(*this);
                break;
            }
            else if(splittedWords.at(0) == "customerStatus"){
                PrintCustomerStatus *actor = new PrintCustomerStatus(stoi(splittedWords.at(1)));
                actor->act(*this);
                break;
            }
            else if(splittedWords.at(0) == "volunteerStatus"){
                PrintVolunteerStatus *actor = new PrintVolunteerStatus(stoi(splittedWords.at(1)));
                actor->act(*this);
                break;
            }
            else if(splittedWords.at(0) == "log"){
                PrintActionsLog *actor = new PrintActionsLog();
                actor->act(*this);
                break;
            }

            //TODO uncomment these once these actions are implemented
            
            // else if(splittedWords.at(0) == "close"){
            //     Close *actor = new Close(stoi(splittedWords.at(1)));
            //     actor->act(*this);
            // }
            // else if(splittedWords.at(0) == "backup"){
            //     BackupWareHouse *actor = new BackupWareHouse(stoi(splittedWords.at(1)));
            //     actor->act(*this);
            // }
            // else if(splittedWords.at(0) == "restore"){
            //     RestoreWareHouse *actor = new RestoreWareHouse(stoi(splittedWords.at(1)));
            //     actor->act(*this);
            // }

        }

    }
}

void WareHouse::simulateStepOnce(){ //TODO think if this function can fail, maybe when all orders are completed?
    vector<int> idsToDelete;
    for (const auto& pendOrd : pendingOrders) {
        if(pendOrd->getStatusAsString() == "Pending"){ //means it needs a collector
            bool foundCollector = this->assignSuitedCollector(*pendOrd);
            if(foundCollector){
                WareHouse::inProcessOrders.push_back(pendOrd);
                idsToDelete.push_back(pendOrd->getId());
            }
        }
        else if(pendOrd->getStatusAsString() == "Collecting"){ //means it needs a driver
            bool foundDriver = this->assignSuitedDriver(*pendOrd);
            if(foundDriver){
                WareHouse::inProcessOrders.push_back(pendOrd);
                idsToDelete.push_back(pendOrd->getId());
            }
        }
    }
    while(!idsToDelete.empty()){
        int poppedElement = idsToDelete.back();
        this->deleteOrderFromPending(poppedElement);
        idsToDelete.pop_back();
    }

    for (const auto& volun : volunteers){
        if(volun->isBusy()){ //volunteer currently process an order
            volun->step();
            if(!(volun->isBusy())){ //means he finished his job with his assigned order
                Order *ord = &(this->getOrder(volun->getCompletedOrderId())); //his just-finished order's id was saved in this field
                if(ord->getStatusAsString() == "Collecting"){ //finished collecting
                    pendingOrders.insert(pendingOrders.begin(), ord); //insert to vector's head to prevent order starvation
                }
                else if(ord->getStatusAsString() == "Delivering"){ //finished delivering
                    ord->setStatus(OrderStatus::COMPLETED);
                    completedOrders.push_back(ord);
                }
                this->deleteOrderFromProccessed(ord->getId()); //delete that order from the processed orders vector
            
                if(!(volun->hasOrdersLeft())){ //isn't busy and doesn't have orders left, therefore is deleted
                    this->deleteVolunteerFromVector(volun->getId());
                }
            }
        }
    }
    //TODO delete later, save for now for testing
    // cout << endl;
    // std::cout << "Pending orders vector:" << std::endl;
    // for (const auto& pendingOrder : pendingOrders) {
    //     std::cout << pendingOrder->toString() << std::endl; //delete later
    // }
    // cout << endl;
    // std::cout << "In proccess orders vector:" << std::endl;
    // for (const auto& inProcessOrder : inProcessOrders) {
    //     std::cout << inProcessOrder->toString() << std::endl; //delete later
    // }
    // cout << endl;
    // std::cout << "completed orders vector:" << std::endl;
    // for (const auto& completedOrder : completedOrders) {
    //     std::cout << completedOrder->toString() << std::endl; //delete later
    // }
    // cout << endl;
}

void WareHouse::deleteOrderFromPending(int orderId){
    int i = 0;
    for (const auto& pendOrd : WareHouse::pendingOrders) {
        if(pendOrd->getId() == orderId){
             WareHouse::pendingOrders.erase(pendingOrders.begin()+i);
             break;
        }
        i++;
    }
}

void WareHouse::deleteOrderFromProccessed(int orderId){
    int i = 0;
    for (const auto& procOr : inProcessOrders) {
        if(procOr->getId() == orderId){
             WareHouse::inProcessOrders.erase(inProcessOrders.begin()+i);
             break;
        }
        i++;
    }
}

void WareHouse::deleteVolunteerFromVector(int volID){
    int i = 0;
    for (const auto& vol : WareHouse::volunteers) {
        if(vol->getId() == volID){
             WareHouse::volunteers.erase(volunteers.begin()+i);
             break;
        }
        i++;
    }
}

//TODO continue from here keep troubleshooting stepOnce

bool WareHouse::assignSuitedCollector(Order& newOrd){
    for (const auto& volunteer : volunteers) {
        if(volunteer->volunteerType() == "Collector" && volunteer->canTakeOrder(newOrd)){
            volunteer->acceptOrder(newOrd);
            newOrd.setStatus(OrderStatus::COLLECTING);
            newOrd.setCollectorId(volunteer->getId());
            return true;
        }
    }
    return false; //indicates no suitable collector was found
}

bool WareHouse::assignSuitedDriver(Order& newOrd){
    for (const auto& volunteer : volunteers) {
        if(volunteer->volunteerType() == "Driver" && volunteer->canTakeOrder(newOrd)){
            volunteer->acceptOrder(newOrd);
            newOrd.setStatus(OrderStatus::DELIVERING);
            newOrd.setDriverId(volunteer->getId());
            return true;
        }
    }
    return false; //indicates no suitable driver was found
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
        Order *ord = new Order(WareHouse::orderCounter, customerId, customerDistance); //TODO remember delete
        custWithOrd.addOrder(orderCounter);
        this->addOrder(ord);
    }
    return canPlace;
}

void WareHouse::AddCustomerToWareHouse(string customerName, string customerType, int distance, int maxOrders){
    if(customerType == "soldier" || customerType == "Soldier"){
        SoldierCustomer *soldierCust = new SoldierCustomer(WareHouse::customerCounter, customerName, distance, maxOrders);
        customers.push_back(soldierCust);
        WareHouse::customerCounter++;
    }

    else if(customerType == "civilian" || customerType == "Civilian"){
        CivilianCustomer *civilianCust = new CivilianCustomer(WareHouse::customerCounter, customerName, distance, maxOrders);
        customers.push_back(civilianCust);
        WareHouse::customerCounter++;
    }
}

void WareHouse::addAction(BaseAction* action){
    WareHouse::actionsLog.push_back(action);
}

Customer &WareHouse::getCustomer(int customerId) const{
    for (const auto& customer : WareHouse::customers) {
        if(customer->getId() == customerId){
            return (*customer);
        }
    }
    return *(WareHouse::flagCustomer); //avoids warnings
}

Volunteer &WareHouse::getVolunteer(int volunteerId) const{
    for (const auto& volunteer : WareHouse::volunteers) {
        if(volunteer->getId() == volunteerId){
            return (*volunteer);
        } 
    }
    return *(WareHouse::flagVolunteer); //avoids warnings
}

bool WareHouse::volunteerExists(int volunteerId){
    for (const auto& volunteers : volunteers) {
        if(volunteers->getId() == volunteerId){
            return true;
        } 
    }
    return false;
}

bool WareHouse::customerExists(int customerId){ 
    return (customerId<WareHouse::customerCounter); //because not deleting customers
}

bool WareHouse::custCanMakeOrder(int customerId){ //checks if exists and can make order. for addOrder operation
    for (const auto& customers : customers) {
        if(customers->getId() == customerId && customers->canMakeOrder()){
            return true;
        } 
    }
    return false;
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

bool WareHouse::orderExists(int orderId){ 
    return (orderId<WareHouse::orderCounter); //the current counter is next to be assigned. and we never delete successfully placed orders
}

const vector<BaseAction*> &WareHouse::getActions() const{
    return WareHouse::actionsLog;
}


//TODO close
// destructor
//


void WareHouse::open(){
    WareHouse::isOpen = true;
}

void WareHouse::parseVolunteers(const string &configFilePath){
    std::fstream file(configFilePath, std::ios::in);
    vector<string> volunteersTemp; 
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
                volunteers.push_back(tempVol);
                this->volunteerCounter++;
                i += 2;
            }
            else if(volunteersTemp[i+1] == "limited_collector"){
                int coolDown = stoi(volunteersTemp[i+2]);
                int maxOrders = stoi(volunteersTemp[i+3]);
                LimitedCollectorVolunteer *tempVol = new LimitedCollectorVolunteer(this->volunteerCounter, name, coolDown, maxOrders);
                volunteers.push_back(tempVol);
                this->volunteerCounter++;
                i += 3;
            }
            else if(volunteersTemp[i+1] == "driver"){
                int maxDistance = stoi(volunteersTemp[i+2]);
                int distancePerStep = stoi(volunteersTemp[i+3]);
                DriverVolunteer *tempVol = new DriverVolunteer(this->volunteerCounter, name, maxDistance, distancePerStep);
                volunteers.push_back(tempVol);
                this->volunteerCounter++;
                i += 3;
            }
            else if(volunteersTemp[i+1] == "limited_driver"){
                int maxDistance = stoi(volunteersTemp[i+2]);
                int distancePerStep = stoi(volunteersTemp[i+3]);
                int maxOrders = stoi(volunteersTemp[i+4]);
                DriverVolunteer *tempVol = new DriverVolunteer(this->volunteerCounter, name, maxDistance, distancePerStep);
                volunteers.push_back(tempVol);
                this->volunteerCounter++;
                i += 4;
            }
        }
    }
}

void WareHouse::parseCustomers(const string &configFilePath){
    std::fstream file(configFilePath, std::ios::in);
    vector<string> customersTemp; 
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
                customers.push_back(tempCust);
                this->customerCounter++;
                i += 3;
            }
            else if(customersTemp[i+1] == "civilian"){
                int distance = stoi(customersTemp[i+2]);
                int maxOrders = stoi(customersTemp[i+3]);
                CivilianCustomer *tempCust = new CivilianCustomer(this->customerCounter, name, distance, maxOrders);
                customers.push_back(tempCust);
                this->customerCounter++;
                i += 3;
            }
        }
    }
}

void WareHouse::printAllOrders(){ //TODO delete later? used for debugging
    cout << endl;
    std::cout << "Pending orders vector:" << std::endl;
    for (const auto& pendingOrder : pendingOrders) {
        std::cout << pendingOrder->toString() << std::endl; //delete later
    }
    std::cout << "In proccess orders vector:" << std::endl;
    for (const auto& inProcessOrder : inProcessOrders) {
        std::cout << inProcessOrder->toString() << std::endl; //delete later
    }
    std::cout << "completed orders vector:" << std::endl;
    for (const auto& completedOrder : completedOrders) {
        std::cout << completedOrder->toString() << std::endl; //delete later
    }
}


int main(){
    string fileLocation = "../bin/rest/configFileExample.txt";
    WareHouse wh(fileLocation);

    wh.start();
    // wh.newOrderbyID(1);
    // wh.newOrderbyID(2);
    // wh.newOrderbyID(1);

    // wh.simulateStepOnce();

    // wh.simulateStepOnce();

    // wh.simulateStepOnce();
    // wh.simulateStepOnce();
    // wh.simulateStepOnce();
    // wh.simulateStepOnce();
    // wh.simulateStepOnce();
    // wh.simulateStepOnce();

    // wh.printAllOrders();
    // Order o1 = wh.getOrder(1);
    // // Order o2 = wh.getOrder(2);

    // cout << o1.toString() << endl;
    // // cout << o2.toString() << endl;


    return 0;
}









