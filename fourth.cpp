#include <iostream>
#include <map>
#include <string>

using namespace std;

class Theme {
    string backgroundColor;
    string fontColor;
    int fontSize;
    string iconStyle;

public:
    // Default constructor
    Theme() : backgroundColor(""), fontColor(""), fontSize(0), iconStyle("") {}

    // Parameterized constructor
    Theme(string bg, string font, int size, string icon)
        : backgroundColor(bg), fontColor(font), fontSize(size), iconStyle(icon) {}

    // Function to display theme details
    void display() const {
        cout << backgroundColor << " Background, "
             << fontColor << " Font, Size " << fontSize << ", Icon Style: " << iconStyle << endl;
    }
};

int main() {
    // Map to store themes
    map<string, Theme> themes;

    // Adding themes to the map
    themes["Classic"] = Theme("Blue", "White", 14, "Simple");
    themes["Sport"] = Theme("Red", "White", 16, "Dynamic");
    themes["Eco"] = Theme("Green", "Black", 12, "Nature");

    string choice;
    cout << "Enter theme (Classic, Sport, Eco): ";
    cin >> choice;

    // Check if the theme exists and display it
    if (themes.find(choice) != themes.end()) {
        cout << choice << " Theme: ";
        themes[choice].display();
    } else {
        cout << "Theme not found!" << endl;
    }

    return 0;
}
