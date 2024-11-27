#include <iostream>
#include <thread>
#include <chrono>
#include <random>
#include <mutex>
#include <iomanip>

class VehicleData {
private:
    double speed;
    double fuelLevel;
    double engineTemperature;
    std::mutex dataMutex; // Protect data during updates

public:
    VehicleData() : speed(0.0), fuelLevel(5.0), engineTemperature(90.0) {}

    void updateData() {
        std::lock_guard<std::mutex> lock(dataMutex);
        // Randomly update the parameters
        speed = getRandomValue(0, 200); // Speed in km/h
        fuelLevel = std::max(0.0, fuelLevel - getRandomValue(0, 1)); // Fuel decreases over time
        engineTemperature = getRandomValue(70, 120); // Engine temperature in °C
    }

    void getData(double& currentSpeed, double& currentFuel, double& currentTemp) {
        std::lock_guard<std::mutex> lock(dataMutex);
        currentSpeed = speed;
        currentFuel = fuelLevel;
        currentTemp = engineTemperature;
    }

private:
    double getRandomValue(double min, double max) {
        static std::random_device rd;
        static std::mt19937 gen(rd());
        std::uniform_real_distribution<> dis(min, max);
        return dis(gen);
    }
};

class Display {
public:
    static void showData(VehicleData& vehicleData) {
        while (true) {
            double speed, fuelLevel, engineTemperature;
            vehicleData.getData(speed, fuelLevel, engineTemperature);

            system("clear"); // Clear the console for real-time updates
            std::cout << std::fixed << std::setprecision(2);
            std::cout << "==== Instrument Cluster ====" << std::endl;
            std::cout << "Speed: " << speed << " km/h" << std::endl;
            std::cout << "Fuel Level: " << fuelLevel << " %" << std::endl;
            std::cout << "Engine Temperature: " << engineTemperature << " °C" << std::endl;

            // Display warnings
            if (fuelLevel < 10) {
                std::cout << "WARNING: Low fuel level!" << std::endl;
            }
            if (engineTemperature > 100) {
                std::cout << "WARNING: Engine overheating!" << std::endl;
            }

            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }
};

void updateVehicleData(VehicleData& vehicleData) {
    while (true) {
        vehicleData.updateData();
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

int main() {
    VehicleData vehicleData;

    // Start threads for updating and displaying data
    std::thread dataUpdater(updateVehicleData, std::ref(vehicleData));
    std::thread dataDisplayer(Display::showData, std::ref(vehicleData));

    dataUpdater.join();
    dataDisplayer.join();

    return 0;
}
