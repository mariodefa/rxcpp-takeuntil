#include <rxcpp/rx.hpp>
#include <iostream>
#include <chrono>

// Helper function to get thread ID as string
std::string get_pid1() {
    std::stringstream ss;
    ss << std::this_thread::get_id();
    return ss.str();
}

//expected to be used in a separate thread only, this is blocking
rxcpp::observable<int> user_input_observable1() {
    return rxcpp::observable<>::just(0).map([](int) {
        std::string input;
        std::getline(std::cin, input); //block the thread
        return 0;
    });
}

int main() {
    printf("//! [take_until sample]\n");

    // Observable source generating values every second, 7 events
    auto sevenEvents1 = rxcpp::observable<>::interval(std::chrono::seconds(1))
        .take(7)
        .map([](long value1) {
            printf("[thread %s] Source emits, value = %ld\n", get_pid1().c_str(), value1);
            return value1;
        });

    //in separate thread wait for user input, blocking
    auto stopSignal1 = user_input_observable1().subscribe_on(rxcpp::synchronize_new_thread());

    // Take until user input is received
    auto eventsChanel1 = sevenEvents1.take_until(stopSignal1);

    //main thread blocker, block until eventsChanel1 is completed
    eventsChanel1
        .as_blocking() //block the thread
        .subscribe(
            [](long value2) { printf("[thread %s] OnNext: %ld\n", get_pid1().c_str(), value2); },
            []() { printf("[thread %s] OnCompleted\n", get_pid1().c_str()); }
        );

    printf("[thread %s] Finish task\n", get_pid1().c_str());
    printf("//! [take_until sample]\n");
    return 0;
}
