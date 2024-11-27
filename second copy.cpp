#include <iostream>
#include <thread>
#include <chrono>
#include <random>
#include <atomic>
#include <iomanip>

using namespace std;

class VehicleData {
private:
    atomic<double> speed;
    atomic<double> fuelLevel;
    atomic<double> engineTemperature;

public:
    VehicleData() : speed(0.0), fuelLevel(5.0), engineTemperature(90.0) {}

    void updateData() {
        // Randomly update the parameters
        speed.store(getRandomValue(0, 200)); // Speed in km/h
        double currentFuel = fuelLevel.load();
        fuelLevel.store(max(0.0, currentFuel - getRandomValue(0, 1))); // Fuel decreases over time
        engineTemperature.store(getRandomValue(70, 120)); // Engine temperature in °C
    }

    void getData(double& currentSpeed, double& currentFuel, double& currentTemp) {
        // Load atomic values into local variables
        currentSpeed = speed.load();
        currentFuel = fuelLevel.load();
        currentTemp = engineTemperature.load();
    }

private:
    double getRandomValue(double min, double max) {
        // Generate a random number between min and max
        return min + (rand() / (RAND_MAX / (max - min)));
    }
};

class Display {
public:
    static void showData(VehicleData& vehicleData) {
        while (true) {
            double speed, fuelLevel, engineTemperature;
            vehicleData.getData(speed, fuelLevel, engineTemperature);

            // Stop the loop if fuel level reaches 0
            if (fuelLevel == 0.0) {
                cout << "Fuel is empty. Stopping the simulation." << endl;
                break; // Exit the loop when fuel is 0
            }

            cout << fixed << setprecision(2);
            cout << "==== Instrument Cluster ====" << endl;
            cout << "Speed: " << speed << " km/h" << endl;
            cout << "Fuel Level: " << fuelLevel << " %" << endl;
            cout << "Engine Temperature: " << engineTemperature << " °C" << endl;

            // Display warnings based on fuel and temperature
            if (fuelLevel >= 1.0 && fuelLevel <= 3.0) {
                cout << "WARNING: Fuel is getting over!" << endl;
            } else if (fuelLevel < 10) {
                cout << "WARNING: Low fuel level!" << endl;
            }

            if (engineTemperature > 100) {
                cout << "WARNING: Engine overheating!" << endl;
            }

            this_thread::sleep_for(chrono::seconds(1));
        }
    }
};

void updateVehicleData(VehicleData& vehicleData) {
    while (true) {
        vehicleData.updateData();
        this_thread::sleep_for(chrono::seconds(1));
    }
}

int main() {
    srand(time(0)); // Seed the random number generator

    VehicleData vehicleData;

    // Start threads for updating and displaying data
    thread dataUpdater(updateVehicleData, ref(vehicleData));
    thread dataDisplayer(Display::showData, ref(vehicleData));

    dataUpdater.join();
    dataDisplayer.join();

    return 0;
}
