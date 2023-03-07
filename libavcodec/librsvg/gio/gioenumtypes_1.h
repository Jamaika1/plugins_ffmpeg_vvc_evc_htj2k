/*** BEGIN file-header ***/
/*
 * Copyright © 2007 Red Hat, Inc.
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General
 * Public License along with this library; if not, see <http://www.gnu.org/licenses/>.
 *
 * Authors: Matthias Clasen <mclasen@redhat.com>
 */

#ifndef __GIO_ENUM_TYPES_H__
#define __GIO_ENUM_TYPES_H__

#include "../glib/glib-object.h"
#include "gversionmacros.h"

G_BEGIN_DECLS
/*** END file-header ***/
#define G_TYPE_APPLICATION_FLAGS	(g_application_flags_get_type ())
#define G_TYPE_APP_INFO_CREATE_FLAGS	(g_app_info_create_flags_get_type ())
#define G_TYPE_ASK_PASSWORD_FLAGS	(g_ask_password_flags_get_type ())
#define G_TYPE_BUS_NAME_OWNER_FLAGS	(g_bus_name_owner_flags_get_type ())
#define G_TYPE_BUS_NAME_WATCHER_FLAGS	(g_bus_name_watcher_flags_get_type ())
#define G_TYPE_BUS_TYPE	(g_bus_type_get_type ())
#define G_TYPE_CONVERTER_FLAGS	(g_converter_flags_get_type ())
#define G_TYPE_CONVERTER_RESULT	(g_converter_result_get_type ())
#define G_TYPE_CREDENTIALS_TYPE	(g_credentials_type_get_type ())
#define G_TYPE_DATA_STREAM_BYTE_ORDER	(g_data_stream_byte_order_get_type ())
#define G_TYPE_DATA_STREAM_NEWLINE_TYPE	(g_data_stream_newline_type_get_type ())
#define G_TYPE_DBUS_CALL_FLAGS	(g_dbus_call_flags_get_type ())
#define G_TYPE_DBUS_CAPABILITY_FLAGS	(g_dbus_capability_flags_get_type ())
#define G_TYPE_DBUS_CONNECTION_FLAGS	(g_dbus_connection_flags_get_type ())
#define G_TYPE_DBUS_ERROR	(g_dbus_error_get_type ())
#define G_TYPE_DBUS_INTERFACE_SKELETON_FLAGS	(g_dbus_interface_skeleton_flags_get_type ())
#define G_TYPE_DBUS_MESSAGE_BYTE_ORDER	(g_dbus_message_byte_order_get_type ())
#define G_TYPE_DBUS_MESSAGE_FLAGS	(g_dbus_message_flags_get_type ())
#define G_TYPE_DBUS_MESSAGE_HEADER_FIELD	(g_dbus_message_header_field_get_type ())
#define G_TYPE_DBUS_MESSAGE_TYPE	(g_dbus_message_type_get_type ())
#define G_TYPE_DBUS_OBJECT_MANAGER_CLIENT_FLAGS	(g_dbus_object_manager_client_flags_get_type ())
#define G_TYPE_DBUS_PROPERTY_INFO_FLAGS	(g_dbus_property_info_flags_get_type ())
#define G_TYPE_DBUS_PROXY_FLAGS	(g_dbus_proxy_flags_get_type ())
#define G_TYPE_DBUS_SEND_MESSAGE_FLAGS	(g_dbus_send_message_flags_get_type ())
#define G_TYPE_DBUS_SERVER_FLAGS	(g_dbus_server_flags_get_type ())
#define G_TYPE_DBUS_SIGNAL_FLAGS	(g_dbus_signal_flags_get_type ())
#define G_TYPE_DBUS_SUBTREE_FLAGS	(g_dbus_subtree_flags_get_type ())
#define G_TYPE_DRIVE_START_FLAGS	(g_drive_start_flags_get_type ())
#define G_TYPE_DRIVE_START_STOP_TYPE	(g_drive_start_stop_type_get_type ())
#define G_TYPE_EMBLEM_ORIGIN	(g_emblem_origin_get_type ())
#define G_TYPE_FILESYSTEM_PREVIEW_TYPE	(g_filesystem_preview_type_get_type ())
#define G_TYPE_FILE_ATTRIBUTE_INFO_FLAGS	(g_file_attribute_info_flags_get_type ())
#define G_TYPE_FILE_ATTRIBUTE_STATUS	(g_file_attribute_status_get_type ())
#define G_TYPE_FILE_ATTRIBUTE_TYPE	(g_file_attribute_type_get_type ())
#define G_TYPE_FILE_COPY_FLAGS	(g_file_copy_flags_get_type ())
#define G_TYPE_FILE_CREATE_FLAGS	(g_file_create_flags_get_type ())
#define G_TYPE_FILE_MONITOR_EVENT	(g_file_monitor_event_get_type ())
#define G_TYPE_FILE_MONITOR_FLAGS	(g_file_monitor_flags_get_type ())
#define G_TYPE_FILE_QUERY_INFO_FLAGS	(g_file_query_info_flags_get_type ())
#define G_TYPE_FILE_TYPE	(g_file_type_get_type ())
#define G_TYPE_IO_ERROR_ENUM	(g_io_error_enum_get_type ())
#define G_TYPE_IO_MODULE_SCOPE_FLAGS	(g_io_module_scope_flags_get_type ())
#define G_TYPE_IO_STREAM_SPLICE_FLAGS	(g_io_stream_splice_flags_get_type ())
#define G_TYPE_MOUNT_MOUNT_FLAGS	(g_mount_mount_flags_get_type ())
#define G_TYPE_MOUNT_OPERATION_RESULT	(g_mount_operation_result_get_type ())
#define G_TYPE_MOUNT_UNMOUNT_FLAGS	(g_mount_unmount_flags_get_type ())
#define G_TYPE_OUTPUT_STREAM_SPLICE_FLAGS	(g_output_stream_splice_flags_get_type ())
#define G_TYPE_PASSWORD_SAVE	(g_password_save_get_type ())
#define G_TYPE_RESOLVER_ERROR	(g_resolver_error_get_type ())
#define G_TYPE_RESOURCE_ERROR	(g_resource_error_get_type ())
#define G_TYPE_RESOURCE_FLAGS	(g_resource_flags_get_type ())
#define G_TYPE_RESOURCE_LOOKUP_FLAGS	(g_resource_lookup_flags_get_type ())
#define G_TYPE_SETTINGS_BIND_FLAGS	(g_settings_bind_flags_get_type ())
#define G_TYPE_SOCKET_CLIENT_EVENT	(g_socket_client_event_get_type ())
#define G_TYPE_SOCKET_FAMILY	(g_socket_family_get_type ())
#define G_TYPE_SOCKET_MSG_FLAGS	(g_socket_msg_flags_get_type ())
#define G_TYPE_SOCKET_PROTOCOL	(g_socket_protocol_get_type ())
#define G_TYPE_SOCKET_TYPE	(g_socket_type_get_type ())
#define G_TYPE_TLS_AUTHENTICATION_MODE	(g_tls_authentication_mode_get_type ())
#define G_TYPE_TLS_CERTIFICATE_FLAGS	(g_tls_certificate_flags_get_type ())
#define G_TYPE_TLS_DATABASE_LOOKUP_FLAGS	(g_tls_database_lookup_flags_get_type ())
#define G_TYPE_TLS_DATABASE_VERIFY_FLAGS	(g_tls_database_verify_flags_get_type ())
#define G_TYPE_TLS_ERROR	(g_tls_error_get_type ())
#define G_TYPE_TLS_INTERACTION_RESULT	(g_tls_interaction_result_get_type ())
#define G_TYPE_TLS_PASSWORD_FLAGS	(g_tls_password_flags_get_type ())
#define G_TYPE_TLS_REHANDSHAKE_MODE	(g_tls_rehandshake_mode_get_type ())
#define G_TYPE_UNIX_SOCKET_ADDRESS_TYPE	(g_unix_socket_address_type_get_type ())
#define G_TYPE_ZLIB_COMPRESSOR_FORMAT	(g_zlib_compressor_format_get_type ())
/*** BEGIN file-production ***/
extern GType g_app_info_create_flags_get_type(void);
extern GType g_application_flags_get_type(void);
extern GType g_ask_password_flags_get_type(void);
extern GType g_bus_name_owner_flags_get_type(void);
extern GType g_bus_name_watcher_flags_get_type(void);
extern GType g_bus_type_get_type(void);
extern GType g_converter_flags_get_type(void);
extern GType g_converter_result_get_type(void);
extern GType g_credentials_type_get_type(void);
extern GType g_data_stream_byte_order_get_type(void);
extern GType g_data_stream_newline_type_get_type(void);
extern GType g_dbus_call_flags_get_type(void);
extern GType g_dbus_capability_flags_get_type(void);
extern GType g_dbus_connection_flags_get_type(void);
extern GType g_dbus_error_get_type(void);
extern GType g_dbus_interface_skeleton_flags_get_type(void);
extern GType g_dbus_message_byte_order_get_type(void);
extern GType g_dbus_message_flags_get_type(void);
extern GType g_dbus_message_header_field_get_type(void);
extern GType g_dbus_message_type_get_type(void);
extern GType g_dbus_object_manager_client_flags_get_type(void);
extern GType g_dbus_property_info_flags_get_type(void);
extern GType g_dbus_proxy_flags_get_type(void);
extern GType g_dbus_send_message_flags_get_type(void);
extern GType g_dbus_server_flags_get_type(void);
extern GType g_dbus_signal_flags_get_type(void);
extern GType g_dbus_subtree_flags_get_type(void);
extern GType g_drive_start_flags_get_type(void);
extern GType g_drive_start_stop_type_get_type(void);
extern GType g_emblem_origin_get_type(void);
extern GType g_file_attribute_info_flags_get_type(void);
extern GType g_file_attribute_status_get_type(void);
extern GType g_file_attribute_type_get_type(void);
extern GType g_file_copy_flags_get_type(void);
extern GType g_file_create_flags_get_type(void);
extern GType g_file_monitor_event_get_type(void);
extern GType g_file_monitor_flags_get_type(void);
extern GType g_file_query_info_flags_get_type(void);
extern GType g_file_type_get_type(void);
extern GType g_filesystem_preview_type_get_type(void);
extern GType g_io_error_enum_get_type(void);
extern GType g_io_module_scope_flags_get_type(void);
extern GType g_io_stream_splice_flags_get_type(void);
extern GType g_mount_mount_flags_get_type(void);
extern GType g_mount_operation_result_get_type(void);
extern GType g_mount_unmount_flags_get_type(void);
extern GType g_output_stream_splice_flags_get_type(void);
extern GType g_password_save_get_type(void);
extern GType g_resolver_error_get_type(void);
extern GType g_resource_error_get_type(void);
extern GType g_resource_flags_get_type(void);
extern GType g_resource_lookup_flags_get_type(void);
extern GType g_settings_bind_flags_get_type(void);
extern GType g_socket_client_event_get_type(void);
extern GType g_socket_family_get_type(void);
extern GType g_socket_msg_flags_get_type(void);
extern GType g_socket_protocol_get_type(void);
extern GType g_socket_type_get_type(void);
extern GType g_tls_authentication_mode_get_type(void);
extern GType g_tls_certificate_flags_get_type(void);
extern GType g_tls_database_lookup_flags_get_type(void);
extern GType g_tls_database_verify_flags_get_type(void);
extern GType g_tls_error_get_type(void);
extern GType g_tls_interaction_result_get_type(void);
extern GType g_tls_password_flags_get_type(void);
extern GType g_tls_rehandshake_mode_get_type(void);
extern GType g_unix_socket_address_type_get_type(void);
extern GType g_zlib_compressor_format_get_type(void);
/* enumerations from "@filename@" */
/*** END file-production ***/

/*** BEGIN value-header ***/
GIO_AVAILABLE_IN_ALL GType prefix_the_xenum_get_type (void) G_GNUC_CONST;
#define PREFIX_TYPE_THE_XENUM (prefix_the_xenum_get_type ())
/*** END value-header ***/

/*** BEGIN file-tail ***/
G_END_DECLS

#endif /* __GIO_ENUM_TYPES_H__ */
/*** END file-tail ***/
