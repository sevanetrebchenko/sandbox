
#ifndef SANDBOX_IMGUI_LOG_H
#define SANDBOX_IMGUI_LOG_H

#include <sandbox_pch.h>

namespace Sandbox {

    class ImGuiLog {
        public:
            static ImGuiLog& Instance();

            void OnImGui();
            void ClearLog();
            void LogTrace(const char* formatString, ...);
            void LogError(const char* formatString, ...);

            void WriteToFile(const std::string& outputFile) const;

        private:
            ImGuiLog();
            ~ImGuiLog();

            void ProcessMessage(bool isErrorMessage, const char* formatString, va_list argsList);

            void PrintErrorMessage(const std::string& message) const;
            void PrintTraceMessage(const std::string& message) const;

            std::mutex _mutex;
            int _processingBufferSize;
            char* _processingBuffer;
            std::set<std::pair<bool, const char*>> _filteredLogMessages;
            std::vector<std::pair<bool, std::string>> guiLogMessages_;

            std::vector<std::pair<bool, std::string>> fileLogMessages_;

            ImGuiTextFilter _filter;
    };

}

#endif //SANDBOX_IMGUI_LOG_H
