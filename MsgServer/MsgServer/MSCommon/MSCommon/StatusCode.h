//
//  StatusCode.h
//  dyncRTConnector
//
//  Created by hp on 1/3/16.
//  Copyright © 2016 Dync. All rights reserved.
//

#ifndef StatusCode_h
#define StatusCode_h

#define MAX_MESSAGE_EXPIRE_TIME (7*24*60*60)

#define Err_Redis_Ok                         (0)
#define Err_Redis_Key_Not_Exist              (-1)
#define Err_Redis_Key_Exist                  (-2)
#define Err_Redis_Key_Expire                 (-3)
#define Err_Redis_Key_Expire_Or_Not_Exist    (-4)
#define Err_Vsersion_Not_Support             (-5)
#define Err_Redis_Setex                      (-6)
#define Err_Redis_Hmset                      (-7)
#define Err_Redis_Type                       (-8)

#define MAX_SOCKET_BUF_64 (1024*1024*64)
#define MAX_SOCKET_BUF_32 (1024*1024*32)

#define PACKED_MSG_ONCE_NUM (1)

#define REDIS_GROUP_CLIENT (1)
#define REDIS_GROUP_CLIENT_MAX (REDIS_GROUP_CLIENT*10007)

//#define REQUEST_BUFFER_SIZE_IN_BYTES_32    (1024*1024*32)
#define REQUEST_BUFFER_SIZE_IN_BYTES_32    (1024*8)

#define USE_QUEUE_TO_CACHE (1)

#define ERR_CODE_SVR_NOT_EXISTS (1)

#define ERR_STR_SVR_NOT_EXISTS "server_not_exists"


#endif /* StatusCode_h */
