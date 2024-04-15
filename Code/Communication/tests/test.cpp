#include <iostream>
#include <string>
#include "asd.pb.h"

int main() {
    mypackage::Person person;
    person.set_name("Alice");
    person.set_id(123);

    std::string serializedPerson = person.SerializeAsString();

    std::cout << "Serialized Person: " << serializedPerson << std::endl;
    
    return 0;
}
