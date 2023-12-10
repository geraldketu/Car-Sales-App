// CMSC 341 - Fall 2023 - Project 4
#ifndef DEALER_H
#define DEALER_H
#include <iostream>
#include <string>
#include "math.h"
using namespace std;
class Grader;
class Tester;
class Car;
class CarDB;
const int MINID = 1000;     // dealer ID
const int MAXID = 9999;     // dealer ID
const int MINPRIME = 101;   // Min size for hash table
const int MAXPRIME = 99991; // Max size for hash table
#define EMPTY Car("",0,0,false)
typedef unsigned int (*hash_fn)(string); // declaration of hash function
enum prob_t { NONE, QUADRATIC, DOUBLEHASH }; // types of collision handling policy
#define DEFPOLCY QUADRATIC

class Car {
    friend class Tester;
    friend class Grader;
    friend class CarDB;
public:
    Car(string model = "", int quantity = 0, int dealer = 0, bool used = false) {
        m_model = model;
        m_quantity = quantity;
        m_dealer = dealer;
        m_used = used;
    }
    void setModel(string model) { m_model = model; }
    void setQuantity(int quantity) { m_quantity = quantity; }
    void setDealer(int dealer) { m_dealer = dealer; }
    void setUsed(bool used) { m_used = used; }
    string getModel() const { return m_model; }
    int getQuantity() const { return m_quantity; }
    int getDealer() const { return m_dealer; }
    bool getUsed() const { return m_used; }
    // overloaded assignment operator
    const Car& operator=(const Car& rhs) {
        if (this != &rhs) {
            m_model = rhs.m_model;
            m_quantity = rhs.m_quantity;
            m_dealer = rhs.m_dealer;
            m_used = rhs.m_used;
        }
        return *this;
    }
    // Overloaded insertion operator
    friend ostream& operator<<(ostream& sout, const Car& d);
    // Overloaded equality operator
    friend bool operator==(const Car& lhs, const Car& rhs);
private:
    string m_model;     // car type, used as a key for finding index in hash table 
    int m_quantity;     // number of card delivered to the dealer
    int m_dealer;       // dealer information, car model and dealer together determine uniqueness of a Car object 
    // the following variable is used for lazy delete scheme in hash table
    // if it is set to false, it means the bucket in the hash table is free for insert
    // if it is set to true, it means the bucket contains live data, and we cannot overwrite it
    bool m_used;
};

class CarDB {
public:
    friend class Grader;
    friend class Tester;
    CarDB(int size, hash_fn hash, prob_t probing);
    ~CarDB();
    // Returns Load factor of the new table
    float lambda() const;
    // Returns the ratio of deleted slots in the new table
    float deletedRatio() const;
    // insert only happens in the new table
    bool insert(Car car);
    // remove can happen from either table
    bool remove(Car car);
    // find can happen in either table
    Car getCar(string model, int dealer) const;
    // update the information
    bool updateQuantity(Car car, int quantity);
    void changeProbPolicy(prob_t policy);
    void dump() const;

private:
    hash_fn    m_hash;          // hash function
    prob_t     m_newPolicy;     // stores the change of policy request

    Car* m_currentTable;  // hash table
    int        m_currentCap;    // hash table size (capacity)
    int        m_currentSize;   // current number of entries
    // m_currentSize includes deleted entries 
    int        m_currNumDeleted;// number of deleted entries
    prob_t     m_currProbing;       // collision handling policy

    Car* m_oldTable;      // hash table
    int        m_oldCap;        // hash table size (capacity)
    int        m_oldSize;       // current number of entries
    // m_oldSize includes deleted entries
    int        m_oldNumDeleted; // number of deleted entries
    prob_t     m_oldProbing;    // collision handling policy

    //private helper functions
    bool isPrime(int number);
    int findNextPrime(int current);

    /******************************************
    * Private function declarations go here! *
    ******************************************/
    int m_rehashIndex;    // Tracks the index for the next rehash transfer
    bool m_isRehashing;   // Indicates if rehashing is in progress
    void transferNodes();
    
    bool updateInTable(Car* table, int tableSize, Car car, int quantity); // Helper function to update car quantity in a specific table
    unsigned int getNextIndex(unsigned int currentIndex, const string& model, unsigned int probeCount, int tableSize) const;
    int findCarIndex(const string& model, int dealer, const Car* table, int tableSize) const; // Find the index of a car in a specified table
    unsigned int secondaryHash(const string& model) const;
    bool removeFromTable(Car car, Car* table, int tableSize);
    int resolveCollision(const string& model, Car* table, int tableSize);
    void rehash();
    void checkRehashingNeed();


};
#endif