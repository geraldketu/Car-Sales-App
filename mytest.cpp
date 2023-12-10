#include "pqueue.h"
#include <iostream>
#include <cassert>
#include <math.h>  
#include <random>

using namespace std;

int priorityFn1(const Patient& p) {
    return static_cast<int>(p.getTemperature());   
}

int priorityFn2(const Patient& p) {
    return static_cast<int>(p.getOxygen());       
}

class Tester {
public:
    bool testInsertMinHeap() {
        PQueue pq(priorityFn1, MINHEAP, SKEW);

         
        for (int i = 0; i < 300; i++) {
            Patient p;
            p.setTemperature(Random(-100, 150));   
            pq.insertPatient(p);
        }

       
        return checkMinHeap(pq.m_heap, priorityFn1);
    }

    bool checkMinHeap(Node* node, prifn_t priFn) {
        if (node == nullptr) {
            return true;
        }

        if (node->m_left != nullptr && priFn(node->m_left->getPatient()) < priFn(node->getPatient())) {
            return false;
        }

        if (node->m_right != nullptr && priFn(node->m_right->getPatient()) < priFn(node->getPatient())) {
            return false;
        }

        return checkMinHeap(node->m_left, priFn) && checkMinHeap(node->m_right, priFn);
    }

    bool testInsertMaxHeap() {
        PQueue pq(priorityFn2, MAXHEAP, LEFTIST);

        for (int i = 0; i < 300; i++) {
            Patient p;
            p.setOxygen(Random(-100, 150));  
            pq.insertPatient(p);
        }

         
        return checkMaxHeap(pq.m_heap, priorityFn2);
    }

    bool checkMaxHeap(Node* node, prifn_t priFn) {
        if (node == nullptr) {
            return true;
        }

        if (node->m_left != nullptr && priFn(node->m_left->getPatient()) > priFn(node->getPatient())) {
            return false;
        }

        if (node->m_right != nullptr && priFn(node->m_right->getPatient()) > priFn(node->getPatient())) {
            return false;
        }

        return checkMaxHeap(node->m_left, priFn) && checkMaxHeap(node->m_right, priFn);
    }

    int Random(int min, int max) {
        static std::random_device rd;  
        static std::mt19937 gen(rd());  
        std::uniform_int_distribution<> distr(min, max);  

        return distr(gen);
    }

    bool testRemoveMinHeap() {
        PQueue pq(priorityFn1, MINHEAP, SKEW);

        for (int i = 0; i < 100; i++) {
            Patient p;
            p.setTemperature(Random(-100, 150));
            pq.insertPatient(p);
        }

        double lastTemp = -INFINITY;
        while (pq.numPatients() > 0) {
            Patient p = pq.getNextPatient();
            if (priorityFn1(p) < lastTemp) {
                return false;
            }
            lastTemp = priorityFn1(p);
        }

        return true;
    }

    bool testRemoveMaxHeap() {
        PQueue pq(priorityFn2, MAXHEAP, LEFTIST);

        for (int i = 0; i < 100; i++) {
            Patient p;
            p.setOxygen(Random(-100, 150));
            pq.insertPatient(p);
        }

        double lastOxygen = INFINITY;
        while (pq.numPatients() > 0) {
            Patient p = pq.getNextPatient();
            if (priorityFn2(p) > lastOxygen) {
                return false;
            }
            lastOxygen = priorityFn2(p);
        }

        return true;
    }

    bool testLeftistHeapNPL() {
        PQueue pq(priorityFn1, MINHEAP, LEFTIST);

         
        for (int i = 0; i < 100; i++) {
            Patient p("Patient" + to_string(i), Random(MINTEMP, MAXTEMP), Random(MINOX, MAXOX), Random(MINRR, MAXRR), Random(MINBP, MAXBP), Random(MINOPINION, MAXOPINION));
            pq.insertPatient(p);
        }

        return checkNPL(pq.m_heap);
    }

    bool checkNPL(Node* node) {
        if (!node) return true;

        int leftNPL = (node->m_left) ? node->m_left->getNPL() : 0;
        int rightNPL = (node->m_right) ? node->m_right->getNPL() : 0;
        bool correctNPL = node->getNPL() == 1 + min(leftNPL, rightNPL);

        return correctNPL && checkNPL(node->m_left) && checkNPL(node->m_right);
    }

    bool testLeftistHeapProperty() {
        PQueue pq(priorityFn1, MINHEAP, LEFTIST);

 
        for (int i = 0; i < 100; i++) {
            Patient p("Patient" + to_string(i), Random(MINTEMP, MAXTEMP), Random(MINOX, MAXOX), Random(MINRR, MAXRR), Random(MINBP, MAXBP), Random(MINOPINION, MAXOPINION));
            pq.insertPatient(p);
        }

        return checkLeftistProperty(pq.m_heap);
    }

    bool checkLeftistProperty(Node* node) {
        if (!node) return true;

        int leftNPL = (node->m_left) ? node->m_left->getNPL() : 0;
        int rightNPL = (node->m_right) ? node->m_right->getNPL() : 0;
        bool leftistProperty = leftNPL >= rightNPL;

        return leftistProperty && checkLeftistProperty(node->m_left) && checkLeftistProperty(node->m_right);
    }

 

    bool testChangePriorityFn() {
        PQueue pq(priorityFn1, MINHEAP, LEFTIST);

        for (int i = 0; i < 100; i++) {
            Patient p("Patient" + to_string(i), Random(MINTEMP, MAXTEMP), Random(MINOX, MAXOX), Random(MINRR, MAXRR), Random(MINBP, MAXBP), Random(MINOPINION, MAXOPINION));
            pq.insertPatient(p);
        }

        pq.setPriorityFn(priorityFn2, MAXHEAP);
        return checkMaxHeap(pq.m_heap, priorityFn2);
    }

    bool testMergeEmpty() {
        PQueue pq1(priorityFn1, MINHEAP, LEFTIST);  
        PQueue pq2(priorityFn1, MINHEAP, LEFTIST);  

         
        for (int i = 0; i < 50; i++) {
            Patient p("Patient" + to_string(i), Random(MINTEMP, MAXTEMP), Random(MINOX, MAXOX), Random(MINRR, MAXRR), Random(MINBP, MAXBP), Random(MINOPINION, MAXOPINION));
            pq1.insertPatient(p);
        }

        int sizeBeforeMerge = pq1.numPatients();
        pq2.mergeWithQueue(pq1);  

        return pq2.numPatients() == sizeBeforeMerge && pq1.numPatients() == 0;
    }


    bool testCopyConstructorNormal() {
        PQueue pq1(priorityFn1, MINHEAP, LEFTIST);

         
        for (int i = 0; i < 50; i++) {
            Patient p("Patient" + to_string(i), Random(MINTEMP, MAXTEMP), Random(MINOX, MAXOX), Random(MINRR, MAXRR), Random(MINBP, MAXBP), Random(MINOPINION, MAXOPINION));
            pq1.insertPatient(p);
        }

        PQueue pq2(pq1);  

         
        if (pq1.numPatients() != pq2.numPatients()) {
            return false;
        }

         
        while (pq1.numPatients() > 0) {
            Patient patient1 = pq1.getNextPatient();
            Patient patient2 = pq2.getNextPatient();

            if (!(patient1 == patient2)) {
                return false;
            }
        }

        return true;
    }

     

    bool testCopyConstructorEdge() {
        PQueue pq1(priorityFn1, MINHEAP, LEFTIST);   

        PQueue pq2(pq1);   

        return pq2.numPatients() == 0;   
    }

    bool testAssignmentOperatorNormal() {
        PQueue pq1(priorityFn1, MINHEAP, LEFTIST);

         
        for (int i = 0; i < 50; i++) {
            Patient p("Patient" + to_string(i), Random(MINTEMP, MAXTEMP), Random(MINOX, MAXOX), Random(MINRR, MAXRR), Random(MINBP, MAXBP), Random(MINOPINION, MAXOPINION));
            pq1.insertPatient(p);
        }

        PQueue pq2;
        pq2 = pq1;  

         
        if (pq1.numPatients() != pq2.numPatients()) {
            return false;
        }

        
        while (pq1.numPatients() > 0) {
            Patient patient1 = pq1.getNextPatient();
            Patient patient2 = pq2.getNextPatient();

            if (!(patient1 == patient2)) {
                return false;
            }
        }

        return true;
    }

    bool testAssignmentOperatorEdge() {
        PQueue pq1;   
        PQueue pq2;
        pq2 = pq1;   

        return pq2.numPatients() == 0;   
    }

     
    bool testDequeueEmpty() {
        PQueue pq(priorityFn1, MINHEAP, LEFTIST);   

        try {
            pq.getNextPatient();
            return false;   
        }
        catch (out_of_range& e) {
            return true;   
        }
    }

    bool testMergeDifferentPriorityFns() {
        PQueue pq1(priorityFn1, MINHEAP, LEFTIST);
        PQueue pq2(priorityFn2, MAXHEAP, SKEW);

        try {
            pq1.mergeWithQueue(pq2);
            return false;   
        }
        catch (domain_error& e) {
            return true;   
        }
    };
};

int main() {
    Tester tester;

    cout << "Test insert min heap: " << tester.testInsertMinHeap() << endl;
    cout << "Test insert max heap: " << tester.testInsertMaxHeap() << endl;
    cout << "Test remove min heap: " << tester.testRemoveMinHeap() << endl;
    cout << "Test remove max heap: " << tester.testRemoveMaxHeap() << endl;
    cout << "Test leftist heap NPL: " << tester.testLeftistHeapNPL() << endl;
    cout << "Test leftist heap property: " << tester.testLeftistHeapProperty() << endl;
    cout << "Test change priority fn: " << tester.testChangePriorityFn() << endl;
    cout << "Test merge empty: " << tester.testMergeEmpty() << endl;
    cout << "Test copy constructor normal: " << tester.testCopyConstructorNormal() << endl;
    cout << "Test copy constructor edge: " << tester.testCopyConstructorEdge() << endl;
    cout << "Test assignment operator normal: " << tester.testAssignmentOperatorNormal() << endl;
    cout << "Test assignment operator edge: " << tester.testAssignmentOperatorEdge() << endl;
    cout << "Test dequeue empty: " << tester.testDequeueEmpty() << endl;
    cout << "Test merge different priority fns: " << tester.testMergeDifferentPriorityFns() << endl;

    return 0;
}


