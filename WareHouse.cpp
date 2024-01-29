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



//neya adds:
WareHouse::~WareHouse(){ //destructor
    for(auto act: this->actionsLog){
        delete act;
    }
    for(auto vol: this->volunteers){
        delete vol;
    }
    for(auto ord: this->pendingOrders){
        delete ord;
    }
    for(auto ord: this->inProcessOrders){
        delete ord;
    }
    for(auto ord: this->completedOrders){
        delete ord;
    }
    for(auto cust: this->customers){
        delete cust;
    }
    delete this->flagCustomer;
    delete this->flagOrder;
    delete this->flagVolunteer;
    cout << "deleted" << endl; //to delete COMPLETELY
}

WareHouse::WareHouse(const WareHouse& other){ //copy constructor

    cout << "copy constructor" << endl; //to delete COMPLETELY

    this->isOpen = other.isOpen;
    this->customerCounter = other.customerCounter;
    this->volunteerCounter = other.volunteerCounter;
    this->orderCounter = other.orderCounter;

    for (const auto& act: other.actionsLog){
        this->actionsLog.push_back(act->clone());
    }

    for (const auto& vol: other.volunteers){
        this->volunteers.push_back(vol->clone());
    }

    for (const auto& cust: other.customers){
        this->customers.push_back(cust->clone());
    }

    for (const auto& ord: other.pendingOrders){
        Order* newOrd = new Order(*ord);
        this->pendingOrders.push_back(newOrd);
    }
    
    for (const auto& ord: other.inProcessOrders){
        Order* newOrd = new Order(*ord);
        this->inProcessOrders.push_back(newOrd);
    }

    for (const auto& ord: other.completedOrders){
        Order* newOrd = new Order(*ord);
        this->completedOrders.push_back(newOrd);
    }

    this->flagCustomer = other.flagCustomer->clone(); 
    this->flagVolunteer = other.flagVolunteer->clone(); 
    this->flagOrder = new Order(*other.flagOrder);

}

//TO DELETEEE COMPLETELY
vector<Customer*> WareHouse::getCustomers(){
    return this->customers;
}
//TO DELETEEE COMPLETELY
void WareHouse::deleteCustomer(){
    vector<Customer*>::iterator it = this->customers.begin() + 2;
    this->customers.erase(it);
}




WareHouse& WareHouse::operator=(const WareHouse& other){
    if (this != &other){ //avoid invalid self assignment
        
        //STEP 1: deallocating old memory
        for(auto act: this->actionsLog){ //delete all actions on the heap
            delete act;
        }
        this->actionsLog.clear(); //clear old actions addresses

        for(auto vol: this->volunteers){
            delete vol;
        }
        this->volunteers.clear();

        for(auto ord: this->pendingOrders){
            delete ord;
        }
        this->pendingOrders.clear();

        for(auto ord: this->inProcessOrders){
            delete ord;
        }
        this->inProcessOrders.clear();

        for(auto ord: this->completedOrders){
            delete ord;
        }
        this->completedOrders.clear();

        for(auto cust: this->customers){
            delete cust;
        }
        this->customers.clear();

        delete this->flagCustomer;
        delete this->flagOrder;
        delete this->flagVolunteer;

        
        //STEP 2: copying the elements & allocating new memory
        this->isOpen = other.isOpen;
        this->customerCounter = other.customerCounter;
        this->volunteerCounter = other.volunteerCounter;
        this->orderCounter = other.orderCounter;

        for (const auto& act: other.actionsLog){
            this->actionsLog.push_back(act->clone());
        }

        for (const auto& vol: other.volunteers){
            this->volunteers.push_back(vol->clone());
        }

        for (const auto& cust: other.customers){
            this->customers.push_back(cust->clone());
        }

        for (const auto& ord: other.pendingOrders){
            Order* newOrd = new Order(*ord);
            this->pendingOrders.push_back(newOrd);
        }
    
        for (const auto& ord: other.inProcessOrders){
            Order* newOrd = new Order(*ord);
            this->inProcessOrders.push_back(newOrd);
        }

        for (const auto& ord: other.completedOrders){
            Order* newOrd = new Order(*ord);
            this->completedOrders.push_back(newOrd);
        }

        this->flagCustomer = other.flagCustomer->clone(); 
        this->flagVolunteer = other.flagVolunteer->clone(); 
        this->flagOrder = new Order(*other.flagOrder);

    }
    return *this;
}


// WareHouse::WareHouse(WareHouse&& other) //move constructor
//     :     {

// }


// int main(){
//     string fileLocation = "../bin/rest/configFileExample.txt";
//     WareHouse wh(fileLocation);

//     wh.AddCustomerToWareHouse("Eilon", "civilian", 7, 3);
//     wh.AddCustomerToWareHouse("Neya", "Solider", 9, 2);
//     wh.AddCustomerToWareHouse("GilTargil", "civilian", 6, 5);

//     cout << "original customers:" << endl;
//     vector<Customer*> f = wh.getCustomers();
//     for (Customer* elem: f){
//         string s = elem->toString();
//         cout << s << endl;
//     }
    //wh.start();

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
    //cout << "original orders:" << endl;
    //wh.printAllOrders();
    // Order o1 = wh.getOrder(1);
    // // Order o2 = wh.getOrder(2);

    // cout << o1.toString() << endl;
    // // cout << o2.toString() << endl;
    // WareHouse wh2 = wh;
    //cout << "copied orders:" << endl;
    //wh2.printAllOrders();


    // cout << "copied customers:" << endl;
    // vector<Customer*> f1 = wh2.getCustomers();
    // for (Customer* elem: f1){
    //     string s = elem->toString();
    //     cout << s << endl;
    // }

    // wh.deleteCustomer();
    // f = wh.getCustomers();
    // cout << "original customers after del:" << endl;
    // for (Customer* elem: f){
    //     string s = elem->toString();
    //     cout << s << endl;
    // }
    // f1 = wh2.getCustomers();
    // cout << "copied customers after del:" << endl;
    // for (Customer* elem: f1){
    //     string s = elem->toString();
    //     cout << s << endl;
    // }
    // Order* d1 = new Order(11,21,13);
    // Order* d2 = new Order(10,5,6);
    // Order* d3 = new Order(7,8,9);
    // wh.addOrder(d1);
    // wh.addOrder(d2);
    // wh.addOrder(d3);
    // cout << "original orders:" << endl;
    // wh.printAllOrders();
    // WareHouse wh2 = wh;
    // cout << "copied orders:" << endl;
    // wh2.printAllOrders();

    // wh.deleteOrderFromPending(7);
    // cout << "original orders after del:" << endl;
    // wh.printAllOrders();

    // cout << "copied orders after del:" << endl;
    // wh2.printAllOrders();

//     return 0;
// }









