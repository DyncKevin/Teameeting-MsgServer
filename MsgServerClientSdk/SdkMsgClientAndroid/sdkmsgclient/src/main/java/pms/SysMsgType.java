// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: sys_msg_type.proto

package pms;

public final class SysMsgType {
  private SysMsgType() {}
  public static void registerAllExtensions(
      com.google.protobuf.ExtensionRegistryLite registry) {
  }
  /**
   * <pre>
   * the sever logic module name
   * it userd between servers, not used in client
   * </pre>
   *
   * Protobuf enum {@code pms.ETransferModule}
   */
  public enum ETransferModule
      implements com.google.protobuf.Internal.EnumLite {
    /**
     * <code>MCONNECTOR = 0;</code>
     */
    MCONNECTOR(0),
    /**
     * <code>MMSGQUEUE = 1;</code>
     */
    MMSGQUEUE(1),
    /**
     * <code>MMEETING = 2;</code>
     */
    MMEETING(2),
    /**
     * <code>MP2P = 3;</code>
     */
    MP2P(3),
    /**
     * <code>MLIVE = 4;</code>
     */
    MLIVE(4),
    /**
     * <code>MCALLCENTER = 5;</code>
     */
    MCALLCENTER(5),
    /**
     * <code>MLOGICAL = 6;</code>
     */
    MLOGICAL(6),
    /**
     * <code>MSEQUENCE = 7;</code>
     */
    MSEQUENCE(7),
    /**
     * <code>MSTORAGE = 8;</code>
     */
    MSTORAGE(8),
    /**
     * <code>MGRPNOTIFY = 9;</code>
     */
    MGRPNOTIFY(9),
    /**
     * <code>MPUSHER = 10;</code>
     */
    MPUSHER(10),
    UNRECOGNIZED(-1),
    ;

    /**
     * <code>MCONNECTOR = 0;</code>
     */
    public static final int MCONNECTOR_VALUE = 0;
    /**
     * <code>MMSGQUEUE = 1;</code>
     */
    public static final int MMSGQUEUE_VALUE = 1;
    /**
     * <code>MMEETING = 2;</code>
     */
    public static final int MMEETING_VALUE = 2;
    /**
     * <code>MP2P = 3;</code>
     */
    public static final int MP2P_VALUE = 3;
    /**
     * <code>MLIVE = 4;</code>
     */
    public static final int MLIVE_VALUE = 4;
    /**
     * <code>MCALLCENTER = 5;</code>
     */
    public static final int MCALLCENTER_VALUE = 5;
    /**
     * <code>MLOGICAL = 6;</code>
     */
    public static final int MLOGICAL_VALUE = 6;
    /**
     * <code>MSEQUENCE = 7;</code>
     */
    public static final int MSEQUENCE_VALUE = 7;
    /**
     * <code>MSTORAGE = 8;</code>
     */
    public static final int MSTORAGE_VALUE = 8;
    /**
     * <code>MGRPNOTIFY = 9;</code>
     */
    public static final int MGRPNOTIFY_VALUE = 9;
    /**
     * <code>MPUSHER = 10;</code>
     */
    public static final int MPUSHER_VALUE = 10;


    public final int getNumber() {
      return value;
    }

    /**
     * @deprecated Use {@link #forNumber(int)} instead.
     */
    @java.lang.Deprecated
    public static ETransferModule valueOf(int value) {
      return forNumber(value);
    }

    public static ETransferModule forNumber(int value) {
      switch (value) {
        case 0: return MCONNECTOR;
        case 1: return MMSGQUEUE;
        case 2: return MMEETING;
        case 3: return MP2P;
        case 4: return MLIVE;
        case 5: return MCALLCENTER;
        case 6: return MLOGICAL;
        case 7: return MSEQUENCE;
        case 8: return MSTORAGE;
        case 9: return MGRPNOTIFY;
        case 10: return MPUSHER;
        default: return null;
      }
    }

    public static com.google.protobuf.Internal.EnumLiteMap<ETransferModule>
        internalGetValueMap() {
      return internalValueMap;
    }
    private static final com.google.protobuf.Internal.EnumLiteMap<
        ETransferModule> internalValueMap =
          new com.google.protobuf.Internal.EnumLiteMap<ETransferModule>() {
            public ETransferModule findValueByNumber(int number) {
              return ETransferModule.forNumber(number);
            }
          };

    private final int value;

    private ETransferModule(int value) {
      this.value = value;
    }

    // @@protoc_insertion_point(enum_scope:pms.ETransferModule)
  }

  /**
   * <pre>
   * the type of server transfering msg
   * </pre>
   *
   * Protobuf enum {@code pms.ETransferType}
   */
  public enum ETransferType
      implements com.google.protobuf.Internal.EnumLite {
    /**
     * <code>TCONN = 0;</code>
     */
    TCONN(0),
    /**
     * <code>TTRANS = 1;</code>
     */
    TTRANS(1),
    /**
     * <code>TQUEUE = 2;</code>
     */
    TQUEUE(2),
    /**
     * <code>TDISPATCH = 3;</code>
     */
    TDISPATCH(3),
    /**
     * <code>TPUSH = 4;</code>
     */
    TPUSH(4),
    /**
     * <code>TLOGIN = 5;</code>
     */
    TLOGIN(5),
    /**
     * <code>TLOGOUT = 6;</code>
     */
    TLOGOUT(6),
    /**
     * <code>TWRITE_REQUEST = 7;</code>
     */
    TWRITE_REQUEST(7),
    /**
     * <code>TWRITE_RESPONSE = 8;</code>
     */
    TWRITE_RESPONSE(8),
    /**
     * <code>TREAD_REQUEST = 9;</code>
     */
    TREAD_REQUEST(9),
    /**
     * <code>TREAD_RESPONSE = 10;</code>
     */
    TREAD_RESPONSE(10),
    /**
     * <code>TERROR = 11;</code>
     */
    TERROR(11),
    UNRECOGNIZED(-1),
    ;

    /**
     * <code>TCONN = 0;</code>
     */
    public static final int TCONN_VALUE = 0;
    /**
     * <code>TTRANS = 1;</code>
     */
    public static final int TTRANS_VALUE = 1;
    /**
     * <code>TQUEUE = 2;</code>
     */
    public static final int TQUEUE_VALUE = 2;
    /**
     * <code>TDISPATCH = 3;</code>
     */
    public static final int TDISPATCH_VALUE = 3;
    /**
     * <code>TPUSH = 4;</code>
     */
    public static final int TPUSH_VALUE = 4;
    /**
     * <code>TLOGIN = 5;</code>
     */
    public static final int TLOGIN_VALUE = 5;
    /**
     * <code>TLOGOUT = 6;</code>
     */
    public static final int TLOGOUT_VALUE = 6;
    /**
     * <code>TWRITE_REQUEST = 7;</code>
     */
    public static final int TWRITE_REQUEST_VALUE = 7;
    /**
     * <code>TWRITE_RESPONSE = 8;</code>
     */
    public static final int TWRITE_RESPONSE_VALUE = 8;
    /**
     * <code>TREAD_REQUEST = 9;</code>
     */
    public static final int TREAD_REQUEST_VALUE = 9;
    /**
     * <code>TREAD_RESPONSE = 10;</code>
     */
    public static final int TREAD_RESPONSE_VALUE = 10;
    /**
     * <code>TERROR = 11;</code>
     */
    public static final int TERROR_VALUE = 11;


    public final int getNumber() {
      return value;
    }

    /**
     * @deprecated Use {@link #forNumber(int)} instead.
     */
    @java.lang.Deprecated
    public static ETransferType valueOf(int value) {
      return forNumber(value);
    }

    public static ETransferType forNumber(int value) {
      switch (value) {
        case 0: return TCONN;
        case 1: return TTRANS;
        case 2: return TQUEUE;
        case 3: return TDISPATCH;
        case 4: return TPUSH;
        case 5: return TLOGIN;
        case 6: return TLOGOUT;
        case 7: return TWRITE_REQUEST;
        case 8: return TWRITE_RESPONSE;
        case 9: return TREAD_REQUEST;
        case 10: return TREAD_RESPONSE;
        case 11: return TERROR;
        default: return null;
      }
    }

    public static com.google.protobuf.Internal.EnumLiteMap<ETransferType>
        internalGetValueMap() {
      return internalValueMap;
    }
    private static final com.google.protobuf.Internal.EnumLiteMap<
        ETransferType> internalValueMap =
          new com.google.protobuf.Internal.EnumLiteMap<ETransferType>() {
            public ETransferType findValueByNumber(int number) {
              return ETransferType.forNumber(number);
            }
          };

    private final int value;

    private ETransferType(int value) {
      this.value = value;
    }

    // @@protoc_insertion_point(enum_scope:pms.ETransferType)
  }

  /**
   * <pre>
   * the priority of transfering msg
   * default is normal
   * </pre>
   *
   * Protobuf enum {@code pms.ETransferPriority}
   */
  public enum ETransferPriority
      implements com.google.protobuf.Internal.EnumLite {
    /**
     * <code>PLOW = 0;</code>
     */
    PLOW(0),
    /**
     * <code>PNORMAL = 1;</code>
     */
    PNORMAL(1),
    /**
     * <code>PHIGH = 2;</code>
     */
    PHIGH(2),
    UNRECOGNIZED(-1),
    ;

    /**
     * <code>PLOW = 0;</code>
     */
    public static final int PLOW_VALUE = 0;
    /**
     * <code>PNORMAL = 1;</code>
     */
    public static final int PNORMAL_VALUE = 1;
    /**
     * <code>PHIGH = 2;</code>
     */
    public static final int PHIGH_VALUE = 2;


    public final int getNumber() {
      return value;
    }

    /**
     * @deprecated Use {@link #forNumber(int)} instead.
     */
    @java.lang.Deprecated
    public static ETransferPriority valueOf(int value) {
      return forNumber(value);
    }

    public static ETransferPriority forNumber(int value) {
      switch (value) {
        case 0: return PLOW;
        case 1: return PNORMAL;
        case 2: return PHIGH;
        default: return null;
      }
    }

    public static com.google.protobuf.Internal.EnumLiteMap<ETransferPriority>
        internalGetValueMap() {
      return internalValueMap;
    }
    private static final com.google.protobuf.Internal.EnumLiteMap<
        ETransferPriority> internalValueMap =
          new com.google.protobuf.Internal.EnumLiteMap<ETransferPriority>() {
            public ETransferPriority findValueByNumber(int number) {
              return ETransferPriority.forNumber(number);
            }
          };

    private final int value;

    private ETransferPriority(int value) {
      this.value = value;
    }

    // @@protoc_insertion_point(enum_scope:pms.ETransferPriority)
  }

  /**
   * <pre>
   * just the flag
   * </pre>
   *
   * Protobuf enum {@code pms.ETransferFlag}
   */
  public enum ETransferFlag
      implements com.google.protobuf.Internal.EnumLite {
    /**
     * <code>FNOACK = 0;</code>
     */
    FNOACK(0),
    /**
     * <code>FNEEDACK = 1;</code>
     */
    FNEEDACK(1),
    /**
     * <code>FACK = 2;</code>
     */
    FACK(2),
    UNRECOGNIZED(-1),
    ;

    /**
     * <code>FNOACK = 0;</code>
     */
    public static final int FNOACK_VALUE = 0;
    /**
     * <code>FNEEDACK = 1;</code>
     */
    public static final int FNEEDACK_VALUE = 1;
    /**
     * <code>FACK = 2;</code>
     */
    public static final int FACK_VALUE = 2;


    public final int getNumber() {
      return value;
    }

    /**
     * @deprecated Use {@link #forNumber(int)} instead.
     */
    @java.lang.Deprecated
    public static ETransferFlag valueOf(int value) {
      return forNumber(value);
    }

    public static ETransferFlag forNumber(int value) {
      switch (value) {
        case 0: return FNOACK;
        case 1: return FNEEDACK;
        case 2: return FACK;
        default: return null;
      }
    }

    public static com.google.protobuf.Internal.EnumLiteMap<ETransferFlag>
        internalGetValueMap() {
      return internalValueMap;
    }
    private static final com.google.protobuf.Internal.EnumLiteMap<
        ETransferFlag> internalValueMap =
          new com.google.protobuf.Internal.EnumLiteMap<ETransferFlag>() {
            public ETransferFlag findValueByNumber(int number) {
              return ETransferFlag.forNumber(number);
            }
          };

    private final int value;

    private ETransferFlag(int value) {
      this.value = value;
    }

    // @@protoc_insertion_point(enum_scope:pms.ETransferFlag)
  }

  /**
   * Protobuf enum {@code pms.ETransferTag}
   */
  public enum ETransferTag
      implements com.google.protobuf.Internal.EnumLite {
    /**
     * <code>SEQN = 0;</code>
     */
    SEQN(0),
    /**
     * <code>DATA = 1;</code>
     */
    DATA(1),
    UNRECOGNIZED(-1),
    ;

    /**
     * <code>SEQN = 0;</code>
     */
    public static final int SEQN_VALUE = 0;
    /**
     * <code>DATA = 1;</code>
     */
    public static final int DATA_VALUE = 1;


    public final int getNumber() {
      return value;
    }

    /**
     * @deprecated Use {@link #forNumber(int)} instead.
     */
    @java.lang.Deprecated
    public static ETransferTag valueOf(int value) {
      return forNumber(value);
    }

    public static ETransferTag forNumber(int value) {
      switch (value) {
        case 0: return SEQN;
        case 1: return DATA;
        default: return null;
      }
    }

    public static com.google.protobuf.Internal.EnumLiteMap<ETransferTag>
        internalGetValueMap() {
      return internalValueMap;
    }
    private static final com.google.protobuf.Internal.EnumLiteMap<
        ETransferTag> internalValueMap =
          new com.google.protobuf.Internal.EnumLiteMap<ETransferTag>() {
            public ETransferTag findValueByNumber(int number) {
              return ETransferTag.forNumber(number);
            }
          };

    private final int value;

    private ETransferTag(int value) {
      this.value = value;
    }

    // @@protoc_insertion_point(enum_scope:pms.ETransferTag)
  }

  /**
   * <pre>
   * the tag used for connecting of module
   * </pre>
   *
   * Protobuf enum {@code pms.EConnTag}
   */
  public enum EConnTag
      implements com.google.protobuf.Internal.EnumLite {
    /**
     * <code>THI = 0;</code>
     */
    THI(0),
    /**
     * <code>THELLO = 1;</code>
     */
    THELLO(1),
    /**
     * <code>THELLOHI = 2;</code>
     */
    THELLOHI(2),
    /**
     * <code>TKEEPALIVE = 3;</code>
     */
    TKEEPALIVE(3),
    UNRECOGNIZED(-1),
    ;

    /**
     * <code>THI = 0;</code>
     */
    public static final int THI_VALUE = 0;
    /**
     * <code>THELLO = 1;</code>
     */
    public static final int THELLO_VALUE = 1;
    /**
     * <code>THELLOHI = 2;</code>
     */
    public static final int THELLOHI_VALUE = 2;
    /**
     * <code>TKEEPALIVE = 3;</code>
     */
    public static final int TKEEPALIVE_VALUE = 3;


    public final int getNumber() {
      return value;
    }

    /**
     * @deprecated Use {@link #forNumber(int)} instead.
     */
    @java.lang.Deprecated
    public static EConnTag valueOf(int value) {
      return forNumber(value);
    }

    public static EConnTag forNumber(int value) {
      switch (value) {
        case 0: return THI;
        case 1: return THELLO;
        case 2: return THELLOHI;
        case 3: return TKEEPALIVE;
        default: return null;
      }
    }

    public static com.google.protobuf.Internal.EnumLiteMap<EConnTag>
        internalGetValueMap() {
      return internalValueMap;
    }
    private static final com.google.protobuf.Internal.EnumLiteMap<
        EConnTag> internalValueMap =
          new com.google.protobuf.Internal.EnumLiteMap<EConnTag>() {
            public EConnTag findValueByNumber(int number) {
              return EConnTag.forNumber(number);
            }
          };

    private final int value;

    private EConnTag(int value) {
      this.value = value;
    }

    // @@protoc_insertion_point(enum_scope:pms.EConnTag)
  }

  /**
   * <pre>
   * the type of connection
   * </pre>
   *
   * Protobuf enum {@code pms.EConnType}
   */
  public enum EConnType
      implements com.google.protobuf.Internal.EnumLite {
    /**
     * <code>TTCP = 0;</code>
     */
    TTCP(0),
    /**
     * <code>THTTP = 1;</code>
     */
    THTTP(1),
    UNRECOGNIZED(-1),
    ;

    /**
     * <code>TTCP = 0;</code>
     */
    public static final int TTCP_VALUE = 0;
    /**
     * <code>THTTP = 1;</code>
     */
    public static final int THTTP_VALUE = 1;


    public final int getNumber() {
      return value;
    }

    /**
     * @deprecated Use {@link #forNumber(int)} instead.
     */
    @java.lang.Deprecated
    public static EConnType valueOf(int value) {
      return forNumber(value);
    }

    public static EConnType forNumber(int value) {
      switch (value) {
        case 0: return TTCP;
        case 1: return THTTP;
        default: return null;
      }
    }

    public static com.google.protobuf.Internal.EnumLiteMap<EConnType>
        internalGetValueMap() {
      return internalValueMap;
    }
    private static final com.google.protobuf.Internal.EnumLiteMap<
        EConnType> internalValueMap =
          new com.google.protobuf.Internal.EnumLiteMap<EConnType>() {
            public EConnType findValueByNumber(int number) {
              return EConnType.forNumber(number);
            }
          };

    private final int value;

    private EConnType(int value) {
      this.value = value;
    }

    // @@protoc_insertion_point(enum_scope:pms.EConnType)
  }


  static {
  }

  // @@protoc_insertion_point(outer_class_scope)
}
