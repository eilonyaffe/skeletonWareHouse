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
        void addOrder(Order* order);
        bool newOrderbyID(int customerId);//new. TODO: make const?
        void AddCustomer(string customerName, string customerType, int distance, int maxOrders); //new. TODO: make const?

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

