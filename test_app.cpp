#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <numeric>
#include <algorithm>
#include <unordered_map>
#include <memory>

#define PROJECT_NAME "test_app"


class ModeHandler {
public:
    virtual void handle(const std::string& filename, const std::string& val) const = 0;
    virtual ~ModeHandler() = default;
};

class WordsModeHandler : public ModeHandler {
public:
    virtual void handle(const std::string& filename, const std::string& val) const override {
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Error: Unable to open file " << filename << "\n";
            std::cout << "Check if you wrote the file name or specified the path correctly" << "\n";
            return;
        }

        std::string word;
        int count = 0;
        while (file >> word) {
            if (word == val)
                count++;
        }

        std::cout << "Occurrences of '" << val << "': " << count << "\n";
    }
};

class ChecksumModeHandler : public ModeHandler {
public:
    virtual void handle(const std::string& filename, const std::string& val = "") const override {
        std::ifstream file(filename, std::ios::binary);
        if (!file.is_open()) {
            std::cerr << "Error: Unable to open file " << filename << "\n";
            std::cout << "Check if you wrote the file name or specified the path correctly" << "\n";
            return;
        }

        uint32_t checksum = 0;
        uint32_t word;
        while (file.read(reinterpret_cast<char*>(&word), sizeof(uint32_t))) {
            checksum += word;
        }

        std::cout << "Checksum: " << checksum << "\n";
    }
};


class TestProgram {
public:
    TestProgram() {
        handlers_["words"] = std::make_unique<WordsModeHandler>();
        handlers_["checksum"] = std::make_unique<ChecksumModeHandler>();
    }

    //Main Function Start
    void run(int argc, char* argv[]) {
       
        if (argc <= 3) {
            if (argc < 3) {
                printUsage();
                return;
            }
            if (std::string(argv[1]) == PROJECT_NAME && std::string(argv[2]) == "-h") {
                printHelp();
                return;
            }
        }

        if (std::string(argv[1]) != PROJECT_NAME || argc < 6) {
            printUsage();
            return;
        }
     
        std::string filename = "";
        std::string modeFlag = "";
        std::string valueToSearch = "";

        for (int i = 2; i < argc; ++i) {
            if (std::string(argv[i]) == "-f" && i + 1 < argc) {
                filename = argv[i + 1];
                ++i;
            }
            else if (std::string(argv[i]) == "-m" && i + 1 < argc) {
                modeFlag = argv[i + 1];
                ++i;
            }
            else if (std::string(argv[i]) == "-v" && i + 1 < argc) {
                valueToSearch = argv[i + 1];
                ++i;
            }
        }

        if (filename.empty() || modeFlag.empty()) {
            printUsage();
            return;
        }

        // Find the mode handler and call its handle function
        auto it = handlers_.find(modeFlag);
        if (it != handlers_.end()) {
            it->second->handle(filename, valueToSearch);
        }
        else {
            std::cerr << "Error: Invalid mode " << modeFlag << "\n";
            printUsage();
        }
    }

private:
    void printUsage() {
        std::cout << "Read instruction attentively!" << "\n";
        std::cout << "Usage: <project_name> -h" << "\n";
    }

    void printHelp() {
        std::cout << "Test program usage:" << "\n";
        std::cout << "<project_name> -f <filename> -m <mode> -v <value>" << "\n";
        std::cout << "Modes:" << "\n";
        std::cout << "  words       Count occurrences of a word in the file" << "\n";
        std::cout << "  checksum    Calculate checksum of the file" << "\n";
        std::cout << "Options:" << "\n";
        std::cout << "  -v <value>  Specify value to search for (only for mode 'words')" << "\n";
    }
   
    std::unordered_map<std::string, std::unique_ptr<ModeHandler>> handlers_;
};

int main(int argc, char* argv[]) {
    TestProgram program;
    program.run(argc, argv);
    return 0;
}
