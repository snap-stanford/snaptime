#include "create_data.hpp"
#include <algorithm>

void CreateData::parseData(std::string filename) {
    std::ifstream infile(filename);
    std::string line;
    signal_count = 0;
    while(std::getline(infile,line)){
        signal_count++;
        std::stringstream ss(line);
        TVec<TPair<TFlt,TFlt> > data;
        std::string point;
        int counter = 0;
        int dtype = 0;
        while(std::getline(ss,point,'\t')){
            if(counter == 0) {
                if(point.compare("bool") != 0 && point.compare("float") != 0){ // currently handling only bools and floats
                    break;
                }
                dtype = point.compare("bool") == 0 ? 0 : 1;
            } else if (counter > 1) {
                std::size_t loc = point.find(",");
                if (loc != std::string::npos){
                    long long int timestamp = stoll(point.substr(0,loc));
                    std::string str_value = point.substr(loc+1);
                    double value;
                    if(dtype == 0){
                        value = str_value.compare("f") == 0 ? 0 : 1;
                    } else {
                        value = stod(str_value);
                    }
                    data.Add(TPair<TFlt,TFlt>(TFlt(timestamp),TFlt(value)));
                }
            }
            counter++;
        }
        signal_values.Add(data);
    }
}

Eigen::MatrixXd CreateData::fillData(long long initialTimestamp, int duration, int granularity) {
    int size = static_cast<int>(duration/granularity);
    Eigen::MatrixXd filledData(size,signal_count);
    std::vector<struct heapData> h_data;
    for(int i = 0 ; i < signal_values.Len(); ++i){
        signal_count++;
        if(signal_values[i].Len() > 0){
            h_data.push_back({i,0});
        }
    }
    if(h_data.size() > 0){
        std::make_heap(h_data.begin(),h_data.end(),[this](struct heapData& a, struct heapData& b){return static_cast<long long>(signal_values[a.signal_index][a.signal_position].Val1.Val) > static_cast<long long>(signal_values[b.signal_index][b.signal_position].Val1.Val);});
        Eigen::MatrixXd runningVector(1,signal_count);
        filledData.setZero(size,signal_count);
        runningVector.setZero(1,signal_count);
        heapData currentMin = h_data.front();
        for(long long t = initialTimestamp ; t < initialTimestamp + duration; t += granularity){
            if (t == static_cast<long long>(signal_values[currentMin.signal_index][currentMin.signal_position].Val1.Val)) { //update values
                while(static_cast<long long>(signal_values[currentMin.signal_index][currentMin.signal_position].Val1.Val) == t && h_data.size() > 0){
                    runningVector(0,currentMin.signal_index) = signal_values[currentMin.signal_index][currentMin.signal_position].Val2.Val;
                    std::pop_heap(h_data.begin(),h_data.end(),[this](struct heapData& a, struct heapData& b){return static_cast<long long>(signal_values[a.signal_index][a.signal_position].Val1.Val) > static_cast<long long>(signal_values[b.signal_index][b.signal_position].Val1.Val);});h_data.pop_back();
                    if(currentMin.signal_position + 1 < signal_values[currentMin.signal_index].Len()){
                        h_data.push_back({currentMin.signal_index,currentMin.signal_position+1});
                        std::push_heap(h_data.begin(),h_data.end(),[this](struct heapData& a, struct heapData& b){return static_cast<long long>(signal_values[a.signal_index][a.signal_position].Val1.Val) > static_cast<long long>(signal_values[b.signal_index][b.signal_position].Val1.Val);});
                    }
                    if(h_data.size() > 0){
                        currentMin = h_data.front();
                    }
                }
            }
            int count = static_cast<int>((t-initialTimestamp)/granularity);
            filledData.block(count,0,1,signal_count) = runningVector;
        }
    }
    return filledData;
}


