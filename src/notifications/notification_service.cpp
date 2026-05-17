#include "notifications/notification_service.h"

#include <libnotify/notify.h>
#include <string>

NotificationService::NotificationService() {
  initialized_ = notify_init("TaskManager");
}

NotificationService::~NotificationService() {
  if (initialized_) {
    notify_uninit();
  }
}

bool NotificationService::Notify(std::string_view title, std::string_view message) {
  if (!initialized_) {
    return false;
  }

  std::string title_text(title);
  std::string message_text(message);

  NotifyNotification* notification =
      notify_notification_new(title_text.c_str(), message_text.c_str(), nullptr);

  if (notification == nullptr) {
    return false;
  }

  notify_notification_set_timeout(notification, 5000);

  GError* error = nullptr;
  gboolean result = notify_notification_show(notification, &error);

  if (error != nullptr) {
    g_error_free(error);
  }

  g_object_unref(G_OBJECT(notification));

  return result == TRUE;
}

bool NotificationService::NotifyTask(std::string_view task_title, std::string_view message) {
  std::string full_message;
  full_message += std::string(task_title);
  full_message += ": ";
  full_message += std::string(message);

  return Notify("TaskManager", full_message);
}