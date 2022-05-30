#include <math.h>
#include <stdio.h>
#include<stdlib.h>
//Physical memory 
struct memory_node*memory[11];
struct memory_node
{
    int size;
    int start;
    struct memory_node*next;
};
void rabbit(int start,int i,int size)
{
    struct memory_node*temp=malloc(sizeof(struct memory_node));
    temp->start=start;
    temp->size=size;
    temp->next=memory[i];
    memory[i]=temp;
}
int allocate(int size)
{
    int memSize=ceil(log2(size));
    int startSplit=memSize;
    while(!memory[startSplit])
    {
        startSplit++;
    }
    for(int i=startSplit;i>memSize;i--)
    {
        int childSize=1<<(i-1);
        struct memory_node*temp=memory[i];
        memory[i]=memory[i]->next;
        int start=temp->start;
        free(temp);
        rabbit(start,i-1,childSize);
        rabbit(start+childSize,i-1,childSize);
    }
    if(memory[memSize])
    {
        struct memory_node*temp=memory[memSize];
        memory[memSize]=memory[memSize]->next;
        int start=temp->start;
        free(temp);
        return start;
    }
}

void deallocate(int start,int size)
{
    int i=ceil(log2(size));
    size=1<<i;
    int index=start/size;
    int op=0;
    if(index%2==0)
    {
         op=1;
    }
    else
    {
        op=-1;
    }
     struct memory_node* temp=memory[i];
        int found=0;
         struct memory_node* prev=NULL;
        while(temp){
            int tempIndex=temp->start/temp->size;
            if(tempIndex==(index+op)){
                if(prev==NULL)
                    memory[i]=temp->next;
                else
                    prev->next=temp->next;
                if(index%2==0)
                {
                    deallocate(start, size*2);
                }
                else
                {
                    deallocate(temp->start, size*2);
                }
                  free(temp);
                found=1;
                break;
            }
            prev=temp;
        temp=temp->next;
        }
        if(!found){
           rabbit(start,i,size);
    }
}

void print()
{
    for(int i=0;i<11;i++)
    {
        int d=1<<i;
        printf("Units of size %d :\n",d);
        struct memory_node*it=memory[i];
        while(it)
        {
            printf("                size: %d, start: %d\n",it->size,it->start);
            it=it->next;
        }
        printf("************\n");
    }
}
int main()
{
    rabbit(0,10,1024);
    while(1)
    {
        print();
        int d,op;
        printf("1)allocate\n2)deallocate\n");
        scanf("%d%d",&op,&d);
        //printf("op=%d",op);
        if(op==1)
        {
            allocate(d);
        }
        else
        {
            int s;
            printf("start= ");
            scanf("%d",&s);
            deallocate(s,d);
        }
    }
    return 0;
}