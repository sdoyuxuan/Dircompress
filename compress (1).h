#include "Heap.h"
#include "huffman.h"

struct CharInfo
{
    CharInfo()
    :_count(0)
    {}
    CharInfo(const CharInfo & _chin)
    :_count(_chin._count),code(_chin.code),_ch(_chin._ch)
    {}
    bool operator<(const CharInfo&_chin)
    {
         return _count < _chin._count;
    }
    bool operator<(const CharInfo&_chin)const//for const object  
    {
         return _count < _chin._count;
    }
    bool operator>(const CharInfo&_chin)
    {
         return _count > _chin._count;
    }
    bool operator>(const CharInfo&_chin)const//for const object  
    {
         return _count > _chin._count;
    }
    bool operator!=(const CharInfo & _chin) 
    {
        return _count!=_chin._count;
    }
    bool operator!=(const CharInfo & _chin)const
    {
        return _count!=_chin._count;
    }
   CharInfo operator+(const CharInfo&)const;
   size_t _count;
   std::string code;
   unsigned char _ch;
};
 CharInfo CharInfo::operator + (const CharInfo& _chin)const 
{
        CharInfo temp(*this);
        temp._count+=_chin._count;
        return temp;
} // const hen zhong yao 
class Filecompress
{
    protected:
    using Node = HuffmanTreeNode<CharInfo>; 
    public:
        Filecompress()
            :_FileName()
        {
              for(int i=0 ; i<256 ; ++i)
              {
                  _charinfo[i]._ch=i;
              }
        }
        void FindFile(std::string Path)
        {
            
            auto pos = Path.find_last_of('/');
            //string Name = Path.substr(pos);
            struct stat buf;
            if(stat(Path.c_str(),&buf)<0)
            {
                cerr<<strerror(errno)<<endl;
                return ;
            }
            if(S_ISDIR(buf.st_mode))
            {
                 DIR *dp = nullptr;
                 struct dirent * dirp = nullptr;
                 if((dp=opendir(Path.c_str()))==NULL)
                 {
                     cerr<<"cant open "<<Path;
                     exit(1);
                 }
                 else 
                 {
    
                     while((dirp=readdir(dp))!=NULL)
                     {
                        string ChildPath(Path);
                        if((strcmp(dirp->d_name,".")==0)||(strcmp(dirp->d_name,".."))==0)
                            continue;
                        if(Path.back()!='/')
                            ChildPath+='/';// for argv ../  FindFile result ..// 
                        ChildPath=ChildPath+dirp->d_name;
                        if(stat(ChildPath.c_str(),&buf)<0)
                        {
                            cerr<<strerror(errno)<<endl;
                            return;
                        }
                        else 
                        {
                            if(S_ISDIR(buf.st_mode))
                            {
                                FindFile(ChildPath);                                                                
                            }
                            else 
                            {
                                _FileName.push_back(ChildPath); 
                            }
                        }
                         
                     }
                 }
                 closedir(dp);
            }
            else 
            {
                _FileName.push_back(Path);    
            }
        }
        void compress(std::string Path)
        {
            umask(0);
            FindFile(Path);
            Path=Path+".yutian";
            int fd=open(Path.c_str(),O_RDWR|O_CREAT|O_TRUNC,0664);
            for(auto & i :_FileName)
            {
                _compress(i.c_str(),fd);
                char buf[]="\n\n";
                write(fd,buf,sizeof(buf)-1);
            }
            close(fd);
        }// fd for .yutian 
        void _compress(std::string Path, int YtFd)
        {
            umask(0);
            int CpFd=0;
            if((CpFd=open(Path.c_str(),O_RDWR,664))<0)
            {
                cout<<strerror(errno)<<endl;
                exit(3);
            }
            std::string Name;
            Name = Path + '\n';
            for(int i=0 ; i < 256 ; ++i)
            {
                 _charinfo[i]._count=0;
                 _charinfo[i]._ch=i;
                 _charinfo[i].code.clear();
            }
            unsigned char buff[1024];
            vector<unsigned char> ComPreObj;
            int  readsz=0;
            while((readsz=read(CpFd,buff,sizeof(buff)-1))>0)
            {
                buff[readsz]=0;
                unsigned char i = 0;
                 for(int j=0;j<readsz;++j)
                {
                     i = buff[j];
                     _charinfo[i]._count++;
                     _charinfo[i]._ch=i;
                     ComPreObj.push_back(i);
                }
            }
            if(readsz==-1)
            {
                cout<<strerror(errno)<<endl;
                exit(2);
            }
            HuffmanTree<CharInfo> HuffmanObject(_charinfo,CharInfo());
            Node * HfRoot=HuffmanObject.GetRoot();
            write(YtFd,Name.c_str(),Name.size());
            if(HfRoot == nullptr)
                return; // kong wen jia 
            GetLeefCode(HfRoot,YtFd,ComPreObj.size());
            if(HfRoot->_pLeft==nullptr&&HfRoot->_pRight==nullptr)
                return; // special bu yong jie ya
            std::string CodeByte;
            int number = 0;
            unsigned char output = 0;
            for(auto & i : ComPreObj)
            {
                CodeByte.append(_charinfo[i].code);
                if(CodeByte.size()>=8)
                {
                    number = CodeByte.size()/8;
                    output = 0;
                    while(number--)
                    {
                         output = 0;   //这里如果不清零当多次迭代必出错
                         for(int i=7 ; i>=0 ;--i)
                         {
                             output|=((CodeByte[7-i]-48)<<i);
                         }
                         write(YtFd,&output,1);
                         CodeByte.erase(0,8);
                    }
                }
            }
            output = 0;
            number = CodeByte.size()-1;// number --> string[seq]  number <--> seq
            if(number>=0)
            {
                unsigned char bit = 7;
                for(int i=number; i>=0 ; --i)
                {
                    output|=((CodeByte[number-i]-48)<<bit);
                    bit--;
                }
                write(YtFd,&output,1);
            }
            close(CpFd);
        }
        void uncompress(std::string Path)
        { 
            for(int i=0 ; i < 256 ; ++i)
            {
                 _charinfo[i]._count=0;
                 _charinfo[i]._ch=i;
                 _charinfo[i].code.clear();
            }
            int CmFd = 0;
            if((CmFd=open(Path.c_str(),O_RDWR))<0)
            {
                cout<<strerror(errno)<<endl;
                exit(5);
            }
            char buff[1024];
            memset(buff,0,1024);
            std::string FileName;
            int readsz=0;
            while((readsz=ReadLine(CmFd,buff))>0)
            {
                   buff[readsz]=0;
                   FileName=buff;
                   int UnCmfd=CreateFile(FileName);
                   _uncompress(UnCmfd,CmFd);
                  /* jieyahou haixu , chu li \n\n*/  
                   ReadLine(CmFd,buff);
                   ReadLine(CmFd,buff);
                   close(UnCmfd);
            }
            if(readsz==0)
            {
                cout<<"uncompress done!!!"<<endl;
                close(CmFd);
            }
            else 
            {
                cout<<strerror(errno)<<endl;    
            }
        }
        void _uncompress(int UnCmfd,int CmFd)
        {
            for(int i=0 ; i < 256 ; ++i)
            {
                 _charinfo[i]._count=0;
                 _charinfo[i]._ch=i;
                 _charinfo[i].code.clear();
            }
            char buff[1024];
            memset(buff,0,sizeof(buff));
            int readsz=0;
            std::string CodeInfm;
         /*   if((readsz=ReadLine(CmFd,buff))>0)
            {
                buff[readsz]=0;
                CodeInfm=buff;
                if(CodeInfm.find_first_of(',')==string::npos)
                {
                    long long  size = stoll(CodeInfm,nullptr,10);
                    specialumcompress(UnCmfd,CmFd,size);
                }
            }*/     // special  size == -1 
            unsigned int pos = 0;
            long long size = 0;
            while((readsz=ReadLine(CmFd,buff))>0)
            {
                if(buff[0]=='\n')
                {
                   readsz=ReadLine(CmFd,buff);
                }
                if(buff[0]=='\n')
                {
                     close(UnCmfd);// for empty size file
                     return;
                }
                buff[readsz]=0;
                CodeInfm=buff;
                if((pos=CodeInfm.find_last_of(','))==string::npos)
                {
                      size = stoll(CodeInfm,nullptr,10);// -1 --> special 
                      break;
                }
                else 
                {
                      unsigned char ch = 0;
                      if(pos!=0)
                            ch = CodeInfm[pos-1];// chu li \n
                      else 
                            ch = '\n';
                      _charinfo[ch]._ch=ch;
                      _charinfo[ch]._count=stoul(CodeInfm.substr(pos+1),nullptr,10);
                }
            }
            if(size==-1)
                specialumcompress(CmFd,UnCmfd);
            HuffmanTree<CharInfo> HuffmanObject(_charinfo,CharInfo());
            Node * const HfRoot = HuffmanObject.GetRoot();
            GetLeefCodeforUncm(HfRoot);
            unsigned char data = 0;
            unsigned char  where = 0;
            unsigned char  bytepos = 0x80;
            unsigned char  i = 0;
            Node * pCur = HfRoot;
            read(CmFd,&data,1);
            while(size>0)
            {
                if(i==8)
                {
                    read(CmFd,&data,1);
                    i=0;
                    bytepos = 0x80;
                } 
                while((pCur->_pLeft!=nullptr||pCur->_pRight!=nullptr)&&i<8)// 这个括号所比加的，因为&& 比||优先级高，所以不加的话，如果pCur不为
                {                                                         // 叶子节点，i 》8之后还一直迭代 就会出错了。
                    where = data&(bytepos>>i++);
                    if(where==0)
                        pCur=pCur->_pLeft;
                    else 
                        pCur=pCur->_pRight;
                }
                if(pCur->_pLeft == nullptr && pCur->_pRight == nullptr)
                {
                    write(UnCmfd,&pCur->_weight._ch,1);
                    size--;
                    pCur = HfRoot;
                }
            }
         }
    private:
       int  CreateFile(std::string Path)
      {               
         umask(0);
         int pos=0;
         int end=Path.size();
         string file;
         char StartPath[512];
         if(getcwd(StartPath,512)==NULL)    // a/b/c  a wufa chuangjian  bug... dei chuli 
         {
             cout<<strerror(errno)<<endl;
         }
         if((pos=Path.find_first_of('/',pos))!=string::npos)
         {
             while(pos<end)
             {
                 pos++;
                 while(pos<end&&Path[pos]!='/')
                 {
                     file=file+Path[pos++];
                 }
                 if(pos<end)
                 {
                     mkdir(file.c_str(),0775);
                     file="./"+file;
                     chdir(file.c_str());
                     file.clear();
                 }
                 if(pos==end)
                 {
                     int ret=0;
                     file.erase(file.end()-1);
                     if((ret=open(file.c_str(),O_EXCL|O_CREAT|O_RDWR,0664))<0)
                         {
                              perror("open");
                              exit(5); 
                         }
                    else 
                    {
                          chdir(StartPath); // 必须保存初始化目录，在创建子文件后就把进程当前目录的属性改变成初始目录
                          return ret;    //否则会出错，现象就是创建多余的文件件
                    }
                 }
             }
          }
       }
        ssize_t ReadLine(int fd , char * buff)
        {
            int sz=0; 
            int ret=0;
            int pos=0;
            while((sz=read(fd,buff+pos,1))>0)
            {
                 ret++;
                 if(buff[pos++]=='\n')
                     return ret;
            }
            if(sz==0)
                return 0;
            else 
            {
                cout<<strerror(errno)<<endl;
                exit(4);
            }
        }
        void special(Node*pRoot,int YtFd)
        {
             string output;
             unsigned char ch = pRoot->_weight._ch;             
             output += ch;
             output += ',';
             output += to_string(_charinfo[ch]._count);
             output += '\n';
             write(YtFd,output.c_str(),output.size());
             char bufsize[] ="-1\n"; // for special , eg : AAAAAAAAAA....  huffmantree only one Node ,So cant compress 
             write(YtFd,bufsize,sizeof(bufsize));
        }
        void specialumcompress(int UnCmfd , int Cmfd )
        {
            
        }
        void GetLeefCode(Node*pRoot,int YtFd , unsigned int size)
        {
             assert(pRoot!=nullptr);
             if(pRoot->_pLeft==nullptr&&pRoot->_pRight)
             {
                 special(pRoot,YtFd);
             }
             stack<Node*> scon;
             scon.push(pRoot);// stack<Node*> scon;
             while(!scon.empty())
             {
                 Node*pCur=scon.top();
                 scon.pop();
                 if(pCur->_pLeft==nullptr&&pCur->_pRight==nullptr)
                 {
                      Getcode(pCur,YtFd);
                 }
                 else 
                 {
                     if(pCur->_pRight)
                         scon.push(pCur->_pRight);
                     if(pCur->_pLeft)
                         scon.push(pCur->_pLeft);
                 }
             }
             string ComByteLenth = to_string(size);
             ComByteLenth += '\n';
             write(YtFd,ComByteLenth.c_str(),ComByteLenth.size());
        }
        
        void Getcode(Node*pCur,int YtFd)
        {
            assert(pCur!=nullptr);
            unsigned char ch = pCur->_weight._ch;
            string output;
            string Hfcode;
            while(pCur->_pParent)
            {
                if(pCur->_pParent->_pLeft==pCur)
                {
                    Hfcode+='0';
                } 
                else 
                {
                    Hfcode+='1';
                }
                pCur=pCur->_pParent;
            }
            std::reverse(Hfcode.begin(),Hfcode.end());
            _charinfo[ch].code = Hfcode;
            output +=ch;
            output +=',';
            output += to_string(_charinfo[ch]._count);
            output += '\n'; 
            write(YtFd,output.c_str(),output.size());
        }
        void GetLeefCodeforUncm(Node*pRoot)
        {
             assert(pRoot!=nullptr);
             stack<Node*> scon;
             scon.push(pRoot);// stack<Node*> scon;
             while(!scon.empty())
             {
                 Node*pCur=scon.top();
                 scon.pop();
                 if(pCur->_pLeft==nullptr&&pCur->_pRight==nullptr)
                 {
                      GetcodeforUncm(pCur);
                 }
                 else 
                 {
                     if(pCur->_pRight)
                         scon.push(pCur->_pRight);
                     if(pCur->_pLeft)
                         scon.push(pCur->_pLeft);
                 }
             }
        }
        
        void GetcodeforUncm(Node*pCur)
        {
            assert(pCur!=nullptr);
            unsigned char ch = pCur->_weight._ch;
            string Hfcode;
            while(pCur->_pParent)
            {
                if(pCur->_pParent->_pLeft==pCur)
                {
                    Hfcode+='0';
                } 
                else 
                {
                    Hfcode+='1';
                }
                pCur=pCur->_pParent;
            }
            std::reverse(Hfcode.begin(),Hfcode.end());
            _charinfo[ch].code = Hfcode;
        }
    public:
        void TestVector()
        {
            for(auto&i:_FileName)
            {
                cout<<i<<endl;
            }
        }
    private:
        CharInfo _charinfo[256];
        vector<string> _FileName;
};

