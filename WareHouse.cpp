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


WareHouse::WareHouse(const string &configFilePath): isOpen(false), actionsLog(), volunteers(),pendingOrders(), inProcessOrders(), completedOrders(), customers(), customerCounter(0), volunteerCounter(0), orderCounter(0), flagCustomer(new SoldierCustomer(-1,"def",-1,-1)), flagVolunteer(new DriverVolunteer(-1,"def",-1,-1)), flagOrder(new Order(-1,-1,-1)){
    this->parseVolunteers(configFilePath);
    this->parseCustomers(configFilePath);
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
        int sentenceSize = splittedWords.size();
        for(int i=0; i<sentenceSize; i++){ 
            if(splittedWords.at(0) == "step"){
                SimulateStep *actor = new SimulateStep(stoi(splittedWords.at(1)));
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
            
            else if(splittedWords.at(0) == "close"){
                Close *actor = new Close();
                actor->act(*this);
                flag = false;
                break;
            }
            else if(splittedWords.at(0) == "backup"){
                BackupWareHouse *actor = new BackupWareHouse();
                actor->act(*this);
            }
            else if(splittedWords.at(0) == "restore"){
                RestoreWareHouse *actor = new RestoreWareHouse();
                actor->act(*this);
            }

        }

    }
}

void WareHouse::simulateStepOnce(){ 
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
        if(volun->isBusy()){ //volunteer currently processes an order
            volun->step();
            if(!(volun->isBusy())){ //means he finished his job with his assigned order
                Order *ord = &(this->getOrder(volun->getCompletedOrderId()));
                if(ord->getStatusAsString() == "Collecting"){ //finished collecting
                    pendingOrders.insert(pendingOrders.begin(), ord); //insert to vector's head to prevent order starvation
                    volun->resetCompletedOrderId();
                }
                else if(ord->getStatusAsString() == "Delivering"){ //finished delivering
                    ord->setStatus(OrderStatus::COMPLETED);
                    completedOrders.push_back(ord);
                    volun->resetCompletedOrderId();
                }
                this->deleteOrderFromProccessed(ord->getId());
            
                if(!(volun->hasOrdersLeft())){
                    this->deleteVolunteerFromVector(volun->getId());
                }
            }
        }
    }
}

void WareHouse::deleteOrderFromPending(int orderId){
    int i = 0;
    for (auto& pendOrd : WareHouse::pendingOrders) {
        if(pendOrd->getId() == orderId){
            delete pendOrd;
            WareHouse::pendingOrders.erase(pendingOrders.begin()+i);
            break;
        }
        i++;
    }
}

void WareHouse::deleteOrderFromProccessed(int orderId){
    int i = 0;
    for (auto& procOr : inProcessOrders) {
        if(procOr->getId() == orderId){
            delete procOr;
            WareHouse::inProcessOrders.erase(inProcessOrders.begin()+i);
            break;
        }
        i++;
    }
}

void WareHouse::deleteVolunteerFromVector(int volID){
    int i = 0;
    for (auto& vol : WareHouse::volunteers) {
        if(vol->getId() == volID){    
            delete vol;
            WareHouse::volunteers.erase(volunteers.begin()+i);
            break;
        }
        i++;
    }
}

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
    WareHouse::pendingOrders.push_back(order);
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

bool WareHouse::custCanMakeOrder(int customerId){ 
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
    return (orderId<WareHouse::orderCounter);
}

const vector<BaseAction*> &WareHouse::getActions() const{
    return WareHouse::actionsLog;
}

void WareHouse::close(){
    putchar('\n');
    //prints all orders
    for (const auto& pendingOrder : pendingOrders) {
        std::cout << pendingOrder->toString() << std::endl; 
    }
    for (const auto& inProcessOrder : inProcessOrders) {
        std::cout << inProcessOrder->toString() << std::endl; 
    }
    for (const auto& completedOrder : completedOrders) {
        std::cout << completedOrder->toString() << std::endl;
    }
    this->isOpen = false;
}


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
                LimitedDriverVolunteer *tempVol = new LimitedDriverVolunteer(this->volunteerCounter, name, maxDistance, distancePerStep, maxOrders);
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

WareHouse::~WareHouse(){ 
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
}

WareHouse::WareHouse(const WareHouse& other): isOpen(other.isOpen), actionsLog(),volunteers(), pendingOrders(), inProcessOrders(), completedOrders(), customers(), customerCounter(other.customerCounter), volunteerCounter(other.volunteerCounter), orderCounter(other.orderCounter), flagCustomer(other.flagCustomer->clone()), flagVolunteer(other.flagVolunteer->clone()), flagOrder(new Order(*other.flagOrder)){ 
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
}


WareHouse& WareHouse::operator=(const WareHouse& other){
    if (this != &other){ //avoid invalid self assignment
        
        //STEP 1: deallocating old memory
        for(auto act: this->actionsLog){ 
            delete act;
        }
        this->actionsLog.clear(); //clears old addresses

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

//move constructor
WareHouse::WareHouse(WareHouse&& other) noexcept: isOpen(std::move(other.isOpen)), actionsLog(std::move(other.actionsLog)), volunteers(std::move(other.volunteers)), pendingOrders(std::move(other.pendingOrders)), inProcessOrders(std::move(other.inProcessOrders)), completedOrders(std::move(other.completedOrders)), customers(std::move(customers)), customerCounter(std::move(other.customerCounter)), volunteerCounter(std::move(other.volunteerCounter)), orderCounter(std::move(other.orderCounter)), flagCustomer(other.flagCustomer), flagVolunteer(other.flagVolunteer), flagOrder(other.flagOrder){   
    other.actionsLog.clear();
    other.volunteers.clear();
    other.pendingOrders.clear();
    other.inProcessOrders.clear();
    other.completedOrders.clear();
    other.customers.clear();
    other.flagCustomer = nullptr;
    other.flagVolunteer = nullptr;
    other.flagOrder = nullptr;
}

//move assignment operator
WareHouse& WareHouse::operator=(WareHouse&& other) noexcept {
    if (this != &other){ //avoid self assignment
        this->isOpen = std::move(other.isOpen);

        this->actionsLog = std::move(other.actionsLog);
        other.actionsLog.clear();
        this->volunteers = std::move(other.volunteers);
        other.volunteers.clear();
        this->pendingOrders = std::move(other.pendingOrders);
        other.pendingOrders.clear();
        this->inProcessOrders = std::move(other.inProcessOrders);
        other.inProcessOrders.clear();
        this->completedOrders = std::move(other.completedOrders);
        other.completedOrders.clear();
        this->customers = std::move(customers);
        other.customers.clear();

        this->customerCounter = std::move(other.customerCounter);
        this->volunteerCounter = std::move(other.volunteerCounter);
        this->orderCounter = std::move(other.orderCounter);

        this->flagCustomer = other.flagCustomer;
        other.flagCustomer = nullptr;
        this->flagVolunteer = other.flagVolunteer;
        other.flagVolunteer = nullptr;
        this->flagOrder = other.flagOrder;
        other.flagOrder = nullptr;
    }
    return *this;
}



