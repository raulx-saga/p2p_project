#include <iostream> 
#include <string>
#include <vector>
#include <fstream> 
int main(){
    std::fstream file("ble car1.mp4", std::ios::binary | std::ios::in);
    int chunk_size_bytes =41943040;
    std::vector<char> red(chunk_size_bytes);
    int i =0;
    while(!file.eof()){
        file.read(red.data(),chunk_size_bytes);
        int x = file.gcount();
        std::string name = "chunk"+ std::to_string(i)+ ".bin";
        std::fstream opf(name,std::ios::binary | std::ios::out);
        opf.write(red.data(),x);
        opf.close();
        i++;
    }
    file.close();
    int j =0;
    std::fstream opm("op_main.mp4", std::ios::binary | std::ios::out);
    while(i-1>=0){
        std::string name = "chunk"+std::to_string(j++)+".bin";
        std::fstream chunk(name,std::ios::binary | std::ios::in);
        chunk.read(red.data(),chunk_size_bytes);
        int x = chunk.gcount();
        opm.write(red.data(),x);
        chunk.close();
        i--;
    }
}
