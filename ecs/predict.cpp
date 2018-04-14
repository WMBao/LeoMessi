#include "predict.h"
#include "ARIMAModel.h"
#include <iostream>
#include <numeric>
#include <exception>

using namespace std;


//��Ҫ��ɵĹ��������
void predict_server(char * info[MAX_INFO_NUM], char * data[MAX_DATA_NUM], int data_num, char * filename)
{
	//��ȡ�����ļ�
	int CPU, MEM, DISK = 0;
	sscanf(info[0], "%d %d %d", &CPU, &MEM, &DISK);
	int numFlavorType = 0;
	sscanf(info[2], "%d", &numFlavorType);
    char tmpFlavorName[512] = "";
	char ** tflavorName = new char*[numFlavorType];
	for (int i=0;i<numFlavorType;i++){
	    tflavorName[i] = new char[10];
	}
    vector<char *> V_name;
    vector<int> V_CPU;
    vector<int> V_MEM;
	int flavorCPU = 0;
	int flavorMEM = 0;
	for (int i = 3; i < numFlavorType+3; i++) {
		sscanf(info[i], "%s %d %d", tmpFlavorName, &flavorCPU, &flavorMEM);
		strcpy(tflavorName[i-3], tmpFlavorName);
        V_CPU.push_back(flavorCPU);
        V_MEM.push_back(flavorMEM/1024);
        V_name.push_back(tflavorName[i-3]);
//        cout << tmpFlavorName[i-3]<<endl;
//        cout <<info[i];
	}
	char optType[5] = "";
	sscanf(info[numFlavorType + 4], "%s", optType);
	bool memOrCPU;
	if (!strcmp(optType, "CPU"))
	    memOrCPU = true;
	else
	    memOrCPU = false;
	struct tm timeStamp1, timeStamp2;
	int year, month, day, hour, minute, second;
	sscanf(info[numFlavorType + 6], "%d-%d-%d %d:%d:%d", &year, &month, &day, &hour, &minute, &second);
	timeStamp1.tm_year = year - 1900;                 // �꣬����tm�ṹ��洢���Ǵ�1900�꿪ʼ��ɦ�䣬����tm_yearΪint��ɦ������ȥ1900��
	timeStamp1.tm_mon = month - 1;                    // �£�����tm�ṹ����·ݴ洢��ΧΪ0-11������tm_monΪint��ɦ������ȥ1��
	timeStamp1.tm_mday = day;                         // �ա�
	timeStamp1.tm_hour = hour;                        // ɦ��
	timeStamp1.tm_min = minute;                       // �֡�
	timeStamp1.tm_sec = second;                       // �롣
	timeStamp1.tm_isdst = 0;                          // ������ɦ��
	sscanf(info[numFlavorType + 7], "%d-%d-%d %d:%d:%d", &year, &month, &day, &hour, &minute, &second);
	timeStamp2.tm_year = year - 1900;                 // �꣬����tm�ṹ��洢���Ǵ�1900�꿪ʼ��ɦ�䣬����tm_yearΪint��ɦ������ȥ1900��
	timeStamp2.tm_mon = month - 1;                    // �£�����tm�ṹ����·ݴ洢��ΧΪ0-11������tm_monΪint��ɦ������ȥ1��
	timeStamp2.tm_mday = day;                         // �ա�
	timeStamp2.tm_hour = hour;                        // ɦ��
	timeStamp2.tm_min = minute;                       // �֡�
	timeStamp2.tm_sec = second;                       // �롣
	timeStamp2.tm_isdst = 0;                          // ������ɦ��
	time_t tS1 = mktime(&timeStamp1);
	time_t tS2 = mktime(&timeStamp2);
	int predictTP = difftime(tS2, tS1) / 86400;
	//��ȡѵ������
	struct tm timeStamp3;
	char uid[512] = "";
	char flavorName[10]= "";
	sscanf(data[0], "%s\t%s\t%d-%d-%d %d:%d:%d", uid, flavorName, &year, &month, &day, &hour, &minute, &second);
	timeStamp3.tm_year = year - 1900;                 // �꣬����tm�ṹ��洢���Ǵ�1900�꿪ʼ��ɦ�䣬����tm_yearΪint��ɦ������ȥ1900��
	timeStamp3.tm_mon = month - 1;                    // �£�����tm�ṹ����·ݴ洢��ΧΪ0-11������tm_monΪint��ɦ������ȥ1��
	timeStamp3.tm_mday = day;                         // �ա�
	timeStamp3.tm_hour = 0;                        // ɦ��
	timeStamp3.tm_min = 0;                       // �֡�
	timeStamp3.tm_sec = 0;                       // �롣
	timeStamp3.tm_isdst = 0;                          // ������ɦ��
	time_t tS3 = mktime(&timeStamp3);
	int trainTP = difftime(tS1, tS3) / 86400;
	vector<vector<double>> trainData;
	vector<double> tmpTrain;
    for (int j = 0; j < trainTP; j++) {
        tmpTrain.push_back(0);
    }
	for (int i = 0; i < numFlavorType; i++) {
	    trainData.push_back(tmpTrain);
	}
//	if (flavorName <= numFlavorType)   trainData[flavorName - 1][0] += 1;
    int flavorIndex = 0;
    bool flavorFlag = false;
    for(int i=0;i<numFlavorType;i++){
        if(!strcmp(V_name[i],flavorName)){
            flavorIndex = i;
            flavorFlag = true;
        }
    }
    if (flavorFlag) trainData[flavorIndex][0] += 1;
	for (int i = 1; i < data_num; i++) {
		sscanf(data[i], "%s\t%s\t%d-%d-%d %d:%d:%d", uid, flavorName, &year, &month, &day, &hour, &minute, &second);
        flavorFlag = false;
		for(int i=0;i<numFlavorType;i++){
            if(!strcmp(V_name[i],flavorName)){
                flavorIndex = i;
                flavorFlag = true;
            }
        }
        if (!flavorFlag)    continue;
		timeStamp3.tm_year = year - 1900;                 // �꣬����tm�ṹ��洢���Ǵ�1900�꿪ʼ��ɦ�䣬����tm_yearΪint��ɦ������ȥ1900��
		timeStamp3.tm_mon = month - 1;                    // �£�����tm�ṹ����·ݴ洢��ΧΪ0-11������tm_monΪint��ɦ������ȥ1��
		timeStamp3.tm_mday = day;                         // �ա�
		timeStamp3.tm_hour = 0;                        // ɦ��
		timeStamp3.tm_min = 0;                       // �֡�
		timeStamp3.tm_sec = 0;                       // �롣
		timeStamp3.tm_isdst = 0;                          // ������ɦ��
		time_t tS3 = mktime(&timeStamp3);
		int tmpTP = difftime(tS1, tS3) / 86400;
		trainData[flavorIndex][trainTP-tmpTP] += 1;
	}

	vector<int> V_num;
    try {
        for (int i = 0; i < numFlavorType; i++) {
            int tmpAccu = 0;
            for (int j = 0; j < predictTP; j++) {
                ARIMAModel *arima = new ARIMAModel(trainData[i]);

                int period = 7;
                int modelCnt = 2;
                int cnt = 0;
                std::vector<std::vector<int>> list;
                std::vector<int> tmpPredict(modelCnt);

                for (int k = 0; k < modelCnt; ++k)            //控制通过多少组参数进行计算最终的结果
                {
                    std::vector<int> bestModel = arima->getARIMAModel(period, list, (k == 0) ? false : true);
                    //std::cout<<bestModel.size()<<std::endl;

                    if (bestModel.size() == 0) {
                        tmpPredict[k] = (int) trainData[i][trainData[i].size() - period];
                        cnt++;
                        break;
                    } else {
                        //std::cout<<bestModel[0]<<bestModel[1]<<std::endl;
                        int predictDiff = arima->predictValue(bestModel[0], bestModel[1], period);
                        //std::cout<<"fuck"<<std::endl;
                        tmpPredict[k] = arima->aftDeal(predictDiff, period);
                        cnt++;
                    }
//				std::cout<<bestModel[0]<<" "<<bestModel[1]<<std::endl;
                    list.push_back(bestModel);
                }

//			double sumPredict = 0.0;
                int sumPredict = 0;
                for (int k = 0; k < cnt; ++k) {
                    if (sumPredict < tmpPredict[k]) sumPredict = tmpPredict[k];
//				sumPredict += ((double)tmpPredict[k])/(double)cnt;
                }
//			int predict = (int)round(sumPredict);
                int predict = sumPredict;
                tmpAccu += predict;
                trainData[i].push_back(predict);
            }
            V_num.push_back(tmpAccu);
        }
    }
    catch (exception& e)
    {
        for (int i=0; i<numFlavorType; i++){
            V_num[i] = 0;
            for( int j=trainTP-predictTP; j<trainTP; j++){
                V_num[i]+= trainData[i][j];
            }
        }
    }
    int sum_V_num = accumulate(V_num.begin(), V_num.end(), 0);
    // ��Ҫ���������

    char *result_file = new char[10240];
    char *tmp = new char[1024];
    sprintf(result_file, "%d\n", sum_V_num, predictTP);
    for (int i = 0; i < numFlavorType; i++) {
        sprintf(tmp, "%s %d\n", V_name[i], V_num[i]);
        strcat(result_file, tmp);
    }
    sprintf(tmp, "\n");
    strcat(result_file, tmp);

    vector<vector<int> > result;
    try {
        result = DP_Bag(MEM, CPU, V_MEM, V_CPU, V_num, memOrCPU);
    }
    catch (exception& e) {
        result = Greedy_Bag(MEM, CPU, V_MEM, V_CPU, V_num, memOrCPU);
    }

    sprintf(tmp,"%d\n", result.size());
    strcat(result_file,tmp);
    for(int i=0; i<result.size(); i++){
        sprintf(tmp,"%d", i+1);
        strcat(result_file,tmp);
        vector<int> tmp_V_num;
        for (int i=0; i< numFlavorType; i++){
            tmp_V_num.push_back(0);
        }
        for (int num_flav = 0; num_flav < result[i].size(); num_flav++)
        {
            tmp_V_num[result[i][num_flav]] +=1;
        }
        for (int i=0; i< numFlavorType; i++){
            if (tmp_V_num[i] > 0){
                sprintf(tmp," %s %d", V_name[i], tmp_V_num[i]);
                strcat(result_file,tmp);
            };
        }
        sprintf(tmp,"\n");
        strcat(result_file,tmp);
    }

	// ֱ�ӵ�������ļ��ķ��������ָ���ļ���(ps��ע���ʽ����ȷ�ԣ�����н⣬��һ��ֻ��һ�����ݣ��ڶ���Ϊ�գ������п�ʼ���Ǿ�������ݣ�����֮����һ���ո�ָ���)
	write_result(result_file, filename);
}
