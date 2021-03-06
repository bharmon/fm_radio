// Copyright (c) 2014 Intel Corporation. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef FM_RADIO_INSTANCE_H_
#define FM_RADIO_INSTANCE_H_

#include <string>
#include <thread> // NOLINT

#include <gio/gio.h>
#include <dbus/dbus-glib.h>
#include "client-bindings.h"

#include "../../extension_common/extension.h"
#include "../../extension_common/picojson.h"

class FMRadioInstance;


typedef struct {
    //DBusGProxyCall  *call;
    FMRadioInstance *obj;
    picojson::value *msg;
} DBusReplyListener;

/**
 * Main class containing crosswalk extension/plugin fm radio implementation.
 * Main purpose of this class is to serve as a bridge between HTML5/JS calls
 * and C/C++ dbus daemon FM radio functionalities.
*/
class FMRadioInstance : public common::Instance {
    public:
        FMRadioInstance();
        ~FMRadioInstance();

        /**
         * Called to close an async message call loop with no
         * particular regard to success criterion of the call
         */
        void PostAsyncReply(const picojson::value& msg,
                            picojson::value::object& value);
        /**
         * Called to close an async message call loop with
         * boilerplate error messages.
         */
        void PostAsyncErrorReply(const picojson::value& msg,
                                 const std::string& error_msg);
        /**
         * Called to close an async message call loop with
         * boilerplate success messages and a specific value
         */
        void PostAsyncSuccessReply(const picojson::value& msg,
                                   const picojson::value& value);
        /**
         * Called to close an async message call loop with
         * boilerplate success messages and general value msg
         */
        void PostAsyncSuccessReply(const picojson::value& msg);

    private:
        /** Main method and AddListener processing function */
        virtual void HandleMessage(const char* msg);
        /** Main property (getters) processing function */
        virtual void HandleSyncMessage(const char* msg);

        // Synchronous messages
        /** Implementation AddListener processing function */
        /** Bridge implementation when property "enabled' is fecthed */
        void HandleGetEnabled(const picojson::value& msg);
        /** Bridge implementation when property "frequency' is fecthed */
        void HandleGetFrequency(const picojson::value& msg);

        /** Bridge implementation when API method "cancelSeek' is called */
        void HandleCancelSeek(const picojson::value& msg);

        // Asynchronous messages
        /** Bridge implementation when API method "enable' is called */
        void HandleEnable(const picojson::value& msg);
        /** Bridge implementation when API method "disable' is called */
        void HandleDisable(const picojson::value& msg);
        /** Bridge implementation when API method "setFrequency' is called */
        void HandleSetFrequency(const picojson::value& msg);
        /** Bridge implementation when API method "seek' is called */
        void HandleSeek(const picojson::value& msg);
        /** Bridge implementation when API method "addListener' is called */
        void HandleAddListener(guint& listener_id,
                               const std::string& signal_name,
                               const picojson::value& msg);
        /** Bridge implementation when API method "removeListener' is called */
        void HandleRemoveListener(guint& listener_id,
                                  const std::string& signal_name,
                                  const picojson::value& msg);

        // Asynchronous dbus reply callback catch-all method
        static void DBusReplyCallback(DBusGProxy *proxy,
                                      GError *error,
                                      gpointer userdata);
        DBusReplyListener* CreateDBusReplyListener(const picojson::value& msg);

        // Synchronous message helpers
        void SendSyncErrorReply(const std::string& error_msg);
        void SendSyncSuccessReply();
        void SendSyncSuccessReply(const picojson::value& value);

        // Asynchronous message helpers
        void SendSignal(const picojson::value& signal_name,
                        const picojson::value& signal_value);

        /** Main Signal (coming from dbus daemon) handler that relay then to UI */
        static void HandleSignal(GDBusConnection* connection,
                                 const gchar* sender_name,
                                 const gchar* object_path,
                                 const gchar* interface_name,
                                 const gchar* signal_name,
                                 GVariant* parameters,
                                 gpointer user_data);

        static void RunMainloop(void* data);

        static guint on_enabled_listener_id_;
        static guint on_disabled_listener_id_;
        static guint on_frequency_changed_listener_id_;
        static guint on_station_found_listener_id_;
        static guint on_rds_clear_listener_id_;
        static guint on_rds_change_listener_id_;
        static guint on_rds_complete_listener_id_;

        GMainLoop* main_loop_;
        std::thread thread_;

        // Those are used to access FMRadioService .xml introspect bindings
        DBusGConnection *bus;
        DBusGProxy *busProxy;

        // TODO: remove
        // static std::list<dbusReplyListener*> listeners;
};

#endif  // FM_RADIO_INSTANCE_H_
