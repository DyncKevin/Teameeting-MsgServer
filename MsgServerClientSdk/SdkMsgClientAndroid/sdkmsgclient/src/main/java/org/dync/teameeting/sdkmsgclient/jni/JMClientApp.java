package org.dync.teameeting.sdkmsgclient.jni;


/**
 * Created by hp on 12/24/15.
 */
public class JMClientApp {
    /**
     * 构造访问jni底层库的对象
     */

    private final long fNativeAppId;

    public JMClientApp(JMClientHelper helper) {
        fNativeAppId = Create(helper);
    }

    private static native long Create(JMClientHelper helper);


    public native int Init(String strUid, String strToken, String strName, int devType, int push, int module);
    public native int Unin();

    public native int RegisterMsgCb();
    public native int UnRegisterMsgCb();

    public native int ConnToServer(String strServer, int port);

    public native int AddGroup(String strGroupId);
    public native int RmvGroup(String strGroupId);


    public native String SndMsg(String strGroupId, String strGroupName, String strMsg, int tag, int type, int module, int flag, int push);
    public native String SndMsgTo(String strGroupId, String strGroupName, String strMsg, int tag, int type, int module, int flag, int push, String[] arrUser, int arrSize);

    public native int FetchSeqn();
    public native int SyncSeqn(long seqn, int role);
    public native int SyncData(long seqn);
    public native int FetchGroupSeqn(String strGroupId);
    public native int SyncGroupSeqn(String strGroupId, long seqn, int role);
    public native int SyncGroupData(String strGroupId, long seqn);

    public native int ConnStatus();
    public native void SetUserId(String strUserid);
    public native void SetToken(String strToken);
    public native void SetNickName(String strNname);
    public native void SetUIconUrl(String strUicon);
    public native void EnablePush(int push);
    public native void MuteNotify(int mute);
    public native void UpdateSetting(long setType, String jsonSetting);
    public native void SetUUID(String strUUID);

    public native void InitUserSeqns(String strSeqnid, long seqn);
    public native void UpdateMaxSeqns(String strSeqnid, long seqn);
    public native void AddGroupInCore(String strSeqnid, long seqn);
    public native void RemoveGroupInCore(String strSeqnid);

    /**
     * 销毁APP
     */
    public native void Destroy();


}
