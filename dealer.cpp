#include "dealer.h"

CarDB::CarDB(int size, hash_fn hashFunc, prob_t probingPolicy = DEFPOLCY) {
    m_currentCap = std::max(MINPRIME, std::min(size, MAXPRIME));
    if (!isPrime(m_currentCap)) m_currentCap = findNextPrime(m_currentCap);
    m_currentTable = new Car[m_currentCap]();
    m_hash = hashFunc;
    m_currProbing = probingPolicy;
    m_oldTable = nullptr;
    m_currentSize = m_currNumDeleted = 0;
}

CarDB::~CarDB() {
    delete[] m_currentTable;
    delete[] m_oldTable;
}

bool CarDB::insert(Car car) {
    if (car.getDealer() < MINID || car.getDealer() > MAXID) return false;

    int index = resolveCollision(car.getModel(), m_currentTable, m_currentCap);
    if (index < 0) return false; // Table full

    m_currentTable[index] = car;
    m_currentTable[index].setUsed(true);
    m_currentSize++;

    checkRehashingNeed();
    if (m_isRehashing) {
        transferNodes();
    }
    return true;
}


bool CarDB::remove(Car car) {
    if (!removeFromTable(car, m_currentTable, m_currentCap)) {
        if (m_oldTable && removeFromTable(car, m_oldTable, m_oldCap)) {
            m_oldSize--;
            m_currNumDeleted++;
            checkRehashingNeed();
            if (m_isRehashing) {
                transferNodes();
            }
            return true;
        }
        return false;
    }
    m_currentSize--;
    m_currNumDeleted++;
    checkRehashingNeed();

    if (m_isRehashing) {
        transferNodes();
    }
    return true;
}




Car CarDB::getCar(string model, int dealer) const {
    int index = findCarIndex(model, dealer, m_currentTable, m_currentCap);
    if (index >= 0) return m_currentTable[index];

    if (m_oldTable) {
        index = findCarIndex(model, dealer, m_oldTable, m_oldCap);
        if (index >= 0) return m_oldTable[index];
    }

    return EMPTY;
}



float CarDB::lambda() const {
    return static_cast<double>(m_currentSize - m_currNumDeleted) / m_currentCap;
}


float CarDB::deletedRatio() const {
    return m_currentSize == 0 ? 0.0f : static_cast<float>(m_currNumDeleted) / m_currentSize;
}



 
void CarDB::checkRehashingNeed() {
    if (lambda() > 0.5 || deletedRatio() > 0.8) {
        rehash();
    }
}





void CarDB::rehash() {
    // Calculate the new size for the hash table
    int newSize = findNextPrime(m_currentSize * 4);

    // Allocate a new table
    Car* newTable = new Car[newSize]();

    // Initialize rehashing parameters
    m_rehashIndex = 0;
    m_isRehashing = true;

    // Assign new table and capacity
    m_oldTable = m_currentTable;
    m_oldCap = m_currentCap;
    m_currentTable = newTable;
    m_currentCap = newSize;

    // Begin transferring nodes
    transferNodes();
}



 
int CarDB::resolveCollision(const string& model, Car* table, int tableSize) {
    unsigned int hashIndex = m_hash(model) % tableSize;
    unsigned int probeCount = 0;

    while (probeCount < tableSize) {
        if (!table[hashIndex].getUsed()) {
            return hashIndex;
        }

        hashIndex = getNextIndex(hashIndex, model, probeCount, tableSize);
        probeCount++;
    }

    return -1; // Table full or no available slot found
}

 
bool CarDB::removeFromTable(Car car, Car* table, int tableSize) {
    int index = findCarIndex(car.getModel(), car.getDealer(), table, tableSize);
    if (index >= 0) {
        table[index].setUsed(false);
        m_currNumDeleted++; // Update the number of deleted entries
        return true;
    }
    return false;
}

 
int CarDB::findCarIndex(const string& model, int dealer, const Car* table, int tableSize) const {
    unsigned int hashIndex = m_hash(model) % tableSize;
    unsigned int probeCount = 0;

    while (probeCount < tableSize && table[hashIndex].getUsed()) {
        if (table[hashIndex].getModel() == model && table[hashIndex].getDealer() == dealer) {
            return hashIndex;
        }

        hashIndex = getNextIndex(hashIndex, model, probeCount, tableSize);
        probeCount++;
    }

    return -1;  
}

 
unsigned int CarDB::getNextIndex(unsigned int currentIndex, const string& model, unsigned int probeCount, int tableSize) const {
     
    if (m_currProbing == QUADRATIC) {
        return (currentIndex + probeCount * probeCount) % tableSize;
    }
    else if (m_currProbing == DOUBLEHASH) {
        return (currentIndex + probeCount * secondaryHash(model)) % tableSize;
    }
    return (currentIndex + probeCount) % tableSize;  
}

 
unsigned int CarDB::secondaryHash(const string& model) const {
     
    unsigned int hashVal = 0;
    for (char ch : model) {
        hashVal = 31 * hashVal + ch;
    }
    return hashVal % 11;
}



void CarDB::dump() const {
    cout << "Dump for the current table: " << endl;
    if (m_currentTable != nullptr)
        for (int i = 0; i < m_currentCap; i++) {
            cout << "[" << i << "] : " << m_currentTable[i] << endl;
        }
    cout << "Dump for the old table: " << endl;
    if (m_oldTable != nullptr)
        for (int i = 0; i < m_oldCap; i++) {
            cout << "[" << i << "] : " << m_oldTable[i] << endl;
        }
}



bool CarDB::isPrime(int number) {
    bool result = true;
    for (int i = 2; i <= number / 2; ++i) {
        if (number % i == 0) {
            result = false;
            break;
        }
    }
    return result;
}

int CarDB::findNextPrime(int current) {
    
    if (current < MINPRIME) current = MINPRIME - 1;
    for (int i = current; i < MAXPRIME; i++) {
        for (int j = 2; j * j <= i; j++) {
            if (i % j == 0)
                break;
            else if (j + 1 > sqrt(i) && i != current) {
                return i;
            }
        }
    }
    




    return MAXPRIME;
}

ostream& operator<<(ostream& sout, const Car& car) {
    if (!car.m_model.empty())
        sout << car.m_model << " (" << car.m_dealer << "," << car.m_quantity << ")";
    else
        sout << "";
    return sout;
}

bool operator==(const Car& lhs, const Car& rhs) {
   
    return ((lhs.m_model == rhs.m_model) && (lhs.m_dealer == rhs.m_dealer));
}



bool CarDB::updateQuantity(Car car, int quantity) {
    
    if (updateInTable(m_currentTable, m_currentCap, car, quantity)) {
        return true;
    }

   
    if (m_oldTable != nullptr) {
        return updateInTable(m_oldTable, m_oldCap, car, quantity);
    }

    return false;
}


bool CarDB::updateInTable(Car* table, int tableSize, Car car, int quantity) {
    int index = findCarIndex(car.getModel(), car.getDealer(), table, tableSize);
    if (index >= 0) {
        table[index].setQuantity(quantity);
        return true;
    }
    return false;
}



void CarDB::changeProbPolicy(prob_t policy) {
    m_newPolicy = policy;
}


void CarDB::transferNodes() {
    int count = 0;
    int transferLimit = std::max(1, m_oldSize / 4); // 25% of old table size, at least 1

    while (count < transferLimit && m_rehashIndex < m_oldCap) {
        if (m_oldTable[m_rehashIndex].getUsed() && !(m_oldTable[m_rehashIndex] == EMPTY)) {
            Car& oldCar = m_oldTable[m_rehashIndex];
            int newIndex = resolveCollision(oldCar.getModel(), m_currentTable, m_currentCap);
            if (newIndex >= 0) {
                m_currentTable[newIndex] = oldCar;
                m_currentTable[newIndex].setUsed(true);
                oldCar.setUsed(false);
                count++;
            }
        }
        m_rehashIndex++;
    }

    if (m_rehashIndex >= m_oldCap) {
        delete[] m_oldTable;
        m_oldTable = nullptr;
        m_oldCap = 0;
        m_oldSize = 0;
        m_isRehashing = false;
        // Reset the delete count as deleted entries are not carried over
        m_currNumDeleted = 0;
    }
}
