
#ifndef SANDBOX_LOG_H
#define SANDBOX_LOG_H

#include "pch.h"
#include "common/utility/singleton.h"
#include "common/utility/directory.h"

namespace Sandbox {

    class ImGuiLog : public Singleton<ImGuiLog> {
        public:
            REGISTER_SINGLETON(ImGuiLog);

            void OnImGui();
            void ClearLog();

            void LogTrace(const char* formatString, ...);
            void LogWarning(const char* formatString, ...);
            void LogError(const char* formatString, ...);

            void Flush();

            // By default, log saves to '/out/log.txt'.
            void WriteToFile();

        private:
            enum Severity {
                TRACE,
                WARNING,
                ERROR
            };

            struct Message {
                Message(Severity severity, std::string message);
                ~Message();

                Severity severity_;
                std::string message_;
            };

            ImGuiLog();
            ~ImGuiLog() override;

            void ProcessMessage(Severity severity, const char* formatString, va_list argsList);

            void PrintTraceMessage(const std::string& message) const;
            void PrintWarningMessage(const std::string& message) const;
            void PrintErrorMessage(const std::string& message) const;

            std::ofstream writer_;

            int _processingBufferSize;
            char* _processingBuffer;

            std::vector<Message> messages_; // Complete record of messages.
            std::vector<Message> gui_;      // Messages that appear in the GUI window.
    };

}

// Defines for logging messages once (useful for eliminating log message spam from errors happening in repeated function calls).
#define LogTraceOnce(MESSAGE, ...)      do {                                                             \
                                            static bool _logged = false;                                 \
                                            if (!_logged) {                                              \
                                                ImGuiLog::Instance().LogTrace(MESSAGE, ##__VA_ARGS__);   \
                                                _logged = true;                                          \
                                            }                                                            \
                                        } while (false)

#define LogWarningOnce(MESSAGE, ...)    do {                                                             \
                                            static bool _logged = false;                                 \
                                            if (!_logged) {                                              \
                                                ImGuiLog::Instance().LogWarning(MESSAGE, ##__VA_ARGS__); \
                                                _logged = true;                                          \
                                            }                                                            \
                                        } while (false)

#define LogErrorOnce(MESSAGE, ...)      do {                                                             \
                                            static bool _logged = false;                                 \
                                            if (!_logged) {                                              \
                                                ImGuiLog::Instance().LogError(MESSAGE, ##__VA_ARGS__);   \
                                                _logged = true;                                          \
                                            }                                                            \
                                        } while (false)

#endif //SANDBOX_LOG_H
