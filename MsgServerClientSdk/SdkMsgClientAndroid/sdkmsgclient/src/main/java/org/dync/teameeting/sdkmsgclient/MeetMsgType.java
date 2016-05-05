// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: meet_msg_type.proto

package org.dync.teameeting.sdkmsgclient;

public final class MeetMsgType {
  private MeetMsgType() {}
  public static void registerAllExtensions(
      com.google.protobuf.ExtensionRegistry registry) {
  }
  /**
   * <pre>
   * the msg content type
   * </pre>
   *
   * Protobuf enum {@code pms.EMsgType}
   */
  public enum EMsgType
      implements com.google.protobuf.ProtocolMessageEnum {
    /**
     * <code>TMSG = 1;</code>
     */
    TMSG(1),
    /**
     * <code>TFILE = 2;</code>
     */
    TFILE(2),
    /**
     * <code>TPIC = 3;</code>
     */
    TPIC(3),
    /**
     * <code>TAUD = 4;</code>
     */
    TAUD(4),
    /**
     * <code>TVID = 5;</code>
     */
    TVID(5),
    ;

    /**
     * <code>TMSG = 1;</code>
     */
    public static final int TMSG_VALUE = 1;
    /**
     * <code>TFILE = 2;</code>
     */
    public static final int TFILE_VALUE = 2;
    /**
     * <code>TPIC = 3;</code>
     */
    public static final int TPIC_VALUE = 3;
    /**
     * <code>TAUD = 4;</code>
     */
    public static final int TAUD_VALUE = 4;
    /**
     * <code>TVID = 5;</code>
     */
    public static final int TVID_VALUE = 5;


    public final int getNumber() {
      return value;
    }

    /**
     * @deprecated Use {@link #forNumber(int)} instead.
     */
    @java.lang.Deprecated
    public static EMsgType valueOf(int value) {
      return forNumber(value);
    }

    public static EMsgType forNumber(int value) {
      switch (value) {
        case 1: return TMSG;
        case 2: return TFILE;
        case 3: return TPIC;
        case 4: return TAUD;
        case 5: return TVID;
        default: return null;
      }
    }

    public static com.google.protobuf.Internal.EnumLiteMap<EMsgType>
        internalGetValueMap() {
      return internalValueMap;
    }
    private static final com.google.protobuf.Internal.EnumLiteMap<
        EMsgType> internalValueMap =
          new com.google.protobuf.Internal.EnumLiteMap<EMsgType>() {
            public EMsgType findValueByNumber(int number) {
              return EMsgType.forNumber(number);
            }
          };

    public final com.google.protobuf.Descriptors.EnumValueDescriptor
        getValueDescriptor() {
      return getDescriptor().getValues().get(ordinal());
    }
    public final com.google.protobuf.Descriptors.EnumDescriptor
        getDescriptorForType() {
      return getDescriptor();
    }
    public static final com.google.protobuf.Descriptors.EnumDescriptor
        getDescriptor() {
      return org.dync.teameeting.sdkmsgclient.MeetMsgType.getDescriptor().getEnumTypes().get(0);
    }

    private static final EMsgType[] VALUES = values();

    public static EMsgType valueOf(
        com.google.protobuf.Descriptors.EnumValueDescriptor desc) {
      if (desc.getType() != getDescriptor()) {
        throw new java.lang.IllegalArgumentException(
          "EnumValueDescriptor is not for this type.");
      }
      return VALUES[desc.getIndex()];
    }

    private final int value;

    private EMsgType(int value) {
      this.value = value;
    }

    // @@protoc_insertion_point(enum_scope:pms.EMsgType)
  }

  /**
   * <pre>
   * the msg tag
   * </pre>
   *
   * Protobuf enum {@code pms.EMsgTag}
   */
  public enum EMsgTag
      implements com.google.protobuf.ProtocolMessageEnum {
    /**
     * <code>TENTER = 1;</code>
     */
    TENTER(1),
    /**
     * <code>TLEAVE = 2;</code>
     */
    TLEAVE(2),
    /**
     * <code>TCHAT = 3;</code>
     */
    TCHAT(3),
    /**
     * <code>TNOTIFY = 4;</code>
     */
    TNOTIFY(4),
    ;

    /**
     * <code>TENTER = 1;</code>
     */
    public static final int TENTER_VALUE = 1;
    /**
     * <code>TLEAVE = 2;</code>
     */
    public static final int TLEAVE_VALUE = 2;
    /**
     * <code>TCHAT = 3;</code>
     */
    public static final int TCHAT_VALUE = 3;
    /**
     * <code>TNOTIFY = 4;</code>
     */
    public static final int TNOTIFY_VALUE = 4;


    public final int getNumber() {
      return value;
    }

    /**
     * @deprecated Use {@link #forNumber(int)} instead.
     */
    @java.lang.Deprecated
    public static EMsgTag valueOf(int value) {
      return forNumber(value);
    }

    public static EMsgTag forNumber(int value) {
      switch (value) {
        case 1: return TENTER;
        case 2: return TLEAVE;
        case 3: return TCHAT;
        case 4: return TNOTIFY;
        default: return null;
      }
    }

    public static com.google.protobuf.Internal.EnumLiteMap<EMsgTag>
        internalGetValueMap() {
      return internalValueMap;
    }
    private static final com.google.protobuf.Internal.EnumLiteMap<
        EMsgTag> internalValueMap =
          new com.google.protobuf.Internal.EnumLiteMap<EMsgTag>() {
            public EMsgTag findValueByNumber(int number) {
              return EMsgTag.forNumber(number);
            }
          };

    public final com.google.protobuf.Descriptors.EnumValueDescriptor
        getValueDescriptor() {
      return getDescriptor().getValues().get(ordinal());
    }
    public final com.google.protobuf.Descriptors.EnumDescriptor
        getDescriptorForType() {
      return getDescriptor();
    }
    public static final com.google.protobuf.Descriptors.EnumDescriptor
        getDescriptor() {
      return org.dync.teameeting.sdkmsgclient.MeetMsgType.getDescriptor().getEnumTypes().get(1);
    }

    private static final EMsgTag[] VALUES = values();

    public static EMsgTag valueOf(
        com.google.protobuf.Descriptors.EnumValueDescriptor desc) {
      if (desc.getType() != getDescriptor()) {
        throw new java.lang.IllegalArgumentException(
          "EnumValueDescriptor is not for this type.");
      }
      return VALUES[desc.getIndex()];
    }

    private final int value;

    private EMsgTag(int value) {
      this.value = value;
    }

    // @@protoc_insertion_point(enum_scope:pms.EMsgTag)
  }

  /**
   * <pre>
   * the msg get tag
   * </pre>
   *
   * Protobuf enum {@code pms.EMsgHead}
   */
  public enum EMsgHead
      implements com.google.protobuf.ProtocolMessageEnum {
    /**
     * <code>HSND = 1;</code>
     */
    HSND(1),
    /**
     * <code>HGET = 2;</code>
     */
    HGET(2),
    ;

    /**
     * <code>HSND = 1;</code>
     */
    public static final int HSND_VALUE = 1;
    /**
     * <code>HGET = 2;</code>
     */
    public static final int HGET_VALUE = 2;


    public final int getNumber() {
      return value;
    }

    /**
     * @deprecated Use {@link #forNumber(int)} instead.
     */
    @java.lang.Deprecated
    public static EMsgHead valueOf(int value) {
      return forNumber(value);
    }

    public static EMsgHead forNumber(int value) {
      switch (value) {
        case 1: return HSND;
        case 2: return HGET;
        default: return null;
      }
    }

    public static com.google.protobuf.Internal.EnumLiteMap<EMsgHead>
        internalGetValueMap() {
      return internalValueMap;
    }
    private static final com.google.protobuf.Internal.EnumLiteMap<
        EMsgHead> internalValueMap =
          new com.google.protobuf.Internal.EnumLiteMap<EMsgHead>() {
            public EMsgHead findValueByNumber(int number) {
              return EMsgHead.forNumber(number);
            }
          };

    public final com.google.protobuf.Descriptors.EnumValueDescriptor
        getValueDescriptor() {
      return getDescriptor().getValues().get(ordinal());
    }
    public final com.google.protobuf.Descriptors.EnumDescriptor
        getDescriptorForType() {
      return getDescriptor();
    }
    public static final com.google.protobuf.Descriptors.EnumDescriptor
        getDescriptor() {
      return org.dync.teameeting.sdkmsgclient.MeetMsgType.getDescriptor().getEnumTypes().get(2);
    }

    private static final EMsgHead[] VALUES = values();

    public static EMsgHead valueOf(
        com.google.protobuf.Descriptors.EnumValueDescriptor desc) {
      if (desc.getType() != getDescriptor()) {
        throw new java.lang.IllegalArgumentException(
          "EnumValueDescriptor is not for this type.");
      }
      return VALUES[desc.getIndex()];
    }

    private final int value;

    private EMsgHead(int value) {
      this.value = value;
    }

    // @@protoc_insertion_point(enum_scope:pms.EMsgHead)
  }


  public static com.google.protobuf.Descriptors.FileDescriptor
      getDescriptor() {
    return descriptor;
  }
  private static  com.google.protobuf.Descriptors.FileDescriptor
      descriptor;
  static {
    java.lang.String[] descriptorData = {
      "\n\023meet_msg_type.proto\022\003pms*=\n\010EMsgType\022\010" +
      "\n\004TMSG\020\001\022\t\n\005TFILE\020\002\022\010\n\004TPIC\020\003\022\010\n\004TAUD\020\004\022" +
      "\010\n\004TVID\020\005*9\n\007EMsgTag\022\n\n\006TENTER\020\001\022\n\n\006TLEA" +
      "VE\020\002\022\t\n\005TCHAT\020\003\022\013\n\007TNOTIFY\020\004*\036\n\010EMsgHead" +
      "\022\010\n\004HSND\020\001\022\010\n\004HGET\020\002B1\n org.dync.teameet" +
      "ing.sdkmsgclientB\013MeetMsgTypeH\002"
    };
    com.google.protobuf.Descriptors.FileDescriptor.InternalDescriptorAssigner assigner =
        new com.google.protobuf.Descriptors.FileDescriptor.    InternalDescriptorAssigner() {
          public com.google.protobuf.ExtensionRegistry assignDescriptors(
              com.google.protobuf.Descriptors.FileDescriptor root) {
            descriptor = root;
            return null;
          }
        };
    com.google.protobuf.Descriptors.FileDescriptor
      .internalBuildGeneratedFileFrom(descriptorData,
        new com.google.protobuf.Descriptors.FileDescriptor[] {
        }, assigner);
  }

  // @@protoc_insertion_point(outer_class_scope)
}
