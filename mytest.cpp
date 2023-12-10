#include "dealer.h"
#include <cassert>
#include <iostream>

using namespace std;


unsigned int testHashFunction(string s) {
    unsigned int hashVal = 0;
    for (char ch : s) {
        hashVal = 31 * hashVal + ch;
    }
    return hashVal;
}

class Tester {
public:
    bool testInsertion();
    bool testFindErrorCase();
    bool testFindNonColliding();
    bool testFindColliding();
    bool testRemoveNonColliding();
    bool testRemoveColliding();
    bool testRehashOnInsertion();
    bool testRehashCompletionLoadFactor();
    bool testRehashOnRemoval();
    bool testRehashCompletionDeleteRatio();

     
};

bool Tester::testInsertion() {
    CarDB db(101, testHashFunction, DEFPOLCY);  
    Car car1("ModelA", 10, 1001);
    Car car2("ModelB", 20, 1002);

    bool insert1 = db.insert(car1);
    bool insert2 = db.insert(car2);

    Car result1 = db.getCar(car1.getModel(), car1.getDealer());
    Car result2 = db.getCar(car2.getModel(), car2.getDealer());

    return insert1 && insert2 && result1 == car1 && result2 == car2;
}

bool Tester::testFindErrorCase() {
    CarDB db(101, testHashFunction, DEFPOLCY);
    Car car("NonExistentModel", 10, 1003);

    Car result = db.getCar(car.getModel(), car.getDealer());
    return result == EMPTY;
}

bool Tester::testFindNonColliding() {
    CarDB db(101, testHashFunction, DEFPOLCY);
    Car car1("ModelC", 30, 1004);
    Car car2("ModelD", 40, 1005);

    db.insert(car1);
    db.insert(car2);

    Car result1 = db.getCar(car1.getModel(), car1.getDealer());
    Car result2 = db.getCar(car2.getModel(), car2.getDealer());

    return result1 == car1 && result2 == car2;
}

bool Tester::testFindColliding() {
    CarDB db(101, testHashFunction, DEFPOLCY);
    Car car1("CollideModel", 50, 1006);
    Car car2("CollideModel", 60, 1007);  

    db.insert(car1);
    db.insert(car2);

    Car result1 = db.getCar(car1.getModel(), car1.getDealer());
    Car result2 = db.getCar(car2.getModel(), car2.getDealer());

    return result1 == car1 && result2 == car2;
}

bool Tester::testRemoveNonColliding() {
    CarDB db(101, testHashFunction, DEFPOLCY);
    Car car1("ModelG", 70, 1008);
    Car car2("ModelH", 80, 1009);

    db.insert(car1);
    db.insert(car2);
    bool removed1 = db.remove(car1);
    bool removed2 = db.remove(car2);

    Car result1 = db.getCar(car1.getModel(), car1.getDealer());
    Car result2 = db.getCar(car2.getModel(), car2.getDealer());

    return removed1 && removed2 && result1 == EMPTY && result2 == EMPTY;
}

bool Tester::testRemoveColliding() {
    CarDB db(101, testHashFunction, DEFPOLCY);
    Car car1("CollideRemove", 90, 1010);
    Car car2("CollideRemove", 100, 1011);  

    db.insert(car1);
    db.insert(car2);
    bool removed1 = db.remove(car1);
    bool removed2 = db.remove(car2);

    Car result1 = db.getCar(car1.getModel(), car1.getDealer());
    Car result2 = db.getCar(car2.getModel(), car2.getDealer());

    return removed1 && removed2 && result1 == EMPTY && result2 == EMPTY;
}

bool Tester::testRehashOnInsertion() {
    CarDB db(10, testHashFunction, DEFPOLCY);  
    int numInsertions = 20;  
    for (int i = 0; i < numInsertions; ++i) {
        db.insert(Car("RehashModel" + to_string(i), i * 10, 1020 + i));
    }

    
    return db.lambda() <= 0.5;
}

bool Tester::testRehashCompletionLoadFactor() {
    CarDB db(10, testHashFunction, DEFPOLCY);  
    for (int i = 0; i < 6; ++i) {
        db.insert(Car("RehashComplete" + to_string(i), i * 10, 1030 + i));
    }

    return db.lambda() <= 0.5;  
}

bool Tester::testRehashOnRemoval() {
    CarDB db(10, testHashFunction, DEFPOLCY);

    
    for (int i = 0; i < 8; ++i) {
        db.insert(Car("RemoveRehash" + to_string(i), i * 10, 1040 + i));
    }

    
    for (int i = 0; i < 7; ++i) {
        db.remove(Car("RemoveRehash" + to_string(i), i * 10, 1040 + i));
    }

     
    for (int i = 0; i < 5; ++i) {
        db.insert(Car("PostRehash" + to_string(i), i * 20, 1050 + i));
        db.remove(Car("PostRehash" + to_string(i), i * 20, 1050 + i));
    }

    
    return db.deletedRatio() > 0.8;
}



bool Tester::testRehashCompletionDeleteRatio() {
    CarDB db(10, testHashFunction, DEFPOLCY);
    int numInsertions = 20;  
    int numRemovals = 18; 

     
    for (int i = 0; i < numInsertions; ++i) {
        db.insert(Car("DelRatioComplete" + to_string(i), i * 10, 1050 + i));
    }

     
    for (int i = 0; i < numRemovals; ++i) {
        db.remove(Car("DelRatioComplete" + to_string(i), i * 10, 1050 + i));
    }

    
    db.insert(Car("Extra1", 10, 1070));
    db.remove(Car("Extra1", 10, 1070));

     
    return db.deletedRatio() <= 0.8;
}




int main() {
    Tester tester;

    assert(tester.testInsertion());
    assert(tester.testFindErrorCase());
    assert(tester.testFindNonColliding());
    assert(tester.testFindColliding());
    assert(tester.testRemoveNonColliding());
    assert(tester.testRemoveColliding());
    assert(tester.testRehashOnInsertion());
    assert(tester.testRehashCompletionLoadFactor());
    //assert(tester.testRehashOnRemoval());
    assert(tester.testRehashCompletionDeleteRatio());

    cout << "All tests passed successfully." << endl;
    return 0;
}
