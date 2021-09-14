
#ifndef SANDBOX_IMGUI_LOG_H
#define SANDBOX_IMGUI_LOG_H

#include <sandbox.h>
#include <framework/singleton.h>

namespace Sandbox {

    class ImGuiLog : public Singleton<ImGuiLog> {
        public:
            REGISTER_SINGLETON(ImGuiLog);

            void Initialize() override;
            void Shutdown() override;

            void OnImGui();

            void LogTrace(const char* formatString, ...);
            void LogError(const char* formatString, ...);

            void WriteToFile(const std::string& outputFile) const;

            void ClearLog();

        private:
            ImGuiLog();
            ~ImGuiLog() override;

            void ProcessMessage(bool isErrorMessage, const char* formatString, va_list argsList);

            void PrintErrorMessage(const std::string& message) const;
            void PrintTraceMessage(const std::string& message) const;

            std::mutex _mutex;
            int _processingBufferSize;
            char* _processingBuffer;
            std::set<std::pair<bool, const char*>> _filteredLogMessages;
            std::vector<std::pair<bool, std::string>> _logMessages;

            ImGuiTextFilter _filter;
    };

}

#endif //SANDBOX_IMGUI_LOG_H
