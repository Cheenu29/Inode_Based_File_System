#include<stdio.h>
//#include<conio.h>
#include <fcntl.h>
#include <string.h>
#include <bits/stdc++.h>
#include <iostream>
#include<string>
#include <unistd.h>
#include <cstring>
#include <stdlib.h>
#include <unistd.h>
#include <string>
#include<dirent.h>
#include <sstream>

using namespace std;

// disk size=512MB
int blockSize=16777216;          // block size = 16 MB = 16777216 B 
int diskBlocks=32;               // number of blocks = 512MB / 16MB   = 32






/*
class FileDetails{

public:
string fileName;
long long fileSize;

    public:
    FileDetails()
    {
      fileName="";
      fileSize=0;
    }
    
};
*/

class Superblock{

public:
int* freeBlock;
string* fileName;
long long* fileSize;

int* rmode;
int* wmode;
int* amode;

   public:
  Superblock()
  {
    
    freeBlock=new int[diskBlocks];
    fileName=new string[diskBlocks];
    fileSize=new long long[diskBlocks];
           
    rmode=new int[diskBlocks];
    wmode=new int[diskBlocks];
    amode=new int[diskBlocks];
    
    
    for(int j=0;j<diskBlocks;j++){
        freeBlock[j]=1;
        fileName[j]="-";
        fileSize[j]=0;
        
        rmode[j]=0;
        wmode[j]=0;
        amode[j]=0;
        
    }
        
  }
};

map<string,Superblock*> diskMap;


long long getStartPoint(int index)
{
   return (long long)index*(long long)blockSize;
}

void createDisk(string name)
{
    int n=name.length();
    char arr[n+1];
    strcpy(arr,name.c_str());
    
 
    
    
   if(access(arr,F_OK)!=-1)
    {
        cout<<"Disk exists with this name\n";
        return;
    }
    
    FILE *ptr=fopen(arr,"wb");
     
  //   cout<<"idhr\n";
     char *store;
     store=new char[blockSize];
     memset(store,0,blockSize);
  //  cout<<"yaha\n";
    int i=0;
    while(i<diskBlocks){
       fwrite(store,1,blockSize,ptr);
       i++;
    } 
    
    Superblock* t=new Superblock();
    diskMap.insert({name,t});        
    
 
}

void createFile(string name,string fileName)
{
     Superblock* temp=diskMap[name];
     
     int space=-1;
     for(int i=1;i<diskBlocks;i++)
     {
          // cout<<i<<" ---> "<<temp->freeBlock[i]<<" "<<temp->fileName[i]<<"\n";
          if(temp->freeBlock[i]==0 && temp->fileName[i]==fileName)
          {
             printf("File Already Exists\n");
             return;
          }
          if(temp->freeBlock[i]==1)
          space=i;
     }
     if(space==-1)
     {
         printf("No more space exists in the disk\n");
         return;
     }
     
     temp->freeBlock[space]=0;
     temp->fileName[space]=fileName;
     temp->fileSize[space]=0;
     printf("File succesfully created\n");
     
}

void openFile(string name,string fileName,string mode)
{

     Superblock* temp=diskMap[name];
     
     int space=-1;
     for(int i=1;i<diskBlocks;i++)
     {
          // cout<<i<<" ---> "<<temp->freeBlock[i]<<" "<<temp->fileName[i]<<"\n";
          if(temp->freeBlock[i]==0 && temp->fileName[i]==fileName)
          {
             if(mode=="read")
             {
                 temp->rmode[i]=1;
             }
             else if(mode=="write"){
                  temp->wmode[i]=1;
             }
             else {
                  temp->amode[i]=1;
             }
             
             printf("File is opened in desired mode and file descriptor = %d \n",i);
             return;
          }
          
     }
     printf("File does not exist\n");
}

void readFile(string name,int i)
{
      if(i<0 || i>=diskBlocks)
    {
       printf("Invalid Descriptor\n");
    }
      Superblock* temp=diskMap[name];
     
     int space=-1;
     
          // cout<<i<<" ---> "<<temp->freeBlock[i]<<" "<<temp->fileName[i]<<"\n";
          if(temp->freeBlock[i]==0 && temp->fileName[i]!="-")
          {
           //  Superblock* temp=diskMap[temp->fileName[i]];
             if(temp->rmode[i]==0)
             {
                 printf("read permisssion not found\n");
                 return;
             }
             else
             { 
                string ans="";
                long long startPoint=getStartPoint(i);
                long long endPoint=startPoint+temp->fileSize[i];
                int n=name.length();
    		char arr[n+1];
    		strcpy(arr,name.c_str());
                FILE *fp = fopen(arr, "r");
                char ch;
                long long j=0;
                while ((ch = fgetc(fp)) != EOF && j<endPoint){
                   if(j>=startPoint)
                   putchar(ch);
                   j++;
                 }
                 printf("\n");
                 return;   
             }
          }
          else
            printf("File does not exist \n");
}

void writeFileActual(int position, char *data,string name)
{
    int n=name.length();
    char arr[n+1];
    strcpy(arr,name.c_str());
    FILE *file = fopen(arr,"r+");
    fseek(file,position,SEEK_SET);
    fwrite(data, strlen(data), 1, file);
    fclose(file);
}


void writeFile(string name,int i,string content)
{
      if(i<0 || i>=diskBlocks)
    {
       printf("Invalid Descriptor\n");
    }
      Superblock* temp=diskMap[name];
     
    
     
          // cout<<i<<" ---> "<<temp->freeBlock[i]<<" "<<temp->fileName[i]<<"\n";
          if(temp->freeBlock[i]==0 && temp->fileName[i]!="-")
          {
             if(temp->wmode[i]==0)
             {
                 printf("write permisssion not found\n");
                 return;
             }
             else
             { 
               // string ans="";
                long long startPoint=getStartPoint(i);
                long long endPoint=startPoint+temp->fileSize[i];
                int n=content.length();
    		 char arr[n+1];
   		 strcpy(arr,content.c_str());
                
                writeFileActual(startPoint,arr,name);
                
                
                temp->fileSize[i]=content.length();
                
                 printf("Message written into file \n");
                 return;   
             }
          }
          
     
     printf("File does not exist\n");
}


void appendFile(string name,int i,string content)
{
      if(i<0 || i>=diskBlocks)
    {
       printf("Invalid Descriptor\n");
    }
      Superblock* temp=diskMap[name];
     
    
          // cout<<i<<" ---> "<<temp->freeBlock[i]<<" "<<temp->fileName[i]<<"\n";
          if(temp->freeBlock[i]==0 && temp->fileName[i]!="")
          {
             if(temp->amode[i]==0)
             {
                 printf("append permisssion not found\n");
                 return;
             }
             else
             { 
               // string ans="";
                long long startPoint=getStartPoint(i)+temp->fileSize[i];
                long long endPoint=startPoint+temp->fileSize[i];
                int n=content.length();
    		 char arr[n+1];
   		 strcpy(arr,content.c_str());
                
                writeFileActual(startPoint,arr,name);
                
                
                temp->fileSize[i]+=content.length();
                
                 printf("Message appended into file \n");
                 return;   
             }
          }
          
     
     printf("File does not exist\n");
}

void closeFile(string name,int i)
{
    if(i<0 || i>=diskBlocks)
    {
       printf("Invalid Descriptor\n");
    }
    
    Superblock* temp=diskMap[name];
     if(temp->freeBlock[i]==0 && temp->fileName[i]!="-"){
    temp->rmode[i]=0;
    temp->wmode[i]=0;
    temp->amode[i]=0;
    cout<<"File is closed\n";
    }
    else
    printf("file does not exist\n");
}


void listFiles(string name)
{
     Superblock* temp=diskMap[name];
     for(int i=1;i<diskBlocks;i++)
     {
          if(temp->freeBlock[i]==0 && temp->fileName[i]!="-")
          cout<<"File Descriptor is : "<<i<<" ---> "<<"Name of file is : "<<temp->fileName[i]<<" ---> size of file in bytes : "<<temp->fileSize[i]<<"\n";
          
     }
}


void openedFiles(string name)
{
     Superblock* temp=diskMap[name];
     for(int i=1;i<diskBlocks;i++)
     {
          if(temp->freeBlock[i]==0 && temp->fileName[i]!="-" && (temp->rmode[i]==1 || temp->wmode[i]==1 || temp->amode[i]==1)){
          cout<<"File Descriptor is : "<<i<<" ---> "<<"Name of file is : "<<temp->fileName[i]<<" ---> size of file in bytes : "<<temp->fileSize[i]<<"\n";
          if(temp->rmode[i]==1)
          cout<<"---> read permission\n";
          if(temp->wmode[i]==1)
          cout<<"---> write permission\n";
          if(temp->amode[i]==1)
          cout<<"---> append permission\n";
          }
          
          
     }
}



void deleteFile(string name,string fileName)
{
     Superblock* temp=diskMap[name];
     
     
     for(int i=1;i<diskBlocks;i++)
     {
          // cout<<i<<" ---> "<<temp->freeBlock[i]<<" "<<temp->fileName[i]<<"\n";
          if(temp->freeBlock[i]==0 && temp->fileName[i]==fileName)
          {
              temp->freeBlock[i]=1;
              temp->fileName[i]="-";
              temp->fileSize[i]=0;
              temp->rmode[i]=0;
              temp->wmode[i]=0;
              temp->amode[i]=0;
             printf("File succesfully deleted\n");
             return;
          }
          
     }
     
     printf("File does not exist\n");
     
}


void displayNestedMenu()
{
        cout << "##############################################\n";
        cout << "1 : create file \n";
        cout << "2 : open file\n" ;
        cout << "3 : read file\n" ;
        cout << "4 : write file\n" ;
        cout << "5 : append file\n" ;
        cout << "6 : close file\n" ;
        cout << "7 : delete file\n" ;
        cout << "8 : list of files\n" ;
        cout << "9 : list of opened files\n" ;
        cout << "10: unmount\n" << endl;
        cout << "##############################################\n";
}

void nested_menu(string name)
{
    int n;
    while(1)
    {
        displayNestedMenu();
        printf("Enter your choice in nested menu : ");
        scanf("%d",&n);
        if(n==1)
        {
            string filename;
            printf("Enter the name of file : ");
            cin>>filename;
            createFile(name,filename); 
           // printf("Inside 1\n");
        }
        else if(n==2)
        {
            string filename,mode;
            printf("Enter the name of file : ");
            cin>>filename;
            printf("Enter the mode to open the file read,write,append : ");
            cin>>mode;
            openFile(name,filename,mode);
           // printf("Inside 2\n");
        }
        else if(n==3)
        {
        
           //string filename;
           printf("Enter the descriptor of file : ");
           int fd;
           cin>>fd;
           //cin>>filename;
           readFile(name,fd);
          // printf("Inside 3\n");
        }
         else if(n==4)
        {
            
            char message[20000];
           printf("Enter the descriptor of file : ");
           int fd;
           cin>>fd;
           cout << "Enter the content to write in the file : \n";
           cin.getline (message,20000);
           //cin.getline (message,20000);
           string line;
            string content="";
	    while(getline(cin, line)){
    		 if (line.empty()){
        	 break;
    		 }
    		 content=content+"\n"+line;
    
	     }
           
           //cout<<"Message is : "<<content<<"\n";
          writeFile(name,fd,content);
          // printf("Inside 4\n");
        }
         else if(n==5)
        {
            string filename;
            char message[20000];
            printf("Enter the descriptor of file : ");
            int fd;
            cin>>fd;
           cout << "Enter the content to append in the file : \n";
           cin.getline (message,20000);
           
           string line;
           string content="";
	    while(getline(cin, line)){
    		 if (line.empty()){
        	 break;
    		 }
    		 content=content+"\n"+line;
    
	     }
           //cin.getline (message,20000);
           //string content(message);
          // cout<<"Message is : "<<content<<"\n";
           appendFile(name,fd,content);
           // printf("Inside 5\n");
        }
         else if(n==6)
        {
            printf("Enter the descriptor of file : ");
            int fd;
            cin>>fd;
            closeFile(name,fd);
          // printf("Inside 6\n");
        }
         else if(n==7)
        {
          string filename;
           printf("Enter the name of file : ");
            cin>>filename;
            deleteFile(name,filename);
          // printf("Inside 7\n");
        }
         else if(n==8)
        {
           listFiles(name);
         //  printf("Inside 8\n");
           
        }
         else if(n==9)
        {
           openedFiles(name);
          // printf("Inside 9\n");
        }
         else if(n==10)
        {
           Superblock* t=diskMap[name];
           string store="";
           for(int i=0;i<diskBlocks;i++)
           store=store+to_string(t->freeBlock[i])+" ";
           for(int i=0;i<diskBlocks;i++)
           store=store+t->fileName[i]+" ";
           for(int i=0;i<diskBlocks;i++)
           store=store+to_string(t->fileSize[i])+" ";
           
     /*      for(int i=0;i<diskBlocks;i++)
           store=store+to_string(t->rmode[i])+" ";
           for(int i=0;i<diskBlocks;i++)
           store=store+to_string(t->wmode[i])+" ";
           for(int i=0;i<diskBlocks;i++)
           store=store+to_string(t->amode[i])+" ";     */
           
      //   cout<<store<<"\n";
           
           int n=store.length();
    	   char arr[n+1];
           strcpy(arr,store.c_str());
           
           
           writeFileActual(0,arr,name);
            break;
          
        }
        else
        {
        
            printf("Invalid choice\n");
        }
    }

}


int diskExistOrNot(string s) 
{
DIR *directory;   // creating pointer of type dirent
struct dirent *x;   // pointer represent directory stream
char cwd[256];

if (getcwd(cwd, sizeof(cwd)) == NULL)
      perror("getcwd() error");
if (( directory= opendir (cwd)) != NULL) {      // check if directory  open
  
  while ((x = readdir (directory)) != NULL) {
      if(s==x->d_name)                    // file found return true or 1
        return 1;
  }
}
 
  
  closedir (directory); 
   return 0;
}

void mount(string name)
{
    fstream file;
    string word;
  
    file.open(name.c_str());
   
    Superblock* t=new Superblock();
    int i=0;
    while (file >> word && i<32)
    {
      
        stringstream obj(word);
        int x = 0;
        obj>>x;
        t->freeBlock[i]=x;
        i++;
    }
    
    
    i=0;
    while (i<32)
    {
        t->fileName[i]=word;
        i++;
        file >> word;
    }
    
    
    i=0;
    while ( i<32)
    {
       
        stringstream obj(word);
        long long x = 0;
        obj>>x;
        t->fileSize[i]=x;
        i++;
        file >> word;
    }
    
    /*
    i=0;
    while (i<32)
    {
       
        stringstream obj(word);
        int x = 0;
        obj>>x;
        t->rmode[i]=x;
        i++;
        file >> word;
    }
    
    
    i=0;
    while (i<32)
    {
       
        stringstream obj(word);
        int x = 0;
        obj>>x;
        t->wmode[i]=x;
        i++;
         file >> word;
      
    }
    
    i=0;
    while (i<32)
    {
       
        stringstream obj(word);
        int x = 0;
        obj>>x;
        t->amode[i]=x;
        i++;
         file >> word;
     
    }                                                    */
    
    diskMap.insert({name,t});  
}

int main()
{
    int n;
    while(1)
    {
        printf("####################################################\n");
        printf("1: Create Disk\n");
        printf("2: Mount Disk\n");
        printf("3: Exit Disk\n");
        printf("####################################################\n");
        printf("Enter your choice : ");
        scanf("%d",&n);
        if(n==1)
        {
            string name;
            printf("Enter the name of disk : ");
            cin>>name;
            if(diskExistOrNot(name))
              printf("Disk already exists\n");
            else  
            createDisk(name);
           // printf("Inside 1\n");
        }
        else if(n==2)
        { 
             string name;
            printf("Enter the name of disk to mount : ");
            cin>>name;
            if(diskMap.find(name)!=diskMap.end()){
            nested_menu(name);
            /*  printf("Disk does not exist\n");
              continue; */
            }                   
            else if(diskExistOrNot(name)==0)
              printf("Disk does not exist\n");
              else{
              mount(name);
              nested_menu(name);
              }
        }
        else if(n==3)
        {
            //printf("Inside 3\n");
            break;
        }
        else
        {
            printf("Invalid choice\n");
        }
    }
    return 1;
}