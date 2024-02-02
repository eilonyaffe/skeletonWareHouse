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
        void simulateStepOnce(); 
        void deleteOrderFromProccessed(int orderId); 
        void deleteOrderFromPending(int orderId); 
        void deleteVolunteerFromVector(int volID); 
        bool assignSuitedCollector(Order& newOrd); 
        bool assignSuitedDriver(Order& newOrd); 
        void addOrder(Order* order);
        bool newOrderbyID(int customerId);
        void AddCustomerToWareHouse(string customerName, string customerType, int distance, int maxOrders);
        void addAction(BaseAction* action);
        Customer &getCustomer(int customerId) const;
        Volunteer &getVolunteer(int volunteerId) const;
        bool volunteerExists(int volunteerId);
        bool customerExists(int customerId);
        bool custCanMakeOrder(int customerId);
        Order &getOrder(int orderId) const;
        bool orderExists(int orderId);
        const vector<BaseAction*> &getActions() const;
        void close();
        void open();
        void parseVolunteers(const string &configFilePath);
        void parseCustomers(const string &configFilePath);
        
        //Rule Of 5
        ~WareHouse(); 
        WareHouse(const WareHouse& other); 
        WareHouse& operator=(const WareHouse& other); 
        WareHouse(WareHouse&& other) noexcept; 
        WareHouse& operator=(WareHouse&& other) noexcept;
        
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

