// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: sys_msg_type.proto

#define INTERNAL_SUPPRESS_PROTOBUF_FIELD_DEPRECATION
#include "sys_msg_type.pb.h"

#include <algorithm>

#include <google/protobuf/stubs/common.h>
#include <google/protobuf/stubs/port.h>
#include <google/protobuf/stubs/once.h>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/wire_format_lite_inl.h>
// @@protoc_insertion_point(includes)

namespace pms {

void protobuf_ShutdownFile_sys_5fmsg_5ftype_2eproto() {
}

#ifdef GOOGLE_PROTOBUF_NO_STATIC_INITIALIZER
void protobuf_AddDesc_sys_5fmsg_5ftype_2eproto_impl() {
  GOOGLE_PROTOBUF_VERIFY_VERSION;

#else
void protobuf_AddDesc_sys_5fmsg_5ftype_2eproto() {
  static bool already_here = false;
  if (already_here) return;
  already_here = true;
  GOOGLE_PROTOBUF_VERIFY_VERSION;

#endif
  ::google::protobuf::internal::OnShutdown(&protobuf_ShutdownFile_sys_5fmsg_5ftype_2eproto);
}

#ifdef GOOGLE_PROTOBUF_NO_STATIC_INITIALIZER
GOOGLE_PROTOBUF_DECLARE_ONCE(protobuf_AddDesc_sys_5fmsg_5ftype_2eproto_once_);
void protobuf_AddDesc_sys_5fmsg_5ftype_2eproto() {
  ::google::protobuf::GoogleOnceInit(&protobuf_AddDesc_sys_5fmsg_5ftype_2eproto_once_,
                 &protobuf_AddDesc_sys_5fmsg_5ftype_2eproto_impl);
}
#else
// Force AddDescriptors() to be called at static initialization time.
struct StaticDescriptorInitializer_sys_5fmsg_5ftype_2eproto {
  StaticDescriptorInitializer_sys_5fmsg_5ftype_2eproto() {
    protobuf_AddDesc_sys_5fmsg_5ftype_2eproto();
  }
} static_descriptor_initializer_sys_5fmsg_5ftype_2eproto_;
#endif
bool ETransferModule_IsValid(int value) {
  switch(value) {
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
    case 8:
    case 9:
    case 10:
      return true;
    default:
      return false;
  }
}

bool ETransferType_IsValid(int value) {
  switch(value) {
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
    case 8:
    case 9:
    case 10:
    case 11:
      return true;
    default:
      return false;
  }
}

bool ETransferPriority_IsValid(int value) {
  switch(value) {
    case 0:
    case 1:
    case 2:
      return true;
    default:
      return false;
  }
}

bool ETransferFlag_IsValid(int value) {
  switch(value) {
    case 0:
    case 1:
    case 2:
      return true;
    default:
      return false;
  }
}

bool ETransferTag_IsValid(int value) {
  switch(value) {
    case 0:
    case 1:
      return true;
    default:
      return false;
  }
}

bool EConnTag_IsValid(int value) {
  switch(value) {
    case 0:
    case 1:
    case 2:
    case 3:
      return true;
    default:
      return false;
  }
}

bool EConnType_IsValid(int value) {
  switch(value) {
    case 0:
    case 1:
      return true;
    default:
      return false;
  }
}


// @@protoc_insertion_point(namespace_scope)

}  // namespace pms

// @@protoc_insertion_point(global_scope)
