#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"
#include "fs.h"




char *source_path,*destination_path;
int recursively=0;

void copy_file(char* dst,char *src){

    int fdr,fdw;

    if ( (fdr=open(src,O_RDONLY)) < 0 ){
        printf(1,"returned read %s -> %s\n",src,dst);
        return;
    }
    if ( (fdw=open(dst,O_CREATE|O_RDWR)) < 0 ){
        printf(1,"returned write %s -> %s\n",src,dst);
        return;
    }
    int buff_len;
    char buffer[128];
    while( ( buff_len=read(fdr,buffer,sizeof(buffer)) )> 0  ){
        write(fdw,buffer,buff_len);
    }
    close(fdr);
    close(fdw);
    printf(2,"copy %s to %s success\n",src,dst);
}
//asd
void _strcat(char *dst,char *src){
    int j=strlen(dst);
    for(int i=0;i<strlen(src);++i,j++){
        dst[j]=src[i];
    }
    dst[j]='\0';
return;
}

char *parent_dir;

void
traverse_dir(char *path){
    if(chdir(path)<0){
        printf(2,"cd: cannot open %s\n",path);
    }
    char *new_path=malloc(256*sizeof(char));
    char *new_src=malloc(256*sizeof(char));//[256];
    char *new_dst=malloc(256*sizeof(char));//[256];

    char buf[512], *p;
    int fd;
    struct dirent de;
    struct stat st;

    if((fd = open(".", 0)) < 0){
        printf(2, "cd: cannot open %s\n", path);
        return;
    }

    if(fstat(fd, &st) < 0){
        printf(2, "cd: cannot stat %s\n", path);
        close(fd);
        return;
    }

    switch(st.type){

    case T_FILE:
        break;

    case T_DIR:

    if(2 + DIRSIZ + 1 > sizeof buf){
        printf(1, "cd: path too long\n");
      break;
    }
    strcpy(buf, ".");
    p = buf+strlen(buf);
    *p++ = '/';
    while(read(fd, &de, sizeof(de)) == sizeof(de)){
        if(de.inum == 0)continue;

        memmove(p, de.name, DIRSIZ);
        p[DIRSIZ] = 0;
        if(stat(buf, &st) < 0){
            printf(1, "cd: cannot stat %s\n", buf);
            continue;
        }
        char *obj_name=buf+1;//karna buf = './namafile'
        if(!strcmp(obj_name,"/.") || !strcmp(obj_name,"/.."))continue;
        if(st.type == 1){
            if(!recursively){
                printf(1,"cp: omitting directory '%s'",obj_name+1);
                    continue;
            }
            *new_path='\0';
            ++obj_name;
            _strcat(new_path,destination_path);
            _strcat(new_path,"/");
            _strcat(new_path,path);
            _strcat(new_path,"/");
            _strcat(new_path,obj_name);

            mkdir(new_path);


            *new_path='\0';
            _strcat(new_path,path);
            _strcat(new_path,obj_name);

            char *temp_pdir=malloc(256*sizeof(char));
            strcpy(temp_pdir,parent_dir);
            _strcat(parent_dir,obj_name);//pdir->d_name);
            _strcat(parent_dir,"/");
            traverse_dir(obj_name);
            strcpy(parent_dir,temp_pdir);
        }
        else{
            *new_dst='\0';
            *new_src='\0';

            _strcat(new_src,obj_name);

            _strcat(new_dst,destination_path);
            _strcat(new_dst,parent_dir);
            _strcat(new_dst,new_src);
            //printf(2,"copy file %d %s from %d %s\n",strlen(obj_name),new_dst,strlen(obj_name),obj_name);
            copy_file(new_dst,obj_name+1);
        }

    }
    break;
  }
  close(fd);
  chdir("..");
}

char* getdir_name(char *path){
    int i=strlen(path)-2;// karna bisa jadi case dir1/dir2/ atau dir1/dir2
    for(; i > -1 && path[i]!='/';--i);
    if(path[0]=='/')
        return path+i+1;
    return path;
}

int main(int argc, char **argv){
    parent_dir=malloc(128*sizeof(char));
    source_path=malloc(128*sizeof(char));
    destination_path=malloc(128*sizeof(char));

    if(argc==1){
        printf(1,"cp: missing file operand\nTry 'cp --help' for more information.\n");
        exit();
    }
    else if(argc==2){
        printf(1,"cp: missing destination file operand after '%s'\nTry 'cp --help' for more information.\n",argv[1]);
        exit();
    }
    for(int i=1;i<argc;++i){
        if(!strcmp(argv[i],"-R")){
            recursively=1;
            continue;
        }
        else if( strlen(source_path)==0 ){
            strcpy(source_path,argv[i]);
        }
        else{
            strcpy(destination_path,argv[i]);
        }
    }
    if(!strcmp(source_path,"*")){
        printf(1,"enter 1\n");
        strcpy(parent_dir,"/");
        traverse_dir(".");
    }
    else if(recursively){//s

        source_path=getdir_name(source_path);
//        printf(2,"hasil %s\n",source_path);
        if(destination_path[ strlen(destination_path)-1 ]!='/'){_strcat(destination_path,"/");}
        if(source_path[ strlen(source_path)-1 ]!='/'){_strcat(source_path,"/");}

        char *new_path=malloc(256*sizeof(char));

        strcpy(parent_dir,source_path);

        _strcat(new_path,destination_path);
        _strcat(new_path,source_path);
        mkdir(new_path);
        traverse_dir(parent_dir);
    }
    else{
        int fd;
        if((fd=open(destination_path,O_RDWR|O_CREATE))<0){
            _strcat(destination_path,"/");
            _strcat(destination_path,source_path);
        }
        copy_file(destination_path,source_path);
    }


exit();
}
