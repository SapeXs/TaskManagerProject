#include "notifications/notification_service.h"

#include <string>

#ifdef TASKMANAGER_ENABLE_NOTIFICATIONS
#include <libnotify/notify.h>
#endif

NotificationService::NotificationService() {
#ifdef TASKMANAGER_ENABLE_NOTIFICATIONS
  initialized_ = notify_init("TaskManager");
#else
  initialized_ = false;
#endif
}

NotificationService::~NotificationService() {
#ifdef TASKMANAGER_ENABLE_NOTIFICATIONS
  if (initialized_) {
    notify_uninit();
  }
#endif
}

bool NotificationService::Notify(std::string_view title, std::string_view message) {
#ifndef TASKMANAGER_ENABLE_NOTIFICATIONS
  (void)title;
  (void)message;
  return false;
#else
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
#endif
}

bool NotificationService::NotifyTask(std::string_view task_title, std::string_view message) {
  std::string full_message;
  full_message += std::string(task_title);
  full_message += ": ";
  full_message += std::string(message);

  return Notify("TaskManager", full_message);
}
