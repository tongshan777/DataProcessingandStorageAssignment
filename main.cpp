#include <iostream>
#include <string>
#include <unordered_map>
using namespace std;

class InMemoryDB{
private:
    unordered_map<string, int> primary_DB;
    unordered_map<string, int> transaction_DB;
    bool isTransaction = false;
    bool isCommited = false;

public:
    // get the value of the key
    int get(const string& key) {
        auto it_transaction = transaction_DB.find(key);
        if (it_transaction != transaction_DB.end()) {
            if(!isCommited){
                return NULL;
            }
            return it_transaction -> second;
        }

        auto it_primary = primary_DB.find(key);
        if(it_primary != primary_DB.end()){
            if(!isCommited){
                return NULL;
            }
            return it_primary -> second;
        }
    }

    // add or update the key-value pair
    void put(const string& key, int val){
        if(!isTransaction){
            throw runtime_error("Transaction is not in progress.");
        }
        transaction_DB[key] = val;
        isCommited = false;
        cout << "Set value of " << key << " to " << transaction_DB[key] << ", but it's not committed yet."<< endl;
    }

    void begin_transaction(){
        if(isTransaction){
            throw runtime_error("Transaction already started.");
        }

        for (auto it = transaction_DB.begin(); it != transaction_DB.end(); ) {
            it = transaction_DB.erase(it);
        }
        isTransaction = true;
        cout << "Starts a new transaction!" << endl;
    }

    void commit(){
        if(!isTransaction){
            throw runtime_error("No transaction to commit.");
        }
        for (const auto& pair : transaction_DB) {
            primary_DB[pair.first] = pair.second;
        }
        for (auto it = transaction_DB.begin(); it != transaction_DB.end(); ) {
            it = transaction_DB.erase(it);
        }
        isTransaction= false;
        cout << "Transaction has committed!" << endl;
        isCommited = true;
    }

    void rollback(){
        if(!isTransaction){
            throw runtime_error("No transaction need rollback.");
        }
        for (auto it = transaction_DB.begin(); it != transaction_DB.end(); ) {
            it = transaction_DB.erase(it);
        }
        isTransaction = false;
        cout << "Transaction need rolled back." << endl;
    }
};

int main() {
    InMemoryDB inmemoryDB;

    try {
        // should return null, because A doesn’t exist in the DB yet
        inmemoryDB.get("A");
    } catch (const runtime_error& e) {
        cout << "Error: " << e.what() << endl;
    }

    try {
        // should throw an error because a transaction is not in progress
        inmemoryDB.put("A", 5);
    } catch (const runtime_error& e) {
        cout << "Error: " << e.what() << endl;
    }

    try {
        // starts a new transaction
        inmemoryDB.begin_transaction();
    } catch (const runtime_error& e) {
        cout << "Error: " << e.what() << endl;
    }

    try {
        // set’s value of A to 5, but its not committed yet
        inmemoryDB.put("A", 5);
    } catch (const runtime_error& e) {
        cout << "Error: " << e.what() << endl;
    }

    try {
        // should return null, because updates to A are not committed yet
        cout << inmemoryDB.get("A") << endl;
    } catch (const runtime_error& e) {
        cout << "Error: " << e.what() << endl;
    }

    try {
        // update A’s value to 6 within the transaction
        inmemoryDB.put("A", 6);
    } catch (const runtime_error& e) {
        cout << "Error: " << e.what() << endl;
    }

    try {
        // commits the open transaction
        inmemoryDB.commit();
    } catch (const runtime_error& e) {
        cout << "Error: " << e.what() << endl;
    }

    try {
        // should return 6, that was the last value of A to be committed
        inmemoryDB.get("A");
    } catch (const runtime_error& e) {
        cout << "Error: " << e.what() << endl;
    }

    try {
        // throws an error, because there is no open transaction
        inmemoryDB.commit();
    } catch (const runtime_error& e) {
        cout << "Error: " << e.what() << endl;
    }

    try {
        // throws an error because there is no ongoing transaction
        inmemoryDB.rollback();
    } catch (const runtime_error& e) {
        cout << "Error: " << e.what() << endl;
    }

    try {
        // should return null because B does not exist in the database
        inmemoryDB.get("B");
    } catch (const runtime_error& e) {
        cout << "Error: " << e.what() << endl;
    }

    try {
        // starts a new transaction
        inmemoryDB.begin_transaction();
    } catch (const runtime_error& e) {
        cout << "Error: " << e.what() << endl;
    }

    try {
        // Set key B’s value to 10 within the transaction
        inmemoryDB.put("B", 10);
    } catch (const runtime_error& e) {
        cout << "Error: " << e.what() << endl;
    }

    try {
        // Rollback the transaction - revert any changes made to B
        inmemoryDB.rollback();
    } catch (const runtime_error& e) {
        std::cout << "Error: " << e.what() << endl;
    }

    try {
        // Should return null because changes to B were rolled back
        inmemoryDB.get("B");
    } catch (const runtime_error& e) {
        cout << "Error: " << e.what() << endl;
    }

    return 0;
}
