// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: common_msg.proto

#import "GPBProtocolBuffers.h"

#if GOOGLE_PROTOBUF_OBJC_GEN_VERSION != 30001
#error This file was generated by a different version of protoc which is incompatible with your Protocol Buffer library sources.
#endif

// @@protoc_insertion_point(imports)

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"

CF_EXTERN_C_BEGIN

NS_ASSUME_NONNULL_BEGIN

#pragma mark - Enum EServerCmd

/// the server cmd
typedef GPB_ENUM(EServerCmd) {
  EServerCmd_Clogin = 1,
  EServerCmd_Csndmsg = 2,
  EServerCmd_Cgetmsg = 3,
  EServerCmd_Clogout = 4,
  EServerCmd_Ckeepalive = 5,
};

GPBEnumDescriptor *EServerCmd_EnumDescriptor(void);

/// Checks to see if the given value is defined by the enum or was not known at
/// the time this source was generated.
BOOL EServerCmd_IsValidValue(int32_t value);

#pragma mark - Enum EModuleType

/// different server module
/// the module means different business
/// it used between client and server
/// it can tell server this client belong to which server
typedef GPB_ENUM(EModuleType) {
  EModuleType_Tmeeting = 3,
  EModuleType_Tp2P = 4,
  EModuleType_Tlive = 5,
  EModuleType_Tcallcenter = 6,
};

GPBEnumDescriptor *EModuleType_EnumDescriptor(void);

/// Checks to see if the given value is defined by the enum or was not known at
/// the time this source was generated.
BOOL EModuleType_IsValidValue(int32_t value);

#pragma mark - CommonMsgRoot

/// Exposes the extension registry for this file.
///
/// The base class provides:
/// @code
///   + (GPBExtensionRegistry *)extensionRegistry;
/// @endcode
/// which is a @c GPBExtensionRegistry that includes all the extensions defined by
/// this file and all files that it depends on.
@interface CommonMsgRoot : GPBRootObject
@end

#pragma mark - MsgReq

typedef GPB_ENUM(MsgReq_FieldNumber) {
  MsgReq_FieldNumber_SvrCmds = 1,
  MsgReq_FieldNumber_ModType = 2,
  MsgReq_FieldNumber_Content = 3,
};

/// this is for request
@interface MsgReq : GPBMessage

@property(nonatomic, readwrite) EServerCmd svrCmds;

@property(nonatomic, readwrite) BOOL hasSvrCmds;
@property(nonatomic, readwrite) EModuleType modType;

@property(nonatomic, readwrite) BOOL hasModType;
@property(nonatomic, readwrite, copy, null_resettable) NSData *content;
/// Test to see if @c content has been set.
@property(nonatomic, readwrite) BOOL hasContent;

@end

#pragma mark - MsgRep

typedef GPB_ENUM(MsgRep_FieldNumber) {
  MsgRep_FieldNumber_SvrCmds = 1,
  MsgRep_FieldNumber_ModType = 2,
  MsgRep_FieldNumber_RspCont = 3,
  MsgRep_FieldNumber_RspCode = 4,
};

/// this is for response
@interface MsgRep : GPBMessage

@property(nonatomic, readwrite) EServerCmd svrCmds;

@property(nonatomic, readwrite) BOOL hasSvrCmds;
@property(nonatomic, readwrite) EModuleType modType;

@property(nonatomic, readwrite) BOOL hasModType;
@property(nonatomic, readwrite, copy, null_resettable) NSData *rspCont;
/// Test to see if @c rspCont has been set.
@property(nonatomic, readwrite) BOOL hasRspCont;

@property(nonatomic, readwrite) int32_t rspCode;

@property(nonatomic, readwrite) BOOL hasRspCode;
@end

#pragma mark - ToUser

typedef GPB_ENUM(ToUser_FieldNumber) {
  ToUser_FieldNumber_UsersArray = 1,
};

/// users send to
@interface ToUser : GPBMessage

@property(nonatomic, readwrite, strong, null_resettable) NSMutableArray<NSString*> *usersArray;
/// The number of items in @c usersArray without causing the array to be created.
@property(nonatomic, readonly) NSUInteger usersArray_Count;

@end

NS_ASSUME_NONNULL_END

CF_EXTERN_C_END

#pragma clang diagnostic pop

// @@protoc_insertion_point(global_scope)
