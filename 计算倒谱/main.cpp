#include <iostream>                             //库文件
#include <cstdio>
#include <cstdlib>
#include <math.h>
#include <algorithm>
#define maxn 1024                               //宏定义
#define pi 3.1415926
using namespace std;

double avg(double a[])
{
    double flag=0;
    for(int i=0;i<maxn;i++)
    {
        flag+=a[i];
    }
    flag=flag/maxn;
    return flag;
}                                             //求均值
void DFT(double x[],double y[])
{
    double XI[maxn]={0},XQ[maxn]={0},PI[maxn]={0},w[maxn];
    for(int n=0;n<maxn;n++)                                             //加权
    {
        PI[n]=x[n];
        w[n]=0.54-0.46*cos(2*pi*n/(maxn-1));
        PI[n]*=w[n];
    }
    for(int k=0;k<maxn;k++)
    {
        for(int n=0;n<maxn;n++)
        {
            XI[k]+=PI[n]*cos(2*pi/maxn*n*k);
            XQ[k]+=-PI[n]*sin(2*pi/maxn*n/k);
        }
        y[k]=XI[k]*XI[k]+XQ[k]*XQ[k];
        y[k]=log10(y[k]);
    }
}

void IDFT(double yi[])
{
    double pI[maxn]={0},pq[maxn]={0},yq[maxn]={0};
    for(int n=0;n<maxn;n++)
    {
        for(int k=1;k<maxn;k++)
        {
            pI[n]+=yi[k]*cos(2*pi/maxn*n*k)-yq[k]*sin(2*pi/maxn*n*k);
            pq[n]+=yq[k]*cos(2*pi/maxn*n*k)+yi[k]*sin(2*pi/maxn*n*k);
        }
        pI[n]/=maxn;
        pq[n]/=maxn;
        pI[n]=sqrt(pI[n]*pI[n]+pq[n]*pq[n]);
    }
    for(int k=0;k<maxn;k++)
    {
        cout<<pI[k]*1000<<"\t";
    }
}

double x[maxn]={0},y[maxn]={0},jf[maxn]={0};
double avg_1=0,avg_2=0;                                                                  //平均值
double Xmax_1=0,Xmax_2=0,test=0,c=0;

int main()
{

    freopen("D:\\学习2（c语言）\\轴承故障分析\\6310.ASC","r",stdin);              //重定向
    freopen("D:\\学习2（c语言）\\计算倒谱\\date.txt","w",stdout);
    for(int i=0;i<maxn;i++)
    {
        cin>>x[i];
    }
    avg_1=avg(x);
    for(int i=0;i<maxn;i++)
    {
        x[i]-=avg_1;
    }
    for(int i=0;i<maxn;i++)
    {
        if(fabs(x[i])>Xmax_1)
            Xmax_1=fabs(x[i]);
    }
    for(int i=0;i<maxn;i++)
    {
        x[i]=x[i]/Xmax_1;
        jf[i]=x[i]*x[i];
    }
    avg_2=avg(jf);
    avg_2=sqrt(avg_2);
    for(int i=0;i<maxn;i++)
    {
        test+=pow(x[i],4);
        if(fabs(x[i])>Xmax_2)
            Xmax_2=fabs(x[i]);
    }
    test=test/(maxn*avg_2*avg_2*avg_2*avg_2);
    c=Xmax_2/avg_2;
    cout<<"最大值为"<<Xmax_1<<endl;
    cout<<"峰值为"<<Xmax_2<<endl;
    cout<<"均平方根为"<<avg_2<<endl;
    cout<<"峰值因子为"<<c<<endl;
    cout<<"峭度为"<<test<<endl;
    DFT(x,y);
    IDFT(y);
    return 0;
}
