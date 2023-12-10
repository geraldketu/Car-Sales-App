#include "pqueue.h"

PQueue::PQueue()
    : m_priorFunc(nullptr), m_heapType(MINHEAP), m_structure(LEFTIST), m_heap(nullptr), m_size(0) {
     
}

 
PQueue::PQueue(prifn_t priFn, HEAPTYPE heapType, STRUCTURE structure)
    : m_priorFunc(priFn), m_heapType(heapType), m_structure(structure), m_heap(nullptr), m_size(0) {}

 
PQueue::~PQueue() {
    clear();
}

 
void PQueue::clear() {
    clearHeap(m_heap);
    m_heap = nullptr;
    m_size = 0;
}

 
void PQueue::clearHeap(Node* node) {
    if (node) {
        clearHeap(node->m_left);
        clearHeap(node->m_right);
        delete node;
    }
}


PQueue::PQueue(const PQueue& rhs) {
    *this = rhs;
}

 
Node* PQueue::copyHeap(Node* node) {
    if (!node) return nullptr;

    Node* newNode = new Node(node->getPatient());
    newNode->m_left = copyHeap(node->m_left);
    newNode->m_right = copyHeap(node->m_right);
    newNode->setNPL(node->getNPL());

    return newNode;
}

 
PQueue& PQueue::operator=(const PQueue& rhs) {
    if (this != &rhs) {
        clear();
        m_heapType = rhs.m_heapType;
        m_priorFunc = rhs.m_priorFunc;
        m_structure = rhs.m_structure;
        m_size = rhs.m_size;
        m_heap = copyHeap(rhs.m_heap);
    }
    return *this;
}

 
void PQueue::mergeWithQueue(PQueue& rhs) {
    if (m_priorFunc != rhs.m_priorFunc || m_structure != rhs.m_structure)
        throw domain_error("Queues have different configurations.");
    if (this == &rhs)
        throw domain_error("Cannot merge a queue with itself.");

    m_heap = merge(m_heap, rhs.m_heap);
    rhs.m_heap = nullptr;
    m_size += rhs.m_size;
    rhs.m_size = 0;
}

void PQueue::insertPatient(const Patient& input) {
    Node* newNode = new Node(input);
    m_heap = merge(m_heap, newNode);
    m_size++;
}

 
Node* PQueue::merge(Node* h1, Node* h2) {
    if (!h1) return h2;
    if (!h2) return h1;

     
    if (m_heapType == MAXHEAP ? m_priorFunc(h1->getPatient()) < m_priorFunc(h2->getPatient()) :
        m_priorFunc(h1->getPatient()) > m_priorFunc(h2->getPatient())) {
        swap(h1, h2);
    }

     
    h1->m_right = merge(h1->m_right, h2);

     
    if (!h1->m_left || h1->m_left->getNPL() < h1->m_right->getNPL()) {
        swap(h1->m_left, h1->m_right);
    }

     
    h1->setNPL(h1->m_right ? h1->m_right->getNPL() + 1 : 0);

    return h1;
}
 
int PQueue::numPatients() const {
    return m_size;
}

 
prifn_t PQueue::getPriorityFn() const {
    return m_priorFunc;
}

 
Patient PQueue::getNextPatient() {
    if (!m_heap) throw out_of_range("Queue is empty");

    Patient topPriorityPatient = m_heap->getPatient();
    Node* leftChild = m_heap->m_left;
    Node* rightChild = m_heap->m_right;

    delete m_heap;
    m_heap = merge(leftChild, rightChild);
    m_size--;

    return topPriorityPatient;
}

 
void PQueue::setPriorityFn(prifn_t priFn, HEAPTYPE heapType) {
    m_priorFunc = priFn;
    m_heapType = heapType;
    reorganizeQueue();
}

 
void PQueue::setStructure(STRUCTURE structure) {
    m_structure = structure;
    reorganizeQueue();
}

 
void PQueue::reorganizeQueue() {
    PQueue tempQueue(m_priorFunc, m_heapType, m_structure);
    while (m_heap) {
        tempQueue.insertPatient(getNextPatient());
    }
    swap(m_heap, tempQueue.m_heap);
    m_size = tempQueue.m_size;
}

 
STRUCTURE PQueue::getStructure() const {
    return m_structure;
}

 
HEAPTYPE PQueue::getHeapType() const {
    return m_heapType;
}

void PQueue::printPatientQueue() const {
    cout << "Contents of the queue:\n";
    preorderPrint(m_heap);
}

void PQueue::preorderPrint(Node* node) const {
    if (node) {
        cout << "[" << m_priorFunc(node->getPatient()) << "] " << node->getPatient() << endl;
        preorderPrint(node->m_left);
        preorderPrint(node->m_right);
    }
}

 
void PQueue::dump() const {
    if (m_size == 0) {
        cout << "Empty heap.\n";
    }
    else {
        dumpHeap(m_heap);
        cout << endl;
    }
}


void PQueue::dumpHeap(Node* node) const {
    if (node) {
        cout << "(";
        dumpHeap(node->m_left);
        
        cout << m_priorFunc(node->getPatient()) << ":" << node->getPatient().getPatient();
        if (m_structure != SKEW) {
            cout << ":" << node->getNPL();  
        }
        dumpHeap(node->m_right);
        cout << ")";
    }
}

 
ostream& operator<<(ostream& out, const Patient& patient) {
    return out << patient.getPatient()
        << ", temperature: " << patient.getTemperature()
        << ", oxygen: " << patient.getOxygen()
        << ", respiratory: " << patient.getRR()
        << ", blood pressure: " << patient.getBP()
        << ", nurse opinion: " << patient.getOpinion();
}

 
ostream& operator<<(ostream& out, const Node& node) {
    return out << node.getPatient();
}

