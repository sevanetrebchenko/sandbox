
#include "common/utility/log.h"
#include "common/utility/directory.h"

namespace Sandbox {

    ImGuiLog& ImGuiLog::Instance() {
        static ImGuiLog instance;
        return instance;
    }

    ImGuiLog::ImGuiLog() : _processingBufferSize(64u),
                           _processingBuffer(new char[_processingBufferSize])
                           {
    }

    ImGuiLog::~ImGuiLog() {
        delete[] _processingBuffer;
    }

    void ImGuiLog::OnImGui() {
        static bool showLogWindow = true;
        if (ImGui::Begin("Log Window", &showLogWindow)) {
            ImGuiIO& io = ImGui::GetIO();

            if (ImGui::Button("Clear")) {
                ClearLog();
            }

            static bool showTrace = true;
            static bool showWarning = true;
            static bool showError = true;

            ImGui::SameLine();
            ImGui::Checkbox("Trace", &showTrace);

            ImGui::SameLine();
            ImGui::PushStyleColor(ImGuiCol_Text, (ImVec4)ImColor(1.0f, 1.0f, 0.0f)); // Yellow.
            ImGui::Checkbox("Warning", &showWarning);
            ImGui::PopStyleColor();

            ImGui::SameLine();
            ImGui::PushStyleColor(ImGuiCol_Text, (ImVec4)ImColor(1.0f, 0.2f, 0.0f)); // Red.
            ImGui::Checkbox("Error", &showError);
            ImGui::PopStyleColor();

            ImGui::Separator();

            for (const Message& data : gui_) {
                switch (data.severity_) {
                    case TRACE:
                        if (showTrace) {
                            PrintTraceMessage(data.message_);
                        }
                        break;
                    case WARNING:
                        if (showWarning) {
                            PrintWarningMessage(data.message_);
                        }
                        break;
                    case ERROR:
                        if (showError) {
                            PrintErrorMessage(data.message_);
                        }
                        break;
                }
            }

            // Autoscroll.
            if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY()) {
                ImGui::SetScrollHereY(1.0f);
            }
        }

        ImGui::End();
    }

    void ImGuiLog::ClearLog() {
        gui_.clear();
    }

    void ImGuiLog::LogTrace(const char *formatString, ...) {
        va_list argsList;
        va_start(argsList, formatString);
        ProcessMessage(Severity::TRACE, formatString, argsList);
        va_end(argsList);
    }

    void ImGuiLog::LogWarning(const char* formatString, ...) {
        va_list argsList;
        va_start(argsList, formatString);
        ProcessMessage(Severity::WARNING, formatString, argsList);
        va_end(argsList);
    }

    void ImGuiLog::LogError(const char *formatString, ...) {
        va_list argsList;
        va_start(argsList, formatString);
        ProcessMessage(Severity::ERROR, formatString, argsList);
        va_end(argsList);
    }

    void ImGuiLog::ProcessMessage(Severity severity, const char *formatString, va_list argsList) {
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
        std::string message = _processingBuffer;

        static const std::string trace =   "[  TRACE  ]  ";
        static const std::string warning = "[ WARNING ]  ";
        static const std::string error =   "[  ERROR  ]  ";

        switch (severity) {
            case TRACE:
                message.insert(0, trace);
                break;
            case WARNING:
                message.insert(0, warning);
                break;
            case ERROR:
                message.insert(0, error);
                break;
        }

        messages_.emplace_back(severity, message);
        gui_.emplace_back(severity, message);

        // Clear buffer.
        memset(_processingBuffer, '\0', _processingBufferSize);
    }

    void ImGuiLog::PrintTraceMessage(const std::string &message) const {
        ImGui::TextUnformatted(message.c_str());
    }

    void ImGuiLog::PrintWarningMessage(const std::string& message) const {
        ImGui::PushStyleColor(ImGuiCol_Text, (ImVec4)ImColor(1.0f, 1.0f, 0.0f)); // Yellow.
        ImGui::TextUnformatted(message.c_str());
        ImGui::PopStyleColor();
    }

    void ImGuiLog::PrintErrorMessage(const std::string &message) const {
        ImGui::PushStyleColor(ImGuiCol_Text, (ImVec4)ImColor(1.0f, 0.2f, 0.0f)); // Red.
        ImGui::TextUnformatted(message.c_str());
        ImGui::PopStyleColor();
    }

    void ImGuiLog::WriteToFile(const std::string &outputFile) const {
        CreateFile(outputFile);

        std::ofstream fileStream;
        fileStream.open(outputFile.c_str());

        if (fileStream.is_open()) {
            for (const Message& data : messages_) {
                fileStream << data.message_ << std::endl;
            }
            fileStream.close();
        }
        else {
            std::cerr << "Failed to open ImGui log file at location: " << outputFile << std::endl;
        }
    }

    ImGuiLog::Message::Message(ImGuiLog::Severity severity, std::string message) : severity_(severity),
                                                                                   message_(std::move(message))
                                                                                   {
    }

    ImGuiLog::Message::~Message() {
    }

}
