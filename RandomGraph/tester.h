#ifndef TESTER_H
#define TESTER_H

class Tester {
public:
    Tester();
    void runAllTests() {
//        runFullGraphTest();
//        runFullGraphRestrictedTest();
//        runGridGraphTest();
        runGridGraphRestrictedTest();
    }
    void runFullGraphTest();
    void runFullGraphRestrictedTest();
    void runGridGraphTest();
    void runGridGraphRestrictedTest();
};

#endif // TESTER_H
