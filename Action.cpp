#include "../include/Action.h"
#include "../include/Order.h"
#include "../include/WareHouse.h"
#include "../include/Customer.h"


#include <string>
#include <vector>
using std::string;
using std::vector;

#include <iostream>
using namespace std; //TODO delete later?



//BaseAction implementation
BaseAction::BaseAction():errorMsg("ERROR_Message_for_the_meantime"), status(ActionStatus::ERROR){} //maybe change the errorMsg? maybe don't start with Error status?

ActionStatus BaseAction::getStatus() const{
    return this->status;
}

string BaseAction::actionStatusAsString() const{
    if(BaseAction::status == ActionStatus::COMPLETED){
        return "COMPLETED";
    }
    else{
        return "ERROR";
    }
}

void BaseAction::complete(){
    this->status = ActionStatus::COMPLETED;
}

void BaseAction::error(string errorMsg){
    this->status = ActionStatus::ERROR; //TODO also says in the guide to update the errorMSG, to what?
    cout << "\nError: " << errorMsg << endl;
}

string BaseAction::getErrorMsg() const{ //TODO need to change it?
    return this->errorMsg;
}



//SimulateStep implementation
SimulateStep::SimulateStep(int numOfSteps):numOfSteps(numOfSteps){}

void SimulateStep::act(WareHouse &wareHouse){
    for(int i=0; i<SimulateStep::numOfSteps; i++){
        wareHouse.simulateStepOnce();
    }
    this->complete();
    wareHouse.addAction(this);
}

string SimulateStep::toString() const{
    return "SimulateStep " + to_string(SimulateStep::numOfSteps) + " " + this->actionStatusAsString();
}

SimulateStep *SimulateStep::clone() const{
    return new SimulateStep(*this);
}

//AddOrder implementation
AddOrder::AddOrder(int id): BaseAction(), customerId(id){}

void AddOrder::act(WareHouse &wareHouse){
    bool triedOrder = (wareHouse.newOrderbyID(AddOrder::customerId)); //checks if exists and can place orders, if can adds order
    if(triedOrder){
        this->complete();
    }
    else{
        this->error("Cannot place this order\n");
    }
    wareHouse.addAction(this);
}

string AddOrder::toString() const{
    return "order " + to_string(AddOrder::customerId) + " " + this->actionStatusAsString();
}

AddOrder *AddOrder::clone() const{
    return new AddOrder(*this);
}


//AddCustomer implementation
AddCustomer::AddCustomer(const string &customerName, const string &customerType, int distance, int maxOrders): BaseAction(), customerName(customerName), customerType((customerType=="Soldier")?CustomerType::Soldier:CustomerType::Civilian), customerTypeString(customerType), distance(distance), maxOrders(maxOrders){}

void AddCustomer::act(WareHouse &wareHouse){
    wareHouse.AddCustomerToWareHouse(customerName,customerTypeString, distance, maxOrders);
    this->complete();
    wareHouse.addAction(this);
}

string AddCustomer::toString() const{
    return "customer " + AddCustomer::customerName + " " + AddCustomer::customerTypeString + " " + to_string(AddCustomer::distance) + " " + to_string(AddCustomer::maxOrders) + " " + this->actionStatusAsString();
}

AddCustomer *AddCustomer::clone() const{
    return new AddCustomer(*this);
}

//PrintOrderStatus implementation
PrintOrderStatus::PrintOrderStatus(int id): BaseAction(), orderId(id){}

void PrintOrderStatus::act(WareHouse &wareHouse){
    bool orderExists = wareHouse.orderExists(PrintOrderStatus::orderId);

    if(orderExists){
        Order ord = wareHouse.getOrder(PrintOrderStatus::orderId);
        cout << "\nOrderId: " << PrintOrderStatus::orderId << endl;
        cout << "OrderStatus: " << ord.getStatusAsString() << endl;
        cout << "CustomerID: " << ord.getCustomerId() << endl;
        int collectorId = ord.getCollectorId();
        if(collectorId != -1){
            cout << "Collector: " << collectorId << endl;
        }
        else{
            cout << "Collector: None" << endl;
        }
        int driverId = ord.getDriverId();
        if(driverId != -1){
            cout << "Driver: " << driverId << "\n" << endl;
        }
        else{
            cout << "Driver: None\n" << endl;
        }
        this->complete();
    }

    else{
        this->error("Order doesn't exist\n");
    }
    wareHouse.addAction(this);
}

PrintOrderStatus *PrintOrderStatus::clone() const{
    return new PrintOrderStatus(*this);
}

string PrintOrderStatus::toString() const{
    return "orderStatus " + to_string(PrintOrderStatus::orderId) + " " + this->actionStatusAsString();
}

//PrintCustomerStatus implementation
PrintCustomerStatus::PrintCustomerStatus(int customerId): BaseAction(), customerId(customerId){}

void PrintCustomerStatus::act(WareHouse &wareHouse){
    bool customerExists = wareHouse.customerExists(PrintCustomerStatus::customerId);
    if(customerExists){
        Customer *cust = &wareHouse.getCustomer(PrintCustomerStatus::customerId); //TODO remember to delete
        cout << "\nCustomerID: " << PrintCustomerStatus::customerId << endl;
        vector<int> custOrders = cust->getOrdersIds();
        for (const auto& currOrdNum : custOrders) {
            Order ord = wareHouse.getOrder(currOrdNum);
            cout << "OrderId: " << currOrdNum << endl; 
            cout << "OrderStatus: " << ord.getStatusAsString() << endl; 
        }
        cout << "numOrdersLeft: " << (cust->getMaxOrders()-cust->getNumOrders()) << "\n" << endl; 
        this->complete();
    }
    else{
        this->error("Customer doesn't exist\n");
    }
    wareHouse.addAction(this);

}

PrintCustomerStatus *PrintCustomerStatus::clone() const{
    return new PrintCustomerStatus(*this);
}

string PrintCustomerStatus::toString() const{
    return "customerStatus " + to_string(PrintCustomerStatus::customerId) + " " + this->actionStatusAsString();
}

//PrintVolunteerStatus implementation
PrintVolunteerStatus::PrintVolunteerStatus(int volunteerId): BaseAction(), volunteerId(volunteerId){}

void PrintVolunteerStatus::act(WareHouse &wareHouse){
    bool volunteerExists = wareHouse.volunteerExists(PrintVolunteerStatus::volunteerId);
    if(volunteerExists){
        Volunteer *vol = &wareHouse.getVolunteer(PrintVolunteerStatus::volunteerId); //TODO remember to delete
        cout << "\nVolunteerID: " << PrintVolunteerStatus::volunteerId << endl;
        bool busy = vol->isBusy();
        if(busy){
            cout << "isBusy: True" << endl;
            cout << "OrderID: " << vol->getActiveOrderId() << endl;
            cout << "OrdersLeft: " << vol->timeOrDistLeft() << endl;
            cout << "OrdersLeft: " << vol->numOrdsLeft() << "\n" << endl;
        }
        else{
            cout << "isBusy: False" << endl;
            cout << "OrderID: None" << endl;
            cout << "TimeLeft: None" << endl;
            cout << "OrdersLeft: " << vol->numOrdsLeft() << "\n" << endl;
        }
        this->complete();
    }
    else{
        this->error("Volunteer doesn't exist\n");
    }
    wareHouse.addAction(this);

}

PrintVolunteerStatus *PrintVolunteerStatus::clone() const{
    return new PrintVolunteerStatus(*this);
}

string PrintVolunteerStatus::toString() const{
    return "volunteerStatus " + to_string(PrintVolunteerStatus::volunteerId) + " " + this->actionStatusAsString();
}

//PrintActionsLog implementation
PrintActionsLog::PrintActionsLog(){}

void PrintActionsLog::act(WareHouse &wareHouse){
    vector<BaseAction*> whActions = wareHouse.getActions();
    putchar('\n');
    for (const auto& action : whActions) {
        cout << action->toString() << endl; 
    }
    putchar('\n');
    this->complete();
    wareHouse.addAction(this);
}

PrintActionsLog *PrintActionsLog::clone() const{
    return new PrintActionsLog(*this);
}

string PrintActionsLog::toString() const{
    return "log " + this->actionStatusAsString();
}

//TODO close
// should use action CLOSE in the warehouse.cpp
// should use destructor of warehouse
//

//Close implementation
Close::Close(){}

void Close::act(WareHouse &wareHouse){
    wareHouse.close();
    this->complete();
    wareHouse.addAction(this);
}

Close *Close::clone() const{
    return new Close(*this);
}

string Close::toString() const{
    return "close " + this->actionStatusAsString();
}

//BackupWareHouse implementation
BackupWareHouse::BackupWareHouse(){}

void BackupWareHouse::act(WareHouse &wareHouse){
    if (backup != nullptr){ //means there is already some version of old backup
        delete backup;
    }
    backup = new WareHouse(wareHouse);

    this->complete();
    wareHouse.addAction(this);
}

BackupWareHouse *BackupWareHouse::clone() const{
    return new BackupWareHouse(*this);
}

string BackupWareHouse::toString() const{
    return "backup " + this->actionStatusAsString();
}



//RestoreWareHouse implementation
RestoreWareHouse::RestoreWareHouse(){}

void RestoreWareHouse::act(WareHouse &wareHouse){
    if (backup != nullptr){
        wareHouse = *backup;
        this->complete();
    }
    else {
        this->error("No backup available\n");
    }
    wareHouse.addAction(this);
}

RestoreWareHouse *RestoreWareHouse::clone() const{
    return new RestoreWareHouse(*this);
}

string RestoreWareHouse::toString() const{
    return "restore " + this->actionStatusAsString();
}


// int main(){
//     string fileLocation = "../bin/rest/configFileExample.txt";
//     WareHouse wh(fileLocation);

//     AddCustomer *addCust = new AddCustomer("Dov", "civilian", 7, 2);
//     addCust->act(wh);

//     AddOrder *adder11 = new AddOrder(1);
//     AddOrder *adder12 = new AddOrder(1);
//     AddOrder *adder21 = new AddOrder(2);
//     AddOrder *adder31 = new AddOrder(3);


//     adder11->act(wh);
//     adder12->act(wh);
//     adder21->act(wh);
//     adder31->act(wh);

//     SimulateStep *stepper = new SimulateStep(7);
//     stepper->act(wh);

//     wh.printAllOrders();

//     PrintActionsLog *prAcLog = new PrintActionsLog();
//     prAcLog->act(wh);
//     return 0;
// }
