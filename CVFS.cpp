# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <unistd.h>
# include <iostream>
// # include <io.h>

# define MAXINODE 50

# define READ 1
# define WRITE 2

# define MAXFILESIZE 2048

# define REGULAR 1
# define SPECIAL 2

# define START 0
# define CURRENT 1
# define END 2

typedef struct superblock
{
    int TotalInodes;
    int FreeInodes;
}SUPERBLOCK, *PSUPERBLOCK;

typedef struct inode
{
    char FileName[50];
    int InodeNumber;
    int FileSize;
    int FileActualSize;
    int FileType;
    char *Buffer;
    int LinkCount;
    int ReferenceCount;
    int permission; // 1 2 3
    struct inode *next;
}INODE, *PINODE, **PPINODE;

typedef struct filetable
{
    int readoffset;
    int writeoffset;
    int count;
    int mode; // 1 2 3
    PINODE ptrinode;
}FILETABLE, *PFILETABLE;

typedef struct ufdt
{
    PFILETABLE ptrfiletable;
}UFDT;

UFDT UFDTArr[50];
SUPERBLOCK SUPERBLOCKobj;
PINODE head = NULL;

///////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Function Name : man
//  Description :   Declaring commands and their usage
//  Input :         Character string
//  Output :        Void 
//  Author :        Ajay Yogesh Varma
//  Date :          3-1-2024
//
///////////////////////////////////////////////////////////////////////////////////////////////////////

void man (char *name)
{
    if (name == NULL)
    {
        return;
    }

    if (strcmp(name,"create") == 0)
    {
        printf("Description : Used to create new regular file \n");
        printf("Usage : create File_name Permission \n");
    }
    else if (strcmp(name,"read") == 0)
    {
        printf("Description : Used to read data from regular file \n");
        printf("Usage : read File_name No_Of_Bytes_To_Read \n");
    }
    else if (strcmp(name,"write") == 0)
    {
        printf("Description : Used to write into regular file \n");
        printf("Usage : write File_name \n After this enter the data that we want to enter \n");
    }
    else if (strcmp(name,"ls") == 0)
    {
        printf("Description : Used to list all information of files \n");
        printf("Usage : ls \n");
    }
    else if (strcmp(name,"stat") == 0)
    {
        printf("Description : Used to display information of files \n");
        printf("Usage : stat File_name \n");
    }
    else if (strcmp(name,"fstat") == 0)
    {
        printf("Description : Used to display information of files \n");
        printf("Usage : stat File_Descriptor \n");
    }
    else if (strcmp(name,"truncate") == 0)
    {
        printf("Description : Used to remove data from file \n");
        printf("Usage : truncate File_name \n");
    }
    else if (strcmp(name,"open") == 0)
    {
        printf("Description : Used to open existing file \n");
        printf("Usage : open File_name Mode \n");
    }
    else if (strcmp(name,"close") == 0)
    {
        printf("Descritption : Used to close opened file \n");
        printf("Usage : close File_name \n");
    }
    else if (strcmp(name,"closeall") == 0)
    {
        printf("Description : Used to close all opened file \n");
        printf("Usage : closeall \n");
    }
    else if (strcmp(name,"lseek") == 0)
    {
        printf("Description : Used to change the file offset \n");
        printf("Usage : lseek File_name ChangeInOffset StartPoint \n");
    }
    else if (strcmp(name,"rm") == 0)
    {
        printf("Description : Used to delete files \n");
        printf("Usage : rm File_name \n");
    }
    else 
    {
        printf("ERROR : No manual entry available \n");
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Function Name : DisplayHelp
//  Description :   This will help with all commands
//  Input :         None
//  Output :        None
//  Author :        Ajay Yogesh Varma
//  Date :          3-1-2024
//
///////////////////////////////////////////////////////////////////////////////////////////////////////

void DisplayHelp()
{
    printf("ls: To list out all files \n");
    printf("clear : To clear console \n");
    printf("open : To open the file \n");
    printf("close : To close the file \n");
    printf("closeall : To close all opened files \n");
    printf("read : To read the contents from the file \n");
    printf("write : To write contents in the file \n");
    printf("exit : To terminate the file system \n");
    printf("stat : To display the information of the file using name \n");
    printf("fstat : To display the information of the file using file descriptor \n");
    printf("truncate : To remove all the data from the file \n");
    printf("rm : To delete the file \n");
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Function Name : GetFDFromName
//  Description :   Used to get the file descriptor using the name 
//  Input :         character string
//  Output :        Integer
//  Author :        Ajay Yogesh Varma
//  Date :          3-1-2024
//
///////////////////////////////////////////////////////////////////////////////////////////////////////

int GetFDFromName(char *name)
{
    int i = 0;
    
    while(i < 50)
    {
        if(UFDTArr[i].ptrfiletable != NULL)
        {
            if(strcmp((UFDTArr[i].ptrfiletable -> ptrinode -> FileName), name) == 0)
            {
                break;
                i++;
            }
        }
    }

    if(i == 50)
    {
        return -1;
    }
    else
    {
        return i;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Function Name : Get_Inode
//  Description :   Used to get the Inode
//  Input :         character string
//  Output :        Pointer (address)
//  Author :        Ajay Yogesh Varma
//  Date :          3-1-2024
//
///////////////////////////////////////////////////////////////////////////////////////////////////////

PINODE Get_Inode(char *name)
{
    PINODE temp = head;
    int i = 0;

    if (name == NULL)
    {
        return NULL;
    }

    while (temp != NULL)
    {
        if(strcmp(name, temp -> FileName) == 0)
        {
            break;
        }
        temp = temp -> next;
    }
    return temp;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Function Name : CreateDILB
//  Description :   To create DILB
//  Input :         None
//  Output :        None
//  Author :        Ajay Yogesh Varma
//  Date :          3-1-2024
//
///////////////////////////////////////////////////////////////////////////////////////////////////////

void CreateDILB()
{
    int i = 0;
    PINODE newn = NULL;
    PINODE temp = head;

    while(i <= MAXINODE)
    {
        newn = (PINODE) malloc(sizeof(INODE));

        newn -> LinkCount = 0;
        newn -> ReferenceCount = 0;
        newn -> FileType = 0;
        newn -> FileSize = 0;
        
        newn -> Buffer = NULL;
        newn -> next = NULL;

        newn -> InodeNumber = i;

        if (temp == NULL)
        {
            head = newn;
            temp = head;
        }
        else
        {
            temp -> next = newn;
            temp = temp -> next;
        }
        i++;
    }
    printf("DILB created successfully \n");
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Function Name : InitialiseSuperBlock
//  Description :   Initialises the Super block
//  Input :         None
//  Output :        None
//  Author :        Ajay Yogesh Varma
//  Date :          3-1-2024
//
///////////////////////////////////////////////////////////////////////////////////////////////////////

void InitialiseSuperBlock()
{
    int i = 0;

    while (i < MAXINODE)
    {
        UFDTArr[i].ptrfiletable = NULL;
        i++;
    }

    SUPERBLOCKobj.TotalInodes = MAXINODE;
    SUPERBLOCKobj.FreeInodes = MAXINODE;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Function Name : CreateFile 
//  Description :   To create the new files
//  Input :         Character string, Integer
//  Output :        Integer
//  Author :        Ajay Yogesh Varma
//  Date :          3-1-2024
//
///////////////////////////////////////////////////////////////////////////////////////////////////////

int CreateFile(char *name, int permission)
{
    int i = 0;
    PINODE temp = head;

    if ((name == NULL) || (permission == 0) || (permission > 3))
    {
        return -1;
    }
    if (SUPERBLOCKobj.FreeInodes == 0)
    {
        return -2;
    }

    (SUPERBLOCKobj.FreeInodes)--;

    if(Get_Inode(name) != NULL)
    {
        return -3;
    }

    while (temp != NULL)
    {
        if (temp -> FileType == 0)
        {
            break;
        }
        temp = temp -> next;
    }

    while (i < 50)
    {
        if (UFDTArr[i].ptrfiletable == NULL)
        {
            break;
        }
        i++;
    }

    UFDTArr[i].ptrfiletable = (PFILETABLE) malloc (sizeof(FILETABLE));

    UFDTArr[i].ptrfiletable -> count = 1;
    UFDTArr[i].ptrfiletable -> mode = permission;
    UFDTArr[i].ptrfiletable -> readoffset = 0;
    UFDTArr[i].ptrfiletable -> writeoffset = 0;

    UFDTArr[i].ptrfiletable -> ptrinode = temp;

    strcpy (UFDTArr[i].ptrfiletable -> ptrinode -> FileName, name);
    UFDTArr[i].ptrfiletable -> ptrinode -> FileType = REGULAR;
    UFDTArr[i].ptrfiletable -> ptrinode -> ReferenceCount = 1;
    UFDTArr[i].ptrfiletable -> ptrinode -> LinkCount = 1;
    UFDTArr[i].ptrfiletable -> ptrinode -> FileSize = MAXFILESIZE;
    UFDTArr[i].ptrfiletable -> ptrinode -> FileActualSize = 0;
    UFDTArr[i].ptrfiletable -> ptrinode -> permission = permission;
    UFDTArr[i].ptrfiletable -> ptrinode -> Buffer = (char *)malloc(MAXFILESIZE);

    return i;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Function Name : rm_File
//  Description :   This is used to delete the files 
//  Input :         Character string
//  Output :        Integer
//  Author :        Ajay Yogesh Varma
//  Date :          3-1-2024
//
///////////////////////////////////////////////////////////////////////////////////////////////////////

// rm_File(Demo.txt)

int rm_File (char *name)
{
    int fd = 0;

    fd = GetFDFromName(name);

    if(fd == -1)
    {
        return -1;
    }

    (UFDTArr[fd].ptrfiletable -> ptrinode -> LinkCount) --;

    if (UFDTArr[fd].ptrfiletable -> ptrinode -> LinkCount == 0)
    {
        UFDTArr[fd].ptrfiletable -> ptrinode -> FileType = 0;
        // free (UFDTArr[fd].ptrfiletable -> ptrinode -> Buffer);
        free (UFDTArr[fd].ptrfiletable);
    } 

    UFDTArr[fd].ptrfiletable = NULL;
    (SUPERBLOCKobj.FreeInodes)++;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Function Name : ReadFile
//  Description :   This function is used to only read the content in the file
//  Input :         Integer, Character Array, Integer
//  Output :        Integer
//  Author :        Ajay Yogesh Varma
//  Date :          3-1-2024
//
///////////////////////////////////////////////////////////////////////////////////////////////////////

int ReadFile(int fd, char *Arr, int isize)
{
    int read_size = 0;

    if(UFDTArr[fd].ptrfiletable == NULL)
    {
        return -1;
    }

    if(UFDTArr[fd].ptrfiletable -> mode != READ && UFDTArr[fd].ptrfiletable -> mode != READ + WRITE)
    {
        return -2;
    }

    if(UFDTArr[fd].ptrfiletable -> ptrinode -> permission != READ && UFDTArr[fd].ptrfiletable -> ptrinode -> permission != READ +WRITE)
    {
        return -2;
    }

    if(UFDTArr[fd].ptrfiletable -> readoffset == UFDTArr[fd].ptrfiletable -> ptrinode -> FileActualSize)
    {
        return -3;
    }

    if(UFDTArr[fd].ptrfiletable -> ptrinode -> FileType != REGULAR)
    {
        return -4;
    }

    read_size = (UFDTArr[fd].ptrfiletable -> ptrinode -> FileActualSize) - (UFDTArr[fd].ptrfiletable -> readoffset);
    if(read_size < isize)
    {
        strncpy(Arr,(UFDTArr[fd].ptrfiletable -> ptrinode -> Buffer) + (UFDTArr[fd].ptrfiletable -> readoffset), read_size);

        UFDTArr[fd].ptrfiletable -> readoffset = UFDTArr[fd].ptrfiletable ->  readoffset + read_size;
    }
    else
    {
        strncpy(Arr,(UFDTArr[fd].ptrfiletable -> ptrinode -> Buffer) + (UFDTArr[fd].ptrfiletable -> readoffset), isize);

        (UFDTArr[fd].ptrfiletable -> readoffset) = (UFDTArr[fd].ptrfiletable -> readoffset) + isize;
    }

    return isize;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Function Name : WriteFile
//  Description :   This is used to write the content in the file
//  Input :         Integer, Character Array, Integer
//  Output :        Integer
//  Author :        Ajay Yogesh Varma
//  Date :          3-1-2024
//
///////////////////////////////////////////////////////////////////////////////////////////////////////

int WriteFile (int fd, char *arr, int isize)
{
    if(((UFDTArr[fd].ptrfiletable -> mode) != WRITE) && ((UFDTArr[fd].ptrfiletable -> mode != READ + WRITE)))
    {
        return -1;
    }

    if(((UFDTArr[fd].ptrfiletable -> ptrinode -> permission) != WRITE) && ((UFDTArr[fd].ptrfiletable -> ptrinode -> permission) != READ + WRITE))
    {
        return -1;
    }

    if((UFDTArr[fd].ptrfiletable -> writeoffset) == MAXFILESIZE)
    {
        return -2;
    }

    if((UFDTArr[fd].ptrfiletable -> ptrinode -> FileType) != REGULAR)
    {
        return -3;
    }

    strncpy((UFDTArr[fd].ptrfiletable -> ptrinode ->Buffer) +(UFDTArr[fd].ptrfiletable -> writeoffset),arr, isize);

    (UFDTArr[fd].ptrfiletable -> writeoffset) = (UFDTArr[fd].ptrfiletable -> writeoffset) + isize;

    (UFDTArr[fd].ptrfiletable -> ptrinode -> FileActualSize) = (UFDTArr[fd].ptrfiletable -> ptrinode -> FileActualSize) + isize;

    return isize;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Function Name : OpenFile
//  Description :   This function used to open the file in the file system in various modes
//  Input :         Character string, Integer
//  Output :        Integer
//  Author :        Ajay Yogesh Varma
//  Date :          5-1-2024
//
///////////////////////////////////////////////////////////////////////////////////////////////////////

int OpenFile(char *name, int mode)
{
    int i = 0;
    PINODE temp = NULL;

    if(name == NULL || mode <= 0)
    {
        return -1;
    }

    temp = Get_Inode(name);
    if(temp == NULL)
    {
        return -2;
    }

    if(temp -> permission < mode)
    {
        return -3;
    }

    while(i < 50)
    {
        if(UFDTArr[i].ptrfiletable == NULL)
        {
            break;
        }
        i++;
    }

    UFDTArr[i].ptrfiletable = (PFILETABLE) malloc (sizeof (FILETABLE));
    if(UFDTArr[i].ptrfiletable == NULL)
    {
        return -1;
    }
    UFDTArr[i].ptrfiletable -> count = 1;
    UFDTArr[i].ptrfiletable -> mode = mode;
    if(mode == READ + WRITE)
    {
        UFDTArr[i].ptrfiletable -> readoffset = 0;
        UFDTArr[i].ptrfiletable -> writeoffset = 0;
    }
    else if(mode == READ)
    {
        UFDTArr[i].ptrfiletable -> readoffset = 0;
    }
    else if(mode == WRITE)
    {
        UFDTArr[i].ptrfiletable -> writeoffset = 0;
    }
    UFDTArr[i].ptrfiletable -> ptrinode = temp;
    (UFDTArr[i].ptrfiletable -> ptrinode -> ReferenceCount)++;

    return i;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Function Name : CloseFileByName
//  Description :   This is function used to close the file by its name
//  Input :         Integer
//  Output :        None
//  Author :        Ajay Yogesh Varma
//  Date :          5-1-2024
//
///////////////////////////////////////////////////////////////////////////////////////////////////////

void CloseFileByName(int fd)
{
    UFDTArr[fd].ptrfiletable -> readoffset = 0;
    UFDTArr[fd].ptrfiletable -> writeoffset = 0;
    (UFDTArr[fd].ptrfiletable -> ptrinode -> ReferenceCount)--;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Function Name : CloseFileByName
//  Description :   Used to close the files by their names
//  Input :         Character string
//  Output :        Integer
//  Author :        Ajay Yogesh Varma   
//  Date :          5-1-2024
//
///////////////////////////////////////////////////////////////////////////////////////////////////////

int CloseFileByName(char *name)
{
    int i = 0;
    i = GetFDFromName(name);

    if( i == -1)
    {
        return -1;
    }

    UFDTArr[i].ptrfiletable -> readoffset = 0;
    UFDTArr[i].ptrfiletable -> writeoffset = 0;
    (UFDTArr[i].ptrfiletable -> ptrinode -> ReferenceCount)--;

    return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Function Name : CloseAllFiles
//  Description :   Used to close all the open existing files
//  Input :         None
//  Output :        None
//  Author :        Ajay Yogesh Varma
//  Date :          5-1-2024
//
///////////////////////////////////////////////////////////////////////////////////////////////////////

void CloseAllFiles()
{
    int i = 0;
    while(i < 50)
    {
        if(UFDTArr[i].ptrfiletable != NULL)
        {
            UFDTArr[i].ptrfiletable -> readoffset = 0;
            UFDTArr[i].ptrfiletable -> writeoffset = 0;
            (UFDTArr[i].ptrfiletable -> ptrinode -> ReferenceCount)--;
            break;
        }
        i++;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Function Name : LseekFile
//  Description :   Used to reposition file offset of an open file descriptor
//  Input :         Integer, Integer, Integer
//  Output :        Integer
//  Author :        Ajay Yogesh Varma
//  Date :          5-1-2024
//
///////////////////////////////////////////////////////////////////////////////////////////////////////

int LseekFile(int fd, int size, int from)
{
    if((fd < 0) || (from > 2))
    {
        return -1;
    }
    if(UFDTArr[fd].ptrfiletable == NULL)
    {
        return -1;
    }

    if((UFDTArr[fd].ptrfiletable -> mode == READ) || (UFDTArr[fd].ptrfiletable -> mode == READ + WRITE))
    {
        if(from == CURRENT)
        {
            if(((UFDTArr[fd].ptrfiletable -> readoffset) + size) > (UFDTArr[fd].ptrfiletable -> ptrinode -> FileActualSize))
            {
                return -1;
            }
            if(((UFDTArr[fd].ptrfiletable -> readoffset + size) < 0))
            {
                return -1;
            }
            (UFDTArr[fd].ptrfiletable -> readoffset) = (UFDTArr[fd].ptrfiletable -> readoffset) + size;
        }
        else if(from == START)
        {
            if(size > (UFDTArr[fd].ptrfiletable -> ptrinode -> FileActualSize))
            {
                return -1;
            }
            if(size < 0)
            {
                return -1;
            }
            (UFDTArr[fd].ptrfiletable -> readoffset) = size;
        }
        else if(from == END)
        {
            if((UFDTArr[fd].ptrfiletable -> ptrinode -> FileActualSize) + size > MAXFILESIZE)
            {
                return -1;
            }
            if(((UFDTArr[fd].ptrfiletable -> readoffset) + size) < 0)
            {
                return -1;
            }
            (UFDTArr[fd].ptrfiletable -> readoffset) = (UFDTArr[fd].ptrfiletable -> ptrinode -> FileActualSize) + size;
        }
    }
    else if(UFDTArr[fd].ptrfiletable -> mode == WRITE)
    {
        if(from == CURRENT)
        {
            if(((UFDTArr[fd].ptrfiletable -> writeoffset) + size) > MAXFILESIZE)
            {
                return -1;
            }
            if(((UFDTArr[fd].ptrfiletable -> writeoffset) + size) < 0)
            {
                return -1;
            }
            if(((UFDTArr[fd].ptrfiletable -> writeoffset) + size) > (UFDTArr[fd].ptrfiletable -> ptrinode ->FileActualSize))
            {
                (UFDTArr[fd].ptrfiletable -> ptrinode -> FileActualSize) = (UFDTArr[fd].ptrfiletable -> writeoffset) + size;
            }
            (UFDTArr[fd].ptrfiletable -> writeoffset) = (UFDTArr[fd].ptrfiletable -> writeoffset) + size;
        }
        else if(from == START)
        {
            if(size < MAXFILESIZE)
            {
                return -1;
            }
            if(size < 0)
            {
                return -1;
            }
            if(size < (UFDTArr[fd].ptrfiletable -> ptrinode -> FileActualSize))
            {
                (UFDTArr[fd].ptrfiletable -> ptrinode -> FileActualSize) = size;
            }
            (UFDTArr[fd].ptrfiletable -> writeoffset) = size;
        }
        else if(from == END)
        {
            if((UFDTArr[fd].ptrfiletable -> ptrinode -> FileActualSize) + size > MAXFILESIZE)
            {
                return -1;
            }
            if(((UFDTArr[fd].ptrfiletable -> writeoffset) + size) < 0)
            {
                return -1;
            }
            (UFDTArr[fd].ptrfiletable -> writeoffset) = (UFDTArr[fd].ptrfiletable -> ptrinode -> FileActualSize) + size;
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Function Name : ls_file
//  Description :   Used to list the all files 
//  Input :         None
//  Output :        None
//  Author :        Ajay Yogesh Varma
//  Date :          5-1-2024
//
///////////////////////////////////////////////////////////////////////////////////////////////////////

void ls_file()
{
    int i = 0;
    PINODE temp = head;

    if(SUPERBLOCKobj.FreeInodes == MAXINODE)
    {
        printf("ERROR : There are no files \n");
        return;
    }

    printf("\nFile Name \tInode number \tFile Size \tLink Count\n");
    printf("-------------------------------------------------------------------------------\n");
    while(temp != NULL)
    {
        if(temp -> FileType != 0)
        {
            printf("%s\t\t%d\t\t%d\t\t%d\n",temp -> FileName, temp -> InodeNumber, temp -> FileActualSize, temp -> LinkCount);
        }
        temp = temp -> next;
    }
    printf("------------------------------------------------------------------------------\n");     
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Function Name : fstat_file
//  Description :   Used to display information of file using file descriptor
//  Input :         Integer
//  Output :        Integer
//  Author :        Ajay Yogesh Varma
//  Date :          5-1-2024
//
///////////////////////////////////////////////////////////////////////////////////////////////////////

int fstat_file(int fd)
{
    PINODE temp = head;
    int i = 0;

    if(fd < 0)
    {
        return -1;
    }
    
    if(UFDTArr[fd].ptrfiletable == NULL)
    {
        return -2;
    }

    temp = UFDTArr[fd].ptrfiletable -> ptrinode;

    printf("\n-------- Statistical Information about file ---------\n");
    printf("File Name : %s \n", temp -> FileName);
    printf("Inode Number : %d \n", temp -> InodeNumber);
    printf("File Size : %d \n", temp -> FileSize);
    printf("Actual File Size : %d \n", temp -> FileActualSize);
    printf("Link Count : %d \n", temp -> LinkCount);
    printf("Reference Count : %d \n", temp -> ReferenceCount);

    if(temp -> permission == 1)
    {
        printf("File Permission : Read Only \n");
    }
    else if(temp -> permission == 2)
    {
        printf("File Permission : Write Only \n");
    }
    else if(temp -> permission == 3)
    {
        printf("File Permission : Read and Write only \n");
    }
    printf("-------------------------------------------------------------------\n\n");

    return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Function Name : stat_file
//  Description :   This is used to show the information of the files by file name
//  Input :         Character string
//  Output :        Integer
//  Author :        Ajay Yogesh Varma
//  Date :          5-1-2024
//
///////////////////////////////////////////////////////////////////////////////////////////////////////

int stat_file(char *name)
{
    PINODE temp = head;
    int i = 0;

    if(name == NULL)
    {
        return -1;
    }

    while(temp != NULL)
    {
        if(strcmp(name, temp -> FileName) == 0)
        {
            break;
        }
        temp = temp -> next;
    }

    if(temp == NULL)
    {
        return -2;
    }

    printf("\n------- Statistical Information about file -------\n");
    printf("File Name : %s \n", temp -> FileName);
    printf("Inode Number : %d \n", temp -> InodeNumber);
    printf("File Size : %d \n", temp -> FileSize);
    printf("Actual File Size : %d \n", temp -> FileActualSize);
    printf("Link Count : %d \n", temp -> LinkCount);
    printf("Reference Count : %d \n", temp -> ReferenceCount);

    if(temp -> permission == 1)
    {
        printf("File Permission : Read Only \n");
    }
    else if(temp -> permission == 2)
    {
        printf("File Permission : Write Only \n");
    }
    else if(temp -> permission == 3)
    {
        printf("File Permission : Read and Write only \n");
    }
    printf("-------------------------------------------------------------------\n");

    return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Function Name : truncate_file
//  Description :   Used to delete the data from file
//  Input :         Character string
//  Output :        Integer
//  Author :        Ajay Yogesh Varma
//  Date :          5-1-2024
//
///////////////////////////////////////////////////////////////////////////////////////////////////////

int truncate_file(char *name)
{
    int fd = GetFDFromName(name);
    if(fd == 1)
    {
        return -1;
    }

    memset(UFDTArr[fd].ptrfiletable -> ptrinode -> Buffer, 0, 1024);
    UFDTArr[fd].ptrfiletable -> readoffset = 0;
    UFDTArr[fd].ptrfiletable -> writeoffset = 0;
    UFDTArr[fd].ptrfiletable -> ptrinode -> FileActualSize = 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  This is the entry point function of the program
//
///////////////////////////////////////////////////////////////////////////////////////////////////////

int main()
{
    char *ptr = NULL;
    int ret = 0, fd = 0, count = 0;
    char command[4][80], str[80],arr[1024];

    InitialiseSuperBlock();
    CreateDILB();

    while (1)
    {
        fflush(stdin);
        strcpy(str,"");

        printf("\n Marvellous VFS : > \n");

        fgets(str,80,stdin);    //scanf("%[^'\n']s",str);
        count == sscanf(str, "%s %s %s %s",command[0],command[1],command[2],command[3]);

        if(count == 1)
        {
            if(strcmp(command[0],"ls") == 0)
            {
                ls_file();
            }
            else if(strcmp(command[0],"closeall") == 0)
            {
                CloseAllFiles();
                printf("All Files closed successfully \n");
                continue;
            }
            else if(strcmp(command[0],"clear") == 0)
            {
                system("cls");
                continue;
            }
            else if(strcmp(command[0],"help") == 0)
            {
                DisplayHelp();
                continue;
            }
            else if(strcmp(command[0],"exit") == 0)
            {
                printf("Terminating the Marvellous Virtual File System \n");
                break;
            }
            else
            {
                printf("ERROR : Command not found \n");
                continue;
            }
        }
        else if(count == 2)
        {
            if(strcmp(command[0],"stat") == 0)
            {
                ret = stat_file(command[1]);
                if(ret == -1)
                {
                    printf("ERROR : Incorrect Parameters \n");
                }
                if(ret == -2)
                {
                    printf("ERROR : There is no such file \n");
                }
                continue;
            }
            else if (strcmp(command[0],"fstat") == 0)
            {
                ret = fstat_file(atoi(command[1]));
                if(ret == -1)
                {
                    printf("ERROR : Invalid Parameter \n");
                }
                if(ret == -2)
                {
                    printf("ERROR : There is no such file \n");
                }
                continue;
            }
            else if (strcmp(command[0],"close") == 0)
            {
                ret = CloseFileByName(command[1]);
                if(ret == -1)
                {
                    printf("ERROR : There is no such file \n");
                }
                continue;
            }
            else if (strcmp(command[0],"rm") == 0)
            {
                ret = rm_File(command[1]);
                if(ret == -1)
                {
                    printf("ERROR : There is no such file \n");
                }
                continue;
            }
            else if (strcmp(command[0],"man") == 0)
            {
                man(command[1]);
            }
            else if (strcmp(command[0],"write") == 0)
            {
                fd = GetFDFromName(command[1]);
                if(fd == -1)
                {
                    printf("ERROR : Incorrect Parameters \n");
                    continue;
                }
                printf("Enter the Data : \n");
                scanf("%[^\n]",arr);

                ret = strlen(arr);
                if(ret == 0)
                {
                    printf("ERROR : Incorrect Parameters \n");
                    continue;
                }
                ret = WriteFile(fd, arr, ret);
                if(ret == -1)
                {
                    printf("ERROR : Permission Denied \n");
                }
                if(ret == -2)
                {
                    printf("ERROR : There is no sufficient memory to write \n");
                }
                if(ret == -3)
                {
                    printf("ERROR : It is not regular file \n");
                }
            }
            else if (strcmp(command[0],"truncate") == 0)
            {
                ret = truncate_file(command[1]);
                if(ret == -1)
                {
                    printf("ERROR : Incorrect Parameters \n");
                }
            }
            else
            {
                printf("\nERROR : Command not found \n");
                continue;
            }
        }
        else if (count == 3)
        {
            if(strcmp(command[0],"create") == 0)
            {
                ret = CreateFile(command[1],atoi(command[2]));
                if(ret >= 0)
                {
                    printf("File is successfully created with the file descriptor \n");
                }
                if(ret == -1)
                {
                    printf("ERROR : Incorrect Parameters \n");
                }
                if(ret == -2)
                {
                    printf("ERROR : There is no inodes \n");
                }
                if(ret == -3)
                {
                    printf("ERROR : File already exists \n");
                }
                if(ret == -4)
                {
                    printf("ERROR : Memory allocation failure \n");
                }
                continue;
            }
            else if (strcmp(command[0],"open") == 0)
            {
                ret = OpenFile(command[1],atoi(command[2]));
                if(ret >= 0)
                {
                    printf("File successfully opened with the file descriptor \n");
                }
                if(ret == -1)
                {
                    printf("ERROR : Incorrect Parameters \n");
                }
                if(ret == -2)
                {
                    printf("ERROR : File is not present \n");
                }
                if(ret == -3)
                {
                    printf("ERROR : Permission denied \n");
                }
                continue;
            }
            else if (strcmp(command[0],"read") == 0)
            {
                fd = GetFDFromName(command[1]);
                if(fd == -1)
                {
                    printf("ERROR : Incorrect Parameters \n");
                    continue;
                }
                ptr = (char *) malloc (sizeof(atoi(command[2])) + 1);
                if(ptr == NULL)
                {
                    printf("ERROR : Memory allocation failure \n");
                    continue;
                }
                ret = ReadFile(fd, ptr, atoi(command[2]));
                if(ret == -1)
                {
                    printf("ERROR : File not exists \n");
                }
                if(ret == -2)
                {
                    printf("ERROR : Permission denied \n");
                }
                if(ret == -3)
                {
                    printf("ERROR : Reached at the end of file \n");
                }
                if(ret == -4)
                {
                    printf("ERROR : It is not regular file \n");
                }
                if(ret == 0)
                {
                    printf("ERROR : File is empty \n");
                }
                if(ret > 0)
                {
                    write(2,ptr,ret);
                }
                continue;
            }
            else
            {
                printf("\nERROR : Command not found \n");
                continue;
            }
        }
        else if (count == 4)
        {
            if(strcmp(command[0],"lseek") == 0)
            {
                fd = GetFDFromName(command[1]);
                if(fd == -1)
                {
                    printf("ERROR : Incorrect Parameters \n");
                    continue;
                }
                ret = LseekFile(fd, atoi(command[2]), atoi(command[3]));
                if(ret == -1)
                {
                    printf("ERROR : Unable to perform lseek \n");
                }
            }
            else
            {
                printf("\nERROR : Command not found \n");
                continue;
            }
        }
        else
        {
            printf("\nERROR : Command not found \n");
            continue;
        }
    }
    return 0;
}