#include <iostream>
#include <thread>
#include <chrono>
#include <fstream>
#include "include/metrics.hpp"

#define ITERATIONS 6

void seqA () {
    auto report = metrics::ReportMetrics::getInstance();
    auto c1_addr = report->getCounter(0);
    auto c2_addr = report->getCounter(1);
    assert(report->getCounter(2));

    auto g1_addr = report->getGauge(0);
    assert(report->getGauge(1));

    auto h_addr = report->getHistogram(0);

    std::this_thread::sleep_for (std::chrono::seconds(2));

    c1_addr->increment();
    h_addr->update(2);
    h_addr->update(5);

    std::this_thread::sleep_for (std::chrono::seconds(2));

    h_addr->update(5);
    c2_addr->increment();
    g1_addr->update(2);
}

void seqB () {
    auto report = metrics::ReportMetrics::getInstance();
    auto c1_addr = report->getCounter(0);
    auto c2_addr = report->getCounter(1);
    assert(report->getCounter(2));

    auto g1_addr = report->getGauge(0);
    assert(report->getGauge(1));

    assert(report->getHistogram(0));

    c1_addr->increment();
    c2_addr->increment();

    std::this_thread::sleep_for (std::chrono::seconds(1));

    c1_addr->decrement(2);
    c2_addr->decrement();

    std::this_thread::sleep_for (std::chrono::seconds(3));

    g1_addr->update(5);
}

void gather () {
    std::string filename = "result.json";
    std::ofstream ofs (filename, std::ofstream::out);
    auto report = metrics::ReportMetrics::getInstance();
    for (auto i = 0U; i < ITERATIONS; i++) {
        report->gather();
        ofs << report->getJSON() << std::endl;
        std::this_thread::sleep_for (std::chrono::seconds(1));
    }
    ofs.close();
}

int main() {
    auto report = metrics::ReportMetrics::getInstance();

    report->registerCounter( "counter1", "counter1 for test", "", 5 );
    report->registerCounter( "counter2", "counter2 for test", "", -2 );
    report->registerCounter( "counter3", "counter3 for test", "", 0 );

    report->registerGauge( "gauge1", "gauge1 for test", "", 3 );
    report->registerGauge( "gauge2", "gauge2 for test", "", -2 );

    report->registerHistogram( "hist", "histogram for test", "");

    std::thread th1 (seqA);
    std::thread th2 (seqB);
    std::thread th3 (gather);

    th1.join();
    th2.join();
    th3.join();

    metrics::ReportMetrics::deleteInstance();
}
