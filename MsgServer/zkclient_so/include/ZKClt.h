#ifndef __ZK_CLT_H__
#define __ZK_CLT_H__

#include <string>

class ZKClt{
    typedef void (*NodeCallback)(const std::string& path);

    public:
        ZKClt();
        ~ZKClt();

    public:
        bool Init(const std::string& config);
        bool Unin();

        void SetCallback(NodeCallback addCb, NodeCallback delCb);
        bool DoUpload();

    private:
        std::string         mNodePath;
        std::string         mConfPath;

};


#endif // __ZK_CLT_H__
