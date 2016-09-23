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


#endif /* StatusCode_h */
