#include "../include/Action.h"
#include "../include/WareHouse.h"

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

void BaseAction::complete(){
    this->status = ActionStatus::COMPLETED;
}

void BaseAction::error(string errorMsg){
    this->status = ActionStatus::ERROR; //TODO also says in the guide to update the errorMSG, to what?
    cout << "Error: " << errorMsg << endl;
}

string BaseAction::getErrorMsg() const{
    return this->errorMsg;
}



//TODO implement SimulateStep

//


//AddOrder implementation
AddOrder::AddOrder(int id): BaseAction(), customerId(id){}

void AddOrder::act(WareHouse &wareHouse){
    bool triedOrder = (wareHouse.newOrderbyID(AddOrder::customerId)); //checks if exists and can place orders, if can adds order
    if(triedOrder){
        wareHouse.newOrderbyID(AddOrder::customerId);
        this->complete();
    }
    else{
        this->error("Cannot place this order”");
    }    
}

string AddOrder::toString() const{
    return "addOrder object, with customerId: " + to_string(AddOrder::customerId);
}

AddOrder *AddOrder::clone() const{
    return new AddOrder(*this);
}


//AddCustomer implementation
AddCustomer::AddCustomer(string customerName, string customerType, int distance, int maxOrders):customerName(customerName), customerType(customerType), distance(distance), maxOrders(maxOrders){}

void AddCustomer::act(WareHouse &wareHouse){
    //TODO need to assign him and id using warehouse customer counter
    

}



int main(){
    string fileLocation = "../bin/rest/configFileExample.txt";
    WareHouse wh(fileLocation);
    AddOrder *adder = new AddOrder(3);
    cout << (*adder).toString() << endl;

    return 0;
}