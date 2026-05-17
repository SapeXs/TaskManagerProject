#pragma once

#include <string>
#include <string_view>

class NotificationService {
public:
    NotificationService();
    ~NotificationService();

    NotificationService(const NotificationService&) = delete;
    NotificationService& operator=(const NotificationService&) = delete;

    NotificationService(NotificationService&&) = delete;
    NotificationService& operator=(NotificationService&&) = delete;

    bool Notify(std::string_view title, std::string_view message);
    bool NotifyTask(std::string_view task_title, std::string_view message);

private:
    bool initialized_ = false;
};