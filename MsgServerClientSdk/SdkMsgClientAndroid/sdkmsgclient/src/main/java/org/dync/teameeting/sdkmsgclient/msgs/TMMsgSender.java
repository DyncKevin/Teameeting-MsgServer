package org.dync.teameeting.sdkmsgclient.msgs;

import android.content.Context;

import org.dync.teameeting.sdkmsgclient.MeetMsgType;
import org.dync.teameeting.sdkmsgclient.jni.JMClientHelper;


/**
 * Created by hp on 12/24/15.
 */

/**
 * TMMsgSender is used to send msg to other
 **/

public class TMMsgSender extends MsgClient {

    private String TAG = "TMMsgSender";

    /**
     * constructor for TMMsgSender
     *
     * params:
     *      context:the context
     *      helper:the JMClientHelper
     **/
    public TMMsgSender(Context context, JMClientHelper helper) {
        super(context, helper);
    }

    /**
     * the status of connection between msgclient and msgserver
     **/
    public int TMConnStatus() {
        return this.MCConnStatus();
    }

    /**
     * init
     *
     * params:
     *      strUid:user identifier, it can be uuid or device id
     *      strToken:token from http server
     *      strNname:nick name
     *      strServer:server ip
     *      nPort:server port
     **/
    public int TMInit(String strUid, String strToken, String strNname, String strServer, int nPort) {
        return this.MCInit(strUid, strToken, strNname, strServer, nPort);
    }

    /**
     * unin
     **/
    public int TMUnin() {
        return this.MCUnin();
    }

    /**
     * send msg to all member in the meeting room
     * params:
     *      strRoomid:the room id
     *      strRname:the room name
     *      strMsg:the msg to send
     **/
    public int TMSndMsg(String strRoomid, String strRname, String strMsg) {
        return this.MCSndMsg(strRoomid, strRname, strMsg);
    }

    /**
     * get msg from server
     * params:
     *      tag:
     **/
    public int TMGetMsg(MeetMsgType.EMsgTag tag) {
        return this.MCGetMsg(tag.getNumber());
    }

    /**
     * operation for room
     * params:
     *      tag:the tag for this msg, enter or leave
     *      strRoomid:the room id
     *      strRname:the room name
     *      strRemain:remain
     **/
    public int TMOptRoom(MeetMsgType.EMsgTag tag, String strRoomid, String strRname,  String strRemain) {
        return this.MCOptRoom(tag.getNumber(), strRoomid, strRname, strRemain);
    }

    /**
     * send msg to some member in meeting room(not used now)
     * params:
     *      strRoomid:the room id
     *      strRname:the room name
     *      strMsg:the msg to send
     *      arrUser:the members to send to
     **/
    public int TMSndMsgTo(String strRoomid, String strRname, String strMsg, String[] arrUser) {
        return  this.MCSndMsgTo(strRoomid, strRname, strMsg, arrUser);
    }

    /**
     * notify others with self publish id
     * params:
     *      strRoomid:the room id
     *      strRname:the room name
     *      tag: the tag of this msg
     *      strMsg:the publish id
     **/
    public int TMNotifyMsg(String strRoomid, String strRname, MeetMsgType.EMsgTag tag, String strMsg) {
        return this.MCNotifyMsg(strRoomid, strRname, tag.getNumber(), strMsg);
    }

    /**
     * set user nick name
     * params:
     *      strNname:nick name
     **/
    public void TMSetNickName(String strNname) {
        this.MCSetNickName(strNname);
    }
}
