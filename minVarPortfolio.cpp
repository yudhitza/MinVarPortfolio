// minVarPortfolio.cpp
//compile with
// c++ -o minVarPort minVarPortfolio.cpp -lcurl -std=c++11 -fopenmp
// execute with
// ./minVarPort

//Your C++ code has to be able to read documents from the internet. This is done using the library cURL.
//Install the library cURL using the command
//sudo apt-get install libcurl4-gnutls-dev
//
// You need to have a file named "tickers.txt" with the stock symbols you want to pull in the same folder
//
// You need to make a file named "quotes" in the same file as this program is saved

//  MTH4300 Minimal Variance Portfolio project
//Created by goleftsherry's mac on 2017/11/17.
//Team: Hu Chen Nan, Francheska Orellana, Alina Armijo, Helesa Lahey
//  Copyright © 2017年. All rights reserved.

#include <iostream>
#include <set>
#include <string>
#include <fstream>
#include <sstream>
#include <list>
#include <vector>
#include<numeric>
#include<cmath>
#include <curl/curl.h>
#include<omp.h>
using namespace std;
typedef long int myint;
void printM();
/*
void covFcn(vector<vector<double>> &returns,vector<vector<double>> &covM){
    vector<double>avgR(returns.size());

    for(int i=0;i<returns.size();++i){
        double avg= accumulate(returns[i].begin(),returns[i].end(),0.0)/(1.00*returns[i].size());
        avgR[i]=avg;
        //for(int j=0;j<returns[i].size();++j){
        //cout<<returns[i][j]<< " ";
        //}
        //cout<<endl;
    }
    //for(int k=0;k<avgR.size();++k){
    //cout<<avgR[k]<<" ";
    //}
    //cout<<endl<<endl<<endl;
    for(int x=0;x<returns.size();++x){
        for(int y=0;y<returns.size();++y){
            //for(int y=0;y<=x;y++)
            double sum=0.0;
            for(int z=0;z<returns[y].size();++z){
                sum+=(returns[x][z]-avgR[x])*(returns[y][z]-avgR[y])/(returns[x].size()-1);
                // cout<<sum<<" ";
            }
            covM[x][y]=sum;
            //covM[y][x]=sum;
            //cout<<covM[x][y]<<" ";
        }
        //cout<<endl;
    }
}

*/


void covFcn(vector<vector<double> > &returns,vector<vector<double> >& covM){


  vector<double>avgR(returns.size());



  for(int i=0;i<returns.size();++i){
      double avg= accumulate(returns[i].begin(),returns[i].end(),0.0)/(1.00*returns[i].size());
      avgR[i]=avg;
      //for(int j=0;j<returns[i].size();++j){
          //cout<<returns[i][j]<< " ";
      //}
      //cout<<endl;
  }


  long int nTN;
  #pragma omp parallel
   {
     if(omp_get_thread_num()==0)
     {
       nTN=omp_get_num_threads();

     }
   }

   #pragma omp parallel num_threads(nTN)
  {
    auto myId=omp_get_thread_num();
    auto x=myId;
    while(x<returns.size()){

    //  for(int x=0;x<returns.size();++x){
      for(int y=0;y<=x;++y){
          double sum=0.0;
          for(int z=0;z<returns[y].size();++z){
              sum+=(returns[x][z]-avgR[x])*(returns[y][z]-avgR[y])/(returns[x].size()-1);
             // cout<<sum<<" ";
           }
          covM[x][y]=sum;
          covM[y][x]=sum;
          //cout<<covM[x][y]<<" ";
        }
        x+=nTN;
      //cout<<endl;
    }
  }

}

 void printToFile(std::string filename, const std::string &text){
 std::ofstream mfile;
 mfile.open(filename);
 for(myint i=0;i<text.length();++i){
 mfile<<text[i];
 }
 mfile.close();
 }

static size_t writerF(void *ptr, size_t size, size_t nmemb, void *userdata)
{
    ((std::string*)userdata)->append((char*)ptr, size * nmemb);
    return size * nmemb;
}

 void stockDataToFile(const std::string &tickerName,
 const std::string &quandl_auth_token,
 const std::string &database="WIKI",
 const std::string &folder="./quotes/")
 {
 std::string mainLink="https://www.quandl.com/api/v3/datasets/";
 mainLink+=database;
 mainLink+="/"+tickerName;
 mainLink+=".csv";
 mainLink+="?sort_order=asc&auth_token=";
 mainLink+=quandl_auth_token;
 CURL *curl;
 std::string quandlData;
 std::string fName=folder;
 fName+=tickerName;
 fName+=".csv";
 curl = curl_easy_init();
 if(curl) {
 const char* linkArrayChar=mainLink.c_str();
 curl_easy_setopt(curl, CURLOPT_URL, linkArrayChar);
 curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writerF);
 curl_easy_setopt(curl, CURLOPT_WRITEDATA, &quandlData);
 curl_easy_perform(curl);
 curl_easy_cleanup(curl);
 printToFile(fName,quandlData);
 }
 }


void singleClosePriceVector(vector<double>& data, string ticker)
//This will take in a stock symbol (ticker) and create a single vector
//of closing prices from the file of historical stock data in
//in the folder "quotes", if such a file exists
{

    int row, col;
    string filePath;
    string line;
    string val;


    data.reserve(800);
    //filePath = ticker+".csv";
    filePath = "quotes/"+ ticker+".csv";
    //filePath = "quotes/MMM.csv";
    {
        std::ifstream file(filePath,ifstream::in);
        std::getline(file, line);//remove the first line
        for(row = 0; row < 751; ++row)
        {
            std::getline(file, line);
            if ( !file.good() )
            {
                break;
            }
            std::stringstream iss(line);
            for (col = 0; col < 5; ++col)
            {
                std::getline(iss, val, ',');
                if ( !iss.good() )
                {
                    break;
                }


            }
            std::stringstream convertor(val);
            double x;
            convertor>>x;
            data.insert(data.begin(),x);
        }
    }


}
void getNextTickerNew(vector<string>& allSymbols)
//This will get stock symbols from the file "tickers.txt" and put in a vector
{
    string symbol;

    ifstream tickers("tickers.txt");

    //store words in array while outputting
    while( tickers)
    {
        tickers>>symbol;
        //       cout<<symbol<<endl;
        allSymbols.push_back(symbol);
    }

}




vector<double> calculateDailyChange_one(const vector<double>& v)
{
vector<double> result;
result.reserve(800);
double difference;
//int i=0;
for(auto bg=v.begin()+1;bg!=v.end();bg++)
{
    difference=*bg-*(bg-1);

    result.push_back(difference/(*(bg-1)));

}
return result;
}


vector<vector<double>> calculateDailyChange(const vector<vector<double>>& v)
{
    //use today's close price to minus yesterday's close price
    //use the result to divide yesterday's close price
    //store the result in a new vector
    vector<vector<double>> result;
    vector<double> temp;
    result.reserve(800);
    for(auto bg=v.begin();bg!=v.end();bg++)
    {
        temp=calculateDailyChange_one(*bg);
        result.push_back(temp);
    }

    return result;



}
void printM(vector<vector<double>> v)
{
    auto row=v.size();
    auto column=v[0].size();
    for(int i=0;i<row;i++)
    {
        for(int j=0;j<column;j++)
            cout<<v[i][j]<<" ";
        cout<<endl;
    }
}
void calculateU(const vector<vector<double>>& matrix,vector<vector<double>>& l,vector<vector<double>>& u, unsigned long i,unsigned long j)
{
//    cout<<"L is "<<endl;
//    printM(l);
//    cout<<"U is "<<endl;
//    printM(u);
    double sum=0.0;
    for(unsigned long k=0;k<i;k++)
        sum+=l[i][k]*u[k][j];
    double temp=(matrix[i][j]-sum)/l[i][i];
    u[i][j]=temp;
}

void calculateL(const vector<vector<double>>& matrix,vector<vector<double>>& l,vector<vector<double>>& u, unsigned long i,unsigned long j)
{

    double sum=0;
    for(unsigned long k=0;k<j;k++)
        sum+=l[i][k]*u[k][j];
    l[i][j]=(matrix[i][j]-sum)/u[j][j];
}

void LU(const vector<vector<double>>& matrix,vector<vector<double>>& l,vector<vector<double>>& u)
{
    auto size=matrix.size();
    l.resize(size);
    u.resize(size);
    for(auto h=0;h<size;h++)
    {
        l[h].resize(size);
        u[h].resize(size);
    }
    for(unsigned long k=0;k<size;k++)
    {
        l[k][k]=1;//u diagonal=1
    }
    for(int i=0;i<size;i++)
    {
        for(int j=i;j<size;j++)
            calculateU(matrix, l, u,i,j);
        for(int k=i+1;k<size;k++)
            calculateL(matrix, l, u, k, i);
    }
}

void calculateM(vector<vector<double>>& result,const vector<vector<double>>& a,const vector<vector<double>>&b,const int&i,const int &j)
{
    auto size=a[0].size();
    double sum=0;
    for(auto k=0;k<size;k++)
    {
        sum+=a[i][k]*b[k][j];
    }
    result[i][j]=sum;
}
/*
vector<vector<double>> matrixMultiply(vector<vector<double>> a,vector<vector<double>> b)
{
    vector<vector<double>> result;
    auto column=b[0].size();
    auto row=a.size();
    result.resize(row);
    for(auto i=0;i<row;i++)
        result[i].resize(column);
    for(auto i=0;i<row;i++)
    {
        for(auto j=0;j<column;j++)
        {
            calculateM(result,a,b,i,j);
        }
    }

    return result;
}*/
vector<vector<double>> matrixMultiply(vector<vector<double>> a,vector<vector<double>> b)
{
    vector<vector<double>> result;
    auto column=b[0].size();
    auto row=a.size();
    result.resize(row);
    long int nTN;
    #pragma omp parallel
   {
     if(omp_get_thread_num()==0)
     {
       nTN=omp_get_num_threads();

     }
   }
    #pragma omp parallel num_threads(nTN)
    {
      auto myId=omp_get_thread_num();
    //  auto job=row/nTN+1;
      auto i=myId;

      while(i<row)
      {
          result[i].resize(column);
          i+=nTN;
      }

      i=myId;

      while(i<row)
      {
          for(auto j=0;j<column;j++)
          {
              calculateM(result,a,b,i,j);
          }
          i+=nTN;
      }
    }
    #pragma parallel barrier
    return result;
}


vector<vector<double>> inverseL(vector<vector<double>> l)
{

    vector<vector<double>> temp;
    auto size=l.size();
    temp.resize(size);
    for(auto i=0;i<size;i++)
        temp[i].resize(size);
    for(auto i=0;i<size;i++)
        for(auto j=0;j<=i;j++)
        {

            if(i==j)
                temp[i][i]=1;
            else
            {
                double sum=0;
                for(auto k=0;k<i;k++)
                    sum+=l[i][k]*temp[k][j];
                temp[i][j]=-1*sum;
            }
        }

    return temp;
}

vector<vector<double>> inverseU(vector<vector<double>> u)
{

    vector<vector<double>> temp;
    int size=u.size();
    temp.resize(size);
    for(auto i=0;i<size;i++)
        temp[i].resize(size);
    for(auto i=size-1;i>=0;i--)
        for(auto j=i;j<size;j++)
        {

            if(i==j)
                temp[i][i]=1/u[i][i];
            else
            {
                double sum=0;
                for(auto k=i;k<j;k++)
                    sum+=temp[i][k]*u[k][j];
                temp[i][j]=-sum/u[j][j];
            }
        }

    return temp;
}


double mean(vector<double> v)
{
    double sum=0;
    for(auto s:v)
        sum+=s;
    return sum/v.size();
}

vector<vector<double>> transpose(vector<vector<double>> v)//from m to mT
{
    vector<vector<double>> result;

    auto size=v[0].size();
    result.resize(size);
    for(int i=0;i<size;i++)
        result[i].push_back(v[0][i]);
    return result;
}


int main() {
        std::vector<string> allTickers;
        vector<double> singleVect;
        vector<vector<double>> bigVect;
    set<string> garbage;
        string ticker;
        //string myQuandlToken="9ccxDp2hA2P-Z5vvR4WG";
        int i, size;
        double miu;
        cout<<"Please enter expected return for portfolio"<<endl;
        cin>>miu;

        //This will get a vector of all the stock symbols
        getNextTickerNew(allTickers);
        allTickers.pop_back();//the last one is the same, pop it
        size = allTickers.size();
        cout<<"Total number of stock is "<<size<<endl;



//loop to pull data from Quandl - you may want to comment out
//if running program repeatedly after successfully pulling data
   string myQuandlToken="9ccxDp2hA2P-Z5vvR4WG";
    for (i=0;i<size;i++)
    {
        ticker=allTickers[i];
        cout<<ticker<<endl;
        stockDataToFile(ticker,myQuandlToken);
    }





        for (i=0;i<size;++i)
        {
            ticker=allTickers[i];
            singleClosePriceVector(singleVect, ticker);
            //        for(auto s:singleVect)
            //            cout<<s<<endl;

//            cout<<singleVect.size()<<endl;
            if(singleVect.size()==751)
                bigVect.push_back(singleVect);
            else
                garbage.insert(ticker);
            //cout<<i<<", "<<singleVect[1]<<endl;
            //cout<<bigVect[i-1][20]<<endl;
            //cout<<ticker<<endl;
            singleVect.clear();
        }

    size=bigVect.size();






    //now we have price in the bigVect

    cout<<"number of chosen stock is "<<size<<endl;
    vector<vector<double>> dailyR=calculateDailyChange(bigVect);

    vector<vector<double>> expectedR;
    expectedR.push_back(vector<double>{0});
    expectedR[0].resize(size);
    for(int i=0;i<size;i++){
        expectedR[0][i]=250*mean(dailyR[i]);//get expected return for each stock
//        cout<<allTickers[i]<<endl;
//        cout<<mean(dailyR[i])<<endl;
    }


    auto expectedR_T=transpose(expectedR);

//    cout<<expectedR[0][0]<<endl;
//    cout<<expectedR[0][1]<<endl;

    vector<vector<double>>covM(dailyR.size(),vector<double>(dailyR.size(),0));
    covFcn(dailyR, covM); //get covariance Matrix
   // printM(covM);
        vector<vector<double>> l,u;
    LU(covM,l,u);//LU decomposition
    auto invu=inverseU(u);
    auto invl=inverseL(l);
    auto invC=matrixMultiply(invu, invl);//get inverse matrix
    auto minvC=matrixMultiply(expectedR, invC);//m*invC

    vector<vector<double>> uM;
    uM.push_back(vector<double>(size,1.0)); //U
    vector<vector<double>> uM_T=transpose(uM);//UT
    auto uinvC=matrixMultiply(uM, invC);//u*invC
    auto minvCmT=matrixMultiply(minvC, expectedR_T);//m*invC*mT
    auto minvCuT=matrixMultiply(minvC, uM_T);//m*invC*uT
    auto uinvCmT=matrixMultiply(uinvC,expectedR_T);//u*invC*mT
    auto uinvCuT=matrixMultiply(uinvC, uM_T);//u*invC*uT


    vector<double> M1,M2;
    M1.push_back(minvCmT[0][0]);
    M1.push_back(minvCuT[0][0]);
    M2.push_back(uinvCmT[0][0]);
    M2.push_back(uinvCuT[0][0]);
    vector<vector<double>>M{M1,M2};

    LU(M,l,u);

    invu=inverseU(u);
    invl=inverseL(l);
    auto invM=matrixMultiply(invu, invl);//get inverse M
    auto d=matrixMultiply(invl, l);
    auto e=matrixMultiply(invM, M);
    vector<vector<double>> X{minvC[0],uinvC[0]};
    auto MX=matrixMultiply(invM, X);


    vector<vector<double>> RM{vector<double>{miu,1.0}};
//    RM.push_back(miu);
//    RM.push_back(1.0);
    auto weight=matrixMultiply(RM, MX);
//    printM(weight);
    // double sum=0;
    // for(auto s:weight[0])
    //     sum+=s;
    // cout<<sum<<endl;

    int a=0;
    int b=0;
    while(b<size)
    {
        if(garbage.find(allTickers[a])==garbage.end())
        {
            cout<<allTickers[a]<<" "<<weight[0][b]<<endl;
            ++a;
            ++b;
        }
        else
        {
            cout<<allTickers[a]<<" "<<0<<endl;
            ++a;
        }
    }



    return 0;
}
