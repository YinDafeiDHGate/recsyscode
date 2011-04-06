/* This file is Copyright (C) 2011 Lv Hongliang. All Rights Reserved.
 * please maitain the copyright information completely when you redistribute the code.
 * 
 * If there are some bugs, please contact me via email honglianglv@gmail.com or submit the bugs 
 * in the google code project http://code.google.com/p/recsyscode/issues/list
 * 
 * my blog: http://lifecrunch.biz
 * my twitter: http://twitter.com/honglianglv
 * my google profile:https://profiles.google.com/honglianglv/about?hl=en
 *
 * It is free software; you can redistribute it and/or modify it under 
 * the license GPLV3.
 *
 * baselineBase.cpp contains some common functions of baseline model.
 * the algorithm is descripted in the page 148 of Recommender system Handbook.
 */
#ifndef BSAE_BASELINEBASE_CPP_
#define BSAE_BASELINEBASE_CPP_
namespace svd{
  	//use some global variables��store the parameter bu, bi, p, q
	double bu[USER_NUM+1] = {0};       // the user bias in the baseline predictor
    double bi[ITEM_NUM+1] = {0};       // the item bias in the baseline predictor
    
    int buNum[USER_NUM+1] = {0};       //�û�u��ֵ�item������ num of user ratings
    int biNum[ITEM_NUM+1] = {0};       //���item i�ֵ��û����� num of item ratings
    
    float mean = 0;                         //ȫ�ֵ�ƽ��ֵ             mean of all ratings
    
    vector < vector<rateNode> > rateMatrix(USER_NUM+1);   //store training set
    vector<testSetNode> probeRow;                            //store test set
    
	//initialize the bias bu and bi, the method in the page 2 of koren's TKDD'09 paper
	void initialBais()
	{
		using namespace svd;
	    int i,j;
	    for(i = 1; i < USER_NUM+1; ++i){
	    	int vSize = rateMatrix[i].size();
			for(j=0; j < vSize; ++j) {
				bi[rateMatrix[i][j].item] += (rateMatrix[i][j].rate - mean);
				biNum[rateMatrix[i][j].item] += 1;
			}			
	    }
	    
	    for(i = 1; i < ITEM_NUM+1; ++i) {
	    	if(biNum[i] >=1)bi[i] = bi[i]/(biNum[i]+25);
	    	else bi[i] = 0.0;
	    	
	    }
	   
        for(i = 1; i < USER_NUM+1; ++i){
	    	int vSize = rateMatrix[i].size();
			for(j=0; j < vSize; ++j) {
				bu[i] += (rateMatrix[i][j].rate - mean - bi[rateMatrix[i][j].item]);
				buNum[i] += 1;
			}			
	    }
	    for(i = 1; i < USER_NUM+1; ++i) {
	    	if(buNum[i]>=1)bu[i] = bu[i]/(buNum[i]+10);
	    	else bu[i] = 0.0;
	    }
	}
	
	void model(int dim, float alpha, float beta, int maxStep=60, double slowRate=1,bool isUpdateBias=true)
    {
        cout << "begin initialization: " << endl;
        loadRating(TRAINING_SET,rateMatrix,RATE_SP);  //load training set
        loadProbe(PROBE_SET,probeRow,RATE_SP);   //load test set
        mean = setMeanRating(USER_NUM,rateMatrix); //calculate the mean
        int i,u,j,k;
        
        srand((unsigned)time(0)); 
        initialBais(); //initialize bu and bi
        
        cout <<"initialization end!"<<endl<< "begin iteration: " << endl;
        
        float pui = 0.0 ; // Ԥ���u��i�Ĵ��
        double preRmse = 1000000000000.0; //���ڼ�¼��һ��rmse����Ϊ��ֹ������һ�֣����rmse�����ˣ���ֹͣ
        double nowRmse = 0.0;
        cout <<"begin testRMSEProbe(): " << endl;
        RMSEProbe(probeRow,K_NUM);
        //main loop
        for(int step = 0; step < maxStep; ++step){  //ֻ����60��
            long double rmse = 0.0;
            int n = 0;
            for( u = 1; u < USER_NUM+1; ++u) {   //ѭ������ÿһ���û�    
                int RuNum = rateMatrix[u].size(); //�û�u����ֵ�item��Ŀ
                float sqrtRuNum = 0.0;
                if(RuNum>1) sqrtRuNum = (1.0/sqrt(RuNum));
                   
                //��������
                for(i=0; i < RuNum; ++i) {// ѭ������u��ֹ���ÿһ��item
                    int itemI = rateMatrix[u][i].item;
                    short rui = rateMatrix[u][i].rate; //ʵ�ʵĴ��
                    pui = predictRate(u,itemI,dim);
                    float eui = rui - pui;
                    
                    if( isnan(eui) ) {// fabs(eui) >= 4.2 || 
                        cout<<u<<'\t'<<i<<'\t'<<pui<<'\t'<<rui<<"    "<<bu[u]<<"    "<<bi[itemI]<<"    "<<mean<<endl;
                        exit(1);
                    }
                    rmse += eui * eui; ++n;
                    if(n % 10000000 == 0)cout<<"step:"<<step<<"    n:"<<n<<" dealed!"<<endl;
                    
                    if(isUpdateBias) {
                    	bu[u] += alpha * (eui - beta * bu[u]);
                    	bi[itemI] += alpha * (eui - beta * bi[itemI]);
                    }
                } 
            }
            nowRmse =  sqrt( rmse / n);
            
            if( nowRmse >= preRmse && step >= 3) break; //���rmse�Ѿ���ʼ�����ˣ�������ѭ��
            else
                preRmse = nowRmse;
            cout << step << "\t" << nowRmse <<'\t'<< preRmse<<"     n:"<<n<<endl;
            RMSEProbe(probeRow,K_NUM);;  // check test set rmse
            
            alpha *= slowRate;    //�𲽼�Сѧϰ����
        }
        RMSEProbe(probeRow,K_NUM);  // �����Լ����
        return;
    }
};

/**
 * predict the rate
 */
float predictRate(int user, int item,int dim)
{
	using namespace svd;
    double ret  = mean+bu[user] + bi[item];
    if(ret < 1.0) ret = 1;
    if(ret > 5.0) ret = 5;
    return ret;
}
#endif // BSAE_BASELINEBASE_CPP_ 
