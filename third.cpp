#include <iostream>
#include <queue>
#include <string>
#include <random>
#include <chrono>
#include <thread>
#include <mutex>
#include <iomanip>

using namespace std;

// Event class representing user interactions
class Event {
public:
    enum EventType { Tap, Swipe }; // Types of events
    EventType eventType;
    int x, y; // Coordinates for the event
    int x2, y2; // For swipe end coordinates
    long long timestamp;

    // Constructor for Tap event
    Event(EventType type, int x, int y, long long time)
        : eventType(type), x(x), y(y), x2(-1), y2(-1), timestamp(time) {}

    // Constructor for Swipe event
    Event(EventType type, int x1, int y1, int x2, int y2, long long time)
        : eventType(type), x(x1), y(y1), x2(x2), y2(y2), timestamp(time) {}
};

// EventQueue class to manage the event queue and processing
class EventQueue {
private:
    queue<Event> eventQueue;
    mutex queueMutex;

public:
    // Add event to the queue
    void addEvent(const Event& event) {
        lock_guard<mutex> lock(queueMutex);
        eventQueue.push(event);
    }

    // Process events in the queue
    void processEvents() {
        while (true) {
            Event event = getNextEvent();
            if (event.eventType == Event::Tap) {
                cout << "[Event: Tap] Position: (" << event.x << ", " << event.y
                     << "), Timestamp: " << event.timestamp << endl;
            } else if (event.eventType == Event::Swipe) {
                string direction = getSwipeDirection(event.x, event.y, event.x2, event.y2);
                cout << "[Event: Swipe] From: (" << event.x << ", " << event.y
                     << ") To: (" << event.x2 << ", " << event.y2
                     << "), Direction: " << direction
                     << ", Timestamp: " << event.timestamp << endl;
            }
            this_thread::sleep_for(chrono::milliseconds(500)); // Simulate processing delay
        }
    }

private:
    // Get the next event from the queue
    Event getNextEvent() {
        unique_lock<mutex> lock(queueMutex);
        while (eventQueue.empty()) {
            lock.unlock();
            this_thread::sleep_for(chrono::milliseconds(100)); // Wait for new events
            lock.lock();
        }
        Event event = eventQueue.front();
        eventQueue.pop();
        return event;
    }

    // Calculate swipe direction based on coordinates
    string getSwipeDirection(int x1, int y1, int x2, int y2) {
        int dx = x2 - x1;
        int dy = y2 - y1;

        if (abs(dx) > abs(dy)) {
            return dx > 0 ? "Right" : "Left";
        } else {
            return dy > 0 ? "Down" : "Up";
        }
    }
};

// Random event generator
void generateEvents(EventQueue& eventQueue) {
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> coordDist(0, 100); // Random coordinates
    uniform_int_distribution<> eventDist(0, 1);  // Random event type

    while (true) {
        int eventType = eventDist(gen);
        int x = coordDist(gen);
        int y = coordDist(gen);
        long long timestamp = chrono::duration_cast<chrono::milliseconds>(
                                  chrono::system_clock::now().time_since_epoch())
                                  .count();

        if (eventType == 0) { // Tap
            eventQueue.addEvent(Event(Event::Tap, x, y, timestamp));
        } else { // Swipe
            int x2 = coordDist(gen);
            int y2 = coordDist(gen);
            eventQueue.addEvent(Event(Event::Swipe, x, y, x2, y2, timestamp));
        }

        this_thread::sleep_for(chrono::seconds(1)); // Simulate random event generation
    }
}

int main() {
    EventQueue eventQueue;

    // Create threads for event generation and processing
    thread eventGenerator(generateEvents, ref(eventQueue));
    thread eventProcessor(&EventQueue::processEvents, &eventQueue);

    // Join threads
    eventGenerator.join();
    eventProcessor.join();

    return 0;
}
