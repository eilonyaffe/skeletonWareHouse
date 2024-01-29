#pragma once
#include <string>
#include <vector>

#include "Order.h"
#include "Customer.h"
#include "Volunteer.h"


class BaseAction;
class Volunteer;

// Warehouse responsible for Volunteers, Customers Actions, and Orders.


class WareHouse {

    public:
        WareHouse(const string &configFilePath);
        void start();
        void simulateStepOnce(); //new
        void deleteOrderFromProccessed(int orderId); //new
        void deleteOrderFromPending(int orderId); //new
        void deleteVolunteerFromVector(int volID); //new
        bool assignSuitedCollector(Order& newOrd); //new
        bool assignSuitedDriver(Order& newOrd); //new
        void addOrder(Order* order);
        bool newOrderbyID(int customerId);//new. TODO: make const?
        void AddCustomerToWareHouse(string customerName, string customerType, int distance, int maxOrders); //new. TODO: make const?
        void addAction(BaseAction* action);
        Customer &getCustomer(int customerId) const;
        Volunteer &getVolunteer(int volunteerId) const;
        bool volunteerExists(int volunteerId); //new TODO keep?
        bool customerExists(int customerId); //new TODO keep?
        bool custCanMakeOrder(int customerId); //new
        Order &getOrder(int orderId) const;
        bool orderExists(int orderId); //new TODO keep?
        const vector<BaseAction*> &getActions() const;
        void close();
        void open();
        void parseVolunteers(const string &configFilePath);
        void parseCustomers(const string &configFilePath);
        void printAllOrders(); //TODO delete later? used for debugging

        //neya adds:
        ~WareHouse(); //overrides the default one
        WareHouse(const WareHouse& other); //overrides the default one
        WareHouse& operator=(const WareHouse& other); //overrides the default one
        //WareHouse(WareHouse&& other); //move constructor rule of 5
        //needs to implement move assignment constructor
        

        vector<Customer*> getCustomers(); //TO DELETEE COMPLETELY
        void deleteCustomer(); //TO DELETEE COMPLETELY

    private:
        bool isOpen;
        vector<BaseAction*> actionsLog;
        vector<Volunteer*> volunteers;
        vector<Order*> pendingOrders;
        vector<Order*> inProcessOrders;
        vector<Order*> completedOrders;
        vector<Customer*> customers;
        int customerCounter; //For assigning unique customer IDs
        int volunteerCounter; //For assigning unique volunteer IDs
        int orderCounter;
        SoldierCustomer *flagCustomer; //TODO remember delete
        DriverVolunteer *flagVolunteer; //TODO remember delete
        Order *flagOrder; //TODO remember delete


};

