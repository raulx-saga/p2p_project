#include <iostream> 
#include <string>
#include <fstream> 
int main(){
    
    std::fstream file("img.bmp",std::ios::binary | std::ios::in);
    for(int i=0;i<10;i++){
        std::string name="chunk" + std::to_string(i) + ".bin";
        std::fstream opf(name, std::ios::out);
        char red[100];
        file.read(reinterpret_cast<char*>(red),100);
        opf.write(reinterpret_cast<char *>(red), 100);
        opf.close();
    }
    file.close();
    std::fstream opm("outputmain.bmp", std::ios::out | std::ios::binary);
    for(int i=0;i<10;i++){
        std::string name="chunk" + std::to_string(i) + ".bin";
        std::fstream file(name, std::ios::in| std::ios::binary);
        char red[100];
        file.read(reinterpret_cast<char *>(red), 100);
        opm.write(reinterpret_cast<char *>(red),100);
        file.close();
    }
    opm.close();
    return 0;
}