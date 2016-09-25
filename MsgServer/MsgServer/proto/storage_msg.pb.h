// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: storage_msg.proto

#ifndef PROTOBUF_storage_5fmsg_2eproto__INCLUDED
#define PROTOBUF_storage_5fmsg_2eproto__INCLUDED

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
#include <google/protobuf/message_lite.h>
#include <google/protobuf/repeated_field.h>
#include <google/protobuf/extension_set.h>
#include "storage_msg_type.pb.h"
#include "common_msg.pb.h"
// @@protoc_insertion_point(includes)

namespace pms {

// Internal implementation detail -- do not call these.
void protobuf_AddDesc_storage_5fmsg_2eproto();
void protobuf_AssignDesc_storage_5fmsg_2eproto();
void protobuf_ShutdownFile_storage_5fmsg_2eproto();

class PackedStoreMsg;
class StorageMsg;

// ===================================================================

class StorageMsg : public ::google::protobuf::MessageLite {
 public:
  StorageMsg();
  virtual ~StorageMsg();

  StorageMsg(const StorageMsg& from);

  inline StorageMsg& operator=(const StorageMsg& from) {
    CopyFrom(from);
    return *this;
  }

  static const StorageMsg& default_instance();

  #ifdef GOOGLE_PROTOBUF_NO_STATIC_INITIALIZER
  // Returns the internal default instance pointer. This function can
  // return NULL thus should not be used by the user. This is intended
  // for Protobuf internal code. Please use default_instance() declared
  // above instead.
  static inline const StorageMsg* internal_default_instance() {
    return default_instance_;
  }
  #endif

  void Swap(StorageMsg* other);

  // implements Message ----------------------------------------------

  inline StorageMsg* New() const { return New(NULL); }

  StorageMsg* New(::google::protobuf::Arena* arena) const;
  void CheckTypeAndMergeFrom(const ::google::protobuf::MessageLite& from);
  void CopyFrom(const StorageMsg& from);
  void MergeFrom(const StorageMsg& from);
  void Clear();
  bool IsInitialized() const;

  int ByteSize() const;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input);
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const;
  void DiscardUnknownFields();
  int GetCachedSize() const { return _cached_size_; }
  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const;
  void InternalSwap(StorageMsg* other);
  private:
  inline ::google::protobuf::Arena* GetArenaNoVirtual() const {
    return _arena_ptr_;
  }
  inline ::google::protobuf::Arena* MaybeArenaPtr() const {
    return _arena_ptr_;
  }
  public:

  ::std::string GetTypeName() const;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  // optional .pms.EServerCmd rsvrcmd = 1;
  void clear_rsvrcmd();
  static const int kRsvrcmdFieldNumber = 1;
  ::pms::EServerCmd rsvrcmd() const;
  void set_rsvrcmd(::pms::EServerCmd value);

  // optional .pms.EServerCmd tsvrcmd = 2;
  void clear_tsvrcmd();
  static const int kTsvrcmdFieldNumber = 2;
  ::pms::EServerCmd tsvrcmd() const;
  void set_tsvrcmd(::pms::EServerCmd value);

  // optional .pms.EStorageTag mtag = 3;
  void clear_mtag();
  static const int kMtagFieldNumber = 3;
  ::pms::EStorageTag mtag() const;
  void set_mtag(::pms::EStorageTag value);

  // optional .pms.EMsgFlag mflag = 4;
  void clear_mflag();
  static const int kMflagFieldNumber = 4;
  ::pms::EMsgFlag mflag() const;
  void set_mflag(::pms::EMsgFlag value);

  // optional .pms.EMsgRole mrole = 5;
  void clear_mrole();
  static const int kMroleFieldNumber = 5;
  ::pms::EMsgRole mrole() const;
  void set_mrole(::pms::EMsgRole value);

  // optional sint32 result = 6;
  void clear_result();
  static const int kResultFieldNumber = 6;
  ::google::protobuf::int32 result() const;
  void set_result(::google::protobuf::int32 value);

  // optional sint64 sequence = 7;
  void clear_sequence();
  static const int kSequenceFieldNumber = 7;
  ::google::protobuf::int64 sequence() const;
  void set_sequence(::google::protobuf::int64 value);

  // optional sint64 maxseqn = 8;
  void clear_maxseqn();
  static const int kMaxseqnFieldNumber = 8;
  ::google::protobuf::int64 maxseqn() const;
  void set_maxseqn(::google::protobuf::int64 value);

  // optional string msgid = 9;
  void clear_msgid();
  static const int kMsgidFieldNumber = 9;
  const ::std::string& msgid() const;
  void set_msgid(const ::std::string& value);
  void set_msgid(const char* value);
  void set_msgid(const char* value, size_t size);
  ::std::string* mutable_msgid();
  ::std::string* release_msgid();
  void set_allocated_msgid(::std::string* msgid);

  // optional string storeid = 10;
  void clear_storeid();
  static const int kStoreidFieldNumber = 10;
  const ::std::string& storeid() const;
  void set_storeid(const ::std::string& value);
  void set_storeid(const char* value);
  void set_storeid(const char* value, size_t size);
  ::std::string* mutable_storeid();
  ::std::string* release_storeid();
  void set_allocated_storeid(::std::string* storeid);

  // optional string ruserid = 11;
  void clear_ruserid();
  static const int kRuseridFieldNumber = 11;
  const ::std::string& ruserid() const;
  void set_ruserid(const ::std::string& value);
  void set_ruserid(const char* value);
  void set_ruserid(const char* value, size_t size);
  ::std::string* mutable_ruserid();
  ::std::string* release_ruserid();
  void set_allocated_ruserid(::std::string* ruserid);

  // optional string groupid = 12;
  void clear_groupid();
  static const int kGroupidFieldNumber = 12;
  const ::std::string& groupid() const;
  void set_groupid(const ::std::string& value);
  void set_groupid(const char* value);
  void set_groupid(const char* value, size_t size);
  ::std::string* mutable_groupid();
  ::std::string* release_groupid();
  void set_allocated_groupid(::std::string* groupid);

  // optional string version = 13;
  void clear_version();
  static const int kVersionFieldNumber = 13;
  const ::std::string& version() const;
  void set_version(const ::std::string& value);
  void set_version(const char* value);
  void set_version(const char* value, size_t size);
  ::std::string* mutable_version();
  ::std::string* release_version();
  void set_allocated_version(::std::string* version);

  // optional string mtype = 14;
  void clear_mtype();
  static const int kMtypeFieldNumber = 14;
  const ::std::string& mtype() const;
  void set_mtype(const ::std::string& value);
  void set_mtype(const char* value);
  void set_mtype(const char* value, size_t size);
  ::std::string* mutable_mtype();
  ::std::string* release_mtype();
  void set_allocated_mtype(::std::string* mtype);

  // optional string ispush = 15;
  void clear_ispush();
  static const int kIspushFieldNumber = 15;
  const ::std::string& ispush() const;
  void set_ispush(const ::std::string& value);
  void set_ispush(const char* value);
  void set_ispush(const char* value, size_t size);
  ::std::string* mutable_ispush();
  ::std::string* release_ispush();
  void set_allocated_ispush(::std::string* ispush);

  // optional bytes content = 16;
  void clear_content();
  static const int kContentFieldNumber = 16;
  const ::std::string& content() const;
  void set_content(const ::std::string& value);
  void set_content(const char* value);
  void set_content(const void* value, size_t size);
  ::std::string* mutable_content();
  ::std::string* release_content();
  void set_allocated_content(::std::string* content);

  // optional .pms.EStoreContType conttype = 17;
  void clear_conttype();
  static const int kConttypeFieldNumber = 17;
  ::pms::EStoreContType conttype() const;
  void set_conttype(::pms::EStoreContType value);

  // optional .pms.EModuleType module = 18;
  void clear_module();
  static const int kModuleFieldNumber = 18;
  ::pms::EModuleType module() const;
  void set_module(::pms::EModuleType value);

  // optional sint64 sdmaxseqn = 19;
  void clear_sdmaxseqn();
  static const int kSdmaxseqnFieldNumber = 19;
  ::google::protobuf::int64 sdmaxseqn() const;
  void set_sdmaxseqn(::google::protobuf::int64 value);

  // @@protoc_insertion_point(class_scope:pms.StorageMsg)
 private:

  ::google::protobuf::internal::ArenaStringPtr _unknown_fields_;
  ::google::protobuf::Arena* _arena_ptr_;

  bool _is_default_instance_;
  int rsvrcmd_;
  int tsvrcmd_;
  int mtag_;
  int mflag_;
  int mrole_;
  ::google::protobuf::int32 result_;
  ::google::protobuf::int64 sequence_;
  ::google::protobuf::int64 maxseqn_;
  ::google::protobuf::internal::ArenaStringPtr msgid_;
  ::google::protobuf::internal::ArenaStringPtr storeid_;
  ::google::protobuf::internal::ArenaStringPtr ruserid_;
  ::google::protobuf::internal::ArenaStringPtr groupid_;
  ::google::protobuf::internal::ArenaStringPtr version_;
  ::google::protobuf::internal::ArenaStringPtr mtype_;
  ::google::protobuf::internal::ArenaStringPtr ispush_;
  ::google::protobuf::internal::ArenaStringPtr content_;
  int conttype_;
  int module_;
  ::google::protobuf::int64 sdmaxseqn_;
  mutable int _cached_size_;
  #ifdef GOOGLE_PROTOBUF_NO_STATIC_INITIALIZER
  friend void  protobuf_AddDesc_storage_5fmsg_2eproto_impl();
  #else
  friend void  protobuf_AddDesc_storage_5fmsg_2eproto();
  #endif
  friend void protobuf_AssignDesc_storage_5fmsg_2eproto();
  friend void protobuf_ShutdownFile_storage_5fmsg_2eproto();

  void InitAsDefaultInstance();
  static StorageMsg* default_instance_;
};
// -------------------------------------------------------------------

class PackedStoreMsg : public ::google::protobuf::MessageLite {
 public:
  PackedStoreMsg();
  virtual ~PackedStoreMsg();

  PackedStoreMsg(const PackedStoreMsg& from);

  inline PackedStoreMsg& operator=(const PackedStoreMsg& from) {
    CopyFrom(from);
    return *this;
  }

  static const PackedStoreMsg& default_instance();

  #ifdef GOOGLE_PROTOBUF_NO_STATIC_INITIALIZER
  // Returns the internal default instance pointer. This function can
  // return NULL thus should not be used by the user. This is intended
  // for Protobuf internal code. Please use default_instance() declared
  // above instead.
  static inline const PackedStoreMsg* internal_default_instance() {
    return default_instance_;
  }
  #endif

  void Swap(PackedStoreMsg* other);

  // implements Message ----------------------------------------------

  inline PackedStoreMsg* New() const { return New(NULL); }

  PackedStoreMsg* New(::google::protobuf::Arena* arena) const;
  void CheckTypeAndMergeFrom(const ::google::protobuf::MessageLite& from);
  void CopyFrom(const PackedStoreMsg& from);
  void MergeFrom(const PackedStoreMsg& from);
  void Clear();
  bool IsInitialized() const;

  int ByteSize() const;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input);
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const;
  void DiscardUnknownFields();
  int GetCachedSize() const { return _cached_size_; }
  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const;
  void InternalSwap(PackedStoreMsg* other);
  private:
  inline ::google::protobuf::Arena* GetArenaNoVirtual() const {
    return _arena_ptr_;
  }
  inline ::google::protobuf::Arena* MaybeArenaPtr() const {
    return _arena_ptr_;
  }
  public:

  ::std::string GetTypeName() const;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  // repeated .pms.StorageMsg msgs = 1;
  int msgs_size() const;
  void clear_msgs();
  static const int kMsgsFieldNumber = 1;
  const ::pms::StorageMsg& msgs(int index) const;
  ::pms::StorageMsg* mutable_msgs(int index);
  ::pms::StorageMsg* add_msgs();
  ::google::protobuf::RepeatedPtrField< ::pms::StorageMsg >*
      mutable_msgs();
  const ::google::protobuf::RepeatedPtrField< ::pms::StorageMsg >&
      msgs() const;

  // @@protoc_insertion_point(class_scope:pms.PackedStoreMsg)
 private:

  ::google::protobuf::internal::ArenaStringPtr _unknown_fields_;
  ::google::protobuf::Arena* _arena_ptr_;

  bool _is_default_instance_;
  ::google::protobuf::RepeatedPtrField< ::pms::StorageMsg > msgs_;
  mutable int _cached_size_;
  #ifdef GOOGLE_PROTOBUF_NO_STATIC_INITIALIZER
  friend void  protobuf_AddDesc_storage_5fmsg_2eproto_impl();
  #else
  friend void  protobuf_AddDesc_storage_5fmsg_2eproto();
  #endif
  friend void protobuf_AssignDesc_storage_5fmsg_2eproto();
  friend void protobuf_ShutdownFile_storage_5fmsg_2eproto();

  void InitAsDefaultInstance();
  static PackedStoreMsg* default_instance_;
};
// ===================================================================


// ===================================================================

#if !PROTOBUF_INLINE_NOT_IN_HEADERS
// StorageMsg

// optional .pms.EServerCmd rsvrcmd = 1;
inline void StorageMsg::clear_rsvrcmd() {
  rsvrcmd_ = 0;
}
inline ::pms::EServerCmd StorageMsg::rsvrcmd() const {
  // @@protoc_insertion_point(field_get:pms.StorageMsg.rsvrcmd)
  return static_cast< ::pms::EServerCmd >(rsvrcmd_);
}
inline void StorageMsg::set_rsvrcmd(::pms::EServerCmd value) {
  
  rsvrcmd_ = value;
  // @@protoc_insertion_point(field_set:pms.StorageMsg.rsvrcmd)
}

// optional .pms.EServerCmd tsvrcmd = 2;
inline void StorageMsg::clear_tsvrcmd() {
  tsvrcmd_ = 0;
}
inline ::pms::EServerCmd StorageMsg::tsvrcmd() const {
  // @@protoc_insertion_point(field_get:pms.StorageMsg.tsvrcmd)
  return static_cast< ::pms::EServerCmd >(tsvrcmd_);
}
inline void StorageMsg::set_tsvrcmd(::pms::EServerCmd value) {
  
  tsvrcmd_ = value;
  // @@protoc_insertion_point(field_set:pms.StorageMsg.tsvrcmd)
}

// optional .pms.EStorageTag mtag = 3;
inline void StorageMsg::clear_mtag() {
  mtag_ = 0;
}
inline ::pms::EStorageTag StorageMsg::mtag() const {
  // @@protoc_insertion_point(field_get:pms.StorageMsg.mtag)
  return static_cast< ::pms::EStorageTag >(mtag_);
}
inline void StorageMsg::set_mtag(::pms::EStorageTag value) {
  
  mtag_ = value;
  // @@protoc_insertion_point(field_set:pms.StorageMsg.mtag)
}

// optional .pms.EMsgFlag mflag = 4;
inline void StorageMsg::clear_mflag() {
  mflag_ = 0;
}
inline ::pms::EMsgFlag StorageMsg::mflag() const {
  // @@protoc_insertion_point(field_get:pms.StorageMsg.mflag)
  return static_cast< ::pms::EMsgFlag >(mflag_);
}
inline void StorageMsg::set_mflag(::pms::EMsgFlag value) {
  
  mflag_ = value;
  // @@protoc_insertion_point(field_set:pms.StorageMsg.mflag)
}

// optional .pms.EMsgRole mrole = 5;
inline void StorageMsg::clear_mrole() {
  mrole_ = 0;
}
inline ::pms::EMsgRole StorageMsg::mrole() const {
  // @@protoc_insertion_point(field_get:pms.StorageMsg.mrole)
  return static_cast< ::pms::EMsgRole >(mrole_);
}
inline void StorageMsg::set_mrole(::pms::EMsgRole value) {
  
  mrole_ = value;
  // @@protoc_insertion_point(field_set:pms.StorageMsg.mrole)
}

// optional sint32 result = 6;
inline void StorageMsg::clear_result() {
  result_ = 0;
}
inline ::google::protobuf::int32 StorageMsg::result() const {
  // @@protoc_insertion_point(field_get:pms.StorageMsg.result)
  return result_;
}
inline void StorageMsg::set_result(::google::protobuf::int32 value) {
  
  result_ = value;
  // @@protoc_insertion_point(field_set:pms.StorageMsg.result)
}

// optional sint64 sequence = 7;
inline void StorageMsg::clear_sequence() {
  sequence_ = GOOGLE_LONGLONG(0);
}
inline ::google::protobuf::int64 StorageMsg::sequence() const {
  // @@protoc_insertion_point(field_get:pms.StorageMsg.sequence)
  return sequence_;
}
inline void StorageMsg::set_sequence(::google::protobuf::int64 value) {
  
  sequence_ = value;
  // @@protoc_insertion_point(field_set:pms.StorageMsg.sequence)
}

// optional sint64 maxseqn = 8;
inline void StorageMsg::clear_maxseqn() {
  maxseqn_ = GOOGLE_LONGLONG(0);
}
inline ::google::protobuf::int64 StorageMsg::maxseqn() const {
  // @@protoc_insertion_point(field_get:pms.StorageMsg.maxseqn)
  return maxseqn_;
}
inline void StorageMsg::set_maxseqn(::google::protobuf::int64 value) {
  
  maxseqn_ = value;
  // @@protoc_insertion_point(field_set:pms.StorageMsg.maxseqn)
}

// optional string msgid = 9;
inline void StorageMsg::clear_msgid() {
  msgid_.ClearToEmptyNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline const ::std::string& StorageMsg::msgid() const {
  // @@protoc_insertion_point(field_get:pms.StorageMsg.msgid)
  return msgid_.GetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline void StorageMsg::set_msgid(const ::std::string& value) {
  
  msgid_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), value);
  // @@protoc_insertion_point(field_set:pms.StorageMsg.msgid)
}
inline void StorageMsg::set_msgid(const char* value) {
  
  msgid_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), ::std::string(value));
  // @@protoc_insertion_point(field_set_char:pms.StorageMsg.msgid)
}
inline void StorageMsg::set_msgid(const char* value, size_t size) {
  
  msgid_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(),
      ::std::string(reinterpret_cast<const char*>(value), size));
  // @@protoc_insertion_point(field_set_pointer:pms.StorageMsg.msgid)
}
inline ::std::string* StorageMsg::mutable_msgid() {
  
  // @@protoc_insertion_point(field_mutable:pms.StorageMsg.msgid)
  return msgid_.MutableNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline ::std::string* StorageMsg::release_msgid() {
  // @@protoc_insertion_point(field_release:pms.StorageMsg.msgid)
  
  return msgid_.ReleaseNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline void StorageMsg::set_allocated_msgid(::std::string* msgid) {
  if (msgid != NULL) {
    
  } else {
    
  }
  msgid_.SetAllocatedNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), msgid);
  // @@protoc_insertion_point(field_set_allocated:pms.StorageMsg.msgid)
}

// optional string storeid = 10;
inline void StorageMsg::clear_storeid() {
  storeid_.ClearToEmptyNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline const ::std::string& StorageMsg::storeid() const {
  // @@protoc_insertion_point(field_get:pms.StorageMsg.storeid)
  return storeid_.GetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline void StorageMsg::set_storeid(const ::std::string& value) {
  
  storeid_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), value);
  // @@protoc_insertion_point(field_set:pms.StorageMsg.storeid)
}
inline void StorageMsg::set_storeid(const char* value) {
  
  storeid_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), ::std::string(value));
  // @@protoc_insertion_point(field_set_char:pms.StorageMsg.storeid)
}
inline void StorageMsg::set_storeid(const char* value, size_t size) {
  
  storeid_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(),
      ::std::string(reinterpret_cast<const char*>(value), size));
  // @@protoc_insertion_point(field_set_pointer:pms.StorageMsg.storeid)
}
inline ::std::string* StorageMsg::mutable_storeid() {
  
  // @@protoc_insertion_point(field_mutable:pms.StorageMsg.storeid)
  return storeid_.MutableNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline ::std::string* StorageMsg::release_storeid() {
  // @@protoc_insertion_point(field_release:pms.StorageMsg.storeid)
  
  return storeid_.ReleaseNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline void StorageMsg::set_allocated_storeid(::std::string* storeid) {
  if (storeid != NULL) {
    
  } else {
    
  }
  storeid_.SetAllocatedNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), storeid);
  // @@protoc_insertion_point(field_set_allocated:pms.StorageMsg.storeid)
}

// optional string ruserid = 11;
inline void StorageMsg::clear_ruserid() {
  ruserid_.ClearToEmptyNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline const ::std::string& StorageMsg::ruserid() const {
  // @@protoc_insertion_point(field_get:pms.StorageMsg.ruserid)
  return ruserid_.GetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline void StorageMsg::set_ruserid(const ::std::string& value) {
  
  ruserid_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), value);
  // @@protoc_insertion_point(field_set:pms.StorageMsg.ruserid)
}
inline void StorageMsg::set_ruserid(const char* value) {
  
  ruserid_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), ::std::string(value));
  // @@protoc_insertion_point(field_set_char:pms.StorageMsg.ruserid)
}
inline void StorageMsg::set_ruserid(const char* value, size_t size) {
  
  ruserid_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(),
      ::std::string(reinterpret_cast<const char*>(value), size));
  // @@protoc_insertion_point(field_set_pointer:pms.StorageMsg.ruserid)
}
inline ::std::string* StorageMsg::mutable_ruserid() {
  
  // @@protoc_insertion_point(field_mutable:pms.StorageMsg.ruserid)
  return ruserid_.MutableNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline ::std::string* StorageMsg::release_ruserid() {
  // @@protoc_insertion_point(field_release:pms.StorageMsg.ruserid)
  
  return ruserid_.ReleaseNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline void StorageMsg::set_allocated_ruserid(::std::string* ruserid) {
  if (ruserid != NULL) {
    
  } else {
    
  }
  ruserid_.SetAllocatedNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), ruserid);
  // @@protoc_insertion_point(field_set_allocated:pms.StorageMsg.ruserid)
}

// optional string groupid = 12;
inline void StorageMsg::clear_groupid() {
  groupid_.ClearToEmptyNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline const ::std::string& StorageMsg::groupid() const {
  // @@protoc_insertion_point(field_get:pms.StorageMsg.groupid)
  return groupid_.GetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline void StorageMsg::set_groupid(const ::std::string& value) {
  
  groupid_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), value);
  // @@protoc_insertion_point(field_set:pms.StorageMsg.groupid)
}
inline void StorageMsg::set_groupid(const char* value) {
  
  groupid_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), ::std::string(value));
  // @@protoc_insertion_point(field_set_char:pms.StorageMsg.groupid)
}
inline void StorageMsg::set_groupid(const char* value, size_t size) {
  
  groupid_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(),
      ::std::string(reinterpret_cast<const char*>(value), size));
  // @@protoc_insertion_point(field_set_pointer:pms.StorageMsg.groupid)
}
inline ::std::string* StorageMsg::mutable_groupid() {
  
  // @@protoc_insertion_point(field_mutable:pms.StorageMsg.groupid)
  return groupid_.MutableNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline ::std::string* StorageMsg::release_groupid() {
  // @@protoc_insertion_point(field_release:pms.StorageMsg.groupid)
  
  return groupid_.ReleaseNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline void StorageMsg::set_allocated_groupid(::std::string* groupid) {
  if (groupid != NULL) {
    
  } else {
    
  }
  groupid_.SetAllocatedNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), groupid);
  // @@protoc_insertion_point(field_set_allocated:pms.StorageMsg.groupid)
}

// optional string version = 13;
inline void StorageMsg::clear_version() {
  version_.ClearToEmptyNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline const ::std::string& StorageMsg::version() const {
  // @@protoc_insertion_point(field_get:pms.StorageMsg.version)
  return version_.GetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline void StorageMsg::set_version(const ::std::string& value) {
  
  version_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), value);
  // @@protoc_insertion_point(field_set:pms.StorageMsg.version)
}
inline void StorageMsg::set_version(const char* value) {
  
  version_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), ::std::string(value));
  // @@protoc_insertion_point(field_set_char:pms.StorageMsg.version)
}
inline void StorageMsg::set_version(const char* value, size_t size) {
  
  version_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(),
      ::std::string(reinterpret_cast<const char*>(value), size));
  // @@protoc_insertion_point(field_set_pointer:pms.StorageMsg.version)
}
inline ::std::string* StorageMsg::mutable_version() {
  
  // @@protoc_insertion_point(field_mutable:pms.StorageMsg.version)
  return version_.MutableNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline ::std::string* StorageMsg::release_version() {
  // @@protoc_insertion_point(field_release:pms.StorageMsg.version)
  
  return version_.ReleaseNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline void StorageMsg::set_allocated_version(::std::string* version) {
  if (version != NULL) {
    
  } else {
    
  }
  version_.SetAllocatedNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), version);
  // @@protoc_insertion_point(field_set_allocated:pms.StorageMsg.version)
}

// optional string mtype = 14;
inline void StorageMsg::clear_mtype() {
  mtype_.ClearToEmptyNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline const ::std::string& StorageMsg::mtype() const {
  // @@protoc_insertion_point(field_get:pms.StorageMsg.mtype)
  return mtype_.GetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline void StorageMsg::set_mtype(const ::std::string& value) {
  
  mtype_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), value);
  // @@protoc_insertion_point(field_set:pms.StorageMsg.mtype)
}
inline void StorageMsg::set_mtype(const char* value) {
  
  mtype_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), ::std::string(value));
  // @@protoc_insertion_point(field_set_char:pms.StorageMsg.mtype)
}
inline void StorageMsg::set_mtype(const char* value, size_t size) {
  
  mtype_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(),
      ::std::string(reinterpret_cast<const char*>(value), size));
  // @@protoc_insertion_point(field_set_pointer:pms.StorageMsg.mtype)
}
inline ::std::string* StorageMsg::mutable_mtype() {
  
  // @@protoc_insertion_point(field_mutable:pms.StorageMsg.mtype)
  return mtype_.MutableNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline ::std::string* StorageMsg::release_mtype() {
  // @@protoc_insertion_point(field_release:pms.StorageMsg.mtype)
  
  return mtype_.ReleaseNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline void StorageMsg::set_allocated_mtype(::std::string* mtype) {
  if (mtype != NULL) {
    
  } else {
    
  }
  mtype_.SetAllocatedNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), mtype);
  // @@protoc_insertion_point(field_set_allocated:pms.StorageMsg.mtype)
}

// optional string ispush = 15;
inline void StorageMsg::clear_ispush() {
  ispush_.ClearToEmptyNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline const ::std::string& StorageMsg::ispush() const {
  // @@protoc_insertion_point(field_get:pms.StorageMsg.ispush)
  return ispush_.GetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline void StorageMsg::set_ispush(const ::std::string& value) {
  
  ispush_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), value);
  // @@protoc_insertion_point(field_set:pms.StorageMsg.ispush)
}
inline void StorageMsg::set_ispush(const char* value) {
  
  ispush_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), ::std::string(value));
  // @@protoc_insertion_point(field_set_char:pms.StorageMsg.ispush)
}
inline void StorageMsg::set_ispush(const char* value, size_t size) {
  
  ispush_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(),
      ::std::string(reinterpret_cast<const char*>(value), size));
  // @@protoc_insertion_point(field_set_pointer:pms.StorageMsg.ispush)
}
inline ::std::string* StorageMsg::mutable_ispush() {
  
  // @@protoc_insertion_point(field_mutable:pms.StorageMsg.ispush)
  return ispush_.MutableNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline ::std::string* StorageMsg::release_ispush() {
  // @@protoc_insertion_point(field_release:pms.StorageMsg.ispush)
  
  return ispush_.ReleaseNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline void StorageMsg::set_allocated_ispush(::std::string* ispush) {
  if (ispush != NULL) {
    
  } else {
    
  }
  ispush_.SetAllocatedNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), ispush);
  // @@protoc_insertion_point(field_set_allocated:pms.StorageMsg.ispush)
}

// optional bytes content = 16;
inline void StorageMsg::clear_content() {
  content_.ClearToEmptyNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline const ::std::string& StorageMsg::content() const {
  // @@protoc_insertion_point(field_get:pms.StorageMsg.content)
  return content_.GetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline void StorageMsg::set_content(const ::std::string& value) {
  
  content_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), value);
  // @@protoc_insertion_point(field_set:pms.StorageMsg.content)
}
inline void StorageMsg::set_content(const char* value) {
  
  content_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), ::std::string(value));
  // @@protoc_insertion_point(field_set_char:pms.StorageMsg.content)
}
inline void StorageMsg::set_content(const void* value, size_t size) {
  
  content_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(),
      ::std::string(reinterpret_cast<const char*>(value), size));
  // @@protoc_insertion_point(field_set_pointer:pms.StorageMsg.content)
}
inline ::std::string* StorageMsg::mutable_content() {
  
  // @@protoc_insertion_point(field_mutable:pms.StorageMsg.content)
  return content_.MutableNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline ::std::string* StorageMsg::release_content() {
  // @@protoc_insertion_point(field_release:pms.StorageMsg.content)
  
  return content_.ReleaseNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline void StorageMsg::set_allocated_content(::std::string* content) {
  if (content != NULL) {
    
  } else {
    
  }
  content_.SetAllocatedNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), content);
  // @@protoc_insertion_point(field_set_allocated:pms.StorageMsg.content)
}

// optional .pms.EStoreContType conttype = 17;
inline void StorageMsg::clear_conttype() {
  conttype_ = 0;
}
inline ::pms::EStoreContType StorageMsg::conttype() const {
  // @@protoc_insertion_point(field_get:pms.StorageMsg.conttype)
  return static_cast< ::pms::EStoreContType >(conttype_);
}
inline void StorageMsg::set_conttype(::pms::EStoreContType value) {
  
  conttype_ = value;
  // @@protoc_insertion_point(field_set:pms.StorageMsg.conttype)
}

// optional .pms.EModuleType module = 18;
inline void StorageMsg::clear_module() {
  module_ = 0;
}
inline ::pms::EModuleType StorageMsg::module() const {
  // @@protoc_insertion_point(field_get:pms.StorageMsg.module)
  return static_cast< ::pms::EModuleType >(module_);
}
inline void StorageMsg::set_module(::pms::EModuleType value) {
  
  module_ = value;
  // @@protoc_insertion_point(field_set:pms.StorageMsg.module)
}

// optional sint64 sdmaxseqn = 19;
inline void StorageMsg::clear_sdmaxseqn() {
  sdmaxseqn_ = GOOGLE_LONGLONG(0);
}
inline ::google::protobuf::int64 StorageMsg::sdmaxseqn() const {
  // @@protoc_insertion_point(field_get:pms.StorageMsg.sdmaxseqn)
  return sdmaxseqn_;
}
inline void StorageMsg::set_sdmaxseqn(::google::protobuf::int64 value) {
  
  sdmaxseqn_ = value;
  // @@protoc_insertion_point(field_set:pms.StorageMsg.sdmaxseqn)
}

// -------------------------------------------------------------------

// PackedStoreMsg

// repeated .pms.StorageMsg msgs = 1;
inline int PackedStoreMsg::msgs_size() const {
  return msgs_.size();
}
inline void PackedStoreMsg::clear_msgs() {
  msgs_.Clear();
}
inline const ::pms::StorageMsg& PackedStoreMsg::msgs(int index) const {
  // @@protoc_insertion_point(field_get:pms.PackedStoreMsg.msgs)
  return msgs_.Get(index);
}
inline ::pms::StorageMsg* PackedStoreMsg::mutable_msgs(int index) {
  // @@protoc_insertion_point(field_mutable:pms.PackedStoreMsg.msgs)
  return msgs_.Mutable(index);
}
inline ::pms::StorageMsg* PackedStoreMsg::add_msgs() {
  // @@protoc_insertion_point(field_add:pms.PackedStoreMsg.msgs)
  return msgs_.Add();
}
inline ::google::protobuf::RepeatedPtrField< ::pms::StorageMsg >*
PackedStoreMsg::mutable_msgs() {
  // @@protoc_insertion_point(field_mutable_list:pms.PackedStoreMsg.msgs)
  return &msgs_;
}
inline const ::google::protobuf::RepeatedPtrField< ::pms::StorageMsg >&
PackedStoreMsg::msgs() const {
  // @@protoc_insertion_point(field_list:pms.PackedStoreMsg.msgs)
  return msgs_;
}

#endif  // !PROTOBUF_INLINE_NOT_IN_HEADERS
// -------------------------------------------------------------------


// @@protoc_insertion_point(namespace_scope)

}  // namespace pms

// @@protoc_insertion_point(global_scope)

#endif  // PROTOBUF_storage_5fmsg_2eproto__INCLUDED
