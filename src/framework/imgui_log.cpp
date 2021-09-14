
#include <framework/imgui_log.h>

namespace Sandbox {

    void ImGuiLog::Initialize() {
        _processingBuffer = new char[_processingBufferSize];
    }

    void ImGuiLog::Shutdown() {
        delete[] _processingBuffer;
    }

    ImGuiLog::ImGuiLog() : _processingBufferSize(64u) {
    }

    ImGuiLog::~ImGuiLog() {
    }

    void ImGuiLog::OnImGui() {
        if (ImGui::Begin("Log Window", nullptr, ImGuiWindowFlags_None)) {
            ImGuiIO& io = ImGui::GetIO();

            if (ImGui::Button("Clear")) {
                ClearLog();
            }

            ImGui::SameLine();
            static bool errorMessagesOnly = false;
            ImGui::Checkbox("Errors Only", &errorMessagesOnly);

            ImGui::Text("Message Filter");
            ImGui::SameLine();
            _filter.Draw("##filter", 400.0f);

            ImGui::Separator();
            ImGui::BeginChild("#scrollingSection", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);

            _mutex.lock();
            if (_filter.IsActive()) {
                io.WantCaptureKeyboard = true;

                _filteredLogMessages.clear();

                // Process.
                for (const auto& messageData : _logMessages) {
                    bool isError = messageData.first;
                    const char* data = messageData.second.c_str();

                    if (_filter.PassFilter(data)) {
                        if (isError) {
                            _filteredLogMessages.insert(std::make_pair(isError, data));
                        }
                        else {
                            _filteredLogMessages.insert(std::make_pair(isError, data));
                        }
                    }
                }

                // Print.
                for (const auto& messageData : _filteredLogMessages) {
                    bool isError = messageData.first;
                    const std::string& message = messageData.second;

                    // Draw error messages in red.
                    if (errorMessagesOnly) {
                        if (isError) {
                            PrintErrorMessage(message);
                        }
                    }
                    else {
                        if (isError) {
                            PrintErrorMessage(message);
                        }
                        else {
                            PrintTraceMessage(message);
                        }
                    }
                }
            }
            else {
                io.WantCaptureKeyboard = false;

                for (auto& messageData : _logMessages) {
                    bool isError = messageData.first;
                    const std::string& message = messageData.second;

                    if (errorMessagesOnly) {
                        if (isError) {
                            PrintErrorMessage(message);
                        }
                    }
                    else {
                        if (isError) {
                            PrintErrorMessage(message);
                        }
                        else {
                            PrintTraceMessage(message);
                        }
                    }
                }
            }

            // Autoscroll.
            if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY()) {
                ImGui::SetScrollHereY(1.0f);
            }

            _mutex.unlock();
            ImGui::EndChild();
        }

        ImGui::End();
    }

    void ImGuiLog::ClearLog() {
        _logMessages.clear();
        _filteredLogMessages.clear();
    }

    void ImGuiLog::LogTrace(const char *formatString, ...) {
        va_list argsList;
        va_start(argsList, formatString);

        _mutex.lock();
        ProcessMessage(false, formatString, argsList);
        _mutex.unlock();

        va_end(argsList);
    }

    void ImGuiLog::LogError(const char *formatString, ...) {
        va_list argsList;
        va_start(argsList, formatString);

        _mutex.lock();
        ProcessMessage(true, formatString, argsList);
        _mutex.unlock();

        va_end(argsList);
    }

    void ImGuiLog::ProcessMessage(bool isErrorMessage, const char *formatString, va_list argsList) {
        int currentBufferSize = _processingBufferSize;

        // Copy args list to not modify passed parameters (yet).
        va_list argsCopy;
        va_copy(argsCopy, argsList);

        // If size of the buffer is zero, nothing is written and buffer may be a null pointer, however the return
        // value (number of bytes that would be written not including the null terminator) is still calculated and returned.
        int writeResult = vsnprintf(nullptr, 0, formatString, argsCopy);

        // If buffer size is equal to write result, there will not be space for the null terminator for the string.
        // Multiple buffer size by two to adequately house string in a new buffer.
        while (_processingBufferSize <= writeResult) {
            _processingBufferSize *= 2;
        }

        // Reallocate buffer.
        if (currentBufferSize != _processingBufferSize) {
            delete [] _processingBuffer;
            _processingBuffer = new char[_processingBufferSize];
        }

        vsnprintf(_processingBuffer, _processingBufferSize, formatString, argsList);
        _logMessages.emplace_back(isErrorMessage, _processingBuffer);

        // Clear buffer.
        memset(_processingBuffer, '\0', _processingBufferSize);
    }

    void ImGuiLog::PrintErrorMessage(const std::string &message) const {
        ImGui::PushStyleColor(ImGuiCol_Text, (ImVec4)ImColor(1.0f, 0.2f, 0.0f));
        ImGui::TextUnformatted(message.c_str());
        ImGui::PopStyleColor();
    }

    void ImGuiLog::PrintTraceMessage(const std::string &message) const {
        ImGui::TextUnformatted(message.c_str());
    }

    void ImGuiLog::WriteToFile(const std::string &outputFile) const {
        std::ofstream fileStream;
        fileStream.open(outputFile.c_str());

        if (fileStream.is_open()) {
            // Message severities.
            const std::string trace = "[TRACE] ";
            const std::string error = "[ERROR] ";

            for (const std::pair<bool, std::string>& logData : _logMessages) {
                bool isError = logData.first;
                std::string logMessage = logData.second;

                if (isError) {
                    logMessage.insert(0, error);
                }
                else {
                    logMessage.insert(0, trace);
                }

                fileStream << logMessage << std::endl;
            }

            fileStream.close();
        }
        else {
            std::cerr << "Failed to open ImGui log file at location: " << outputFile << std::endl;
        }
    }

}
