// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: meet_msg_type.proto

#ifndef PROTOBUF_meet_5fmsg_5ftype_2eproto__INCLUDED
#define PROTOBUF_meet_5fmsg_5ftype_2eproto__INCLUDED

#include <string>

#include <google/protobuf/stubs/common.h>

#if GOOGLE_PROTOBUF_VERSION < 3000000
#error This file was generated by a newer version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please update
#error your headers.
#endif
#if 3000000 < GOOGLE_PROTOBUF_MIN_PROTOC_VERSION
#error This file was generated by an older version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please
#error regenerate this file with a newer version of protoc.
#endif

#include <google/protobuf/arena.h>
#include <google/protobuf/arenastring.h>
#include <google/protobuf/generated_message_util.h>
#include <google/protobuf/metadata.h>
#include <google/protobuf/repeated_field.h>
#include <google/protobuf/extension_set.h>
#include <google/protobuf/generated_enum_reflection.h>
// @@protoc_insertion_point(includes)

namespace pms {

// Internal implementation detail -- do not call these.
void protobuf_AddDesc_meet_5fmsg_5ftype_2eproto();
void protobuf_AssignDesc_meet_5fmsg_5ftype_2eproto();
void protobuf_ShutdownFile_meet_5fmsg_5ftype_2eproto();


enum EMsgType {
  TMSG = 1,
  TFILE = 2,
  TPIC = 3,
  TAUD = 4,
  TVID = 5
};
bool EMsgType_IsValid(int value);
const EMsgType EMsgType_MIN = TMSG;
const EMsgType EMsgType_MAX = TVID;
const int EMsgType_ARRAYSIZE = EMsgType_MAX + 1;

const ::google::protobuf::EnumDescriptor* EMsgType_descriptor();
inline const ::std::string& EMsgType_Name(EMsgType value) {
  return ::google::protobuf::internal::NameOfEnum(
    EMsgType_descriptor(), value);
}
inline bool EMsgType_Parse(
    const ::std::string& name, EMsgType* value) {
  return ::google::protobuf::internal::ParseNamedEnum<EMsgType>(
    EMsgType_descriptor(), name, value);
}
enum EMsgTag {
  TENTER = 1,
  TLEAVE = 2,
  TCHAT = 3,
  TNOTIFY = 4
};
bool EMsgTag_IsValid(int value);
const EMsgTag EMsgTag_MIN = TENTER;
const EMsgTag EMsgTag_MAX = TNOTIFY;
const int EMsgTag_ARRAYSIZE = EMsgTag_MAX + 1;

const ::google::protobuf::EnumDescriptor* EMsgTag_descriptor();
inline const ::std::string& EMsgTag_Name(EMsgTag value) {
  return ::google::protobuf::internal::NameOfEnum(
    EMsgTag_descriptor(), value);
}
inline bool EMsgTag_Parse(
    const ::std::string& name, EMsgTag* value) {
  return ::google::protobuf::internal::ParseNamedEnum<EMsgTag>(
    EMsgTag_descriptor(), name, value);
}
enum EMsgHead {
  HSND = 1,
  HGET = 2
};
bool EMsgHead_IsValid(int value);
const EMsgHead EMsgHead_MIN = HSND;
const EMsgHead EMsgHead_MAX = HGET;
const int EMsgHead_ARRAYSIZE = EMsgHead_MAX + 1;

const ::google::protobuf::EnumDescriptor* EMsgHead_descriptor();
inline const ::std::string& EMsgHead_Name(EMsgHead value) {
  return ::google::protobuf::internal::NameOfEnum(
    EMsgHead_descriptor(), value);
}
inline bool EMsgHead_Parse(
    const ::std::string& name, EMsgHead* value) {
  return ::google::protobuf::internal::ParseNamedEnum<EMsgHead>(
    EMsgHead_descriptor(), name, value);
}
// ===================================================================


// ===================================================================


// ===================================================================

#if !PROTOBUF_INLINE_NOT_IN_HEADERS
#endif  // !PROTOBUF_INLINE_NOT_IN_HEADERS

// @@protoc_insertion_point(namespace_scope)

}  // namespace pms

#ifndef SWIG
namespace google {
namespace protobuf {

template <> struct is_proto_enum< ::pms::EMsgType> : ::google::protobuf::internal::true_type {};
template <>
inline const EnumDescriptor* GetEnumDescriptor< ::pms::EMsgType>() {
  return ::pms::EMsgType_descriptor();
}
template <> struct is_proto_enum< ::pms::EMsgTag> : ::google::protobuf::internal::true_type {};
template <>
inline const EnumDescriptor* GetEnumDescriptor< ::pms::EMsgTag>() {
  return ::pms::EMsgTag_descriptor();
}
template <> struct is_proto_enum< ::pms::EMsgHead> : ::google::protobuf::internal::true_type {};
template <>
inline const EnumDescriptor* GetEnumDescriptor< ::pms::EMsgHead>() {
  return ::pms::EMsgHead_descriptor();
}

}  // namespace protobuf
}  // namespace google
#endif  // SWIG

// @@protoc_insertion_point(global_scope)

#endif  // PROTOBUF_meet_5fmsg_5ftype_2eproto__INCLUDED
